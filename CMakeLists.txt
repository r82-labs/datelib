cmake_minimum_required(VERSION 3.14)
project(datelib VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Coverage option (enabled via -DENABLE_COVERAGE=ON)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# Library source files
add_library(datelib SHARED
    src/date.cpp
)

# Compiler warnings
target_compile_options(datelib PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Coverage flags
if(ENABLE_COVERAGE)
    target_compile_options(datelib PRIVATE --coverage)
    target_link_options(datelib PRIVATE --coverage)
endif()

# Include directories
target_include_directories(datelib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set library properties
set_target_properties(datelib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/datelib/date.h
)

# Enable testing
enable_testing()

# Download and configure Catch2
include(cmake/Catch2Setup.cmake)

# Add tests subdirectory
add_subdirectory(tests)

# Code formatting targets
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
    # Collect all source files
    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h
    )

    # Target to format all files
    add_custom_target(format
        COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
        COMMENT "Formatting all source files with clang-format"
        VERBATIM
    )

    # Target to check formatting without modifying files
    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT} --dry-run --Werror ${ALL_SOURCE_FILES}
        COMMENT "Checking code formatting with clang-format"
        VERBATIM
    )
else()
    message(WARNING "clang-format not found. Format targets will not be available.")
endif()

# Installation rules
install(TARGETS datelib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/datelib
)
