#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "datelib/date.h"

using namespace datelib;

TEST_CASE("Date construction", "[date]") {
    SECTION("Can create a date with year, month, day") {
        Date date(2024, 1, 15);
        REQUIRE(date.getYear() == 2024);
        REQUIRE(date.getMonth() == 1);
        REQUIRE(date.getDay() == 15);
    }
    
    SECTION("Invalid month throws exception") {
        REQUIRE_THROWS(Date(2024, 0, 15));
        REQUIRE_THROWS(Date(2024, 13, 15));
    }
    
    SECTION("Invalid day throws exception") {
        REQUIRE_THROWS(Date(2024, 1, 0));
        REQUIRE_THROWS(Date(2024, 1, 32));
    }
}

TEST_CASE("Date comparison", "[date]") {
    Date date1(2024, 1, 15);
    Date date2(2024, 1, 20);
    Date date3(2024, 1, 15);
    
    SECTION("Equality works correctly") {
        REQUIRE(date1 == date3);
        REQUIRE(date1 != date2);
    }
    
    SECTION("isBefore works correctly") {
        REQUIRE(date1.isBefore(date2));
        REQUIRE_FALSE(date2.isBefore(date1));
        REQUIRE_FALSE(date1.isBefore(date3));
    }
    
    SECTION("isAfter works correctly") {
        REQUIRE(date2.isAfter(date1));
        REQUIRE_FALSE(date1.isAfter(date2));
        REQUIRE_FALSE(date1.isAfter(date3));
    }
}

TEST_CASE("Date arithmetic", "[date]") {
    Date date1(2024, 1, 15);
    Date date2(2024, 1, 20);
    
    SECTION("daysBetween works correctly") {
        int days = date1.daysBetween(date2);
        REQUIRE(days == 5);
        REQUIRE(date2.daysBetween(date1) == -5);
    }
    
    SECTION("Same dates have zero days between") {
        Date date3(2024, 1, 15);
        REQUIRE(date1.daysBetween(date3) == 0);
    }
}

TEST_CASE("Date formatting", "[date]") {
    SECTION("toISOString formats correctly") {
        Date date(2024, 1, 15);
        REQUIRE(date.toISOString() == "2024-01-15");
        
        Date date2(2024, 12, 5);
        REQUIRE(date2.toISOString() == "2024-12-05");
    }
}
