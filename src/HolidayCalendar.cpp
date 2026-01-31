#include "datelib/HolidayCalendar.h"

#include <algorithm>

namespace datelib {

HolidayCalendar::HolidayCalendar(const HolidayCalendar& other)
    : explicitHolidays_(other.explicitHolidays_) {
    // Deep copy the rules
    rules_.reserve(other.rules_.size());
    for (const auto& rule : other.rules_) {
        rules_.push_back(rule->clone());
    }
}

HolidayCalendar& HolidayCalendar::operator=(const HolidayCalendar& other) {
    if (this != &other) {
        explicitHolidays_ = other.explicitHolidays_;

        // Deep copy the rules
        rules_.clear();
        rules_.reserve(other.rules_.size());
        for (const auto& rule : other.rules_) {
            rules_.push_back(rule->clone());
        }
    }
    return *this;
}

void HolidayCalendar::addHoliday(const Date& date) {
    explicitHolidays_.insert(date);
}

void HolidayCalendar::addRule(std::unique_ptr<HolidayRule> rule) {
    if (rule) {
        rules_.push_back(std::move(rule));
    }
}

bool HolidayCalendar::isHoliday(const Date& date) const {
    // Check explicit holidays
    if (explicitHolidays_.find(date) != explicitHolidays_.end()) {
        return true;
    }

    // Check rule-based holidays
    int year = date.year();
    for (const auto& rule : rules_) {
        try {
            Date ruleDate = rule->calculateDate(year);
            if (ruleDate == date) {
                return true;
            }
        } catch (...) {
            // Rule might not be applicable for this year, skip it
            continue;
        }
    }

    return false;
}

std::vector<Date> HolidayCalendar::getHolidays(int year) const {
    std::set<Date> holidays;

    // Add explicit holidays for this year
    for (const auto& date : explicitHolidays_) {
        if (date.year() == year) {
            holidays.insert(date);
        }
    }

    // Add rule-based holidays
    for (const auto& rule : rules_) {
        try {
            Date ruleDate = rule->calculateDate(year);
            holidays.insert(ruleDate);
        } catch (...) {
            // Rule might not be applicable for this year, skip it
            continue;
        }
    }

    // Convert set to vector (already sorted)
    return std::vector<Date>(holidays.begin(), holidays.end());
}

std::vector<std::string> HolidayCalendar::getHolidayNames(const Date& date) const {
    std::vector<std::string> names;

    // Check if it's an explicit holiday (no name stored for these)
    if (explicitHolidays_.find(date) != explicitHolidays_.end()) {
        names.push_back("Holiday");
    }

    // Check rule-based holidays
    int year = date.year();
    for (const auto& rule : rules_) {
        try {
            Date ruleDate = rule->calculateDate(year);
            if (ruleDate == date) {
                names.push_back(rule->getName());
            }
        } catch (...) {
            // Rule might not be applicable for this year, skip it
            continue;
        }
    }

    return names;
}

void HolidayCalendar::clear() {
    explicitHolidays_.clear();
    rules_.clear();
}

} // namespace datelib
