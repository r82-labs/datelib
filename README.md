# datelib
A C++ library of date utilities

## Development Setup

### Prerequisites
- CMake 3.14 or later
- C++20 compatible compiler (GCC, Clang, or MSVC)
- Python 3.7+ (for pre-commit hooks)

### Pre-commit Hooks

This project uses [pre-commit](https://pre-commit.com/) to automatically format code before commits.

**Installation:**

```bash
# Install pre-commit (one-time setup)
pip install pre-commit

# Install the git hooks (in the repository root)
pre-commit install
```

**Usage:**

Once installed, the hooks will run automatically on `git commit`. To run manually:

```bash
# Run on all files
pre-commit run --all-files

# Run on staged files only
pre-commit run
```

**What gets checked:**
- C++ code formatting with clang-format
- CMake file formatting
- Trailing whitespace removal
- End-of-file fixes
- YAML validation
- Merge conflict detection

### Building the Project

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run tests
cd build && ctest --output-on-failure
```

## Development with GitHub Copilot

This repository uses GitHub Copilot custom agents to assist with development. See [`.github/CUSTOM_AGENTS.md`](.github/CUSTOM_AGENTS.md) for information about:
- Available custom agents (including the Architect agent)
- How to use custom agents with issues and PRs
- Current limitations and workarounds

For project-wide agent instructions, see [`AGENTS.md`](AGENTS.md).

## GitHub Actions Notifications

To learn how to stop receiving email notifications from GitHub Actions workflows, see [`.github/NOTIFICATIONS.md`](.github/NOTIFICATIONS.md).
