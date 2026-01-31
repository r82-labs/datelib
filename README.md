# datelib
A C++ library of date utilities

## Development

### Code Formatting

This project uses [clang-format](https://clang.llvm.org/docs/ClangFormat.html) to maintain consistent code style. The configuration is defined in `.clang-format`.

**Format your code before committing:**
```bash
# Format all C++ files (after configuring CMake)
cmake --build build --target format

# Check formatting without modifying files
cmake --build build --target format-check
```

**Alternative (without CMake):**
```bash
# Format all C++ files
find src include tests -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec clang-format -i {} +

# Check formatting without modifying files
find src include tests -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec clang-format --dry-run --Werror {} +
```

The CI pipeline automatically checks code formatting on all pull requests and will fail if code is not properly formatted.

## Development with GitHub Copilot

This repository uses GitHub Copilot custom agents to assist with development. See [`.github/CUSTOM_AGENTS.md`](.github/CUSTOM_AGENTS.md) for information about:
- Available custom agents (including the Architect agent)
- How to use custom agents with issues and PRs
- Current limitations and workarounds

For project-wide agent instructions, see [`AGENTS.md`](AGENTS.md).

## GitHub Actions Notifications

To learn how to stop receiving email notifications from GitHub Actions workflows, see [`.github/NOTIFICATIONS.md`](.github/NOTIFICATIONS.md).
