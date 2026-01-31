#pragma once

#include "datelib/Date.h"
#include "datelib/HolidayRule.h"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace datelib {

/**
 * @brief A calendar that manages holidays using explicit dates and/or
 * rule-based generation
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
     * @brief Add an explicit holiday date
     * @param date The date to mark as a holiday
     */
    void addHoliday(const Date& date);

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
    bool isHoliday(const Date& date) const;

    /**
     * @brief Get all holidays for a given year
     * @param year The year to get holidays for
     * @return A sorted vector of all holiday dates in that year
     */
    std::vector<Date> getHolidays(int year) const;

    /**
     * @brief Get the names of all holidays on a given date
     * @param date The date to check
     * @return A vector of holiday names for that date
     */
    std::vector<std::string> getHolidayNames(const Date& date) const;

    /**
     * @brief Clear all holidays and rules
     */
    void clear();

  private:
    std::set<Date> explicitHolidays_;
    std::vector<std::unique_ptr<HolidayRule>> rules_;
};

} // namespace datelib
