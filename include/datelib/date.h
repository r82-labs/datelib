#pragma once

#include <chrono>
#include <set>
#include <string>

namespace datelib {

/**
 * @brief Returns a hello world message
 * @return A greeting string
 */
std::string hello_world();

/**
 * @brief Type alias for a holiday calendar
 *
 * A holiday calendar is represented as a set of dates that are considered
 * holidays.
 */
using HolidayCalendar = std::set<std::chrono::year_month_day>;

/**
 * @brief Determines if a given date is a business day
 *
 * A business day is defined as a day that is not:
 * - A weekend (Saturday or Sunday)
 * - A holiday as defined in the provided holiday calendar
 *
 * @param date The date to check
 * @param holidays A set of dates that are considered holidays
 * @return true if the date is a business day, false otherwise
 */
bool isBusinessDay(const std::chrono::year_month_day& date, const HolidayCalendar& holidays);

} // namespace datelib
