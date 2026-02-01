#include "datelib/business_day_adjuster.h"

#include "datelib/HolidayCalendar.h"
#include "datelib/date.h"

#include <unordered_map>

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

// FollowingAdjuster implementation
std::chrono::year_month_day
FollowingAdjuster::adjust(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
                         const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const {
    return moveToNextBusinessDay(date, calendar, weekend_days);
}

// ModifiedFollowingAdjuster implementation
std::chrono::year_month_day ModifiedFollowingAdjuster::adjust(
    const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
    const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const {
    auto adjusted = moveToNextBusinessDay(date, calendar, weekend_days);
    // If we crossed into a new month, go backward instead
    if (adjusted.month() != date.month()) {
        adjusted = moveToPreviousBusinessDay(date, calendar, weekend_days);
    }
    return adjusted;
}

// PrecedingAdjuster implementation
std::chrono::year_month_day
PrecedingAdjuster::adjust(const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
                         const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const {
    return moveToPreviousBusinessDay(date, calendar, weekend_days);
}

// ModifiedPrecedingAdjuster implementation
std::chrono::year_month_day ModifiedPrecedingAdjuster::adjust(
    const std::chrono::year_month_day& date, const HolidayCalendar& calendar,
    const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const {
    auto adjusted = moveToPreviousBusinessDay(date, calendar, weekend_days);
    // If we crossed into a different month, go forward instead
    if (adjusted.month() != date.month()) {
        adjusted = moveToNextBusinessDay(date, calendar, weekend_days);
    }
    return adjusted;
}

// UnadjustedAdjuster implementation
std::chrono::year_month_day
UnadjustedAdjuster::adjust(const std::chrono::year_month_day& date,
                          const HolidayCalendar& /* calendar */,
                          const std::unordered_set<std::chrono::weekday, WeekdayHash>& /* weekend_days */) const {
    return date;
}

// Factory function implementation using singleton instances
const BusinessDayAdjuster& getAdjuster(BusinessDayConvention convention) {
    using enum BusinessDayConvention;
    
    static const FollowingAdjuster following_adjuster;
    static const ModifiedFollowingAdjuster modified_following_adjuster;
    static const PrecedingAdjuster preceding_adjuster;
    static const ModifiedPrecedingAdjuster modified_preceding_adjuster;
    static const UnadjustedAdjuster unadjusted_adjuster;
    
    static const std::unordered_map<BusinessDayConvention, const BusinessDayAdjuster*> adjuster_map = {
        {Following, &following_adjuster},
        {ModifiedFollowing, &modified_following_adjuster},
        {Preceding, &preceding_adjuster},
        {ModifiedPreceding, &modified_preceding_adjuster},
        {Unadjusted, &unadjusted_adjuster}
    };
    
    auto it = adjuster_map.find(convention);
    if (it != adjuster_map.end()) {
        return *it->second;
    }
    
    // This should never be reached as all enum values are handled above
    throw UnhandledEnumException("Unhandled BusinessDayConvention in getAdjuster()");
}

} // namespace datelib
