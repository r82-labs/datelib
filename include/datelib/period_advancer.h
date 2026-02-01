#pragma once

#include "datelib/date_util.h"
#include "datelib/period.h"

#include <chrono>
#include <memory>
#include <unordered_set>

namespace datelib {

// Forward declaration
class HolidayCalendar;

/**
 * @brief Abstract base class for period advancement strategies
 * 
 * This class defines the interface for different period unit advancement behaviors.
 * Each concrete strategy implements how to advance a date by its specific unit type.
 */
class PeriodAdvancer {
  public:
    virtual ~PeriodAdvancer() = default;

    /**
     * @brief Advance a date by the specified period value
     * @param date The starting date
     * @param value The number of units to advance (can be negative)
     * @param calendar The holiday calendar to use for business day calculations
     * @param weekend_days The set of weekdays considered as weekend
     * @return The advanced date (before business day adjustment)
     * @throws std::invalid_argument if the input date is invalid
     * @throws BusinessDaySearchException if unable to find a business day within reasonable range
     */
    [[nodiscard]] virtual std::chrono::year_month_day
    advance(const std::chrono::year_month_day& date, int value,
            const HolidayCalendar& calendar,
            const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const = 0;
};

/**
 * @brief Advance by business days (skipping weekends and holidays)
 */
class DaysAdvancer : public PeriodAdvancer {
  public:
    [[nodiscard]] std::chrono::year_month_day
    advance(const std::chrono::year_month_day& date, int value,
            const HolidayCalendar& calendar,
            const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const override;
};

/**
 * @brief Advance by weeks (7 calendar days per week)
 */
class WeeksAdvancer : public PeriodAdvancer {
  public:
    [[nodiscard]] std::chrono::year_month_day
    advance(const std::chrono::year_month_day& date, int value,
            const HolidayCalendar& calendar,
            const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const override;
};

/**
 * @brief Advance by calendar months
 */
class MonthsAdvancer : public PeriodAdvancer {
  public:
    [[nodiscard]] std::chrono::year_month_day
    advance(const std::chrono::year_month_day& date, int value,
            const HolidayCalendar& calendar,
            const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const override;
};

/**
 * @brief Advance by calendar years
 */
class YearsAdvancer : public PeriodAdvancer {
  public:
    [[nodiscard]] std::chrono::year_month_day
    advance(const std::chrono::year_month_day& date, int value,
            const HolidayCalendar& calendar,
            const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const override;
};

/**
 * @brief Factory function to get the appropriate advancer for a period unit
 * @param unit The period unit
 * @return A reference to the appropriate advancer (singleton instances)
 */
[[nodiscard]] const PeriodAdvancer& getAdvancer(Period::Unit unit);

} // namespace datelib
