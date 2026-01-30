#ifndef DATELIB_DATE_H
#define DATELIB_DATE_H

#include <string>
#include <ctime>

namespace datelib {

/**
 * @brief A simple Date class for curve bootstrapping analytics
 */
class Date {
public:
    /**
     * @brief Construct a Date from year, month, day
     * @param year The year
     * @param month The month (1-12)
     * @param day The day (1-31)
     */
    Date(int year, int month, int day);
    
    /**
     * @brief Default constructor - creates today's date
     */
    Date();
    
    /**
     * @brief Get the year component
     */
    int getYear() const;
    
    /**
     * @brief Get the month component (1-12)
     */
    int getMonth() const;
    
    /**
     * @brief Get the day component (1-31)
     */
    int getDay() const;
    
    /**
     * @brief Calculate the number of days between two dates
     * @param other The other date
     * @return Number of days (can be negative)
     */
    int daysBetween(const Date& other) const;
    
    /**
     * @brief Add a number of days to this date
     * @param days Number of days to add (can be negative)
     * @return New Date object
     */
    Date addDays(int days) const;
    
    /**
     * @brief Convert date to ISO 8601 string format (YYYY-MM-DD)
     */
    std::string toISOString() const;
    
    /**
     * @brief Check if this date is before another date
     */
    bool isBefore(const Date& other) const;
    
    /**
     * @brief Check if this date is after another date
     */
    bool isAfter(const Date& other) const;
    
    /**
     * @brief Check if two dates are equal
     */
    bool operator==(const Date& other) const;
    
    /**
     * @brief Check if two dates are not equal
     */
    bool operator!=(const Date& other) const;

private:
    int year_;
    int month_;
    int day_;
    
    /**
     * @brief Convert date to days since epoch (for calculations)
     */
    int toDaysSinceEpoch() const;
    
    /**
     * @brief Create date from days since epoch
     */
    static Date fromDaysSinceEpoch(int days);
};

} // namespace datelib

#endif // DATELIB_DATE_H
