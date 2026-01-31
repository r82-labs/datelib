#include "datelib/period.h"

#include <cctype>
#include <stdexcept>

namespace datelib {

Period Period::parse(const std::string& period_str) {
    if (period_str.empty()) {
        throw std::invalid_argument("Period string cannot be empty");
    }

    // Find where the numeric part ends
    size_t numeric_end = 0;
    bool has_sign = false;

    // Check for optional sign at the beginning
    if (period_str[0] == '+' || period_str[0] == '-') {
        has_sign = true;
        numeric_end = 1;
    }

    // Find the end of numeric portion
    while (numeric_end < period_str.length() && std::isdigit(period_str[numeric_end])) {
        numeric_end++;
    }

    // We need at least one digit
    if (numeric_end == (has_sign ? 1 : 0)) {
        throw std::invalid_argument("Period string must contain a numeric value: " + period_str);
    }

    // We need exactly one character after the number for the unit
    if (numeric_end + 1 != period_str.length()) {
        throw std::invalid_argument(
            "Period string must end with a single unit character (D/W/M/Y): " + period_str);
    }

    // Parse the numeric value
    int value;
    try {
        value = std::stoi(period_str.substr(0, numeric_end));
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid numeric value in period string: " + period_str);
    }

    // Parse the unit character (case-insensitive)
    char unit_char = std::toupper(period_str[numeric_end]);
    Unit unit;

    switch (unit_char) {
    case 'D':
        unit = Unit::Days;
        break;
    case 'W':
        unit = Unit::Weeks;
        break;
    case 'M':
        unit = Unit::Months;
        break;
    case 'Y':
        unit = Unit::Years;
        break;
    default:
        throw std::invalid_argument("Invalid period unit '" +
                                    std::string(1, period_str[numeric_end]) +
                                    "'. Must be D, W, M, or Y: " + period_str);
    }

    return Period(value, unit);
}

} // namespace datelib
