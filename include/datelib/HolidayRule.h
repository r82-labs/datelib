#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace datelib {

/**
 * @brief Enum for specifying occurrence of a weekday in a month
 */
enum class Occurrence { First = 1, Second = 2, Third = 3, Fourth = 4, Fifth = 5, Last = -1 };

/**
 * @brief Abstract base class for holiday calculation rules
 */
class HolidayRule {
  public:
    virtual ~HolidayRule() = default;

    /**
     * @brief Calculate the holiday date for a given year
     * @param year The year to calculate the holiday for
     * @return The date of the holiday in that year
     * @throws std::exception if the rule cannot be applied to this year
     */
    virtual std::chrono::year_month_day calculateDate(int year) const = 0;

    /**
     * @brief Get the name of this holiday
     * @return The holiday name
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Clone this rule
     * @return A unique pointer to a copy of this rule
     */
    virtual std::unique_ptr<HolidayRule> clone() const = 0;
};

/**
 * @brief Rule for an explicit date that is always a holiday
 * Example: One-time holidays or non-recurring dates
 */
class ExplicitDateRule : public HolidayRule {
  public:
    /**
     * @brief Construct an explicit date holiday rule
     * @param name The name of the holiday
     * @param date The specific date
     */
    ExplicitDateRule(std::string name, std::chrono::year_month_day date);

    std::chrono::year_month_day calculateDate(int year) const override;
    std::string getName() const override { return name_; }
    std::unique_ptr<HolidayRule> clone() const override;

  private:
    std::string name_;
    std::chrono::year_month_day date_;
};

/**
 * @brief Rule for holidays that occur on a fixed date each year
 * Example: Christmas (December 25), New Year's Day (January 1)
 */
class FixedDateRule : public HolidayRule {
  public:
    /**
     * @brief Construct a fixed date holiday rule
     * @param name The name of the holiday
     * @param month The month (1-12)
     * @param day The day of month (1-31)
     */
    FixedDateRule(std::string name, unsigned month, unsigned day);

    std::chrono::year_month_day calculateDate(int year) const override;
    std::string getName() const override { return name_; }
    std::unique_ptr<HolidayRule> clone() const override;

  private:
    std::string name_;
    std::chrono::month month_;
    std::chrono::day day_;
};

/**
 * @brief Rule for holidays that occur on the Nth occurrence of a weekday in a
 * month Example: Thanksgiving (4th Thursday of November), Labor Day (1st
 * Monday of September)
 */
class NthWeekdayRule : public HolidayRule {
  public:
    /**
     * @brief Construct an Nth weekday holiday rule
     * @param name The name of the holiday
     * @param month The month (1-12)
     * @param weekday The day of week (0=Sunday, 6=Saturday)
     * @param occurrence Which occurrence (First, Second, Third, Fourth, Fifth, or Last)
     */
    NthWeekdayRule(std::string name, unsigned month, unsigned weekday, Occurrence occurrence);

    std::chrono::year_month_day calculateDate(int year) const override;
    std::string getName() const override { return name_; }
    std::unique_ptr<HolidayRule> clone() const override;

  private:
    std::string name_;
    std::chrono::month month_;
    std::chrono::weekday weekday_;
    Occurrence occurrence_;
};

} // namespace datelib
