#include "datelib/date.h"

#include "datelib/HolidayCalendar.h"
#include "datelib/business_day_adjuster.h"
#include "datelib/period.h"
#include "datelib/period_advancer.h"

namespace datelib {

bool isBusinessDay(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
                   const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    // Validate the date is well-formed
    if (!date.ok()) {
        throw std::invalid_argument("Invalid date provided to isBusinessDay");
    }

    // Convert to sys_days to get the weekday
    const auto sys_days_date = std::chrono::sys_days{date};
    std::chrono::weekday wd{sys_days_date};

    // Check if the day is not a weekend day
    const bool is_not_weekend = !weekend_days.contains(wd);

    // A business day is not a weekend day and not a holiday
    return is_not_weekend && !calendar.isHoliday(date);
}

std::chrono::year_month_day
adjust(const std::chrono::year_month_day& date, const BusinessDayConvention convention,
       const HolidayCalendar& calendar,
       const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    // Validate the input date
    if (!date.ok()) {
        throw std::invalid_argument("Invalid date provided to adjust");
    }

    // If already a business day, no adjustment needed
    if (isBusinessDay(date, calendar, weekend_days)) {
        return date;
    }

    // Use the strategy pattern to apply the appropriate adjustment
    const BusinessDayAdjuster& adjuster = getAdjuster(convention);
    return adjuster.adjust(date, calendar, weekend_days);
}

std::chrono::year_month_day
advance(const std::chrono::year_month_day& date, const Period& period,
        BusinessDayConvention convention, const HolidayCalendar& calendar,
        const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    // Validate the input date
    if (!date.ok()) {
        throw InvalidDateException("Invalid date provided to advance");
    }

    // Use the strategy pattern to advance by the period
    const PeriodAdvancer& advancer = getAdvancer(period.unit());
    std::chrono::year_month_day result_date = advancer.advance(date, period.value(), calendar, weekend_days);
    
    // For days, business days already account for holidays, so return directly
    if (period.unit() == Period::Unit::Days) {
        return result_date;
    }

    // Apply business day convention to the result for other units
    return adjust(result_date, convention, calendar, weekend_days);
}

std::chrono::year_month_day
advance(const std::chrono::year_month_day& date, std::string_view period,
        BusinessDayConvention convention, const HolidayCalendar& calendar,
        const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    // Parse the period string
    Period parsed_period = Period::parse(period);

    // Call the Period-based overload
    return advance(date, parsed_period, convention, calendar, weekend_days);
}

} // namespace datelib
