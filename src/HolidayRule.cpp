#include "datelib/HolidayRule.h"

#include <stdexcept>

namespace datelib {

// FixedDateRule implementation
FixedDateRule::FixedDateRule(std::string name, int month, int day)
    : name_(std::move(name)), month_(month), day_(day) {
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }
    if (day < 1 || day > 31) {
        throw std::invalid_argument("Day must be between 1 and 31");
    }
}

Date FixedDateRule::calculateDate(int year) const {
    return Date(year, month_, day_);
}

std::unique_ptr<HolidayRule> FixedDateRule::clone() const {
    return std::make_unique<FixedDateRule>(name_, month_, day_);
}

// NthWeekdayRule implementation
NthWeekdayRule::NthWeekdayRule(std::string name, int month, int weekday, int occurrence)
    : name_(std::move(name)), month_(month), weekday_(weekday), occurrence_(occurrence) {
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }
    if (weekday < 0 || weekday > 6) {
        throw std::invalid_argument("Weekday must be between 0 and 6");
    }
    if (occurrence == 0 || occurrence < -1 || occurrence > 5) {
        throw std::invalid_argument("Occurrence must be 1-5 or -1 for last occurrence");
    }
}

Date NthWeekdayRule::calculateDate(int year) const {
    // Find the first day of the month
    Date firstDay(year, month_, 1);
    int firstWeekday = firstDay.dayOfWeek();

    // Calculate which day of month the first occurrence of our target weekday
    // is
    int daysToAdd = (weekday_ - firstWeekday + 7) % 7;
    int firstOccurrenceDay = 1 + daysToAdd;

    if (occurrence_ > 0) {
        // Nth occurrence from the start
        int targetDay = firstOccurrenceDay + (occurrence_ - 1) * 7;

        // Validate that this day exists in the month
        int daysInMonth = [year, month = month_]() {
            static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            int result = days[month - 1];
            if (month == 2 && ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))) {
                result = 29;
            }
            return result;
        }();

        if (targetDay > daysInMonth) {
            throw std::runtime_error("Requested occurrence does not exist in this month");
        }

        return Date(year, month_, targetDay);
    } else {
        // Last occurrence (occurrence_ == -1)
        // Start from the last day of the month and work backwards
        int daysInMonth = [year, month = month_]() {
            static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            int result = days[month - 1];
            if (month == 2 && ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))) {
                result = 29;
            }
            return result;
        }();

        Date lastDay(year, month_, daysInMonth);
        int lastWeekday = lastDay.dayOfWeek();

        // Calculate how many days to go back to find the last occurrence
        int daysToSubtract = (lastWeekday - weekday_ + 7) % 7;
        int lastOccurrenceDay = daysInMonth - daysToSubtract;

        return Date(year, month_, lastOccurrenceDay);
    }
}

std::unique_ptr<HolidayRule> NthWeekdayRule::clone() const {
    return std::make_unique<NthWeekdayRule>(name_, month_, weekday_, occurrence_);
}

} // namespace datelib
