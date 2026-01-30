cmake_minimum_required(VERSION 3.14)
project(datelib VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library source files
add_library(datelib SHARED
    src/date.cpp
)

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
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.10
)
FetchContent_MakeAvailable(Catch2)

# Add tests subdirectory
add_subdirectory(tests)

# Installation rules
install(TARGETS datelib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/datelib
)
