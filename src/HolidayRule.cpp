#include "datelib/HolidayRule.h"

#include "datelib/exceptions.h"

#include <stdexcept>
#include <utility>

namespace datelib {

using std::chrono::day;
using std::chrono::days;
using std::chrono::month;
using std::chrono::month_day_last;
using std::chrono::sys_days;
using std::chrono::weekday;
using std::chrono::year;
using std::chrono::year_month_day;
using std::chrono::year_month_day_last;

namespace {
// Month and day validation constants
constexpr unsigned MIN_MONTH = 1;
constexpr unsigned MAX_MONTH = 12;
constexpr unsigned MIN_DAY = 1;
constexpr unsigned MAX_DAY = 31;
constexpr unsigned MAX_WEEKDAY = 6;
constexpr unsigned DAYS_PER_WEEK = 7;
} // namespace

// ExplicitDateRule implementation
ExplicitDateRule::ExplicitDateRule(std::string name, const year_month_day date)
    : name_(std::move(name)), date_(date) {
    if (!date_.ok()) {
        throw std::invalid_argument("Invalid date");
    }
}

bool ExplicitDateRule::appliesTo(const int year) const {
    return static_cast<int>(date_.year()) == year;
}

year_month_day ExplicitDateRule::calculateDate(const int year) const {
    // Only return the date if it matches the requested year
    if (static_cast<int>(date_.year()) == year) {
        return date_;
    }
    throw DateNotInYearException("Explicit date does not exist in this year");
}

std::unique_ptr<HolidayRule> ExplicitDateRule::clone() const {
    return std::make_unique<ExplicitDateRule>(name_, date_);
}

// FixedDateRule implementation
FixedDateRule::FixedDateRule(std::string name, const unsigned month, const unsigned day)
    : name_(std::move(name)), month_{month}, day_{day} {
    if (month < MIN_MONTH || month > MAX_MONTH) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }
    if (day < MIN_DAY || day > MAX_DAY) {
        throw std::invalid_argument("Day must be between 1 and 31");
    }
}

bool FixedDateRule::appliesTo(const int year) const {
    const year_month_day ymd{std::chrono::year{year}, month_, day_};
    return ymd.ok();
}

year_month_day FixedDateRule::calculateDate(const int year) const {
    const year_month_day ymd{std::chrono::year{year}, month_, day_};
    if (!ymd.ok()) {
        throw InvalidDateException("Invalid date for this year");
    }
    return ymd;
}

std::unique_ptr<HolidayRule> FixedDateRule::clone() const {
    return std::make_unique<FixedDateRule>(name_, static_cast<unsigned>(month_),
                                           static_cast<unsigned>(day_));
}

// NthWeekdayRule implementation
NthWeekdayRule::NthWeekdayRule(std::string name, const unsigned month, const unsigned weekday,
                               const Occurrence occurrence)
    : name_(std::move(name)), month_{month}, weekday_{weekday}, occurrence_(occurrence) {
    if (month < MIN_MONTH || month > MAX_MONTH) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }
    if (weekday > MAX_WEEKDAY) {
        throw std::invalid_argument("Weekday must be between 0 and 6");
    }
    if (const int occ_val = std::to_underlying(occurrence); occ_val == 0 || occ_val < -1 || occ_val > 5) {
        throw std::invalid_argument("Occurrence must be First through Fifth or Last");
    }
}

bool NthWeekdayRule::appliesTo(const int year) const {
    // For the Last occurrence, it always applies
    const int occ_val = std::to_underlying(occurrence_);
    if (occ_val < 0) {
        return true;
    }

    // For the Nth occurrence, check if it exists in the month
    const year_month_day first_of_month{std::chrono::year{year}, month_, day{1}};
    const sys_days first_sd{first_of_month};
    const weekday first_weekday{first_sd};

    const unsigned weekday_encoding = weekday_.c_encoding();
    const unsigned first_weekday_encoding = first_weekday.c_encoding();
    const unsigned days_until_target =
        (weekday_encoding + DAYS_PER_WEEK - first_weekday_encoding) % DAYS_PER_WEEK;
    const auto occ_val_u = static_cast<unsigned>(occ_val);
    const auto offset_days =
        static_cast<days::rep>(days_until_target + (occ_val_u - 1U) * DAYS_PER_WEEK);
    const sys_days target_sd = first_sd + days{offset_days};
    const year_month_day result{target_sd};

    return result.month() == month_;
}

year_month_day NthWeekdayRule::calculateDate(const int year) const {
    // Get the first day of the month
    const year_month_day first_of_month{std::chrono::year{year}, month_, day{1}};

    // Convert to sys_days to work with weekdays
    const sys_days first_sd{first_of_month};
    const weekday first_weekday{first_sd};

    if (const int occ_val = std::to_underlying(occurrence_); occ_val > 0) {
        // Find the Nth occurrence of the target weekday
        // Calculate days to add to reach first occurrence of target weekday
        const unsigned weekday_encoding = weekday_.c_encoding();
        const unsigned first_weekday_encoding = first_weekday.c_encoding();
        const unsigned days_until_target =
            (weekday_encoding + DAYS_PER_WEEK - first_weekday_encoding) % DAYS_PER_WEEK;

        // Add weeks to get to the Nth occurrence
        const auto occ_val_u = static_cast<unsigned>(occ_val);
        const auto offset_days =
            static_cast<days::rep>(days_until_target + (occ_val_u - 1U) * DAYS_PER_WEEK);
        const sys_days target_sd = first_sd + days{offset_days};
        const year_month_day result{target_sd};

        // Verify we're still in the same month
        if (result.month() != month_) {
            throw OccurrenceNotFoundException("Requested occurrence does not exist in this month");
        }

        return result;
    } else {
        // Last occurrence
        // Get the last day of the month
        const year_month_day_last last_of_month{std::chrono::year{year}, month_day_last{month_}};
        const year_month_day last_day{last_of_month};

        const sys_days last_sd{last_day};
        const weekday last_weekday{last_sd};

        // Calculate days to subtract to get to last occurrence of target weekday
        const unsigned last_weekday_encoding = last_weekday.c_encoding();
        const unsigned weekday_encoding = weekday_.c_encoding();
        const unsigned days_to_subtract =
            (last_weekday_encoding + DAYS_PER_WEEK - weekday_encoding) % DAYS_PER_WEEK;

        const sys_days target_sd = last_sd - days{static_cast<days::rep>(days_to_subtract)};
        const year_month_day result{target_sd};

        return result;
    }
}

std::unique_ptr<HolidayRule> NthWeekdayRule::clone() const {
    return std::make_unique<NthWeekdayRule>(name_, static_cast<unsigned>(month_),
                                            weekday_.c_encoding(), occurrence_);
}

} // namespace datelib
