#include "datelib/HolidayCalendar.h"

#include "catch2/catch.hpp"

TEST_CASE("HolidayCalendar construction", "[HolidayCalendar]") {
    REQUIRE_NOTHROW(datelib::HolidayCalendar());
}

TEST_CASE("HolidayCalendar with explicit dates", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar;

    SECTION("Add and check explicit holiday") {
        datelib::Date newYear(2024, 1, 1);
        calendar.addHoliday(newYear);

        REQUIRE(calendar.isHoliday(newYear));
        REQUIRE_FALSE(calendar.isHoliday(datelib::Date(2024, 1, 2)));
    }

    SECTION("Get holidays for a year with explicit dates") {
        calendar.addHoliday(datelib::Date(2024, 1, 1));
        calendar.addHoliday(datelib::Date(2024, 7, 4));
        calendar.addHoliday(datelib::Date(2024, 12, 25));
        calendar.addHoliday(datelib::Date(2025, 1, 1)); // Different year

        auto holidays = calendar.getHolidays(2024);
        REQUIRE(holidays.size() == 3);
        REQUIRE(holidays[0] == datelib::Date(2024, 1, 1));
        REQUIRE(holidays[1] == datelib::Date(2024, 7, 4));
        REQUIRE(holidays[2] == datelib::Date(2024, 12, 25));
    }
}

TEST_CASE("HolidayCalendar with rules", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar;

    SECTION("Add and check rule-based holiday") {
        calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

        REQUIRE(calendar.isHoliday(datelib::Date(2024, 12, 25)));
        REQUIRE(calendar.isHoliday(datelib::Date(2025, 12, 25)));
        REQUIRE_FALSE(calendar.isHoliday(datelib::Date(2024, 12, 24)));
    }

    SECTION("Multiple rules") {
        calendar.addRule(std::make_unique<datelib::FixedDateRule>("New Year's Day", 1, 1));
        calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));
        calendar.addRule(std::make_unique<datelib::NthWeekdayRule>("Thanksgiving", 11, 4, 4));

        auto holidays = calendar.getHolidays(2024);
        REQUIRE(holidays.size() == 3);

        // Verify they're sorted
        REQUIRE(holidays[0] == datelib::Date(2024, 1, 1));
        REQUIRE(holidays[1] == datelib::Date(2024, 11, 28)); // Thanksgiving
        REQUIRE(holidays[2] == datelib::Date(2024, 12, 25));
    }
}

TEST_CASE("HolidayCalendar with mixed explicit and rule-based", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar;

    calendar.addHoliday(datelib::Date(2024, 7, 4));                                  // Explicit
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25)); // Rule

    SECTION("Both types are recognized") {
        REQUIRE(calendar.isHoliday(datelib::Date(2024, 7, 4)));
        REQUIRE(calendar.isHoliday(datelib::Date(2024, 12, 25)));
    }

    SECTION("Get all holidays") {
        auto holidays = calendar.getHolidays(2024);
        REQUIRE(holidays.size() == 2);
        REQUIRE(holidays[0] == datelib::Date(2024, 7, 4));
        REQUIRE(holidays[1] == datelib::Date(2024, 12, 25));
    }
}

TEST_CASE("HolidayCalendar getHolidayNames", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar;

    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Boxing Day", 12, 26));

    SECTION("Get name for holiday") {
        auto names = calendar.getHolidayNames(datelib::Date(2024, 12, 25));
        REQUIRE(names.size() == 1);
        REQUIRE(names[0] == "Christmas");
    }

    SECTION("Get empty list for non-holiday") {
        auto names = calendar.getHolidayNames(datelib::Date(2024, 12, 24));
        REQUIRE(names.empty());
    }
}

TEST_CASE("HolidayCalendar copy operations", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar1;
    calendar1.addHoliday(datelib::Date(2024, 7, 4));
    calendar1.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

    SECTION("Copy constructor") {
        datelib::HolidayCalendar calendar2(calendar1);

        REQUIRE(calendar2.isHoliday(datelib::Date(2024, 7, 4)));
        REQUIRE(calendar2.isHoliday(datelib::Date(2024, 12, 25)));

        auto holidays = calendar2.getHolidays(2024);
        REQUIRE(holidays.size() == 2);
    }

    SECTION("Copy assignment") {
        datelib::HolidayCalendar calendar2;
        calendar2 = calendar1;

        REQUIRE(calendar2.isHoliday(datelib::Date(2024, 7, 4)));
        REQUIRE(calendar2.isHoliday(datelib::Date(2024, 12, 25)));

        auto holidays = calendar2.getHolidays(2024);
        REQUIRE(holidays.size() == 2);
    }
}

TEST_CASE("HolidayCalendar clear", "[HolidayCalendar]") {
    datelib::HolidayCalendar calendar;
    calendar.addHoliday(datelib::Date(2024, 7, 4));
    calendar.addRule(std::make_unique<datelib::FixedDateRule>("Christmas", 12, 25));

    REQUIRE(calendar.isHoliday(datelib::Date(2024, 7, 4)));

    calendar.clear();

    REQUIRE_FALSE(calendar.isHoliday(datelib::Date(2024, 7, 4)));
    REQUIRE_FALSE(calendar.isHoliday(datelib::Date(2024, 12, 25)));

    auto holidays = calendar.getHolidays(2024);
    REQUIRE(holidays.empty());
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
                                                                 3)); // 3rd Monday in January
    usHolidays.addRule(std::make_unique<datelib::NthWeekdayRule>("Presidents' Day", 2, 1,
                                                                 3)); // 3rd Monday in February
    usHolidays.addRule(
        std::make_unique<datelib::NthWeekdayRule>("Memorial Day", 5, 1, -1)); // Last Monday in May
    usHolidays.addRule(
        std::make_unique<datelib::NthWeekdayRule>("Labor Day", 9, 1, 1)); // 1st Monday in September
    usHolidays.addRule(std::make_unique<datelib::NthWeekdayRule>("Columbus Day", 10, 1,
                                                                 2)); // 2nd Monday in October
    usHolidays.addRule(std::make_unique<datelib::NthWeekdayRule>("Thanksgiving", 11, 4,
                                                                 4)); // 4th Thursday in November

    SECTION("Verify 2024 holidays") {
        auto holidays2024 = usHolidays.getHolidays(2024);
        REQUIRE(holidays2024.size() == 10);

        // Spot check a few
        REQUIRE(usHolidays.isHoliday(datelib::Date(2024, 1, 1)));   // New Year
        REQUIRE(usHolidays.isHoliday(datelib::Date(2024, 1, 15)));  // MLK Day (3rd Mon in Jan)
        REQUIRE(usHolidays.isHoliday(datelib::Date(2024, 11, 28))); // Thanksgiving
        REQUIRE(usHolidays.isHoliday(datelib::Date(2024, 12, 25))); // Christmas
    }

    SECTION("Get holiday names") {
        auto names = usHolidays.getHolidayNames(datelib::Date(2024, 11, 28));
        REQUIRE(names.size() == 1);
        REQUIRE(names[0] == "Thanksgiving");
    }
}
