#pragma once

#include <chrono>
#include <memory>
#include <string>

namespace datelib {

/**
 * @brief Abstract base class for day count conventions
 *
 * Day count conventions are used primarily in financial calculations to determine
 * the fraction of a year between two dates. Different conventions use different
 * methods to count days and determine the year basis.
 *
 * Common conventions include:
 * - Actual/Actual (ISDA): Actual days over actual days in year
 * - Actual/360: Actual days over 360-day year
 * - Actual/365: Actual days over 365-day year (Fixed)
 * - 30/360 (Bond Basis): 30-day months, 360-day year
 */
class DayCountConvention {
  public:
    /**
     * @brief Virtual destructor for polymorphic destruction
     */
    virtual ~DayCountConvention() = default;

    /**
     * @brief Calculate the day count fraction between two dates
     *
     * @param start_date The start date (inclusive)
     * @param end_date The end date (exclusive)
     * @return The day count fraction representing the portion of a year
     * @throws std::invalid_argument if start_date > end_date or dates are invalid
     *
     * The day count fraction is used to calculate accrued interest and present values.
     * For example, a fraction of 0.5 represents half a year.
     */
    [[nodiscard]] virtual double
    yearFraction(const std::chrono::year_month_day& start_date,
                 const std::chrono::year_month_day& end_date) const = 0;

    /**
     * @brief Calculate the number of days between two dates according to convention
     *
     * @param start_date The start date (inclusive)
     * @param end_date The end date (exclusive)
     * @return The number of days according to this convention
     * @throws std::invalid_argument if start_date > end_date or dates are invalid
     *
     * Note: This may not be the actual calendar days for conventions like 30/360
     */
    [[nodiscard]] virtual int dayCount(const std::chrono::year_month_day& start_date,
                                       const std::chrono::year_month_day& end_date) const = 0;

    /**
     * @brief Get the name of this day count convention
     * @return A string describing the convention (e.g., "Actual/Actual (ISDA)")
     */
    [[nodiscard]] virtual std::string name() const = 0;

    /**
     * @brief Clone this day count convention
     * @return A unique pointer to a copy of this convention
     *
     * Enables copying polymorphic objects without knowing the concrete type
     */
    [[nodiscard]] virtual std::unique_ptr<DayCountConvention> clone() const = 0;

  protected:
    /**
     * @brief Protected default constructor (only derived classes can construct)
     */
    DayCountConvention() = default;

    /**
     * @brief Protected copy constructor
     */
    DayCountConvention(const DayCountConvention&) = default;

    /**
     * @brief Protected copy assignment
     */
    DayCountConvention& operator=(const DayCountConvention&) = default;
};

/**
 * @brief Actual/Actual (ISDA) day count convention
 *
 * This convention uses the actual number of days in the period and the actual
 * number of days in the year. For periods spanning multiple years, it calculates
 * the fraction for each year separately.
 *
 * Formula:
 * - If same year: (end - start) / days_in_year
 * - If different years: sum of fractions for each year
 */
class ActualActual : public DayCountConvention {
  public:
    ActualActual() = default;
    ~ActualActual() override = default;

    [[nodiscard]] double yearFraction(const std::chrono::year_month_day& start_date,
                                      const std::chrono::year_month_day& end_date) const override;

    [[nodiscard]] int dayCount(const std::chrono::year_month_day& start_date,
                               const std::chrono::year_month_day& end_date) const override;

    [[nodiscard]] std::string name() const override;

    [[nodiscard]] std::unique_ptr<DayCountConvention> clone() const override;
};

/**
 * @brief Actual/360 day count convention
 *
 * This convention uses the actual number of days in the period divided by 360.
 * Commonly used in money market instruments and some floating rate bonds.
 *
 * Formula: (end - start) / 360
 */
class Actual360 : public DayCountConvention {
  public:
    Actual360() = default;
    ~Actual360() override = default;

    [[nodiscard]] double yearFraction(const std::chrono::year_month_day& start_date,
                                      const std::chrono::year_month_day& end_date) const override;

    [[nodiscard]] int dayCount(const std::chrono::year_month_day& start_date,
                               const std::chrono::year_month_day& end_date) const override;

    [[nodiscard]] std::string name() const override;

    [[nodiscard]] std::unique_ptr<DayCountConvention> clone() const override;
};

/**
 * @brief Actual/365 (Fixed) day count convention
 *
 * This convention uses the actual number of days in the period divided by 365.
 * Used in some sterling and Canadian instruments.
 *
 * Formula: (end - start) / 365
 */
class Actual365Fixed : public DayCountConvention {
  public:
    Actual365Fixed() = default;
    ~Actual365Fixed() override = default;

    [[nodiscard]] double yearFraction(const std::chrono::year_month_day& start_date,
                                      const std::chrono::year_month_day& end_date) const override;

    [[nodiscard]] int dayCount(const std::chrono::year_month_day& start_date,
                               const std::chrono::year_month_day& end_date) const override;

    [[nodiscard]] std::string name() const override;

    [[nodiscard]] std::unique_ptr<DayCountConvention> clone() const override;
};

/**
 * @brief 30/360 (Bond Basis, US) day count convention
 *
 * This convention assumes 30 days in each month and 360 days in a year.
 * Commonly used in US corporate and municipal bonds.
 *
 * The convention adjusts the day values according to specific rules:
 * - If D1 is 31, change D1 to 30
 * - If D2 is 31 and D1 is 30 or 31, change D2 to 30
 *
 * Formula: ((Y2-Y1)*360 + (M2-M1)*30 + (D2-D1)) / 360
 */
class Thirty360 : public DayCountConvention {
  public:
    Thirty360() = default;
    ~Thirty360() override = default;

    [[nodiscard]] double yearFraction(const std::chrono::year_month_day& start_date,
                                      const std::chrono::year_month_day& end_date) const override;

    [[nodiscard]] int dayCount(const std::chrono::year_month_day& start_date,
                               const std::chrono::year_month_day& end_date) const override;

    [[nodiscard]] std::string name() const override;

    [[nodiscard]] std::unique_ptr<DayCountConvention> clone() const override;
};

} // namespace datelib
