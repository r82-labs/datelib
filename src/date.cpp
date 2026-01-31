#include "datelib/date.h"

namespace datelib {

std::string hello_world() {
    return "Hello, World!";
}

bool isBusinessDay(const std::chrono::year_month_day& date, const HolidayCalendar& holidays) {
    // Convert to sys_days to get weekday information
    std::chrono::sys_days sd{date};
    std::chrono::weekday wd{sd};

    // Check if it's a weekend (Saturday or Sunday)
    if (wd == std::chrono::Saturday || wd == std::chrono::Sunday) {
        return false;
    }

    // Check if it's a holiday
    if (holidays.find(date) != holidays.end()) {
        return false;
    }

    return true;
}

} // namespace datelib
