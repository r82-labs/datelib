#include "datelib/DayCountConvention.h"

#include "datelib/exceptions.h"

#include <chrono>
#include <cmath>
#include <stdexcept>

namespace datelib {

namespace {

/**
 * @brief Check if a year is a leap year
 */
[[nodiscard]] bool is_leap_year(const std::chrono::year& y) {
    return y.is_leap();
}

/**
 * @brief Get the number of days in a year
 */
[[nodiscard]] int days_in_year(const std::chrono::year& y) {
    return is_leap_year(y) ? 366 : 365;
}

/**
 * @brief Calculate actual days between two dates
 */
[[nodiscard]] int actual_days(const std::chrono::year_month_day& start,
                              const std::chrono::year_month_day& end) {
    const auto start_sys = std::chrono::sys_days{start};
    const auto end_sys = std::chrono::sys_days{end};
    return (end_sys - start_sys).count();
}

/**
 * @brief Validate that dates are valid and in correct order
 */
void validate_dates(const std::chrono::year_month_day& start_date,
                    const std::chrono::year_month_day& end_date) {
    if (!start_date.ok()) {
        throw InvalidDateException("Start date is invalid");
    }
    if (!end_date.ok()) {
        throw InvalidDateException("End date is invalid");
    }
    if (start_date > end_date) {
        throw std::invalid_argument("Start date must not be after end date");
    }
}

} // anonymous namespace

// ============================================================================
// ActualActual Implementation
// ============================================================================

double ActualActual::yearFraction(const std::chrono::year_month_day& start_date,
                                  const std::chrono::year_month_day& end_date) const {
    validate_dates(start_date, end_date);

    const auto start_year = start_date.year();
    const auto end_year = end_date.year();

    // If dates are in the same year, simple calculation
    if (start_year == end_year) {
        const int days = actual_days(start_date, end_date);
        const int year_days = days_in_year(start_year);
        return static_cast<double>(days) / static_cast<double>(year_days);
    }

    // For multi-year periods, calculate fraction for each year
    double fraction = 0.0;

    // First partial year: from start_date to end of year
    const std::chrono::year_month_day year_end{start_year, std::chrono::December,
                                               std::chrono::day{31}};
    const int days_in_first_year = actual_days(start_date, year_end) + 1; // include Dec 31
    fraction +=
        static_cast<double>(days_in_first_year) / static_cast<double>(days_in_year(start_year));

    // Complete years in between
    for (auto y = start_year + std::chrono::years{1}; y < end_year; y += std::chrono::years{1}) {
        fraction += 1.0; // each complete year contributes 1.0
    }

    // Last partial year: from start of year to end_date
    const std::chrono::year_month_day year_start{end_year, std::chrono::January,
                                                 std::chrono::day{1}};
    const int days_in_last_year = actual_days(year_start, end_date);
    fraction +=
        static_cast<double>(days_in_last_year) / static_cast<double>(days_in_year(end_year));

    return fraction;
}

int ActualActual::dayCount(const std::chrono::year_month_day& start_date,
                           const std::chrono::year_month_day& end_date) const {
    validate_dates(start_date, end_date);
    return actual_days(start_date, end_date);
}

std::string ActualActual::name() const {
    return "Actual/Actual (ISDA)";
}

std::unique_ptr<DayCountConvention> ActualActual::clone() const {
    return std::make_unique<ActualActual>(*this);
}

// ============================================================================
// Actual360 Implementation
// ============================================================================

double Actual360::yearFraction(const std::chrono::year_month_day& start_date,
                               const std::chrono::year_month_day& end_date) const {
    validate_dates(start_date, end_date);
    const int days = actual_days(start_date, end_date);
    return static_cast<double>(days) / 360.0;
}

int Actual360::dayCount(const std::chrono::year_month_day& start_date,
                        const std::chrono::year_month_day& end_date) const {
    validate_dates(start_date, end_date);
    return actual_days(start_date, end_date);
}

std::string Actual360::name() const {
    return "Actual/360";
}

std::unique_ptr<DayCountConvention> Actual360::clone() const {
    return std::make_unique<Actual360>(*this);
}

// ============================================================================
// Actual365Fixed Implementation
// ============================================================================

double Actual365Fixed::yearFraction(const std::chrono::year_month_day& start_date,
                                    const std::chrono::year_month_day& end_date) const {
    validate_dates(start_date, end_date);
    const int days = actual_days(start_date, end_date);
    return static_cast<double>(days) / 365.0;
}

int Actual365Fixed::dayCount(const std::chrono::year_month_day& start_date,
                             const std::chrono::year_month_day& end_date) const {
    validate_dates(start_date, end_date);
    return actual_days(start_date, end_date);
}

std::string Actual365Fixed::name() const {
    return "Actual/365 (Fixed)";
}

std::unique_ptr<DayCountConvention> Actual365Fixed::clone() const {
    return std::make_unique<Actual365Fixed>(*this);
}

// ============================================================================
// Thirty360 Implementation
// ============================================================================

int Thirty360::dayCount(const std::chrono::year_month_day& start_date,
                        const std::chrono::year_month_day& end_date) const {
    validate_dates(start_date, end_date);

    int d1 = static_cast<unsigned>(start_date.day());
    int m1 = static_cast<unsigned>(start_date.month());
    int y1 = static_cast<int>(start_date.year());

    int d2 = static_cast<unsigned>(end_date.day());
    int m2 = static_cast<unsigned>(end_date.month());
    int y2 = static_cast<int>(end_date.year());

    // Apply 30/360 US (Bond Basis) adjustment rules
    // Save original d1 value to check for the second rule
    const int original_d1 = d1;
    if (d1 == 31) {
        d1 = 30;
    }
    if (d2 == 31 && (original_d1 == 30 || original_d1 == 31)) {
        d2 = 30;
    }

    // Calculate day count using 30-day months and 360-day year
    return (y2 - y1) * 360 + (m2 - m1) * 30 + (d2 - d1);
}

double Thirty360::yearFraction(const std::chrono::year_month_day& start_date,
                               const std::chrono::year_month_day& end_date) const {
    const int days = dayCount(start_date, end_date);
    return static_cast<double>(days) / 360.0;
}

std::string Thirty360::name() const {
    return "30/360 (Bond Basis)";
}

std::unique_ptr<DayCountConvention> Thirty360::clone() const {
    return std::make_unique<Thirty360>(*this);
}

} // namespace datelib
