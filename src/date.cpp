#include "datelib/date.h"

#include "datelib/HolidayCalendar.h"

namespace datelib {

namespace {
// Maximum number of days to search for a business day (one year)
constexpr int MAX_DAYS_TO_SEARCH = 366;

/**
 * @brief Move forward to the next business day
 */
std::chrono::year_month_day
moveToNextBusinessDay(const std::chrono::year_month_day& start, const HolidayCalendar& calendar,
                      const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    auto adjusted = std::chrono::sys_days{start};
    std::chrono::year_month_day adjusted_ymd{adjusted};
    int iterations = 0;

    while (!isBusinessDay(adjusted_ymd, calendar, weekend_days)) {
        if (++iterations > MAX_DAYS_TO_SEARCH) {
            throw BusinessDaySearchException(
                "Unable to find next business day within reasonable range");
        }
        adjusted += std::chrono::days{1};
        adjusted_ymd = std::chrono::year_month_day{adjusted};
    }

    return adjusted_ymd;
}

/**
 * @brief Move backward to the previous business day
 */
std::chrono::year_month_day moveToPreviousBusinessDay(
    const std::chrono::year_month_day& start, const HolidayCalendar& calendar,
    const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    auto adjusted = std::chrono::sys_days{start};
    std::chrono::year_month_day adjusted_ymd{adjusted};
    int iterations = 0;

    while (!isBusinessDay(adjusted_ymd, calendar, weekend_days)) {
        if (++iterations > MAX_DAYS_TO_SEARCH) {
            throw BusinessDaySearchException(
                "Unable to find previous business day within reasonable range");
        }
        adjusted -= std::chrono::days{1};
        adjusted_ymd = std::chrono::year_month_day{adjusted};
    }

    return adjusted_ymd;
}
} // namespace

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
    bool is_not_weekend = !weekend_days.contains(wd);

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

    // If already a business day, no adjustment needed
    if (isBusinessDay(date, calendar, weekend_days)) {
        return date;
    }

    // Apply the convention
    using enum BusinessDayConvention;
    switch (convention) {
    case Following:
        return moveToNextBusinessDay(date, calendar, weekend_days);

    case ModifiedFollowing: {
        auto adjusted = moveToNextBusinessDay(date, calendar, weekend_days);
        // If we crossed into a new month, go backward instead
        if (adjusted.month() != date.month()) {
            adjusted = moveToPreviousBusinessDay(date, calendar, weekend_days);
        }
        return adjusted;
    }

    case Preceding:
        return moveToPreviousBusinessDay(date, calendar, weekend_days);

    case ModifiedPreceding: {
        auto adjusted = moveToPreviousBusinessDay(date, calendar, weekend_days);
        // If we crossed into a different month, go forward instead
        if (adjusted.month() != date.month()) {
            adjusted = moveToNextBusinessDay(date, calendar, weekend_days);
        }
        return adjusted;
    }

    case Unadjusted:
        // Return the date unchanged
        return date;
    }

    // This should never be reached as all enum values are handled above
    // If we reach here, it's a logic error (e.g., uninitialized enum)
    throw std::logic_error("Unhandled BusinessDayConvention in adjust()");
}

} // namespace datelib
