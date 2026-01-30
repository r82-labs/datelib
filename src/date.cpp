#include "datelib/date.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace datelib {

Date::Date(int year, int month, int day)
    : year_(year), month_(month), day_(day) {
    // Basic validation
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }
    if (day < 1 || day > 31) {
        throw std::invalid_argument("Day must be between 1 and 31");
    }
}

Date::Date() {
    // Create today's date
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    year_ = now->tm_year + 1900;
    month_ = now->tm_mon + 1;
    day_ = now->tm_mday;
}

int Date::getYear() const {
    return year_;
}

int Date::getMonth() const {
    return month_;
}

int Date::getDay() const {
    return day_;
}

int Date::toDaysSinceEpoch() const {
    // Simple algorithm: count days since year 0
    // This is a simplified version for demonstration
    int y = year_;
    int m = month_;
    int d = day_;
    
    // Adjust for month
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    
    // Calculate days (using a simplified Julian day calculation)
    return 365 * y + y / 4 - y / 100 + y / 400 + (153 * m + 2) / 5 + d;
}

Date Date::fromDaysSinceEpoch(int totalDays) {
    // Inverse of toDaysSinceEpoch - simplified algorithm
    int a = totalDays + 32044;
    int b = (4 * a + 3) / 146097;
    int c = a - (146097 * b) / 4;
    int d = (4 * c + 3) / 1461;
    int e = c - (1461 * d) / 4;
    int m = (5 * e + 2) / 153;
    
    int day = e - (153 * m + 2) / 5 + 1;
    int month = m + 3 - 12 * (m / 10);
    int year = 100 * b + d - 4800 + m / 10;
    
    return Date(year, month, day);
}

int Date::daysBetween(const Date& other) const {
    return other.toDaysSinceEpoch() - this->toDaysSinceEpoch();
}

Date Date::addDays(int days) const {
    int currentDays = this->toDaysSinceEpoch();
    return fromDaysSinceEpoch(currentDays + days);
}

std::string Date::toISOString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << year_ << "-"
        << std::setfill('0') << std::setw(2) << month_ << "-"
        << std::setfill('0') << std::setw(2) << day_;
    return oss.str();
}

bool Date::isBefore(const Date& other) const {
    return this->toDaysSinceEpoch() < other.toDaysSinceEpoch();
}

bool Date::isAfter(const Date& other) const {
    return this->toDaysSinceEpoch() > other.toDaysSinceEpoch();
}

bool Date::operator==(const Date& other) const {
    return year_ == other.year_ && month_ == other.month_ && day_ == other.day_;
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

} // namespace datelib
