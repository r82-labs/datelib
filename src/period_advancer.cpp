#include "datelib/period_advancer.h"

#include "datelib/HolidayCalendar.h"
#include "datelib/date.h"

#include <cmath>
#include <unordered_map>

namespace datelib {

namespace {
// Maximum number of days to search for a business day (one year)
constexpr int MAX_DAYS_TO_SEARCH = 366;

/**
 * @brief Add a number of business days to a date
 * @param start The starting date
 * @param num_business_days Number of business days to add (can be negative)
 * @param calendar The holiday calendar
 * @param weekend_days The set of weekdays considered as weekend
 * @return The date after adding the specified number of business days
 */
std::chrono::year_month_day
addBusinessDays(const std::chrono::year_month_day& start, int num_business_days,
                const HolidayCalendar& calendar,
                const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    if (num_business_days == 0) {
        return start;
    }

    auto current = std::chrono::sys_days{start};
    std::chrono::year_month_day current_ymd{current};
    int days_added = 0;
    int direction = (num_business_days > 0) ? 1 : -1;
    int target = std::abs(num_business_days);
    int iterations = 0;

    while (days_added < target) {
        if (++iterations > MAX_DAYS_TO_SEARCH) {
            throw BusinessDaySearchException("Unable to add business days within reasonable range");
        }

        // Move one calendar day in the appropriate direction
        current += std::chrono::days{direction};
        current_ymd = std::chrono::year_month_day{current};

        // Check if this is a business day
        if (isBusinessDay(current_ymd, calendar, weekend_days)) {
            days_added++;
        }
    }

    return current_ymd;
}
} // namespace

// DaysAdvancer implementation
std::chrono::year_month_day
DaysAdvancer::advance(const std::chrono::year_month_day& date, int value,
                     const HolidayCalendar& calendar,
                     const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) const {
    // For days, add business days (skipping weekends and holidays)
    return addBusinessDays(date, value, calendar, weekend_days);
}

// WeeksAdvancer implementation
std::chrono::year_month_day
WeeksAdvancer::advance(const std::chrono::year_month_day& date, int value,
                      const HolidayCalendar& /* calendar */,
                      const std::unordered_set<std::chrono::weekday, WeekdayHash>& /* weekend_days */) const {
    // Add weeks (7 days per week)
    return std::chrono::year_month_day{std::chrono::sys_days{date} +
                                       std::chrono::days{value * 7}};
}

// MonthsAdvancer implementation
std::chrono::year_month_day
MonthsAdvancer::advance(const std::chrono::year_month_day& date, int value,
                       const HolidayCalendar& /* calendar */,
                       const std::unordered_set<std::chrono::weekday, WeekdayHash>& /* weekend_days */) const {
    // Add months (calendar-aware)
    auto y = date.year();
    auto m = date.month();
    auto d = date.day();

    // Calculate new month/year
    int total_months = static_cast<int>(unsigned{m}) + value;

    // Handle month overflow/underflow
    int year_offset = 0;
    while (total_months > 12) {
        total_months -= 12;
        year_offset++;
    }
    while (total_months < 1) {
        total_months += 12;
        year_offset--;
    }

    auto new_year = y + std::chrono::years{year_offset};
    auto new_month = std::chrono::month{static_cast<unsigned>(total_months)};

    // Handle day overflow (e.g., Jan 31 + 1M = Feb 28/29, not Feb 31)
    auto result_date = std::chrono::year_month_day{new_year, new_month, d};
    if (!result_date.ok()) {
        // Day is invalid for this month, use last day of month
        result_date = std::chrono::year_month_day{new_year / new_month / std::chrono::last};
    }

    return result_date;
}

// YearsAdvancer implementation
std::chrono::year_month_day
YearsAdvancer::advance(const std::chrono::year_month_day& date, int value,
                      const HolidayCalendar& /* calendar */,
                      const std::unordered_set<std::chrono::weekday, WeekdayHash>& /* weekend_days */) const {
    // Add years (calendar-aware)
    auto y = date.year();
    auto m = date.month();
    auto d = date.day();

    auto new_year = y + std::chrono::years{value};

    // Handle leap year edge case (Feb 29 -> Feb 28 in non-leap year)
    auto result_date = std::chrono::year_month_day{new_year, m, d};
    if (!result_date.ok()) {
        // Day is invalid for this year/month (e.g., Feb 29 in non-leap year)
        result_date = std::chrono::year_month_day{new_year / m / std::chrono::last};
    }

    return result_date;
}

// Factory function implementation using singleton instances
const PeriodAdvancer& getAdvancer(Period::Unit unit) {
    using enum Period::Unit;
    
    static const DaysAdvancer days_advancer;
    static const WeeksAdvancer weeks_advancer;
    static const MonthsAdvancer months_advancer;
    static const YearsAdvancer years_advancer;
    
    static const std::unordered_map<Period::Unit, const PeriodAdvancer*> advancer_map = {
        {Days, &days_advancer},
        {Weeks, &weeks_advancer},
        {Months, &months_advancer},
        {Years, &years_advancer}
    };
    
    auto it = advancer_map.find(unit);
    if (it != advancer_map.end()) {
        return *it->second;
    }
    
    // This should never be reached as all enum values are handled above
    throw std::logic_error("Unhandled Period::Unit in getAdvancer()");
}

} // namespace datelib
