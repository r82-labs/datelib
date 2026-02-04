# Test executable
add_executable(test_datelib 
  test_date.cpp 
  test_HolidayRule.cpp
  test_HolidayCalendar.cpp
  test_DayCountConvention.cpp
)

# Link libraries (Catch2 v3 uses Catch2::Catch2WithMain)
target_link_libraries(test_datelib PRIVATE 
  datelib 
  Catch2::Catch2WithMain
)

# Coverage flags for test executable
if(ENABLE_COVERAGE)
  target_compile_options(test_datelib PRIVATE --coverage)
  target_link_options(test_datelib PRIVATE --coverage)
endif()

# Include directories (use target-based approach)
target_include_directories(test_datelib PRIVATE 
  ${CMAKE_SOURCE_DIR}/include
)

# Use Catch2's test discovery for better CTest integration
include(CTest)
include(Catch)
catch_discover_tests(test_datelib)
