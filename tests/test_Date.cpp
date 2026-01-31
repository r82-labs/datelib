#include "datelib/Date.h"

#include <stdexcept>

#include "catch2/catch.hpp"

TEST_CASE("Date construction", "[Date]") {
    SECTION("Valid dates can be constructed") {
        REQUIRE_NOTHROW(datelib::Date(2024, 1, 1));
        REQUIRE_NOTHROW(datelib::Date(2024, 12, 31));
        REQUIRE_NOTHROW(datelib::Date(2024, 2, 29)); // Leap year
    }

    SECTION("Invalid months are rejected") {
        REQUIRE_THROWS_AS(datelib::Date(2024, 0, 1), std::invalid_argument);
        REQUIRE_THROWS_AS(datelib::Date(2024, 13, 1), std::invalid_argument);
    }

    SECTION("Invalid days are rejected") {
        REQUIRE_THROWS_AS(datelib::Date(2024, 1, 0), std::invalid_argument);
        REQUIRE_THROWS_AS(datelib::Date(2024, 1, 32), std::invalid_argument);
        REQUIRE_THROWS_AS(datelib::Date(2023, 2, 29),
                          std::invalid_argument); // Not a leap year
        REQUIRE_THROWS_AS(datelib::Date(2024, 4, 31),
                          std::invalid_argument); // April has 30 days
    }
}

TEST_CASE("Date getters", "[Date]") {
    datelib::Date date(2024, 3, 15);

    REQUIRE(date.year() == 2024);
    REQUIRE(date.month() == 3);
    REQUIRE(date.day() == 15);
}

TEST_CASE("Date comparison", "[Date]") {
    datelib::Date date1(2024, 1, 1);
    datelib::Date date2(2024, 1, 1);
    datelib::Date date3(2024, 1, 2);
    datelib::Date date4(2024, 2, 1);
    datelib::Date date5(2025, 1, 1);

    SECTION("Equality") {
        REQUIRE(date1 == date2);
        REQUIRE_FALSE(date1 == date3);
    }

    SECTION("Ordering") {
        REQUIRE(date1 < date3);
        REQUIRE(date3 < date4);
        REQUIRE(date4 < date5);
        REQUIRE(date5 > date1);
    }
}

TEST_CASE("Date toString", "[Date]") {
    datelib::Date date1(2024, 1, 1);
    REQUIRE(date1.toString() == "2024-01-01");

    datelib::Date date2(2024, 12, 31);
    REQUIRE(date2.toString() == "2024-12-31");
}

TEST_CASE("Date dayOfWeek", "[Date]") {
    // Known dates and their weekdays
    REQUIRE(datelib::Date(2024, 1, 1).dayOfWeek() == 1);   // Monday, Jan 1, 2024
    REQUIRE(datelib::Date(2024, 1, 7).dayOfWeek() == 0);   // Sunday, Jan 7, 2024
    REQUIRE(datelib::Date(2024, 12, 25).dayOfWeek() == 3); // Wednesday, Dec 25, 2024
    REQUIRE(datelib::Date(2024, 11, 28).dayOfWeek() == 4); // Thursday, Nov 28, 2024 (Thanksgiving)
}

TEST_CASE("Leap year handling", "[Date]") {
    SECTION("Leap years") {
        REQUIRE_NOTHROW(datelib::Date(2024, 2, 29)); // Divisible by 4
        REQUIRE_NOTHROW(datelib::Date(2000, 2, 29)); // Divisible by 400
    }

    SECTION("Non-leap years") {
        REQUIRE_THROWS_AS(datelib::Date(2023, 2, 29), std::invalid_argument);
        REQUIRE_THROWS_AS(datelib::Date(1900, 2, 29),
                          std::invalid_argument); // Divisible by 100 but not
                                                  // 400
    }
}
