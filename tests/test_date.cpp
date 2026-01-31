#define CATCH_CONFIG_MAIN
#include "datelib/date.h"

#include "catch2/catch.hpp"

TEST_CASE("hello_world returns greeting", "[hello]") {
    std::string result = datelib::hello_world();
    REQUIRE(result == "Hello, World!");
}

TEST_CASE("isBusinessDay - weekday with no holidays", "[isBusinessDay]") {
    using namespace std::chrono;
    datelib::HolidayCalendar empty_calendar;

    // Monday, January 6, 2025
    year_month_day monday{year{2025}, month{1}, day{6}};
    REQUIRE(datelib::isBusinessDay(monday, empty_calendar) == true);

    // Tuesday, January 7, 2025
    year_month_day tuesday{year{2025}, month{1}, day{7}};
    REQUIRE(datelib::isBusinessDay(tuesday, empty_calendar) == true);

    // Wednesday, January 8, 2025
    year_month_day wednesday{year{2025}, month{1}, day{8}};
    REQUIRE(datelib::isBusinessDay(wednesday, empty_calendar) == true);

    // Thursday, January 9, 2025
    year_month_day thursday{year{2025}, month{1}, day{9}};
    REQUIRE(datelib::isBusinessDay(thursday, empty_calendar) == true);

    // Friday, January 10, 2025
    year_month_day friday{year{2025}, month{1}, day{10}};
    REQUIRE(datelib::isBusinessDay(friday, empty_calendar) == true);
}

TEST_CASE("isBusinessDay - weekends are not business days", "[isBusinessDay]") {
    using namespace std::chrono;
    datelib::HolidayCalendar empty_calendar;

    // Saturday, January 11, 2025
    year_month_day saturday{year{2025}, month{1}, day{11}};
    REQUIRE(datelib::isBusinessDay(saturday, empty_calendar) == false);

    // Sunday, January 12, 2025
    year_month_day sunday{year{2025}, month{1}, day{12}};
    REQUIRE(datelib::isBusinessDay(sunday, empty_calendar) == false);
}

TEST_CASE("isBusinessDay - holidays on weekdays are not business days", "[isBusinessDay]") {
    using namespace std::chrono;

    // Create a holiday calendar with New Year's Day 2025 (Wednesday)
    year_month_day new_years{year{2025}, month{1}, day{1}};
    datelib::HolidayCalendar holidays{new_years};

    // New Year's Day should not be a business day
    REQUIRE(datelib::isBusinessDay(new_years, holidays) == false);

    // Day after should be a business day
    year_month_day jan_2{year{2025}, month{1}, day{2}};
    REQUIRE(datelib::isBusinessDay(jan_2, holidays) == true);
}

TEST_CASE("isBusinessDay - holidays on weekends are still not business days", "[isBusinessDay]") {
    using namespace std::chrono;

    // Saturday, January 4, 2025
    year_month_day saturday_holiday{year{2025}, month{1}, day{4}};
    datelib::HolidayCalendar holidays{saturday_holiday};

    // Even though it's marked as a holiday, it's still not a business day
    // because it's a weekend
    REQUIRE(datelib::isBusinessDay(saturday_holiday, holidays) == false);
}

TEST_CASE("isBusinessDay - multiple holidays", "[isBusinessDay]") {
    using namespace std::chrono;

    // Create calendar with multiple holidays
    year_month_day new_years{year{2025}, month{1}, day{1}};
    year_month_day mlk_day{year{2025}, month{1}, day{20}};
    year_month_day presidents_day{year{2025}, month{2}, day{17}};

    datelib::HolidayCalendar holidays{new_years, mlk_day, presidents_day};

    // All holidays should not be business days
    REQUIRE(datelib::isBusinessDay(new_years, holidays) == false);
    REQUIRE(datelib::isBusinessDay(mlk_day, holidays) == false);
    REQUIRE(datelib::isBusinessDay(presidents_day, holidays) == false);

    // Regular weekday should be a business day
    year_month_day regular_day{year{2025}, month{1}, day{15}};
    REQUIRE(datelib::isBusinessDay(regular_day, holidays) == true);
}
