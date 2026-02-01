#include "datelib/period.h"

#include <cctype>
#include <format>
#include <stdexcept>
#include <string_view>

namespace datelib {

namespace {
// Helper function to validate period string format and find numeric portion
std::pair<size_t, bool> validateAndFindNumericEnd(std::string_view period_str) {
    if (period_str.empty()) {
        throw std::invalid_argument("Period string cannot be empty");
    }

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
        throw std::invalid_argument(
            std::format("Period string must contain a numeric value: {}", period_str));
    }

    // We need exactly one character after the number for the unit
    if (numeric_end + 1 != period_str.length()) {
        throw std::invalid_argument(std::format(
            "Period string must end with a single unit character (D/W/M/Y): {}", period_str));
    }

    return {numeric_end, has_sign};
}

// Helper function to parse the numeric value from period string
int parseNumericValue(std::string_view period_str, size_t numeric_end) {
    try {
        // string_view doesn't have stoi, need to convert to string
        return std::stoi(std::string(period_str.substr(0, numeric_end)));
    } catch (const std::exception&) {
        throw std::invalid_argument(
            std::format("Invalid numeric value in period string: {}", period_str));
    }
}

// Helper function to parse the unit character
Period::Unit parseUnit(std::string_view period_str, size_t unit_index) {
    char unit_char = std::toupper(period_str[unit_index]);

    using enum Period::Unit;
    switch (unit_char) {
    case 'D':
        return Days;
    case 'W':
        return Weeks;
    case 'M':
        return Months;
    case 'Y':
        return Years;
    default:
        throw std::invalid_argument(
            std::format("Invalid period unit '{}'. Must be D, W, M, or Y: {}",
                        period_str[unit_index], period_str));
    }
}
} // namespace

Period Period::parse(std::string_view period_str) {
    auto [numeric_end, has_sign] = validateAndFindNumericEnd(period_str);
    int value = parseNumericValue(period_str, numeric_end);
    Unit unit = parseUnit(period_str, numeric_end);

    return Period(value, unit);
}

} // namespace datelib
