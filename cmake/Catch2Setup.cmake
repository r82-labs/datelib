# Catch2 Setup - Modern FetchContent configuration for Catch2 v3

include(FetchContent)

# Use Catch2 v3 with modern FetchContent best practices
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.7.1  # Latest stable v3 release
  GIT_SHALLOW    TRUE    # Faster download
  FIND_PACKAGE_ARGS NAMES Catch2  # Try find_package first
)

FetchContent_MakeAvailable(Catch2)

# Include Catch2 CMake modules for test discovery
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
