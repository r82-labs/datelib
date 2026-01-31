#pragma once

#include <chrono>
#include <string>

namespace datelib {

/**
 * @brief Represents a time period (tenor) like 2W, 6M, 10Y
 *
 * A Period represents a duration specified in days (D), weeks (W), months (M), or years (Y).
 * It can be parsed from strings like "2W" (2 weeks), "6M" (6 months), or "10Y" (10 years).
 */
class Period {
  public:
    /**
     * @brief Time unit for the period
     */
    enum class Unit {
        Days,   ///< Days
        Weeks,  ///< Weeks
        Months, ///< Months
        Years   ///< Years
    };

    /**
     * @brief Construct a period from a value and unit
     * @param value The number of units
     * @param unit The time unit
     */
    Period(int value, Unit unit) : value_(value), unit_(unit) {}

    /**
     * @brief Parse a period from a string (e.g., "2W", "6M", "10Y")
     * @param period_str String representation of the period
     * @return Parsed Period object
     * @throws std::invalid_argument if the string is not a valid period
     *
     * Valid formats:
     * - "nD" or "nd" for n days
     * - "nW" or "nw" for n weeks
     * - "nM" or "nm" for n months
     * - "nY" or "ny" for n years
     */
    [[nodiscard]] static Period parse(const std::string& period_str);

    /**
     * @brief Get the numeric value of the period
     */
    [[nodiscard]] int value() const { return value_; }

    /**
     * @brief Get the unit of the period
     */
    [[nodiscard]] Unit unit() const { return unit_; }

  private:
    int value_;
    Unit unit_;
};

} // namespace datelib
