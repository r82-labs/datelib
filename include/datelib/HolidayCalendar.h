#pragma once

#include "datelib/HolidayRule.h"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace datelib {

/**
 * @brief A calendar that manages holidays using rule-based generation
 */
class HolidayCalendar {
  public:
    /**
     * @brief Construct an empty holiday calendar
     */
    HolidayCalendar() = default;

    /**
     * @brief Copy constructor
     */
    HolidayCalendar(const HolidayCalendar& other);

    /**
     * @brief Copy assignment operator
     */
    HolidayCalendar& operator=(const HolidayCalendar& other);

    /**
     * @brief Move constructor
     */
    HolidayCalendar(HolidayCalendar&& other) noexcept = default;

    /**
     * @brief Move assignment operator
     */
    HolidayCalendar& operator=(HolidayCalendar&& other) noexcept = default;

    /**
     * @brief Destructor
     */
    ~HolidayCalendar() = default;

    /**
     * @brief Add an explicit holiday date
     * @param name The name of the holiday
     * @param date The date to mark as a holiday
     */
    void addHoliday(const std::string& name, const std::chrono::year_month_day& date);

    /**
     * @brief Add a rule for generating holidays
     * @param rule The holiday rule to add (ownership is transferred)
     */
    void addRule(std::unique_ptr<HolidayRule> rule);

    /**
     * @brief Check if a given date is a holiday
     * @param date The date to check
     * @return true if the date is a holiday, false otherwise
     */
    [[nodiscard]] bool isHoliday(const std::chrono::year_month_day& date) const;

    /**
     * @brief Get all holidays for a given year
     * @param year The year to get holidays for
     * @return A sorted vector of all holiday dates in that year
     */
    [[nodiscard]] std::vector<std::chrono::year_month_day> getHolidays(int year) const;

    /**
     * @brief Get the names of all holidays on a given date
     * @param date The date to check
     * @return A vector of holiday names for that date
     */
    std::vector<std::string> getHolidayNames(const std::chrono::year_month_day& date) const;

  private:
    std::vector<std::unique_ptr<HolidayRule>> rules_;
};

} // namespace datelib
