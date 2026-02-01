#include "datelib/HolidayCalendar.h"
#include "datelib/date.h"
#include "datelib/period.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

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
        std::unordered_set<weekday, datelib::WeekdayHash> friday_saturday_weekend = {Friday,
                                                                                     Saturday};

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
        std::unordered_set<weekday, datelib::WeekdayHash> sunday_only_weekend = {Sunday};

        // Saturday, January 6, 2024 should be a business day
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{6}}, calendar,
                                       sunday_only_weekend));

        // Sunday, January 7, 2024 should NOT be a business day (weekend)
        REQUIRE_FALSE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{7}}, calendar,
                                             sunday_only_weekend));
    }

    SECTION("No weekend days (seven-day work week)") {
        // Empty set means no weekend days
        std::unordered_set<weekday, datelib::WeekdayHash> no_weekend = {};

        // All days should be business days (if not holidays)
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{6}}, calendar,
                                       no_weekend)); // Saturday
        REQUIRE(datelib::isBusinessDay(year_month_day{year{2024}, month{1}, day{7}}, calendar,
                                       no_weekend)); // Sunday
    }
}

TEST_CASE("adjust with Following convention", "[adjust]") {
    datelib::HolidayCalendar calendar;

    SECTION("Business day remains unchanged") {
        // Tuesday, January 2, 2024
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(adjusted == date);
    }

    SECTION("Saturday moves to Monday") {
        // Saturday, January 6, 2024 -> Monday, January 8, 2024
        auto date = year_month_day{year{2024}, month{1}, day{6}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{1}, day{8}});
    }

    SECTION("Sunday moves to Monday") {
        // Sunday, January 7, 2024 -> Monday, January 8, 2024
        auto date = year_month_day{year{2024}, month{1}, day{7}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{1}, day{8}});
    }

    SECTION("Holiday on weekday moves to next business day") {
        calendar.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
        // Monday, January 1, 2024 (New Year's) -> Tuesday, January 2, 2024
        auto date = year_month_day{year{2024}, month{1}, day{1}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{1}, day{2}});
    }
}

TEST_CASE("adjust with ModifiedFollowing convention", "[adjust]") {
    datelib::HolidayCalendar calendar;

    SECTION("Business day remains unchanged") {
        // Tuesday, January 2, 2024
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto adjusted =
            datelib::adjust(date, datelib::BusinessDayConvention::ModifiedFollowing, calendar);
        REQUIRE(adjusted == date);
    }

    SECTION("Weekend within same month moves forward") {
        // Saturday, January 6, 2024 -> Monday, January 8, 2024
        auto date = year_month_day{year{2024}, month{1}, day{6}};
        auto adjusted =
            datelib::adjust(date, datelib::BusinessDayConvention::ModifiedFollowing, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{1}, day{8}});
    }

    SECTION("Weekend at month end crosses into next month, moves backward") {
        // Saturday, June 29, 2024 (last Sat of June)
        // Following would give Monday, July 1
        // ModifiedFollowing gives Friday, June 28 (to stay in June)
        auto date = year_month_day{year{2024}, month{6}, day{29}};
        auto adjusted =
            datelib::adjust(date, datelib::BusinessDayConvention::ModifiedFollowing, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{6}, day{28}});
    }

    SECTION("Multiple non-business days at month end") {
        // Sunday, June 30, 2024 (last day of June)
        // Following would give Monday, July 1
        // ModifiedFollowing gives Friday, June 28
        auto date = year_month_day{year{2024}, month{6}, day{30}};
        auto adjusted =
            datelib::adjust(date, datelib::BusinessDayConvention::ModifiedFollowing, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{6}, day{28}});
    }
}

TEST_CASE("adjust with Preceding convention", "[adjust]") {
    datelib::HolidayCalendar calendar;

    SECTION("Business day remains unchanged") {
        // Tuesday, January 2, 2024
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Preceding, calendar);
        REQUIRE(adjusted == date);
    }

    SECTION("Saturday moves to Friday") {
        // Saturday, January 6, 2024 -> Friday, January 5, 2024
        auto date = year_month_day{year{2024}, month{1}, day{6}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Preceding, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{1}, day{5}});
    }

    SECTION("Sunday moves to Friday") {
        // Sunday, January 7, 2024 -> Friday, January 5, 2024
        auto date = year_month_day{year{2024}, month{1}, day{7}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Preceding, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{1}, day{5}});
    }

    SECTION("Holiday on weekday moves to previous business day") {
        calendar.addRule(std::make_unique<datelib::FixedDateRule>("Independence Day", 7, 4));
        // Thursday, July 4, 2024 (Independence Day) -> Wednesday, July 3, 2024
        auto date = year_month_day{year{2024}, month{7}, day{4}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Preceding, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{7}, day{3}});
    }
}

TEST_CASE("adjust with ModifiedPreceding convention", "[adjust]") {
    datelib::HolidayCalendar calendar;

    SECTION("Business day remains unchanged") {
        // Tuesday, January 2, 2024
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto adjusted =
            datelib::adjust(date, datelib::BusinessDayConvention::ModifiedPreceding, calendar);
        REQUIRE(adjusted == date);
    }

    SECTION("Weekend within same month moves backward") {
        // Saturday, January 6, 2024 -> Friday, January 5, 2024
        auto date = year_month_day{year{2024}, month{1}, day{6}};
        auto adjusted =
            datelib::adjust(date, datelib::BusinessDayConvention::ModifiedPreceding, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{1}, day{5}});
    }

    SECTION("Weekend at month start crosses into previous month, moves forward") {
        // Sunday, September 1, 2024 (first day is Sunday)
        // Preceding would give Friday, August 30
        // ModifiedPreceding gives Monday, September 2 (to stay in September)
        auto date = year_month_day{year{2024}, month{9}, day{1}};
        auto adjusted =
            datelib::adjust(date, datelib::BusinessDayConvention::ModifiedPreceding, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{9}, day{2}});
    }
}

TEST_CASE("adjust with Unadjusted convention", "[adjust]") {
    datelib::HolidayCalendar calendar;

    SECTION("Business day remains unchanged") {
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Unadjusted, calendar);
        REQUIRE(adjusted == date);
    }

    SECTION("Weekend remains unchanged") {
        auto date = year_month_day{year{2024}, month{1}, day{6}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Unadjusted, calendar);
        REQUIRE(adjusted == date);
    }

    SECTION("Holiday remains unchanged") {
        calendar.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
        auto date = year_month_day{year{2024}, month{1}, day{1}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Unadjusted, calendar);
        REQUIRE(adjusted == date);
    }
}

TEST_CASE("adjust with complex scenarios", "[adjust][edge_cases]") {
    datelib::HolidayCalendar usHolidays;
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("Independence Day", 7, 4));

    SECTION("Holiday at month end with Following") {
        // Tuesday, December 31, 2024 is a business day
        auto date = year_month_day{year{2024}, month{12}, day{31}};
        auto adjusted =
            datelib::adjust(date, datelib::BusinessDayConvention::Following, usHolidays);
        REQUIRE(adjusted == date);
    }

    SECTION("Long weekend at month end with ModifiedFollowing") {
        // Create a scenario where we have multiple consecutive non-business days
        // For example, if we had holidays Thursday-Friday at month end
        datelib::HolidayCalendar calendar;
        calendar.addHoliday("Special", year_month_day{year{2024}, month{5}, day{30}});  // Thursday
        calendar.addHoliday("Special2", year_month_day{year{2024}, month{5}, day{31}}); // Friday

        // Thursday, May 30, 2024 is a holiday, Fri May 31 is holiday, Sat-Sun are weekend
        // Following would give Monday, June 3
        // ModifiedFollowing should give Wednesday, May 29 (to stay in May)
        auto date = year_month_day{year{2024}, month{5}, day{30}};
        auto adjusted =
            datelib::adjust(date, datelib::BusinessDayConvention::ModifiedFollowing, calendar);
        REQUIRE(adjusted == year_month_day{year{2024}, month{5}, day{29}});
    }
}

TEST_CASE("adjust with invalid dates", "[adjust][edge_cases]") {
    datelib::HolidayCalendar calendar;

    SECTION("Invalid date throws exception") {
        // February 30th is invalid
        auto date = year_month_day{year{2024}, month{2}, day{30}};
        REQUIRE_THROWS_WITH(
            datelib::adjust(date, datelib::BusinessDayConvention::Following, calendar),
            "Invalid date provided to adjust");
    }
}

TEST_CASE("adjust with custom weekend days", "[adjust][configurable]") {
    datelib::HolidayCalendar calendar;
    std::unordered_set<weekday, datelib::WeekdayHash> friday_saturday_weekend = {Friday, Saturday};

    SECTION("Following with Friday-Saturday weekend") {
        // Friday, January 5, 2024 -> Sunday, January 7, 2024
        auto date = year_month_day{year{2024}, month{1}, day{5}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Following, calendar,
                                        friday_saturday_weekend);
        REQUIRE(adjusted == year_month_day{year{2024}, month{1}, day{7}});
    }

    SECTION("Preceding with Friday-Saturday weekend") {
        // Saturday, January 6, 2024 -> Thursday, January 4, 2024
        auto date = year_month_day{year{2024}, month{1}, day{6}};
        auto adjusted = datelib::adjust(date, datelib::BusinessDayConvention::Preceding, calendar,
                                        friday_saturday_weekend);
        REQUIRE(adjusted == year_month_day{year{2024}, month{1}, day{4}});
    }
}

TEST_CASE("adjust with invalid enum value", "[adjust][edge_cases]") {
    datelib::HolidayCalendar calendar;
    // Use a weekend (Saturday) to ensure it's not a business day
    auto date = year_month_day{year{2024}, month{1}, day{6}};

    SECTION("Invalid convention value throws logic_error") {
        // Force an invalid enum value by casting
        auto invalid_convention = static_cast<datelib::BusinessDayConvention>(999);
        REQUIRE_THROWS_AS(datelib::adjust(date, invalid_convention, calendar), std::logic_error);
    }
}

TEST_CASE("adjust with pathological calendar throws after MAX_DAYS_TO_SEARCH",
          "[adjust][edge_cases]") {
    // Create a calendar where every single day for multiple years is a holiday
    datelib::HolidayCalendar allHolidaysCalendar;

    // Add all days for 2023, 2024, and 2025 as holidays to prevent finding
    // business days in adjacent years
    for (int y = 2023; y <= 2025; ++y) {
        for (unsigned m = 1; m <= 12; ++m) {
            unsigned days_in_month = 31;
            if (m == 2)
                days_in_month = (y == 2024) ? 29 : 28; // Leap year check
            else if (m == 4 || m == 6 || m == 9 || m == 11)
                days_in_month = 30;

            for (unsigned d = 1; d <= days_in_month; ++d) {
                allHolidaysCalendar.addHoliday("Holiday",
                                               year_month_day{year{y}, month{m}, day{d}});
            }
        }
    }

    // Use a Saturday (weekend) as the starting date so it's definitely not a business day
    auto date = year_month_day{year{2024}, month{1}, day{6}}; // Saturday

    SECTION("Following convention throws when no business day found") {
        REQUIRE_THROWS_WITH(
            datelib::adjust(date, datelib::BusinessDayConvention::Following, allHolidaysCalendar),
            "Unable to find next business day within reasonable range");
    }

    SECTION("Preceding convention throws when no business day found") {
        REQUIRE_THROWS_WITH(
            datelib::adjust(date, datelib::BusinessDayConvention::Preceding, allHolidaysCalendar),
            "Unable to find previous business day within reasonable range");
    }

    SECTION("ModifiedFollowing throws on forward search") {
        // ModifiedFollowing will first try going forward, which should hit the limit
        REQUIRE_THROWS_WITH(datelib::adjust(date, datelib::BusinessDayConvention::ModifiedFollowing,
                                            allHolidaysCalendar),
                            "Unable to find next business day within reasonable range");
    }

    SECTION("ModifiedPreceding throws on backward search") {
        // ModifiedPreceding will first try going backward, which should hit the limit
        REQUIRE_THROWS_WITH(datelib::adjust(date, datelib::BusinessDayConvention::ModifiedPreceding,
                                            allHolidaysCalendar),
                            "Unable to find previous business day within reasonable range");
    }
}

TEST_CASE("Period::parse with valid strings", "[period]") {
    SECTION("Days") {
        auto period = datelib::Period::parse("5D");
        REQUIRE(period.value() == 5);
        REQUIRE(period.unit() == datelib::Period::Unit::Days);

        period = datelib::Period::parse("1d"); // lowercase
        REQUIRE(period.value() == 1);
        REQUIRE(period.unit() == datelib::Period::Unit::Days);
    }

    SECTION("Weeks") {
        auto period = datelib::Period::parse("2W");
        REQUIRE(period.value() == 2);
        REQUIRE(period.unit() == datelib::Period::Unit::Weeks);

        period = datelib::Period::parse("3w"); // lowercase
        REQUIRE(period.value() == 3);
        REQUIRE(period.unit() == datelib::Period::Unit::Weeks);
    }

    SECTION("Months") {
        auto period = datelib::Period::parse("6M");
        REQUIRE(period.value() == 6);
        REQUIRE(period.unit() == datelib::Period::Unit::Months);

        period = datelib::Period::parse("12m"); // lowercase
        REQUIRE(period.value() == 12);
        REQUIRE(period.unit() == datelib::Period::Unit::Months);
    }

    SECTION("Years") {
        auto period = datelib::Period::parse("10Y");
        REQUIRE(period.value() == 10);
        REQUIRE(period.unit() == datelib::Period::Unit::Years);

        period = datelib::Period::parse("1y"); // lowercase
        REQUIRE(period.value() == 1);
        REQUIRE(period.unit() == datelib::Period::Unit::Years);
    }

    SECTION("Large values") {
        auto period = datelib::Period::parse("365D");
        REQUIRE(period.value() == 365);
        REQUIRE(period.unit() == datelib::Period::Unit::Days);

        period = datelib::Period::parse("100Y");
        REQUIRE(period.value() == 100);
        REQUIRE(period.unit() == datelib::Period::Unit::Years);
    }
}

TEST_CASE("Period::parse with negative values", "[period]") {
    SECTION("Negative days") {
        auto period = datelib::Period::parse("-5D");
        REQUIRE(period.value() == -5);
        REQUIRE(period.unit() == datelib::Period::Unit::Days);
    }

    SECTION("Negative months") {
        auto period = datelib::Period::parse("-6M");
        REQUIRE(period.value() == -6);
        REQUIRE(period.unit() == datelib::Period::Unit::Months);
    }
}

TEST_CASE("Period::parse with invalid strings", "[period][edge_cases]") {
    SECTION("Empty string") {
        REQUIRE_THROWS_WITH(datelib::Period::parse(""), "Period string cannot be empty");
    }

    SECTION("No numeric value") {
        REQUIRE_THROWS_WITH(datelib::Period::parse("D"),
                            "Period string must contain a numeric value: D");
        REQUIRE_THROWS_WITH(datelib::Period::parse("M"),
                            "Period string must contain a numeric value: M");
    }

    SECTION("No unit") {
        REQUIRE_THROWS_WITH(datelib::Period::parse("10"),
                            "Period string must end with a single unit character (D/W/M/Y): 10");
    }

    SECTION("Invalid unit") {
        REQUIRE_THROWS_WITH(datelib::Period::parse("5X"),
                            "Invalid period unit 'X'. Must be D, W, M, or Y: 5X");
        REQUIRE_THROWS_WITH(datelib::Period::parse("5H"),
                            "Invalid period unit 'H'. Must be D, W, M, or Y: 5H");
    }

    SECTION("Multiple unit characters") {
        REQUIRE_THROWS_WITH(datelib::Period::parse("5DD"),
                            "Period string must end with a single unit character (D/W/M/Y): 5DD");
        REQUIRE_THROWS_WITH(datelib::Period::parse("5DW"),
                            "Period string must end with a single unit character (D/W/M/Y): 5DW");
    }

    SECTION("Invalid numeric format") {
        REQUIRE_THROWS_WITH(datelib::Period::parse("5.5D"),
                            "Period string must end with a single unit character (D/W/M/Y): 5.5D");
    }

    SECTION("Numeric overflow") {
        // Test with a number too large to fit in an int (causes std::stoi to throw)
        REQUIRE_THROWS_WITH(datelib::Period::parse("999999999999999999999D"),
                            "Invalid numeric value in period string: 999999999999999999999D");
    }
}

TEST_CASE("Period construction and use with advance", "[period][advance]") {
    datelib::HolidayCalendar calendar;

    SECTION("Construct Period directly and use with advance") {
        // Create a Period object directly using the constructor
        datelib::Period period(3, datelib::Period::Unit::Months);
        REQUIRE(period.value() == 3);
        REQUIRE(period.unit() == datelib::Period::Unit::Months);

        // Use the Period object with advance
        auto date = year_month_day{year{2024}, month{1}, day{15}};
        auto result =
            datelib::advance(date, period, datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{4}, day{15}});
    }
}

TEST_CASE("advance with days period", "[advance]") {
    datelib::HolidayCalendar calendar;

    SECTION("Advance by 5 business days") {
        // Tuesday, January 2, 2024 + 5 business days
        // Wed 3, Thu 4, Fri 5, Mon 8, Tue 9 = January 9, 2024
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto result =
            datelib::advance(date, "5D", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{1}, day{9}});
    }

    SECTION("Advance by 1 business day") {
        // Tuesday, January 2, 2024 + 1 business day = Wednesday, January 3, 2024
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto result =
            datelib::advance(date, "1D", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{1}, day{3}});
    }

    SECTION("Advance by 0 business days") {
        // Tuesday, January 2, 2024 + 0 business days = Tuesday, January 2, 2024
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto result =
            datelib::advance(date, "0D", datelib::BusinessDayConvention::Unadjusted, calendar);
        REQUIRE(result == date);
    }
}

TEST_CASE("advance with weeks period", "[advance]") {
    datelib::HolidayCalendar calendar;

    SECTION("Advance by 2 weeks with Following convention") {
        // Tuesday, January 2, 2024 + 2W = Tuesday, January 16, 2024 (business day)
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto result =
            datelib::advance(date, "2W", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{1}, day{16}});
    }

    SECTION("Advance by 1 week, landing on weekend") {
        // Monday, January 1, 2024 + 1W = Monday, January 8, 2024 (business day)
        auto date = year_month_day{year{2024}, month{1}, day{1}};
        auto result =
            datelib::advance(date, "1W", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{1}, day{8}});
    }
}

TEST_CASE("advance with months period", "[advance]") {
    datelib::HolidayCalendar calendar;

    SECTION("Advance by 6 months with Following convention") {
        // Tuesday, January 2, 2024 + 6M = Tuesday, July 2, 2024 (business day)
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto result =
            datelib::advance(date, "6M", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{7}, day{2}});
    }

    SECTION("Advance by 1 month with day overflow") {
        // January 31, 2024 + 1M = February 29, 2024 (leap year, Thursday, business day)
        auto date = year_month_day{year{2024}, month{1}, day{31}};
        auto result =
            datelib::advance(date, "1M", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{2}, day{29}});
    }

    SECTION("Advance by 1 month with day overflow in non-leap year") {
        // January 31, 2023 + 1M = February 28, 2023 (Tuesday, business day)
        auto date = year_month_day{year{2023}, month{1}, day{31}};
        auto result =
            datelib::advance(date, "1M", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2023}, month{2}, day{28}});
    }

    SECTION("Advance by 12 months (1 year equivalent)") {
        // January 15, 2024 + 12M = January 15, 2025 (Wednesday, business day)
        auto date = year_month_day{year{2024}, month{1}, day{15}};
        auto result =
            datelib::advance(date, "12M", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2025}, month{1}, day{15}});
    }
}

TEST_CASE("advance with years period", "[advance]") {
    datelib::HolidayCalendar calendar;

    SECTION("Advance by 10 years with Following convention") {
        // Tuesday, January 2, 2024 + 10Y = Monday, January 2, 2034 (business day)
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        auto result =
            datelib::advance(date, "10Y", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2034}, month{1}, day{2}});
    }

    SECTION("Advance by 1 year from leap day") {
        // February 29, 2024 + 1Y = February 28, 2025 (Friday, business day)
        auto date = year_month_day{year{2024}, month{2}, day{29}};
        auto result =
            datelib::advance(date, "1Y", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2025}, month{2}, day{28}});
    }

    SECTION("Advance by 4 years from leap day") {
        // February 29, 2024 + 4Y = February 29, 2028 (Tuesday, business day)
        auto date = year_month_day{year{2024}, month{2}, day{29}};
        auto result =
            datelib::advance(date, "4Y", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2028}, month{2}, day{29}});
    }
}

TEST_CASE("advance with ModifiedFollowing convention", "[advance]") {
    datelib::HolidayCalendar calendar;

    SECTION("Advance to end of month with ModifiedFollowing") {
        // January 26, 2024 (Friday) + 1W = February 2, 2024 (Friday, business day)
        auto date = year_month_day{year{2024}, month{1}, day{26}};
        auto result = datelib::advance(date, "1W",
                                       datelib::BusinessDayConvention::ModifiedFollowing, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{2}, day{2}});
    }

    SECTION("Month end adjustment with ModifiedFollowing") {
        // May 31, 2024 (Friday) + 1M = June 30, 2024 (Sunday)
        // Following would give July 1, ModifiedFollowing gives June 28 (Friday)
        auto date = year_month_day{year{2024}, month{5}, day{31}};
        auto result = datelib::advance(date, "1M",
                                       datelib::BusinessDayConvention::ModifiedFollowing, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{6}, day{28}});
    }
}

TEST_CASE("advance with Period object", "[advance]") {
    datelib::HolidayCalendar calendar;

    SECTION("Using Period object directly") {
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        datelib::Period period(2, datelib::Period::Unit::Weeks);
        auto result =
            datelib::advance(date, period, datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{1}, day{16}});
    }
}

TEST_CASE("advance with holidays", "[advance]") {
    datelib::HolidayCalendar calendar;
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Independence Day", 7, 4));

    SECTION("Advance landing on holiday with Following") {
        // December 26, 2023 + 1W = January 1, 2024 (Monday, New Year's Day holiday)
        // Following adjusts to Tuesday, January 2, 2024
        auto date = year_month_day{year{2023}, month{12}, day{26}};
        auto result =
            datelib::advance(date, "1W", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{1}, day{2}});
    }

    SECTION("Advance landing on holiday with Preceding") {
        // July 1, 2024 (Mon) + 3 business days = July 2 (Tue), July 3 (Wed), July 5 (Fri)
        // (skipping July 4 holiday) = July 5, 2024
        auto date = year_month_day{year{2024}, month{7}, day{1}};
        auto result =
            datelib::advance(date, "3D", datelib::BusinessDayConvention::Preceding, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{7}, day{5}});
    }
}

TEST_CASE("advance with negative periods", "[advance]") {
    datelib::HolidayCalendar calendar;

    SECTION("Advance by -5 business days (go backward)") {
        // Monday, January 8, 2024 - 5 business days
        // Fri 5, Thu 4, Wed 3, Tue 2, Mon 1 = January 1, 2024
        auto date = year_month_day{year{2024}, month{1}, day{8}};
        auto result =
            datelib::advance(date, "-5D", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{1}, day{1}});
    }

    SECTION("Advance by -1 month") {
        // February 15, 2024 - 1M = January 15, 2024 (Monday, business day)
        auto date = year_month_day{year{2024}, month{2}, day{15}};
        auto result =
            datelib::advance(date, "-1M", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{1}, day{15}});
    }

    SECTION("Advance by -13 months (cross year boundary)") {
        // February 15, 2024 - 13M = January 15, 2023 (Sunday, adjusts to Monday Jan 16)
        auto date = year_month_day{year{2024}, month{2}, day{15}};
        auto result =
            datelib::advance(date, "-13M", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2023}, month{1}, day{16}});
    }

    SECTION("Advance by -1 year") {
        // January 15, 2025 - 1Y = January 15, 2024 (Monday, business day)
        auto date = year_month_day{year{2025}, month{1}, day{15}};
        auto result =
            datelib::advance(date, "-1Y", datelib::BusinessDayConvention::Following, calendar);
        REQUIRE(result == year_month_day{year{2024}, month{1}, day{15}});
    }
}

TEST_CASE("advance with invalid input", "[advance][edge_cases]") {
    datelib::HolidayCalendar calendar;

    SECTION("Invalid date throws") {
        auto date = year_month_day{year{2024}, month{2}, day{30}}; // Invalid
        REQUIRE_THROWS_AS(
            datelib::advance(date, "1M", datelib::BusinessDayConvention::Following, calendar),
            datelib::InvalidDateException);

        // Verify the exception message
        REQUIRE_THROWS_WITH(
            datelib::advance(date, "1M", datelib::BusinessDayConvention::Following, calendar),
            "Invalid date provided to advance");
    }

    SECTION("Invalid period string throws") {
        auto date = year_month_day{year{2024}, month{1}, day{2}};
        REQUIRE_THROWS_AS(
            datelib::advance(date, "invalid", datelib::BusinessDayConvention::Following, calendar),
            std::invalid_argument);
    }

    SECTION("Too many business days throws") {
        // Create a calendar where every single day is a holiday
        // and all days of the week are weekends
        datelib::HolidayCalendar all_holidays;

        // Add all valid dates in 2024 as holidays
        for (int month = 1; month <= 12; ++month) {
            // Determine days in month (simplified approach)
            int days_in_month = 31;
            if (month == 2) {
                days_in_month = 29; // 2024 is a leap year
            } else if (month == 4 || month == 6 || month == 9 || month == 11) {
                days_in_month = 30;
            }

            for (int day = 1; day <= days_in_month; ++day) {
                auto holiday_date =
                    year_month_day{year{2024}, std::chrono::month{static_cast<unsigned>(month)},
                                   std::chrono::day{static_cast<unsigned>(day)}};
                all_holidays.addHoliday("Holiday", holiday_date);
            }
        }

        // Make all days of the week weekends
        std::unordered_set<std::chrono::weekday, datelib::WeekdayHash> all_weekends = {
            std::chrono::Sunday,    std::chrono::Monday,   std::chrono::Tuesday,
            std::chrono::Wednesday, std::chrono::Thursday, std::chrono::Friday,
            std::chrono::Saturday};

        auto date = year_month_day{year{2024}, month{1}, day{1}};
        REQUIRE_THROWS_AS(datelib::advance(date, "1D", datelib::BusinessDayConvention::Following,
                                           all_holidays, all_weekends),
                          datelib::BusinessDaySearchException);
        REQUIRE_THROWS_WITH(datelib::advance(date, "1D", datelib::BusinessDayConvention::Following,
                                             all_holidays, all_weekends),
                            "Unable to add business days within reasonable range");
    }
}

TEST_CASE("advance real-world scenarios", "[advance]") {
    datelib::HolidayCalendar usHolidays;
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("Independence Day", 7, 4));
    usHolidays.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

    SECTION("Calculate settlement date (T+2 business days)") {
        // Trade on Friday, January 5, 2024
        // T+2 business days = Monday, January 8 and Tuesday, January 9, 2024
        auto trade_date = year_month_day{year{2024}, month{1}, day{5}};
        auto settlement = datelib::advance(trade_date, "2D",
                                           datelib::BusinessDayConvention::Following, usHolidays);
        REQUIRE(settlement == year_month_day{year{2024}, month{1}, day{9}});
    }

    SECTION("Calculate option expiry (3 months from today)") {
        // Start: January 15, 2024 (Monday)
        // +3M = April 15, 2024 (Monday, business day)
        auto start_date = year_month_day{year{2024}, month{1}, day{15}};
        auto expiry = datelib::advance(
            start_date, "3M", datelib::BusinessDayConvention::ModifiedFollowing, usHolidays);
        REQUIRE(expiry == year_month_day{year{2024}, month{4}, day{15}});
    }

    SECTION("Calculate bond maturity (10 years from issue)") {
        // Issue: January 2, 2024 (Tuesday)
        // +10Y = January 2, 2034 (Monday, business day)
        auto issue_date = year_month_day{year{2024}, month{1}, day{2}};
        auto maturity = datelib::advance(issue_date, "10Y",
                                         datelib::BusinessDayConvention::Following, usHolidays);
        REQUIRE(maturity == year_month_day{year{2034}, month{1}, day{2}});
    }
}

// =========================
// dayCount function tests
// =========================

TEST_CASE("dayCount with ActualActual convention", "[dayCount]") {
    using namespace datelib;

    SECTION("Same date returns zero") {
        auto date = year_month_day{year{2024}, month{1}, day{15}};
        REQUIRE(dayCount(date, date, DayCountConvention::ActualActual) == 0);
    }

    SECTION("One day difference") {
        auto start = year_month_day{year{2024}, month{1}, day{15}};
        auto end = year_month_day{year{2024}, month{1}, day{16}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActual) == 1);
    }

    SECTION("One week difference") {
        auto start = year_month_day{year{2024}, month{1}, day{15}};
        auto end = year_month_day{year{2024}, month{1}, day{22}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActual) == 7);
    }

    SECTION("One month difference (31 days)") {
        auto start = year_month_day{year{2024}, month{1}, day{1}};
        auto end = year_month_day{year{2024}, month{2}, day{1}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActual) == 31);
    }

    SECTION("Leap year February (29 days)") {
        auto start = year_month_day{year{2024}, month{2}, day{1}};
        auto end = year_month_day{year{2024}, month{3}, day{1}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActual) == 29);
    }

    SECTION("Non-leap year February (28 days)") {
        auto start = year_month_day{year{2023}, month{2}, day{1}};
        auto end = year_month_day{year{2023}, month{3}, day{1}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActual) == 28);
    }

    SECTION("One year difference (365 days)") {
        auto start = year_month_day{year{2023}, month{1}, day{1}};
        auto end = year_month_day{year{2024}, month{1}, day{1}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActual) == 365);
    }

    SECTION("Leap year (366 days)") {
        auto start = year_month_day{year{2024}, month{1}, day{1}};
        auto end = year_month_day{year{2025}, month{1}, day{1}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActual) == 366);
    }

    SECTION("Negative difference when end < start") {
        auto start = year_month_day{year{2024}, month{1}, day{22}};
        auto end = year_month_day{year{2024}, month{1}, day{15}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActual) == -7);
    }

    SECTION("Large date range") {
        auto start = year_month_day{year{2020}, month{1}, day{1}};
        auto end = year_month_day{year{2025}, month{1}, day{1}};
        // 2020 (366) + 2021 (365) + 2022 (365) + 2023 (365) + 2024 (366) = 1827
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActual) == 1827);
    }
}

TEST_CASE("dayCount with ActualActualISDA convention", "[dayCount]") {
    using namespace datelib;

    SECTION("Same date returns zero") {
        auto date = year_month_day{year{2024}, month{1}, day{15}};
        REQUIRE(dayCount(date, date, DayCountConvention::ActualActualISDA) == 0);
    }

    SECTION("One day difference") {
        auto start = year_month_day{year{2024}, month{1}, day{15}};
        auto end = year_month_day{year{2024}, month{1}, day{16}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActualISDA) == 1);
    }

    SECTION("Leap year February (29 days)") {
        auto start = year_month_day{year{2024}, month{2}, day{1}};
        auto end = year_month_day{year{2024}, month{3}, day{1}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActualISDA) == 29);
    }

    SECTION("Non-leap year February (28 days)") {
        auto start = year_month_day{year{2023}, month{2}, day{1}};
        auto end = year_month_day{year{2023}, month{3}, day{1}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActualISDA) == 28);
    }

    SECTION("Full leap year (366 days)") {
        auto start = year_month_day{year{2024}, month{1}, day{1}};
        auto end = year_month_day{year{2025}, month{1}, day{1}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActualISDA) == 366);
    }

    SECTION("Full non-leap year (365 days)") {
        auto start = year_month_day{year{2023}, month{1}, day{1}};
        auto end = year_month_day{year{2024}, month{1}, day{1}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActualISDA) == 365);
    }

    SECTION("Negative difference when end < start") {
        auto start = year_month_day{year{2024}, month{1}, day{22}};
        auto end = year_month_day{year{2024}, month{1}, day{15}};
        REQUIRE(dayCount(start, end, DayCountConvention::ActualActualISDA) == -7);
    }

    SECTION("Same as ActualActual for day counting") {
        auto start = year_month_day{year{2020}, month{1}, day{1}};
        auto end = year_month_day{year{2025}, month{1}, day{1}};
        int actual = dayCount(start, end, DayCountConvention::ActualActual);
        int isda = dayCount(start, end, DayCountConvention::ActualActualISDA);
        REQUIRE(actual == isda);
        REQUIRE(isda == 1827);
    }
}

TEST_CASE("dayCount with Thirty360 convention", "[dayCount]") {
    using namespace datelib;

    SECTION("Same date returns zero") {
        auto date = year_month_day{year{2024}, month{1}, day{15}};
        REQUIRE(dayCount(date, date, DayCountConvention::Thirty360) == 0);
    }

    SECTION("One day difference") {
        auto start = year_month_day{year{2024}, month{1}, day{15}};
        auto end = year_month_day{year{2024}, month{1}, day{16}};
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == 1);
    }

    SECTION("One month exactly (30 days)") {
        auto start = year_month_day{year{2024}, month{1}, day{15}};
        auto end = year_month_day{year{2024}, month{2}, day{15}};
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == 30);
    }

    SECTION("One year exactly (360 days)") {
        auto start = year_month_day{year{2024}, month{1}, day{15}};
        auto end = year_month_day{year{2025}, month{1}, day{15}};
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == 360);
    }

    SECTION("Start day 31 is adjusted to 30") {
        auto start = year_month_day{year{2024}, month{1}, day{31}};
        auto end = year_month_day{year{2024}, month{3}, day{15}};
        // 30 days to Feb + 15 days in March = 45 days
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == 45);
    }

    SECTION("End day 31 is adjusted when start is 30 or 31") {
        auto start = year_month_day{year{2024}, month{1}, day{31}};
        auto end = year_month_day{year{2024}, month{3}, day{31}};
        // Start: Jan 31 -> 30, End: Mar 31 -> 30, diff: 2*30 = 60
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == 60);
    }

    SECTION("End day 31 is NOT adjusted when start is < 30") {
        auto start = year_month_day{year{2024}, month{1}, day{29}};
        auto end = year_month_day{year{2024}, month{3}, day{31}};
        // Start: Jan 29, End: Mar 31, diff: (2 * 30) + 2 = 62
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == 62);
    }

    SECTION("February month transitions") {
        auto start = year_month_day{year{2024}, month{1}, day{31}};
        auto end = year_month_day{year{2024}, month{2}, day{29}};
        // Start: Jan 31 -> 30, End: Feb 29, diff: 29 days (30 - 30 + 29 = 29)
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == 29);
    }

    SECTION("Negative difference when end < start") {
        auto start = year_month_day{year{2024}, month{2}, day{15}};
        auto end = year_month_day{year{2024}, month{1}, day{15}};
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == -30);
    }

    SECTION("Multiple years") {
        auto start = year_month_day{year{2020}, month{6}, day{15}};
        auto end = year_month_day{year{2024}, month{6}, day{15}};
        // 4 years * 360 = 1440
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == 1440);
    }

    SECTION("Complex date with all adjustments") {
        auto start = year_month_day{year{2023}, month{5}, day{31}};
        auto end = year_month_day{year{2024}, month{8}, day{31}};
        // Start: May 31 -> 30, End: Aug 31 -> 30
        // Years: 1*360 = 360, Months: 3*30 = 90, Days: 0
        // Total: 360 + 90 = 450
        REQUIRE(dayCount(start, end, DayCountConvention::Thirty360) == 450);
    }
}

TEST_CASE("dayCount with invalid dates", "[dayCount][edge_cases]") {
    using namespace datelib;

    SECTION("Invalid start date throws") {
        auto invalid_start = year_month_day{year{2024}, month{2}, day{30}};
        auto valid_end = year_month_day{year{2024}, month{3}, day{15}};
        REQUIRE_THROWS_AS(dayCount(invalid_start, valid_end, DayCountConvention::ActualActual),
                          std::invalid_argument);
    }

    SECTION("Invalid end date throws") {
        auto valid_start = year_month_day{year{2024}, month{1}, day{15}};
        auto invalid_end = year_month_day{year{2024}, month{2}, day{30}};
        REQUIRE_THROWS_AS(dayCount(valid_start, invalid_end, DayCountConvention::ActualActual),
                          std::invalid_argument);
    }

    SECTION("Both invalid dates throws") {
        auto invalid_start = year_month_day{year{2024}, month{2}, day{30}};
        auto invalid_end = year_month_day{year{2024}, month{13}, day{1}};
        REQUIRE_THROWS_AS(dayCount(invalid_start, invalid_end, DayCountConvention::ActualActual),
                          std::invalid_argument);
    }
}

TEST_CASE("dayCount comparison between conventions", "[dayCount][comparison]") {
    using namespace datelib;

    SECTION("Same month, same day count") {
        auto start = year_month_day{year{2024}, month{1}, day{15}};
        auto end = year_month_day{year{2024}, month{1}, day{25}};
        auto actual = dayCount(start, end, DayCountConvention::ActualActual);
        auto thirty360 = dayCount(start, end, DayCountConvention::Thirty360);
        REQUIRE(actual == 10);
        REQUIRE(thirty360 == 10);
    }

    SECTION("Full month shows difference (31 days vs 30 days)") {
        auto start = year_month_day{year{2024}, month{1}, day{1}};
        auto end = year_month_day{year{2024}, month{2}, day{1}};
        auto actual = dayCount(start, end, DayCountConvention::ActualActual);
        auto thirty360 = dayCount(start, end, DayCountConvention::Thirty360);
        REQUIRE(actual == 31); // January has 31 days
        REQUIRE(thirty360 == 30); // 30/360 assumes 30 days per month
    }

    SECTION("Full year shows difference (365/366 vs 360)") {
        auto start = year_month_day{year{2023}, month{1}, day{1}};
        auto end = year_month_day{year{2024}, month{1}, day{1}};
        auto actual = dayCount(start, end, DayCountConvention::ActualActual);
        auto thirty360 = dayCount(start, end, DayCountConvention::Thirty360);
        REQUIRE(actual == 365); // 2023 is not a leap year
        REQUIRE(thirty360 == 360); // 30/360 assumes 360 days per year
    }
}
