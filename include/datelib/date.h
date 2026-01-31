#pragma once

#include "datelib/date_util.h"

#include <chrono>
#include <unordered_set>

namespace datelib {

// Forward declaration
class HolidayCalendar;

/**
 * @brief Business day adjustment conventions for date rolling
 */
enum class BusinessDayConvention {
    /**
     * @brief Move forward to the next business day
     */
    Following,

    /**
     * @brief Move forward to the next business day, unless it crosses into a new month,
     * in which case move backward to the previous business day
     */
    ModifiedFollowing,

    /**
     * @brief Move backward to the previous business day
     */
    Preceding,

    /**
     * @brief Move backward to the previous business day, unless it crosses into a new month,
     * in which case move forward to the next business day
     */
    ModifiedPreceding,

    /**
     * @brief Do not adjust the date (return as-is)
     */
    Unadjusted
};

/**
 * @brief Check if a given date is a business day
 * @param date The date to check
 * @param calendar The holiday calendar to use for checking holidays
 * @param weekend_days The set of weekdays considered as weekend (defaults to Saturday and Sunday)
 * @return true if the date is not a weekend day and not a holiday, false otherwise
 * @throws std::invalid_argument if the date is invalid (e.g., February 30th)
 */
bool isBusinessDay(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
                   const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days = {
                       std::chrono::Saturday, std::chrono::Sunday});

/**
 * @brief Adjust a date according to a business day convention
 * @param date The date to adjust
 * @param convention The business day convention to apply
 * @param calendar The holiday calendar to use for checking business days
 * @param weekend_days The set of weekdays considered as weekend (defaults to Saturday and Sunday)
 * @return The adjusted date according to the specified convention
 * @throws std::invalid_argument if the input date is invalid
 *
 * This function adjusts non-business days according to market conventions:
 * - Following: Moves to the next business day
 * - ModifiedFollowing: Moves to the next business day unless it crosses into a new month;
 *   if it does, moves to the previous business day
 * - Preceding: Moves to the previous business day
 * - ModifiedPreceding: Moves to the previous business day unless it crosses into a new month;
 *   if it does, moves to the next business day
 * - Unadjusted: Returns the date unchanged
 */
std::chrono::year_month_day
adjust(const std::chrono::year_month_day& date, BusinessDayConvention convention,
       const HolidayCalendar& calendar,
       const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days = {
           std::chrono::Saturday, std::chrono::Sunday});

} // namespace datelib
