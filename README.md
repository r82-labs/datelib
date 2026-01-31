# datelib
A C++ library of date utilities

## Documentation

Complete API documentation is available at [https://r82-labs.github.io/datelib/](https://r82-labs.github.io/datelib/). The documentation is automatically generated from code comments using Doxygen and updated with each new release.

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

### SonarQube Code Analysis

This project uses [SonarCloud](https://sonarcloud.io) for continuous code quality and security analysis.

**Setup for Repository Maintainers:**

1. **Create a SonarCloud account** (free for open source projects)
   - Go to [SonarCloud](https://sonarcloud.io)
   - Sign in with your GitHub account

2. **Create a new project** on SonarCloud
   - Import your GitHub repository
   - Note your organization key and project key

3. **Update `sonar-project.properties`** (if needed)
   - Update `sonar.organization` with your SonarCloud organization key
   - Update `sonar.projectKey` with your SonarCloud project key (format: `org_repo`)

4. **Add SonarCloud token to GitHub Secrets:**
   - In SonarCloud, go to **My Account → Security → Generate Token**
   - Copy the generated token
   - In GitHub, go to **Settings → Secrets and variables → Actions**
   - Add a new repository secret named `SONAR_TOKEN` with the token value

5. **Analysis runs automatically:**
   - On every push to `main`/`master` branch
   - On pull requests from the same repository
   - View results at `https://sonarcloud.io/project/overview?id=your_project_key`

**Version Synchronization:**

The project version in `sonar-project.properties` is automatically synchronized with `CMakeLists.txt` during the release process. When a release is prepared using the "Prepare Release" workflow, both files are updated together, ensuring the SonarQube project version stays in sync with the library version.

**For Contributors:**

The SonarQube job will automatically skip if the `SONAR_TOKEN` secret is not available (e.g., for forks). Your pull requests will still pass CI checks without SonarQube analysis.

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

### Generating Documentation Locally

The project uses [Doxygen](https://www.doxygen.nl/) to generate API documentation from code comments.

**Prerequisites:**
```bash
# Install Doxygen and Graphviz
sudo apt-get install -y doxygen graphviz  # Ubuntu/Debian
brew install doxygen graphviz              # macOS
```

**Generate documentation:**
```bash
# Generate HTML documentation in docs/html/
doxygen Doxyfile

# View the documentation
open docs/html/index.html  # macOS
xdg-open docs/html/index.html  # Linux
```

**Note:** The `docs/` directory is gitignored. Documentation is automatically generated and published to [GitHub Pages](https://r82-labs.github.io/datelib/) when a new release is published or when the documentation workflow is manually triggered.

> **First-time setup:** If GitHub Pages has not been configured yet, see [`.github/GITHUB_PAGES_SETUP.md`](.github/GITHUB_PAGES_SETUP.md) for instructions on enabling GitHub Pages for this repository.

## Development with GitHub Copilot

This repository uses GitHub Copilot custom agents to assist with development. See [`.github/CUSTOM_AGENTS.md`](.github/CUSTOM_AGENTS.md) for information about:
- Available custom agents (including the Architect agent)
- How to use custom agents with issues and PRs
- Current limitations and workarounds

For project-wide agent instructions, see [`AGENTS.md`](AGENTS.md).

## GitHub Actions Notifications

To learn how to stop receiving email notifications from GitHub Actions workflows, see [`.github/NOTIFICATIONS.md`](.github/NOTIFICATIONS.md).
