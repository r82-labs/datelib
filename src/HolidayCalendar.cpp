#include "datelib/HolidayCalendar.h"

#include <algorithm>
#include <optional>

namespace datelib {

using namespace std::chrono;

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

    return std::any_of(rules_.begin(), rules_.end(), [&](const auto& rule) {
        try {
            return rule->calculateDate(year) == date;
        } catch (...) {
            return false;
        }
    });
}

std::vector<year_month_day> HolidayCalendar::getHolidays(int year) const {
    std::vector<year_month_day> holidays;
    holidays.reserve(rules_.size());

    // Collect all holidays from rules using transform_reduce pattern
    std::for_each(rules_.begin(), rules_.end(), [&](const auto& rule) {
        try {
            holidays.push_back(rule->calculateDate(year));
        } catch (...) {
            // Rule not applicable for this year
        }
    });

    // Sort and remove duplicates
    std::sort(holidays.begin(), holidays.end());
    holidays.erase(std::unique(holidays.begin(), holidays.end()), holidays.end());

    return holidays;
}

std::vector<std::string> HolidayCalendar::getHolidayNames(const year_month_day& date) const {
    std::vector<std::string> names;
    int year = static_cast<int>(date.year());

    std::for_each(rules_.begin(), rules_.end(), [&](const auto& rule) {
        try {
            if (rule->calculateDate(year) == date) {
                names.push_back(rule->getName());
            }
        } catch (...) {
            // Rule not applicable for this year
        }
    });

    return names;
}

} // namespace datelib
