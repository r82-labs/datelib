# Test executable
add_executable(test_datelib test_date.cpp)

# Link libraries
target_link_libraries(test_datelib PRIVATE datelib Catch2::Catch2)

# Coverage flags for test executable
if(ENABLE_COVERAGE)
  target_compile_options(test_datelib PRIVATE --coverage)
  target_link_options(test_datelib PRIVATE --coverage)
endif()

# Include directories
target_include_directories(test_datelib PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add test to CTest
add_test(NAME test_datelib COMMAND test_datelib)
