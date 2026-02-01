#pragma once

#include <stdexcept>

namespace datelib {

/**
 * @brief Exception thrown when unable to find a business day within a reasonable range
 */
class BusinessDaySearchException : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Exception thrown when a date is invalid for the specified year
 */
class InvalidDateException : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Exception thrown when an explicit date does not exist in the requested year
 */
class DateNotInYearException : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Exception thrown when a requested weekday occurrence does not exist in a month
 */
class OccurrenceNotFoundException : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Exception thrown when an enum value is not handled in a switch statement
 */
class UnhandledEnumException : public std::logic_error {
  public:
    using std::logic_error::logic_error;
};

} // namespace datelib
