# Documentation Workflow Guide

This document explains how the documentation generation workflow works and how to use it.

## Overview

The `generate-docs.yml` workflow automatically generates API documentation using Doxygen and publishes it to GitHub Pages using the official GitHub Pages deployment action.

## Triggers

The workflow can be triggered in two ways:

1. **Automatically**: Called by other workflows (using `workflow_call`)
2. **Manually**: Via GitHub Actions UI (using `workflow_dispatch`)

## Manual Triggering

To manually generate and publish documentation:

1. Go to the **Actions** tab in the repository
2. Select **"Generate Documentation"** from the workflows list on the left
3. Click the **"Run workflow"** button (top right)
4. Select the branch you want to generate documentation from
5. Click **"Run workflow"** to start the process

## Workflow Steps

1. **Checkout code**: Checks out the repository
2. **Install Doxygen and Graphviz**: Installs required documentation tools
3. **Generate documentation**: Runs Doxygen to create HTML documentation in `docs/html/`
4. **Setup Pages**: Configures GitHub Pages settings (automatic)
5. **Upload artifact**: Uploads the generated documentation as a deployment artifact
6. **Deploy to GitHub Pages**: Publishes the documentation to GitHub Pages

## GitHub Pages Configuration

The workflow uses the official `actions/deploy-pages` action, which **automatically configures GitHub Pages** when run for the first time. No manual configuration in repository settings is required.

The documentation will be available at: `https://r82-labs.github.io/datelib/`

## Deployment Method

This workflow uses the modern GitHub Pages deployment approach:
- **Deployment source**: GitHub Actions (configured automatically)
- **Artifact-based deployment**: The documentation is uploaded as a build artifact and then deployed
- **No branch required**: Unlike older methods, this doesn't require a `gh-pages` branch

## Benefits of the Current Setup

1. **Automatic configuration**: GitHub Pages is configured automatically on first run
2. **Improved security**: Uses built-in `GITHUB_TOKEN` with proper permission scoping
3. **Better visibility**: Deployment status is visible in the Actions tab and Settings → Pages
4. **Official support**: Uses GitHub's official deployment action
5. **Environment protection**: Can optionally configure deployment environments and protection rules

## Local Documentation Generation

To generate documentation locally without running the workflow:

```bash
# Install Doxygen and Graphviz (if not already installed)
sudo apt-get install -y doxygen graphviz  # Ubuntu/Debian
brew install doxygen graphviz              # macOS

# Generate documentation
doxygen Doxyfile

# View documentation
open docs/html/index.html  # macOS
xdg-open docs/html/index.html  # Linux
```

## Troubleshooting

### Workflow fails with permission errors

If the workflow fails with permission errors:
1. Go to **Settings** → **Actions** → **General**
2. Under "Workflow permissions", ensure proper permissions are set
3. Verify the workflow has `pages: write` and `id-token: write` permissions
4. Re-run the workflow

### Documentation not updating

If documentation generation succeeds but the site doesn't update:
1. Check the workflow logs for any errors in the deployment step
2. Go to **Settings** → **Pages** to verify the deployment status
3. Wait a few minutes as deployment can take time
4. Clear your browser cache and try again

### First-time deployment issues

If this is the first time running the workflow:
1. The workflow will automatically configure GitHub Pages on first run
2. The initial deployment may take a few extra minutes
3. Check **Settings** → **Pages** to confirm "GitHub Actions" is set as the source
4. If the source is not set, try running the workflow again

## Migration from Old Setup

If you previously used the `gh-pages` branch for deployment:
- The new workflow uses the official GitHub Actions deployment method
- The `gh-pages` branch is no longer needed and can be deleted
- The workflow will automatically migrate to the new deployment method

## Related Documentation

- [GitHub Pages Setup](.github/GITHUB_PAGES_SETUP.md)
- [Release Workflow Setup](.github/RELEASE_WORKFLOW_SETUP.md)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Doxygen Manual](https://www.doxygen.nl/manual/)
