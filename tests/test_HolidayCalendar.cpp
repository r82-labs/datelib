#include "datelib/HolidayCalendar.h"

#include "catch2/catch.hpp"

using namespace std::chrono;

TEST_CASE("HolidayCalendar construction", "[HolidayCalendar]") {
    REQUIRE_NOTHROW(datelib::HolidayCalendar());
}

TEST_CASE("HolidayCalendar with explicit dates", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar;

    SECTION("Add and check explicit holiday") {
        year_month_day newYear{year{2024}, month{1}, day{1}};
        calendar.addHoliday("New Year's Day 2024", newYear);

        REQUIRE(calendar.isHoliday(newYear));
        REQUIRE_FALSE(calendar.isHoliday(year_month_day{year{2024}, month{1}, day{2}}));
    }

    SECTION("Get holidays for a year with explicit dates") {
        calendar.addHoliday("New Year", year_month_day{year{2024}, month{1}, day{1}});
        calendar.addHoliday("Independence Day", year_month_day{year{2024}, month{7}, day{4}});
        calendar.addHoliday("Christmas", year_month_day{year{2024}, month{12}, day{25}});
        calendar.addHoliday("Different Year", year_month_day{year{2025}, month{1}, day{1}});

        auto holidays = calendar.getHolidays(2024);
        REQUIRE(holidays.size() == 3);
        REQUIRE(holidays[0] == year_month_day{year{2024}, month{1}, day{1}});
        REQUIRE(holidays[1] == year_month_day{year{2024}, month{7}, day{4}});
        REQUIRE(holidays[2] == year_month_day{year{2024}, month{12}, day{25}});
    }

    SECTION("Multiple explicit rules for same year") {
        // Example: Adding multiple one-time holidays for 2024
        // Each ExplicitDateRule is independent and can be queried
        calendar.addHoliday("Eclipse Day", year_month_day{year{2024}, month{4}, day{8}});
        calendar.addHoliday("Company Anniversary", year_month_day{year{2024}, month{6}, day{15}});
        calendar.addHoliday("Special Event", year_month_day{year{2024}, month{9}, day{20}});

        // All three holidays are recognized for 2024
        REQUIRE(calendar.isHoliday(year_month_day{year{2024}, month{4}, day{8}}));
        REQUIRE(calendar.isHoliday(year_month_day{year{2024}, month{6}, day{15}}));
        REQUIRE(calendar.isHoliday(year_month_day{year{2024}, month{9}, day{20}}));

        // getHolidays returns all explicit holidays for that year
        auto holidays2024 = calendar.getHolidays(2024);
        REQUIRE(holidays2024.size() == 3);

        // getHolidayNames returns the name of each explicit holiday
        auto names = calendar.getHolidayNames(year_month_day{year{2024}, month{4}, day{8}});
        REQUIRE(names.size() == 1);
        REQUIRE(names[0] == "Eclipse Day");

        // Querying for a different year returns no holidays (explicit rules only apply to their
        // specific year)
        auto holidays2025 = calendar.getHolidays(2025);
        REQUIRE(holidays2025.empty());

        // Test isHoliday with explicit date from different year (triggers exception path)
        REQUIRE_FALSE(calendar.isHoliday(year_month_day{year{2025}, month{4}, day{8}}));

        // Test getHolidayNames with explicit date from different year (triggers exception path)
        auto names2025 = calendar.getHolidayNames(year_month_day{year{2025}, month{4}, day{8}});
        REQUIRE(names2025.empty());
    }
}

TEST_CASE("HolidayCalendar with rules", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar;

    SECTION("Add and check rule-based holiday") {
        calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

        REQUIRE(calendar.isHoliday(year_month_day{year{2024}, month{12}, day{25}}));
        REQUIRE(calendar.isHoliday(year_month_day{year{2025}, month{12}, day{25}}));
        REQUIRE_FALSE(calendar.isHoliday(year_month_day{year{2024}, month{12}, day{24}}));
    }

    SECTION("Multiple rules") {
        calendar.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
        calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));
        calendar.addRule(std::make_unique<datelib::NthWeekdayRule>("Thanksgiving", 11, 4,
                                                                   datelib::Occurrence::Fourth));

        auto holidays = calendar.getHolidays(2024);
        REQUIRE(holidays.size() == 3);

        // Verify they're sorted
        REQUIRE(holidays[0] == year_month_day{year{2024}, month{1}, day{1}});
        REQUIRE(holidays[1] == year_month_day{year{2024}, month{11}, day{28}});
        REQUIRE(holidays[2] == year_month_day{year{2024}, month{12}, day{25}});
    }
}

TEST_CASE("HolidayCalendar with mixed explicit and rule-based", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar;

    calendar.addHoliday("July 4th 2024", year_month_day{year{2024}, month{7}, day{4}});
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

    SECTION("Both types are recognized") {
        REQUIRE(calendar.isHoliday(year_month_day{year{2024}, month{7}, day{4}}));
        REQUIRE(calendar.isHoliday(year_month_day{year{2024}, month{12}, day{25}}));
    }

    SECTION("Get all holidays") {
        auto holidays = calendar.getHolidays(2024);
        REQUIRE(holidays.size() == 2);
        REQUIRE(holidays[0] == year_month_day{year{2024}, month{7}, day{4}});
        REQUIRE(holidays[1] == year_month_day{year{2024}, month{12}, day{25}});
    }
}

TEST_CASE("HolidayCalendar getHolidayNames", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar;

    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Boxing Day", 12, 26));

    SECTION("Get name for holiday") {
        auto names = calendar.getHolidayNames(year_month_day{year{2024}, month{12}, day{25}});
        REQUIRE(names.size() == 1);
        REQUIRE(names[0] == "Christmas");
    }

    SECTION("Get empty list for non-holiday") {
        auto names = calendar.getHolidayNames(year_month_day{year{2024}, month{12}, day{24}});
        REQUIRE(names.empty());
    }
}

TEST_CASE("HolidayCalendar copy operations", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar1;
    calendar1.addHoliday("July 4th", year_month_day{year{2024}, month{7}, day{4}});
    calendar1.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

    SECTION("Copy constructor") {
        datelib::HolidayCalendar calendar2(calendar1);

        REQUIRE(calendar2.isHoliday(year_month_day{year{2024}, month{7}, day{4}}));
        REQUIRE(calendar2.isHoliday(year_month_day{year{2024}, month{12}, day{25}}));

        auto holidays = calendar2.getHolidays(2024);
        REQUIRE(holidays.size() == 2);
    }

    SECTION("Copy assignment") {
        datelib::HolidayCalendar calendar2;
        calendar2 = calendar1;

        REQUIRE(calendar2.isHoliday(year_month_day{year{2024}, month{7}, day{4}}));
        REQUIRE(calendar2.isHoliday(year_month_day{year{2024}, month{12}, day{25}}));

        auto holidays = calendar2.getHolidays(2024);
        REQUIRE(holidays.size() == 2);
    }
}

TEST_CASE("Real-world US holidays example", "[HolidayCalendar]") {
    datelib::HolidayCalendar usHolidays;

    // Fixed date holidays
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("Independence Day", 7, 4));
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("Veterans Day", 11, 11));
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

    // Nth weekday holidays
    usHolidays.addRule(std::make_unique<datelib::NthWeekdayRule>("Martin Luther King Jr. Day", 1, 1,
                                                                 datelib::Occurrence::Third));
    usHolidays.addRule(std::make_unique<datelib::NthWeekdayRule>("Presidents' Day", 2, 1,
                                                                 datelib::Occurrence::Third));
    usHolidays.addRule(
        std::make_unique<datelib::NthWeekdayRule>("Memorial Day", 5, 1, datelib::Occurrence::Last));
    usHolidays.addRule(
        std::make_unique<datelib::NthWeekdayRule>("Labor Day", 9, 1, datelib::Occurrence::First));
    usHolidays.addRule(std::make_unique<datelib::NthWeekdayRule>("Columbus Day", 10, 1,
                                                                 datelib::Occurrence::Second));
    usHolidays.addRule(std::make_unique<datelib::NthWeekdayRule>("Thanksgiving", 11, 4,
                                                                 datelib::Occurrence::Fourth));

    SECTION("Verify 2024 holidays") {
        auto holidays2024 = usHolidays.getHolidays(2024);
        REQUIRE(holidays2024.size() == 10);

        // Spot check a few
        REQUIRE(usHolidays.isHoliday(year_month_day{year{2024}, month{1}, day{1}}));
        REQUIRE(usHolidays.isHoliday(year_month_day{year{2024}, month{1}, day{15}}));
        REQUIRE(usHolidays.isHoliday(year_month_day{year{2024}, month{11}, day{28}}));
        REQUIRE(usHolidays.isHoliday(year_month_day{year{2024}, month{12}, day{25}}));
    }

    SECTION("Get holiday names") {
        auto names = usHolidays.getHolidayNames(year_month_day{year{2024}, month{11}, day{28}});
        REQUIRE(names.size() == 1);
        REQUIRE(names[0] == "Thanksgiving");
    }
}
