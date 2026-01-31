#include "datelib/HolidayRule.h"

#include <stdexcept>

namespace datelib {

using namespace std::chrono;

// ExplicitDateRule implementation
ExplicitDateRule::ExplicitDateRule(std::string name, year_month_day date)
    : name_(std::move(name)), date_(date) {
    if (!date_.ok()) {
        throw std::invalid_argument("Invalid date");
    }
}

bool ExplicitDateRule::appliesTo(int year) const {
    return static_cast<int>(date_.year()) == year;
}

year_month_day ExplicitDateRule::calculateDate(int year) const {
    // Only return the date if it matches the requested year
    if (static_cast<int>(date_.year()) == year) {
        return date_;
    }
    throw std::runtime_error("Explicit date does not exist in this year");
}

std::unique_ptr<HolidayRule> ExplicitDateRule::clone() const {
    return std::make_unique<ExplicitDateRule>(name_, date_);
}

// FixedDateRule implementation
FixedDateRule::FixedDateRule(std::string name, unsigned month, unsigned day)
    : name_(std::move(name)), month_{month}, day_{day} {
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }
    if (day < 1 || day > 31) {
        throw std::invalid_argument("Day must be between 1 and 31");
    }
}

bool FixedDateRule::appliesTo(int year) const {
    year_month_day ymd{std::chrono::year{year}, month_, day_};
    return ymd.ok();
}

year_month_day FixedDateRule::calculateDate(int year) const {
    year_month_day ymd{std::chrono::year{year}, month_, day_};
    if (!ymd.ok()) {
        throw std::runtime_error("Invalid date for this year");
    }
    return ymd;
}

std::unique_ptr<HolidayRule> FixedDateRule::clone() const {
    return std::make_unique<FixedDateRule>(name_, static_cast<unsigned>(month_),
                                           static_cast<unsigned>(day_));
}

// NthWeekdayRule implementation
NthWeekdayRule::NthWeekdayRule(std::string name, unsigned month, unsigned weekday_val,
                               Occurrence occurrence)
    : name_(std::move(name)), month_{month}, weekday_{weekday_val}, occurrence_(occurrence) {
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Month must be between 1 and 12");
    }
    if (weekday_val > 6) {
        throw std::invalid_argument("Weekday must be between 0 and 6");
    }
    int occ_val = static_cast<int>(occurrence);
    if (occ_val == 0 || occ_val < -1 || occ_val > 5) {
        throw std::invalid_argument("Occurrence must be First through Fifth or Last");
    }
}

bool NthWeekdayRule::appliesTo(int year) const {
    // For Last occurrence, it always applies
    int occ_val = static_cast<int>(occurrence_);
    if (occ_val < 0) {
        return true;
    }

    // For Nth occurrence, check if it exists in the month
    year_month_day first_of_month{std::chrono::year{year}, month_, day{1}};
    sys_days first_sd{first_of_month};
    weekday first_weekday{first_sd};

    int days_to_first = (weekday_.c_encoding() - first_weekday.c_encoding() + 7) % 7;
    sys_days target_sd = first_sd + days{days_to_first + (occ_val - 1) * 7};
    year_month_day result{target_sd};

    return result.month() == month_;
}

year_month_day NthWeekdayRule::calculateDate(int year) const {
    // Get the first day of the month
    year_month_day first_of_month{std::chrono::year{year}, month_, day{1}};

    // Convert to sys_days to work with weekdays
    sys_days first_sd{first_of_month};
    weekday first_weekday{first_sd};

    int occ_val = static_cast<int>(occurrence_);
    if (occ_val > 0) {
        // Find the Nth occurrence of the target weekday
        // Calculate days to add to get to first occurrence of target weekday
        int days_to_first = (weekday_.c_encoding() - first_weekday.c_encoding() + 7) % 7;

        // Add weeks to get to the Nth occurrence
        sys_days target_sd = first_sd + days{days_to_first + (occ_val - 1) * 7};
        year_month_day result{target_sd};

        // Verify we're still in the same month
        if (result.month() != month_) {
            throw std::runtime_error("Requested occurrence does not exist in this month");
        }

        return result;
    } else {
        // Last occurrence
        // Get the last day of the month
        year_month_day_last last_of_month{std::chrono::year{year}, month_day_last{month_}};
        year_month_day last_day{last_of_month};

        sys_days last_sd{last_day};
        weekday last_weekday{last_sd};

        // Calculate days to subtract to get to last occurrence of target weekday
        int days_to_subtract = (last_weekday.c_encoding() - weekday_.c_encoding() + 7) % 7;

        sys_days target_sd = last_sd - days{days_to_subtract};
        year_month_day result{target_sd};

        return result;
    }
}

std::unique_ptr<HolidayRule> NthWeekdayRule::clone() const {
    return std::make_unique<NthWeekdayRule>(name_, static_cast<unsigned>(month_),
                                            weekday_.c_encoding(), occurrence_);
}

} // namespace datelib
