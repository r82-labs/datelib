#include "datelib/date.h"

#include "datelib/HolidayCalendar.h"
#include "datelib/period.h"

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
    throw UnhandledEnumException("Unhandled BusinessDayConvention in adjust()");
}

std::chrono::year_month_day
advance(const std::chrono::year_month_day& date, const Period& period,
        BusinessDayConvention convention, const HolidayCalendar& calendar,
        const std::unordered_set<std::chrono::weekday, WeekdayHash>& weekend_days) {
    // Validate the input date
    if (!date.ok()) {
        throw InvalidDateException("Invalid date provided to advance");
    }

    std::chrono::year_month_day result_date = date;

    // Advance the date based on the period unit
    using enum Period::Unit;
    switch (period.unit()) {
    case Days:
        // For days, add business days (skipping weekends and holidays)
        result_date = addBusinessDays(date, period.value(), calendar, weekend_days);
        // Business days already account for holidays, so return directly without further adjustment
        return result_date;

    case Weeks:
        // Add weeks (7 days per week)
        result_date = std::chrono::year_month_day{std::chrono::sys_days{date} +
                                                  std::chrono::days{period.value() * 7}};
        break;

    case Months: {
        // Add months (calendar-aware)
        auto y = date.year();
        auto m = date.month();
        auto d = date.day();

        // Calculate new month/year
        int total_months = static_cast<int>(unsigned{m}) + period.value();

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
        result_date = std::chrono::year_month_day{new_year, new_month, d};
        if (!result_date.ok()) {
            // Day is invalid for this month, use last day of month
            result_date = std::chrono::year_month_day{new_year / new_month / std::chrono::last};
        }
        break;
    }

    case Years: {
        // Add years (calendar-aware)
        auto y = date.year();
        auto m = date.month();
        auto d = date.day();

        auto new_year = y + std::chrono::years{period.value()};

        // Handle leap year edge case (Feb 29 -> Feb 28 in non-leap year)
        result_date = std::chrono::year_month_day{new_year, m, d};
        if (!result_date.ok()) {
            // Day is invalid for this year/month (e.g., Feb 29 in non-leap year)
            result_date = std::chrono::year_month_day{new_year / m / std::chrono::last};
        }
        break;
    }
    }

    // Apply business day convention to the result
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

int dayCount(const std::chrono::year_month_day& start, const std::chrono::year_month_day& end,
             DayCountConvention convention) {
    // Validate both dates
    if (!start.ok()) {
        throw std::invalid_argument("Invalid start date provided to dayCount");
    }
    if (!end.ok()) {
        throw std::invalid_argument("Invalid end date provided to dayCount");
    }

    using enum DayCountConvention;
    switch (convention) {
    case ActualActual: {
        // Actual/Actual: exact difference in calendar days
        auto start_days = std::chrono::sys_days{start};
        auto end_days = std::chrono::sys_days{end};
        auto diff = end_days - start_days;
        return diff.count();
    }

    case Thirty360: {
        // 30/360 US (NASD) convention
        int y1 = static_cast<int>(start.year());
        int m1 = static_cast<unsigned>(start.month());
        int d1 = static_cast<unsigned>(start.day());

        int y2 = static_cast<int>(end.year());
        int m2 = static_cast<unsigned>(end.month());
        int d2 = static_cast<unsigned>(end.day());

        // Apply 30/360 US rules:
        // If d1 is 31, change it to 30
        if (d1 == 31) {
            d1 = 30;
        }

        // If d2 is 31 and d1 is 30 or 31, change d2 to 30
        if (d2 == 31 && (static_cast<unsigned>(start.day()) == 30 ||
                         static_cast<unsigned>(start.day()) == 31)) {
            d2 = 30;
        }

        // Calculate day count: 360*(Y2-Y1) + 30*(M2-M1) + (D2-D1)
        return 360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1);
    }
    }

    // This should never be reached as all enum values are handled above
    throw UnhandledEnumException("Unhandled DayCountConvention in dayCount()");
}

} // namespace datelib
