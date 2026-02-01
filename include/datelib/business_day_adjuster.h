#pragma once

#include "datelib/date.h"
#include "datelib/date_util.h"

#include <chrono>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace datelib {

// Forward declaration
class HolidayCalendar;

/**
 * @brief Abstract base class for business day adjustment strategies
 * 
 * This class defines the interface for different business day adjustment conventions.
 * Each concrete strategy implements the adjust() method according to its specific rules.
 */
class BusinessDayAdjuster {
  public:
    virtual ~BusinessDayAdjuster() = default;

    /**
     * @brief Adjust a date according to the specific business day convention
     * @param date The date to adjust
     * @param calendar The holiday calendar to use for checking business days
     * @param weekend_days The set of weekdays considered as weekend
     * @return The adjusted date according to the convention
     * @throws std::invalid_argument if the input date is invalid
     * @throws BusinessDaySearchException if unable to find a business day within reasonable range
     */
    [[nodiscard]] virtual std::chrono::year_month_day
    adjust(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
           const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const = 0;
};

/**
 * @brief Move forward to the next business day
 */
class FollowingAdjuster : public BusinessDayAdjuster {
  public:
    [[nodiscard]] std::chrono::year_month_day
    adjust(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
           const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const override;
};

/**
 * @brief Move forward to the next business day, unless it crosses into a new month,
 * in which case move backward to the previous business day
 */
class ModifiedFollowingAdjuster : public BusinessDayAdjuster {
  public:
    [[nodiscard]] std::chrono::year_month_day
    adjust(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
           const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const override;
};

/**
 * @brief Move backward to the previous business day
 */
class PrecedingAdjuster : public BusinessDayAdjuster {
  public:
    [[nodiscard]] std::chrono::year_month_day
    adjust(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
           const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const override;
};

/**
 * @brief Move backward to the previous business day, unless it crosses into a new month,
 * in which case move forward to the next business day
 */
class ModifiedPrecedingAdjuster : public BusinessDayAdjuster {
  public:
    [[nodiscard]] std::chrono::year_month_day
    adjust(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
           const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const override;
};

/**
 * @brief Return the date unchanged
 */
class UnadjustedAdjuster : public BusinessDayAdjuster {
  public:
    [[nodiscard]] std::chrono::year_month_day
    adjust(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
           const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const override;
};

/**
 * @brief Factory function to get the appropriate adjuster for a convention
 * @param convention The business day convention
 * @return A reference to the appropriate adjuster (singleton instances)
 */
[[nodiscard]] const BusinessDayAdjuster& getAdjuster(BusinessDayConvention convention);

} // namespace datelib
