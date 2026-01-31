#include "datelib/date.h"

#include "datelib/HolidayCalendar.h"

namespace datelib {

bool isBusinessDay(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
                   const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    // Validate the date is well-formed
    if (!date.ok()) {
        throw std::invalid_argument("Invalid date provided to isBusinessDay");
    }

    // Convert to sys_days to get the weekday
    auto sys_days_date = std::chrono::sys_days{date};
    std::chrono::weekday wd{sys_days_date};

    // Check if the day is not a weekend day
    bool is_not_weekend = (weekend_days.find(wd) == weekend_days.end());

    // A business day is not a weekend day and not a holiday
    return is_not_weekend && !calendar.isHoliday(date);
}

std::chrono::year_month_day
adjust(const std::chrono::year_month_day& date, BusinessDayConvention convention,
       const HolidayCalendar& calendar,
       const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    // Validate the input date
    if (!date.ok()) {
        throw std::invalid_argument("Invalid date provided to adjust");
    }

    // If unadjusted, return the date as-is
    if (convention == BusinessDayConvention::Unadjusted) {
        return date;
    }

    // If already a business day, no adjustment needed
    if (isBusinessDay(date, calendar, weekend_days)) {
        return date;
    }

    // Convert to sys_days for date arithmetic
    auto sys_days_date = std::chrono::sys_days{date};
    std::chrono::month original_month = date.month();

    // Apply the convention
    switch (convention) {
    case BusinessDayConvention::Following: {
        // Move forward until we find a business day
        auto adjusted = sys_days_date;
        std::chrono::year_month_day adjusted_ymd{adjusted};
        while (!isBusinessDay(adjusted_ymd, calendar, weekend_days)) {
            adjusted += std::chrono::days{1};
            adjusted_ymd = std::chrono::year_month_day{adjusted};
        }
        return adjusted_ymd;
    }

    case BusinessDayConvention::ModifiedFollowing: {
        // Move forward until we find a business day
        auto adjusted = sys_days_date;
        std::chrono::year_month_day adjusted_ymd{adjusted};
        while (!isBusinessDay(adjusted_ymd, calendar, weekend_days)) {
            adjusted += std::chrono::days{1};
            adjusted_ymd = std::chrono::year_month_day{adjusted};
        }

        // If we crossed into a new month, go backward instead
        if (adjusted_ymd.month() != original_month) {
            adjusted = sys_days_date;
            adjusted_ymd = std::chrono::year_month_day{adjusted};
            while (!isBusinessDay(adjusted_ymd, calendar, weekend_days)) {
                adjusted -= std::chrono::days{1};
                adjusted_ymd = std::chrono::year_month_day{adjusted};
            }
        }
        return adjusted_ymd;
    }

    case BusinessDayConvention::Preceding: {
        // Move backward until we find a business day
        auto adjusted = sys_days_date;
        std::chrono::year_month_day adjusted_ymd{adjusted};
        while (!isBusinessDay(adjusted_ymd, calendar, weekend_days)) {
            adjusted -= std::chrono::days{1};
            adjusted_ymd = std::chrono::year_month_day{adjusted};
        }
        return adjusted_ymd;
    }

    case BusinessDayConvention::ModifiedPreceding: {
        // Move backward until we find a business day
        auto adjusted = sys_days_date;
        std::chrono::year_month_day adjusted_ymd{adjusted};
        while (!isBusinessDay(adjusted_ymd, calendar, weekend_days)) {
            adjusted -= std::chrono::days{1};
            adjusted_ymd = std::chrono::year_month_day{adjusted};
        }

        // If we crossed into a different month, go forward instead
        if (adjusted_ymd.month() != original_month) {
            adjusted = sys_days_date;
            adjusted_ymd = std::chrono::year_month_day{adjusted};
            while (!isBusinessDay(adjusted_ymd, calendar, weekend_days)) {
                adjusted += std::chrono::days{1};
                adjusted_ymd = std::chrono::year_month_day{adjusted};
            }
        }
        return adjusted_ymd;
    }

    case BusinessDayConvention::Unadjusted:
        // Already handled above
        return date;
    }

    // Should never reach here
    return date;
}

} // namespace datelib
