#include "datelib/HolidayRule.h"

#include <stdexcept>

#include "catch2/catch.hpp"

using namespace std::chrono;

TEST_CASE("ExplicitDateRule construction", "[HolidayRule]") {
    SECTION("Valid explicit date rules") {
        year_month_day ymd{year{2024}, month{10}, day{31}};
        REQUIRE_NOTHROW(datelib::ExplicitDateRule("Halloween 2024", ymd));
    }

    SECTION("Invalid dates") {
        year_month_day invalid_ymd{year{2024}, month{2}, day{30}};
        REQUIRE_THROWS_WITH(datelib::ExplicitDateRule("Invalid", invalid_ymd), "Invalid date");
    }
}

TEST_CASE("ExplicitDateRule calculates correct dates", "[HolidayRule]") {
    year_month_day halloween_2024{year{2024}, month{10}, day{31}};
    datelib::ExplicitDateRule halloween("Halloween 2024", halloween_2024);

    SECTION("Returns date for correct year") {
        auto date = halloween.calculateDate(2024);
        REQUIRE(date == halloween_2024);
        REQUIRE(halloween.getName() == "Halloween 2024");
    }

    SECTION("Throws for different year") {
        REQUIRE_THROWS_WITH(halloween.calculateDate(2025),
                            "Explicit date does not exist in this year");
    }
}

TEST_CASE("FixedDateRule construction", "[HolidayRule]") {
    SECTION("Valid fixed date rules") {
        REQUIRE_NOTHROW(datelib::FixedDateRule("Christmas", 12, 25));
        REQUIRE_NOTHROW(datelib::FixedDateRule("New Year's Day", 1, 1));
    }

    SECTION("Invalid months") {
        REQUIRE_THROWS_WITH(datelib::FixedDateRule("Invalid", 0, 1),
                            "Month must be between 1 and 12");
        REQUIRE_THROWS_WITH(datelib::FixedDateRule("Invalid", 13, 1),
                            "Month must be between 1 and 12");
    }

    SECTION("Invalid days") {
        REQUIRE_THROWS_WITH(datelib::FixedDateRule("Invalid", 1, 0),
                            "Day must be between 1 and 31");
        REQUIRE_THROWS_WITH(datelib::FixedDateRule("Invalid", 1, 32),
                            "Day must be between 1 and 31");
    }

    SECTION("Invalid date combinations") {
        // Feb 30 should fail during calculation, not construction
        REQUIRE_NOTHROW(datelib::FixedDateRule("Invalid Feb 30", 2, 30));
        datelib::FixedDateRule feb30("Invalid Feb 30", 2, 30);
        REQUIRE_THROWS_WITH(feb30.calculateDate(2024), "Invalid date for this year");
    }
}

TEST_CASE("FixedDateRule calculates correct dates", "[HolidayRule]") {
    datelib::FixedDateRule christmas("Christmas", 12, 25);

    SECTION("Calculates for different years") {
        auto date2024 = christmas.calculateDate(2024);
        REQUIRE(date2024 == year_month_day{year{2024}, month{12}, day{25}});

        auto date2025 = christmas.calculateDate(2025);
        REQUIRE(date2025 == year_month_day{year{2025}, month{12}, day{25}});
    }

    SECTION("Returns name") {
        REQUIRE(christmas.getName() == "Christmas");
    }
}

TEST_CASE("NthWeekdayRule construction", "[HolidayRule]") {
    SECTION("Valid Nth weekday rules") {
        REQUIRE_NOTHROW(
            datelib::NthWeekdayRule("Thanksgiving", 11, 4, datelib::Occurrence::Fourth));
        REQUIRE_NOTHROW(datelib::NthWeekdayRule("Labor Day", 9, 1, datelib::Occurrence::First));
    }

    SECTION("Invalid parameters") {
        REQUIRE_THROWS_WITH(datelib::NthWeekdayRule("Invalid", 0, 1, datelib::Occurrence::First),
                            "Month must be between 1 and 12");
        REQUIRE_THROWS_WITH(datelib::NthWeekdayRule("Invalid", 1, 7, datelib::Occurrence::First),
                            "Weekday must be between 0 and 6");
        // Test invalid occurrence value (casting 0 to Occurrence enum)
        REQUIRE_THROWS_WITH(
            datelib::NthWeekdayRule("Invalid", 1, 1, static_cast<datelib::Occurrence>(0)),
            "Occurrence must be First through Fifth or Last");
    }
}

TEST_CASE("NthWeekdayRule calculates correct dates", "[HolidayRule]") {
    SECTION("Thanksgiving - 4th Thursday of November") {
        datelib::NthWeekdayRule thanksgiving("Thanksgiving", 11, 4, datelib::Occurrence::Fourth);

        auto date2024 = thanksgiving.calculateDate(2024);
        REQUIRE(date2024 == year_month_day{year{2024}, month{11}, day{28}});

        // Verify it's a Thursday
        sys_days sd{date2024};
        weekday wd{sd};
        REQUIRE(wd.c_encoding() == 4);
    }

    SECTION("Labor Day - 1st Monday of September") {
        datelib::NthWeekdayRule laborDay("Labor Day", 9, 1, datelib::Occurrence::First);

        auto date2024 = laborDay.calculateDate(2024);
        REQUIRE(date2024 == year_month_day{year{2024}, month{9}, day{2}});

        // Verify it's a Monday
        sys_days sd{date2024};
        weekday wd{sd};
        REQUIRE(wd.c_encoding() == 1);
    }

    SECTION("Memorial Day - Last Monday of May") {
        datelib::NthWeekdayRule memorialDay("Memorial Day", 5, 1, datelib::Occurrence::Last);

        auto date2024 = memorialDay.calculateDate(2024);
        REQUIRE(date2024 == year_month_day{year{2024}, month{5}, day{27}});

        // Verify it's a Monday
        sys_days sd{date2024};
        weekday wd{sd};
        REQUIRE(wd.c_encoding() == 1);
    }

    SECTION("Fifth occurrence that doesn't exist throws exception") {
        // February 2024 only has 4 Saturdays, so 5th Saturday doesn't exist
        datelib::NthWeekdayRule fifthSaturday("Fifth Saturday", 2, 6, datelib::Occurrence::Fifth);
        REQUIRE_THROWS_WITH(fifthSaturday.calculateDate(2024),
                            "Requested occurrence does not exist in this month");
    }
}

TEST_CASE("HolidayRule clone", "[HolidayRule]") {
    SECTION("ExplicitDateRule clone") {
        year_month_day ymd{year{2024}, month{10}, day{31}};
        datelib::ExplicitDateRule original("Halloween", ymd);
        auto cloned = original.clone();

        REQUIRE(cloned->getName() == original.getName());
        REQUIRE(cloned->calculateDate(2024) == original.calculateDate(2024));
    }

    SECTION("FixedDateRule clone") {
        datelib::FixedDateRule original("Christmas", 12, 25);
        auto cloned = original.clone();

        REQUIRE(cloned->getName() == original.getName());
        REQUIRE(cloned->calculateDate(2024) == original.calculateDate(2024));
    }

    SECTION("NthWeekdayRule clone") {
        datelib::NthWeekdayRule original("Thanksgiving", 11, 4, datelib::Occurrence::Fourth);
        auto cloned = original.clone();

        REQUIRE(cloned->getName() == original.getName());
        REQUIRE(cloned->calculateDate(2024) == original.calculateDate(2024));
    }
}

TEST_CASE("HolidayRule polymorphic destruction", "[HolidayRule]") {
    SECTION("Delete through base class pointer") {
        // Test virtual destructor by deleting through base class pointer
        // This ensures all destructor variants (D0, D1, D2) are covered
        {
            std::unique_ptr<datelib::HolidayRule> rule1 =
                std::make_unique<datelib::ExplicitDateRule>(
                    "Test", year_month_day{year{2024}, month{1}, day{1}});
            // Destructor called when unique_ptr goes out of scope
        }
        {
            std::unique_ptr<datelib::HolidayRule> rule2 =
                std::make_unique<datelib::FixedDateRule>("Test", 12, 25);
            // Destructor called when unique_ptr goes out of scope
        }
        {
            std::unique_ptr<datelib::HolidayRule> rule3 = std::make_unique<datelib::NthWeekdayRule>(
                "Test", 11, 4, datelib::Occurrence::Fourth);
            // Destructor called when unique_ptr goes out of scope
        }
        REQUIRE(true); // Just need to ensure destructors are called
    }
}
