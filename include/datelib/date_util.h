#pragma once

#include <chrono>

namespace datelib {

// Hash function for std::chrono::weekday to use in unordered_set
struct WeekdayHash {
    std::size_t operator()(const std::chrono::weekday& wd) const noexcept {
        return std::hash<unsigned>{}(wd.c_encoding());
    }
};

} // namespace datelib
