# datelib
A C++ library of date utilities

## Releases

### Downloading Releases

Pre-built releases are available on the [GitHub Releases page](https://github.com/datelib/datelib/releases). Each release includes:
- Compiled library files (`.so` for Linux)
- Public header files
- LICENSE file
- Installation instructions

### Using Release Artifacts

1. **Download the release archive** for your platform (e.g., `datelib-1.0.0-linux-x86_64.tar.gz`)

2. **Extract the archive:**
   ```bash
   tar -xzf datelib-1.0.0-linux-x86_64.tar.gz
   cd datelib-1.0.0
   ```

3. **Install to your system** (optional):
   ```bash
   sudo cp -r include/datelib /usr/local/include/
   sudo cp lib/libdatelib.so* /usr/local/lib/
   sudo ldconfig
   ```

4. **Use in your CMake project:**
   ```cmake
   # If installed system-wide
   find_library(DATELIB_LIBRARY datelib)
   target_link_libraries(your_target PRIVATE ${DATELIB_LIBRARY})
   
   # Or specify the path directly
   target_include_directories(your_target PRIVATE /path/to/datelib/include)
   target_link_libraries(your_target PRIVATE /path/to/datelib/lib/libdatelib.so)
   ```

### Creating a Release

Releases are created through GitHub Actions using semantic versioning (MAJOR.MINOR.PATCH).

> **⚠️ First-time setup required:** If this is your first time running the release workflow, you need to configure a Personal Access Token (PAT). See [`.github/RELEASE_WORKFLOW_SETUP.md`](.github/RELEASE_WORKFLOW_SETUP.md) for detailed setup instructions.

**To create a new release:**

1. **Trigger the release preparation:**
   - Go to the **Actions** tab in the GitHub repository
   - Select the **Prepare Release** workflow
   - Click **Run workflow**
   - Choose the version bump type:
     - **patch** (1.0.0 → 1.0.1): Bug fixes and minor changes
     - **minor** (1.0.0 → 1.1.0): New features, backwards compatible
     - **major** (1.0.0 → 2.0.0): Breaking changes
   - Click **Run workflow**

2. **Review and merge the release PR:**
   - The workflow will create a Pull Request with the version bump
   - Review the changes in the PR
   - Merge the PR to `main`

3. **Automatic release publication:**
   - After merging, the **Publish Release** workflow runs automatically
   - It will:
     - Create and push a git tag (e.g., `v1.0.1`)
     - Build the library
     - Package artifacts (library + headers + LICENSE)
     - Create a GitHub release with downloadable artifacts

> **Why a two-step process?**
> This approach provides a safety checkpoint where you can review the version bump before it's published. If you need to include additional changes or fix issues, you can do so before merging the release PR.

### Versioning Strategy

This project follows [Semantic Versioning](https://semver.org/):

- **MAJOR** version: Incompatible API changes
- **MINOR** version: Backwards-compatible functionality additions
- **PATCH** version: Backwards-compatible bug fixes

The version is defined in `CMakeLists.txt` and serves as the single source of truth.

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
