# Copilot Instructions for datelib

## Repository Overview

This is **datelib**, a C++ library providing date utilities. The repository is currently in its initial setup phase.

**Current State**: Minimal skeleton repository with LICENSE, README, and .gitignore configured for C++ development.

## Project Information

- **Language**: C++ (modern C++ standards recommended)
- **License**: Apache 2.0
- **Repository Type**: Library (not an executable application)
- **Target**: Cross-platform date utility functions

## Project Structure

```
datelib/
├── .github/              # GitHub configuration and workflows
├── src/                  # Source files (to be created)
├── include/datelib/     # Public header files (to be created)
├── tests/               # Test files (to be created)
├── examples/            # Example usage (to be created)
├── .gitignore           # C++ build artifacts exclusion
├── LICENSE              # Apache 2.0 license
└── README.md            # Project documentation
```

## Development Guidelines

### Build System

When implementing the build system:
- **Use CMake** as the build system (industry standard for C++ projects)
- Create `CMakeLists.txt` in the root directory
- Support common build types: Debug, Release, RelWithDebInfo
- Organize build artifacts in a `build/` directory (consider adding `build/` to .gitignore)

### Code Organization

**Header Files**:
- Place public API headers in `include/datelib/`
- Use standard header guards (recommended for cross-platform compatibility), though `#pragma once` is widely supported
- Keep implementation details in `src/` directory

**Source Files**:
- Place implementation files in `src/`
- Match header file names with source file names

### Testing

When adding tests:
- **Recommended Framework**: Google Test (gtest) or Catch2
- Place test files in `tests/` directory
- Name test files with `_test.cpp` suffix (e.g., `date_utils_test.cpp`)
- Run tests before committing changes

### Build and Test Commands

*Note: These will be established once the build system is in place*

**Expected build workflow** (once CMake is configured):
```bash
# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build build

# Run tests (once test framework is set up)
cd build && ctest --output-on-failure
```

### Code Style and Conventions

- **Naming Conventions**:
  - Use `snake_case` for function and variable names
  - Use `PascalCase` for class names
  - Use `UPPER_CASE` for constants and macros
  
- **Documentation**:
  - Add Doxygen-style comments for public APIs
  - Document function parameters, return values, and exceptions
  
- **Modern C++**:
  - Target C++17 as minimum standard (or newer)
  - Use RAII for resource management
  - Prefer `std::` containers and smart pointers

### Git Workflow

- Make focused, atomic commits
- Write clear commit messages
- Keep changes minimal and targeted
- Don't commit build artifacts (already excluded via .gitignore)

### Continuous Integration

*To be configured*: When adding CI/CD:
- Use GitHub Actions for automated builds and tests
- Create `.github/workflows/ci.yml` for build validation
- Run tests on multiple platforms (Linux, macOS, Windows)
- Verify compilation with major compilers (GCC, Clang, MSVC)

## File Exclusions

The `.gitignore` is configured to exclude common C++ build artifacts:
- Prerequisites: *.d
- Object files: *.slo, *.lo, *.o, *.obj
- Precompiled headers: *.gch, *.pch
- Linker files: *.ilk
- Debug info: *.pdb, *.dwo
- Static libraries: *.lai, *.la, *.a, *.lib
- Dynamic libraries: *.so, *.dylib, *.dll
- Executables: *.exe, *.out, *.app
- Fortran modules: *.mod, *.smod

## Important Notes for Copilot Coding Agent

1. **Build system not yet established**: Before adding features, ensure CMake build configuration is in place
2. **No existing tests**: When making changes, establish test framework first
3. **Minimal existing code**: You'll likely be creating foundational structure
4. **Cross-platform consideration**: Design with Linux, macOS, and Windows compatibility in mind
5. **Header-only option**: Consider if this library should be header-only for easier integration

## Validation Steps

When implementing changes:
1. Ensure code compiles without warnings
2. Run all tests (when test suite exists)
3. Verify changes work on target platforms
4. Update README.md with usage examples if adding new features
5. Add appropriate documentation to public APIs

## Key Priorities

1. **Simplicity**: Keep the library focused and easy to use
2. **Correctness**: Date/time calculations must be accurate
3. **Performance**: Optimize for common use cases
4. **Documentation**: Clear examples and API documentation
5. **Testing**: Comprehensive test coverage for date edge cases
