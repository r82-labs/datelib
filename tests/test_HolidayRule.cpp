#include "datelib/HolidayRule.h"

#include <stdexcept>

#include "catch2/catch.hpp"

TEST_CASE("FixedDateRule construction", "[HolidayRule]") {
    SECTION("Valid fixed date rules") {
        REQUIRE_NOTHROW(datelib::FixedDateRule("Christmas", 12, 25));
        REQUIRE_NOTHROW(datelib::FixedDateRule("New Year's Day", 1, 1));
    }

    SECTION("Invalid months") {
        REQUIRE_THROWS_AS(datelib::FixedDateRule("Invalid", 0, 1), std::invalid_argument);
        REQUIRE_THROWS_AS(datelib::FixedDateRule("Invalid", 13, 1), std::invalid_argument);
    }

    SECTION("Invalid days") {
        REQUIRE_THROWS_AS(datelib::FixedDateRule("Invalid", 1, 0), std::invalid_argument);
        REQUIRE_THROWS_AS(datelib::FixedDateRule("Invalid", 1, 32), std::invalid_argument);
    }
}

TEST_CASE("FixedDateRule calculates correct dates", "[HolidayRule]") {
    datelib::FixedDateRule christmas("Christmas", 12, 25);

    SECTION("Calculates for different years") {
        datelib::Date date2024 = christmas.calculateDate(2024);
        REQUIRE(date2024.year() == 2024);
        REQUIRE(date2024.month() == 12);
        REQUIRE(date2024.day() == 25);

        datelib::Date date2025 = christmas.calculateDate(2025);
        REQUIRE(date2025.year() == 2025);
        REQUIRE(date2025.month() == 12);
        REQUIRE(date2025.day() == 25);
    }

    SECTION("Returns name") {
        REQUIRE(christmas.getName() == "Christmas");
    }
}

TEST_CASE("NthWeekdayRule construction", "[HolidayRule]") {
    SECTION("Valid Nth weekday rules") {
        REQUIRE_NOTHROW(datelib::NthWeekdayRule("Thanksgiving", 11, 4, 4)); // 4th Thursday
                                                                            // of November
        REQUIRE_NOTHROW(datelib::NthWeekdayRule("Labor Day", 9, 1,
                                                1)); // 1st Monday of September
    }

    SECTION("Invalid parameters") {
        REQUIRE_THROWS_AS(datelib::NthWeekdayRule("Invalid", 0, 1, 1),
                          std::invalid_argument); // Invalid month
        REQUIRE_THROWS_AS(datelib::NthWeekdayRule("Invalid", 1, 7, 1),
                          std::invalid_argument); // Invalid weekday
        REQUIRE_THROWS_AS(datelib::NthWeekdayRule("Invalid", 1, 1, 0),
                          std::invalid_argument); // Invalid occurrence
    }
}

TEST_CASE("NthWeekdayRule calculates correct dates", "[HolidayRule]") {
    SECTION("Thanksgiving - 4th Thursday of November") {
        datelib::NthWeekdayRule thanksgiving("Thanksgiving", 11, 4, 4);

        datelib::Date date2024 = thanksgiving.calculateDate(2024);
        REQUIRE(date2024 == datelib::Date(2024, 11, 28));
        REQUIRE(date2024.dayOfWeek() == 4); // Thursday

        datelib::Date date2025 = thanksgiving.calculateDate(2025);
        REQUIRE(date2025 == datelib::Date(2025, 11, 27));
        REQUIRE(date2025.dayOfWeek() == 4); // Thursday
    }

    SECTION("Labor Day - 1st Monday of September") {
        datelib::NthWeekdayRule laborDay("Labor Day", 9, 1, 1);

        datelib::Date date2024 = laborDay.calculateDate(2024);
        REQUIRE(date2024 == datelib::Date(2024, 9, 2));
        REQUIRE(date2024.dayOfWeek() == 1); // Monday

        datelib::Date date2025 = laborDay.calculateDate(2025);
        REQUIRE(date2025 == datelib::Date(2025, 9, 1));
        REQUIRE(date2025.dayOfWeek() == 1); // Monday
    }

    SECTION("Memorial Day - Last Monday of May") {
        datelib::NthWeekdayRule memorialDay("Memorial Day", 5, 1, -1);

        datelib::Date date2024 = memorialDay.calculateDate(2024);
        REQUIRE(date2024 == datelib::Date(2024, 5, 27));
        REQUIRE(date2024.dayOfWeek() == 1); // Monday

        datelib::Date date2025 = memorialDay.calculateDate(2025);
        REQUIRE(date2025 == datelib::Date(2025, 5, 26));
        REQUIRE(date2025.dayOfWeek() == 1); // Monday
    }
}

TEST_CASE("HolidayRule clone", "[HolidayRule]") {
    SECTION("FixedDateRule clone") {
        datelib::FixedDateRule original("Christmas", 12, 25);
        auto cloned = original.clone();

        REQUIRE(cloned->getName() == original.getName());
        REQUIRE(cloned->calculateDate(2024) == original.calculateDate(2024));
    }

    SECTION("NthWeekdayRule clone") {
        datelib::NthWeekdayRule original("Thanksgiving", 11, 4, 4);
        auto cloned = original.clone();

        REQUIRE(cloned->getName() == original.getName());
        REQUIRE(cloned->calculateDate(2024) == original.calculateDate(2024));
    }
}
