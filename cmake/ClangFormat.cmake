# ClangFormat.cmake - Code formatting targets using clang-format
#
# Targets:
#   format       - Format all C++ source files
#   format-check - Check formatting without modifying files

find_program(CLANG_FORMAT NAMES clang-format)

if(CLANG_FORMAT)
  # Collect all source files using CONFIGURE_DEPENDS for automatic reconfiguration
  file(GLOB_RECURSE ALL_SOURCE_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
  )

  # Target to format all files
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
    COMMENT "Formatting all source files with clang-format"
    VERBATIM
    COMMAND_EXPAND_LISTS
  )

  # Target to check formatting without modifying files
  add_custom_target(format-check
    COMMAND ${CLANG_FORMAT} --dry-run --Werror ${ALL_SOURCE_FILES}
    COMMENT "Checking code formatting with clang-format"
    VERBATIM
    COMMAND_EXPAND_LISTS
  )
else()
  message(WARNING "clang-format not found. Format targets will not be available.")
endif()
