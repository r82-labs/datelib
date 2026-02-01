# GitHub Pages Migration Guide

## Overview

This document explains the migration from the legacy GitHub Pages deployment method to the modern GitHub Actions-based deployment.

## What Changed

The documentation deployment workflow has been updated to use the official GitHub Pages deployment action (`actions/deploy-pages`) instead of the community action (`peaceiris/actions-gh-pages`).

### Old Method (Legacy)
- Used `peaceiris/actions-gh-pages@v4` to push documentation to the `gh-pages` branch
- Required manual configuration in repository Settings → Pages
- Deployment source: "Deploy from a branch" (gh-pages branch)

### New Method (Modern - Recommended)
- Uses official `actions/deploy-pages@v4` for artifact-based deployment
- Automatically configures GitHub Pages on first workflow run
- Deployment source: "GitHub Actions"
- No manual configuration required

## Benefits of the New Method

1. **Automatic Setup**: No need to manually configure GitHub Pages in repository settings
2. **Better Security**: Uses scoped permissions (`pages: write`, `id-token: write`)
3. **Official Support**: Uses GitHub's official deployment action
4. **Improved Visibility**: Deployment status visible in Actions tab and Pages settings
5. **Environment Protection**: Can configure deployment environments and protection rules
6. **No Branch Management**: Doesn't require maintaining a separate `gh-pages` branch

## Migration Impact

### For Repository Maintainers
- **No action required**: The workflow will automatically handle the migration
- The next workflow run will set up GitHub Pages automatically
- The old `gh-pages` branch can be deleted if desired (optional)

### For Documentation Consumers
- **No change**: Documentation URL remains the same: `https://r82-labs.github.io/datelib/`
- Documentation will continue to be updated on each release

## Triggering the Migration

The migration happens automatically when you:
1. Run the "Generate Documentation" workflow manually, OR
2. Publish a new release (which triggers documentation generation)

The workflow will:
- Generate the documentation using Doxygen
- Configure GitHub Pages to use "GitHub Actions" as the source (if not already set)
- Deploy the documentation
- Make it available at the same URL as before

## Verification

After running the workflow, verify the migration succeeded:

1. Go to **Settings** → **Pages**
2. Under "Build and deployment":
   - **Source** should show "GitHub Actions"
   - You should see "Your site is live at https://r82-labs.github.io/datelib/"

## Rollback (If Needed)

If you need to rollback to the old method for any reason:

1. Revert the changes to `.github/workflows/generate-docs.yml`
2. Manually configure GitHub Pages:
   - Go to Settings → Pages
   - Set Source to "Deploy from a branch"
   - Select the `gh-pages` branch
3. Run the workflow again

## Technical Details

### Workflow Changes

**Permissions:**
```yaml
# Old
permissions:
  contents: write

# New
permissions:
  contents: read
  pages: write
  id-token: write
```

**Deployment Steps:**
```yaml
# Old
- name: Deploy to GitHub Pages
  uses: peaceiris/actions-gh-pages@v4
  with:
    github_token: ${{ secrets.GITHUB_TOKEN }}
    publish_dir: ./docs/html
    publish_branch: gh-pages

# New
- name: Setup Pages
  uses: actions/configure-pages@v5

- name: Upload artifact
  uses: actions/upload-pages-artifact@v3
  with:
    path: ./docs/html

- name: Deploy to GitHub Pages
  id: deployment
  uses: actions/deploy-pages@v4
```

**Additional Configuration:**
```yaml
# New: Added environment for deployment tracking
environment:
  name: github-pages
  url: ${{ steps.deployment.outputs.page_url }}

# New: Prevent concurrent deployments
concurrency:
  group: "pages"
  cancel-in-progress: false
```

## Questions?

If you have questions about this migration or encounter any issues, please:
1. Check the [GitHub Pages Setup Guide](.github/GITHUB_PAGES_SETUP.md)
2. Review the [Documentation Workflow Guide](.github/DOCUMENTATION_WORKFLOW.md)
3. Open an issue in the repository
