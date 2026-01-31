#include "datelib/HolidayCalendar.h"

#include <algorithm>

namespace datelib {

using std::chrono::year_month_day;

HolidayCalendar::HolidayCalendar(const HolidayCalendar& other) {
    // Deep copy the rules
    rules_.reserve(other.rules_.size());
    for (const auto& rule : other.rules_) {
        rules_.push_back(rule->clone());
    }
}

HolidayCalendar& HolidayCalendar::operator=(const HolidayCalendar& other) {
    if (this != &other) {
        // Deep copy the rules
        rules_.clear();
        rules_.reserve(other.rules_.size());
        for (const auto& rule : other.rules_) {
            rules_.push_back(rule->clone());
        }
    }
    return *this;
}

void HolidayCalendar::addHoliday(const std::string& name, const year_month_day& date) {
    rules_.push_back(std::make_unique<ExplicitDateRule>(name, date));
}

void HolidayCalendar::addRule(std::unique_ptr<HolidayRule> rule) {
    rules_.push_back(std::move(rule));
}

bool HolidayCalendar::isHoliday(const year_month_day& date) const {
    int year = static_cast<int>(date.year());

    return std::ranges::any_of(rules_, [&](const auto& rule) {
        return rule->appliesTo(year) && rule->calculateDate(year) == date;
    });
}

std::vector<year_month_day> HolidayCalendar::getHolidays(int year) const {
    std::vector<year_month_day> holidays;
    holidays.reserve(rules_.size());

    // Collect all holidays from rules that apply to this year
    for (const auto& rule : rules_) {
        if (rule->appliesTo(year)) {
            holidays.push_back(rule->calculateDate(year));
        }
    }

    // Sort and remove duplicates
    std::sort(holidays.begin(), holidays.end());
    holidays.erase(std::unique(holidays.begin(), holidays.end()), holidays.end());

    return holidays;
} // LCOV_EXCL_LINE

std::vector<std::string> HolidayCalendar::getHolidayNames(const year_month_day& date) const {
    std::vector<std::string> names;
    int year = static_cast<int>(date.year());

    for (const auto& rule : rules_) {
        if (rule->appliesTo(year) && rule->calculateDate(year) == date) {
            names.push_back(rule->getName());
        }
    }

    return names;
} // LCOV_EXCL_LINE

} // namespace datelib
