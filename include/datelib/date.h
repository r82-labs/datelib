#pragma once

#include "datelib/date_util.h"
#include "datelib/exceptions.h"
#include "datelib/period.h"

#include <chrono>
#include <string>
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
[[nodiscard]] bool
isBusinessDay(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
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
 * @throws BusinessDaySearchException if unable to find a business day within reasonable range
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
[[nodiscard]] std::chrono::year_month_day
adjust(const std::chrono::year_month_day& date, BusinessDayConvention convention,
       const HolidayCalendar& calendar,
       const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days = {
           std::chrono::Saturday, std::chrono::Sunday});

/**
 * @brief Advance a date by a period and adjust according to business day convention
 * @param date The starting date
 * @param period The period to advance (e.g., "2W", "6M", "10Y")
 * @param convention The business day convention to apply after advancing
 * @param calendar The holiday calendar to use for business day adjustment
 * @param weekend_days The set of weekdays considered as weekend (defaults to Saturday and Sunday)
 * @return The advanced and adjusted date
 * @throws std::invalid_argument if the period string is invalid
 * @throws InvalidDateException if the input date is invalid
 * @throws BusinessDaySearchException if unable to find a business day within reasonable range
 *
 * This is the workhorse function for date calculations. It:
 * 1. Parses the period string (e.g., "2W" for 2 weeks, "6M" for 6 months, "10Y" for 10 years)
 * 2. Advances the date by the specified period
 * 3. Adjusts the resulting date according to the business day convention
 *
 * Examples:
 * - advance(2024-01-15, "2W", Following, calendar) -> advances by 2 weeks then adjusts
 * - advance(2024-01-31, "1M", ModifiedFollowing, calendar) -> advances by 1 month then adjusts
 * - advance(2024-12-25, "1Y", Preceding, calendar) -> advances by 1 year then adjusts
 */
[[nodiscard]] std::chrono::year_month_day
advance(const std::chrono::year_month_day& date, std::string_view period,
        BusinessDayConvention convention, const HolidayCalendar& calendar,
        const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days = {
            std::chrono::Saturday, std::chrono::Sunday});

/**
 * @brief Advance a date by a Period object and adjust according to business day convention
 * @param date The starting date
 * @param period The Period object representing the duration to advance
 * @param convention The business day convention to apply after advancing
 * @param calendar The holiday calendar to use for business day adjustment
 * @param weekend_days The set of weekdays considered as weekend (defaults to Saturday and Sunday)
 * @return The advanced and adjusted date
 * @throws std::invalid_argument if the input date is invalid
 * @throws BusinessDaySearchException if unable to find a business day within reasonable range
 */
[[nodiscard]] std::chrono::year_month_day
advance(const std::chrono::year_month_day& date, const Period& period,
        BusinessDayConvention convention, const HolidayCalendar& calendar,
        const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days = {
            std::chrono::Saturday, std::chrono::Sunday});

/**
 * @brief Calculate the difference in calendar days between two dates
 * @param start_date The start date
 * @param end_date The end date
 * @return The number of calendar days from start_date to end_date (positive if end_date is after
 * start_date, negative otherwise)
 * @throws std::invalid_argument if either date is invalid
 *
 * This function calculates the difference in calendar days between two dates.
 * The result is positive if end_date is after start_date, negative if before,
 * and zero if the dates are the same.
 *
 * Examples:
 * - diff(2024-01-01, 2024-01-10) -> 9 (9 days difference)
 * - diff(2024-01-10, 2024-01-01) -> -9 (negative difference)
 * - diff(2024-01-01, 2024-01-01) -> 0 (same date)
 */
[[nodiscard]] int diff(const std::chrono::year_month_day& start_date,
                       const std::chrono::year_month_day& end_date);

/**
 * @brief Calculate the difference in business days between two dates
 * @param start_date The start date
 * @param end_date The end date
 * @param calendar The holiday calendar to use for determining business days
 * @param weekend_days The set of weekdays considered as weekend (defaults to Saturday and Sunday)
 * @return The number of business days from start_date to end_date (positive if end_date is after
 * start_date, negative otherwise)
 * @throws std::invalid_argument if either date is invalid
 *
 * This function calculates the difference in business days (excluding weekends and holidays)
 * between two dates. The result is positive if end_date is after start_date,
 * negative if before, and zero if the dates are the same or within the same business day.
 *
 * Examples:
 * - diff(2024-01-01 (Mon), 2024-01-05 (Fri), calendar) -> 4 business days
 * - diff(2024-01-01 (Mon), 2024-01-08 (Mon), calendar) -> 5 business days (skipping weekend)
 * - diff(2024-01-05 (Fri), 2024-01-01 (Mon), calendar) -> -4 business days
 */
[[nodiscard]] int businessDaysDiff(
    const std::chrono::year_month_day& start_date, const std::chrono::year_month_day& end_date,
    const HolidayCalendar& calendar,
    const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days = {
        std::chrono::Saturday, std::chrono::Sunday});

} // namespace datelib
