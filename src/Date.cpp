#include "datelib/Date.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace datelib {

Date::Date(int year, int month, int day) : year_(year), month_(month), day_(day) {
    validate();
}

void Date::validate() const {
    if (month_ < 1 || month_ > 12) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }

    int maxDays = daysInMonth(year_, month_);
    if (day_ < 1 || day_ > maxDays) {
        throw std::invalid_argument("Day is out of range for the given month");
    }
}

bool Date::isLeapYear(int year) {
    // A year is a leap year if:
    // - It's divisible by 4 AND
    // - (It's not divisible by 100 OR it's divisible by 400)
    return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}

int Date::daysInMonth(int year, int month) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month < 1 || month > 12) {
        return 0;
    }

    int result = days[month - 1];
    if (month == 2 && isLeapYear(year)) {
        result = 29;
    }

    return result;
}

int Date::dayOfWeek() const {
    // Zeller's congruence algorithm for calculating day of week
    // Returns: 0=Sunday, 1=Monday, ..., 6=Saturday
    int y = year_;
    int m = month_;
    int d = day_;

    // For Zeller's, January and February are counted as months 13 and 14 of
    // the previous year
    if (m < 3) {
        m += 12;
        y -= 1;
    }

    int c = y / 100;
    int k = y % 100;

    // Zeller's formula
    int h = (d + (13 * (m + 1)) / 5 + k + k / 4 + c / 4 - 2 * c) % 7;

    // Convert Zeller's result (0=Saturday) to standard (0=Sunday)
    int dayOfWeek = (h + 6) % 7;

    return dayOfWeek;
}

std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << year_ << '-' << std::setw(2) << month_ << '-'
        << std::setw(2) << day_;
    return oss.str();
}

} // namespace datelib
