#define CATCH_CONFIG_MAIN
#include "datelib/HolidayCalendar.h"
#include "datelib/date.h"

#include "catch2/catch.hpp"

using namespace std::chrono;

TEST_CASE("isBusinessDay with empty calendar", "[isBusinessDay]") {
    datelib::HolidayCalendar emptyCalendar;

    SECTION("Weekdays are business days") {
        // Monday, January 1, 2024
        REQUIRE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{1}}, emptyCalendar));

        // Tuesday, January 2, 2024
        REQUIRE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{2}}, emptyCalendar));

        // Wednesday, January 3, 2024
        REQUIRE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{3}}, emptyCalendar));

        // Thursday, January 4, 2024
        REQUIRE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{4}}, emptyCalendar));

        // Friday, January 5, 2024
        REQUIRE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{5}}, emptyCalendar));
    }

    SECTION("Weekends are not business days") {
        // Saturday, January 6, 2024
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{6}}, emptyCalendar));

        // Sunday, January 7, 2024
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{7}}, emptyCalendar));
    }
}

TEST_CASE("isBusinessDay with holidays", "[isBusinessDay]") {
    datelib::HolidayCalendar calendar;

    // Add some fixed holidays
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Independence Day", 7, 4));
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

    SECTION("Holidays on weekdays are not business days") {
        // Monday, January 1, 2024 (New Year's Day)
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{1}}, calendar));

        // Thursday, July 4, 2024 (Independence Day)
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{7}, day{4}}, calendar));

        // Wednesday, December 25, 2024 (Christmas)
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{12}, day{25}}, calendar));
    }

    SECTION("Non-holiday weekdays are business days") {
        // Tuesday, January 2, 2024
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{2}}, calendar));

        // Friday, July 5, 2024
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{7}, day{5}}, calendar));
    }

    SECTION("Weekends are still not business days") {
        // Saturday, January 6, 2024
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{6}}, calendar));

        // Sunday, January 7, 2024
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{7}}, calendar));
    }
}

TEST_CASE("isBusinessDay with Nth weekday holidays", "[isBusinessDay]") {
    datelib::HolidayCalendar calendar;

    // Add Thanksgiving (4th Thursday of November)
    calendar.addRule(std::make_unique<datelib::NthWeekdayRule>("Thanksgiving", 11, 4,
                                                               datelib::Occurrence::Fourth));

    SECTION("Thanksgiving 2024 is not a business day") {
        // Thursday, November 28, 2024 (Thanksgiving)
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{11}, day{28}}, calendar));
    }

    SECTION("Other Thursdays in November are business days") {
        // Thursday, November 7, 2024
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{11}, day{7}}, calendar));

        // Thursday, November 21, 2024
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{11}, day{21}}, calendar));
    }
}

TEST_CASE("isBusinessDay with explicit date holidays", "[isBusinessDay]") {
    datelib::HolidayCalendar calendar;

    // Add a one-time holiday
    calendar.addHoliday("Company Anniversary", year_month_day{year{2024}, month{6}, day{13}});

    SECTION("Explicit holiday on weekday is not a business day") {
        // Thursday, June 13, 2024 (Company Anniversary)
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2024}, month{6}, day{13}}, calendar));
    }

    SECTION("Same date in different year is a business day") {
        // Friday, June 13, 2025 (not a holiday)
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2025}, month{6}, day{13}}, calendar));
    }
}

TEST_CASE("isBusinessDay real-world scenario", "[isBusinessDay]") {
    datelib::HolidayCalendar usHolidays;

    // Add US federal holidays
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("Independence Day", 7, 4));
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));
    usHolidays.addRule(
        std::make_unique<datelib::NthWeekdayRule>("Memorial Day", 5, 1, datelib::Occurrence::Last));
    usHolidays.addRule(
        std::make_unique<datelib::NthWeekdayRule>("Labor Day", 9, 1, datelib::Occurrence::First));
    usHolidays.addRule(std::make_unique<datelib::NthWeekdayRule>("Thanksgiving", 11, 4,
                                                                 datelib::Occurrence::Fourth));

    SECTION("Verify business days in a typical week") {
        // Week of January 2-8, 2024 (New Year's on Monday Jan 1)
        REQUIRE_FALSE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{1}},
                                             usHolidays)); // Monday - New Year's
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{2}},
                                       usHolidays)); // Tuesday
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{3}},
                                       usHolidays)); // Wednesday
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{4}},
                                       usHolidays)); // Thursday
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{5}},
                                       usHolidays)); // Friday
        REQUIRE_FALSE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{6}},
                                             usHolidays)); // Saturday
        REQUIRE_FALSE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{7}},
                                             usHolidays)); // Sunday
    }
}

TEST_CASE("isBusinessDay with invalid dates", "[isBusinessDay][edge_cases]") {
    datelib::HolidayCalendar calendar;

    SECTION("Invalid dates should throw") {
        // February 30th (invalid)
        REQUIRE_THROWS_WITH(
            datelib::isBusinessDay(year_month_day{year{2024}, month{2}, day{30}}, calendar),
            "Invalid date provided to isBusinessDay");

        // April 31st (invalid)
        REQUIRE_THROWS_WITH(
            datelib::isBusinessDay(year_month_day{year{2024}, month{4}, day{31}}, calendar),
            "Invalid date provided to isBusinessDay");

        // February 29 on non-leap year
        REQUIRE_THROWS_WITH(
            datelib::isBusinessDay(year_month_day{year{2023}, month{2}, day{29}}, calendar),
            "Invalid date provided to isBusinessDay");

        // Day 0 (invalid)
        REQUIRE_THROWS_WITH(
            datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{0}}, calendar),
            "Invalid date provided to isBusinessDay");
    }
}

TEST_CASE("isBusinessDay with leap year dates", "[isBusinessDay][edge_cases]") {
    datelib::HolidayCalendar calendar;

    SECTION("February 29 on leap years") {
        // 2024 is a leap year (divisible by 4)
        // February 29, 2024 is a Thursday
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{2}, day{29}}, calendar));

        // 2000 was a leap year (divisible by 400)
        // February 29, 2000 was a Tuesday
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2000}, month{2}, day{29}}, calendar));
    }

    SECTION("Century years that are NOT leap years") {
        // 1900 was NOT a leap year (divisible by 100 but not 400)
        // February 29, 1900 is INVALID
        REQUIRE_THROWS_WITH(
            datelib::isBusinessDay(year_month_day{year{1900}, month{2}, day{29}}, calendar),
            "Invalid date provided to isBusinessDay");

        // 2100 will NOT be a leap year
        REQUIRE_THROWS_WITH(
            datelib::isBusinessDay(year_month_day{year{2100}, month{2}, day{29}}, calendar),
            "Invalid date provided to isBusinessDay");
    }
}

TEST_CASE("isBusinessDay across year boundaries", "[isBusinessDay][edge_cases]") {
    datelib::HolidayCalendar calendar;

    SECTION("Last day of year") {
        // December 31, 2024 is a Tuesday
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{12}, day{31}}, calendar));

        // December 31, 2023 was a Sunday
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2023}, month{12}, day{31}}, calendar));
    }

    SECTION("First day of year") {
        // January 1, 2025 is a Wednesday
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2025}, month{1}, day{1}}, calendar));
    }
}

TEST_CASE("isBusinessDay with holidays falling on weekends", "[isBusinessDay][edge_cases]") {
    datelib::HolidayCalendar calendar;
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

    SECTION("Holiday on Saturday") {
        // December 25, 2021 was a Saturday (Christmas)
        // It's both a weekend AND a holiday
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2021}, month{12}, day{25}}, calendar));
    }

    SECTION("Holiday on Sunday") {
        // December 25, 2022 was a Sunday (Christmas)
        REQUIRE_FALSE(
            datelib::isBusinessDay(year_month_day{year{2022}, month{12}, day{25}}, calendar));
    }
}

TEST_CASE("isBusinessDay with custom weekend days", "[isBusinessDay][configurable]") {
    datelib::HolidayCalendar calendar;

    SECTION("Friday-Saturday weekend (Middle East)") {
        // In some Middle Eastern countries, the weekend is Friday-Saturday
        // Sunday=0, Monday=1, Tuesday=2, Wednesday=3, Thursday=4, Friday=5, Saturday=6
        std::unordered_set<unsigned> friday_saturday_weekend = {5u, 6u};

        // Thursday, January 4, 2024 should be a business day
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{4}}, calendar,
                                       friday_saturday_weekend));

        // Friday, January 5, 2024 should NOT be a business day (weekend)
        REQUIRE_FALSE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{5}}, calendar,
                                             friday_saturday_weekend));

        // Saturday, January 6, 2024 should NOT be a business day (weekend)
        REQUIRE_FALSE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{6}}, calendar,
                                             friday_saturday_weekend));

        // Sunday, January 7, 2024 should be a business day
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{7}}, calendar,
                                       friday_saturday_weekend));
    }

    SECTION("Sunday-only weekend") {
        // Some countries have only Sunday as weekend
        std::unordered_set<unsigned> sunday_only_weekend = {0u};

        // Saturday, January 6, 2024 should be a business day
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{6}}, calendar,
                                       sunday_only_weekend));

        // Sunday, January 7, 2024 should NOT be a business day (weekend)
        REQUIRE_FALSE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{7}}, calendar,
                                             sunday_only_weekend));
    }

    SECTION("No weekend days (seven-day work week)") {
        // Empty set means no weekend days
        std::unordered_set<unsigned> no_weekend = {};

        // All days should be business days (if not holidays)
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{6}}, calendar,
                                       no_weekend)); // Saturday
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{7}}, calendar,
                                       no_weekend)); // Sunday
    }
}
