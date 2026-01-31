#pragma once

#include <compare>
#include <cstdint>
#include <string>

namespace datelib {

/**
 * @brief Represents a calendar date with year, month, and day
 */
class Date {
  public:
    /**
     * @brief Construct a Date
     * @param year The year (e.g., 2024)
     * @param month The month (1-12)
     * @param day The day of month (1-31)
     */
    Date(int year, int month, int day);

    /**
     * @brief Get the year
     * @return The year value
     */
    int year() const { return year_; }

    /**
     * @brief Get the month
     * @return The month value (1-12)
     */
    int month() const { return month_; }

    /**
     * @brief Get the day
     * @return The day value (1-31)
     */
    int day() const { return day_; }

    /**
     * @brief Get the day of week (0=Sunday, 6=Saturday)
     * @return Day of week as integer
     */
    int dayOfWeek() const;

    /**
     * @brief Convert date to string in ISO 8601 format (YYYY-MM-DD)
     * @return String representation of the date
     */
    std::string toString() const;

    /**
     * @brief Three-way comparison operator
     */
    auto operator<=>(const Date& other) const = default;

    /**
     * @brief Equality comparison operator
     */
    bool operator==(const Date& other) const = default;

  private:
    int year_;
    int month_;
    int day_;

    /**
     * @brief Validate the date components
     * @throws std::invalid_argument if date is invalid
     */
    void validate() const;

    /**
     * @brief Check if a year is a leap year
     * @param year The year to check
     * @return true if leap year, false otherwise
     */
    static bool isLeapYear(int year);

    /**
     * @brief Get the number of days in a month
     * @param year The year
     * @param month The month (1-12)
     * @return Number of days in the month
     */
    static int daysInMonth(int year, int month);
};

} // namespace datelib
