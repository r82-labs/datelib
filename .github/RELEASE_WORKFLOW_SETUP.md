# Release Workflow Setup Guide

This document explains how to set up the automated release workflow for the datelib repository.

## Problem

GitHub Actions has security restrictions that prevent the default `GITHUB_TOKEN` from creating or approving pull requests in repositories with certain security settings enabled. This is a security feature to prevent unauthorized PR creation.

Reference: [GitHub Docs - Permissions for the GITHUB_TOKEN](https://docs.github.com/en/actions/security-guides/automatic-token-authentication#permissions-for-the-github_token)

## Solution

To enable the automated release preparation workflow to create pull requests, you need to create a Personal Access Token (PAT) with the necessary permissions.

## Setup Instructions

### Step 1: Create a Personal Access Token (PAT)

1. Go to GitHub Settings → Developer settings → Personal access tokens → **Tokens (classic)**
   - Direct link: https://github.com/settings/tokens

2. Click **"Generate new token"** → **"Generate new token (classic)"**

3. Configure the token:
   - **Note**: `datelib-release-workflow` (or any descriptive name)
   - **Expiration**: Choose an appropriate expiration (90 days, 1 year, or custom)
   - **Scopes**: Select the following permissions:
     - ✅ `repo` (Full control of private repositories)
       - This includes: `repo:status`, `repo_deployment`, `public_repo`, `repo:invite`, `security_events`
     - ✅ `workflow` (Update GitHub Action workflows)

4. Click **"Generate token"** at the bottom of the page

5. **IMPORTANT**: Copy the token immediately! You won't be able to see it again.

### Step 2: Add the Token as a Repository Secret

1. Go to your repository: https://github.com/r82-labs/datelib

2. Navigate to **Settings** → **Secrets and variables** → **Actions**

3. Click **"New repository secret"**

4. Configure the secret:
   - **Name**: `PAT_TOKEN`
   - **Value**: Paste the token you copied in Step 1

5. Click **"Add secret"**

### Step 3: Verify the Workflow

1. Go to **Actions** tab in your repository

2. Select the **"Prepare Release"** workflow from the left sidebar

3. Click **"Run workflow"** button

4. Select the bump type (patch, minor, or major) and click **"Run workflow"**

5. The workflow should now successfully create a pull request for the version bump

## How It Works

The `release-prepare.yml` workflow now uses:

```yaml
token: ${{ secrets.PAT_TOKEN || secrets.GITHUB_TOKEN }}
```

This means:
- If `PAT_TOKEN` is configured, it will use that token (with full permissions)
- If `PAT_TOKEN` is not configured, it will fall back to `GITHUB_TOKEN` (which may fail with branch protection)

## Security Considerations

1. **Token Rotation**: Consider rotating the PAT periodically for security
2. **Minimal Permissions**: The token only has the minimum required permissions (repo and workflow)
3. **Repository Secrets**: GitHub encrypts secrets and only exposes them to workflows
4. **Fine-grained PAT**: For enhanced security, consider using [Fine-grained Personal Access Tokens](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token#creating-a-fine-grained-personal-access-token) instead of classic tokens

### Using Fine-grained PAT (Recommended for Enhanced Security)

If you prefer using a fine-grained PAT:

1. Go to https://github.com/settings/tokens?type=beta
2. Click **"Generate new token"**
3. Configure:
   - **Token name**: `datelib-release-workflow`
   - **Expiration**: Choose appropriate duration
   - **Repository access**: Select "Only select repositories" → choose `r82-labs/datelib`
   - **Permissions**:
     - Repository permissions:
       - **Contents**: Read and write
       - **Pull requests**: Read and write
       - **Workflows**: Read and write

## Troubleshooting

### Error: "GitHub Actions is not permitted to create or approve pull requests"

**Cause**: The `PAT_TOKEN` secret is not configured, and the workflow is falling back to `GITHUB_TOKEN` which has insufficient permissions.

**Solution**: Follow the setup instructions above to create and configure the `PAT_TOKEN` secret.

### Error: "Resource not accessible by integration"

**Cause**: The PAT token doesn't have the required scopes.

**Solution**: 
1. Verify the token has both `repo` and `workflow` scopes (classic PAT)
2. Or verify it has Contents, Pull requests, and Workflows permissions (fine-grained PAT)

### Pull Request Not Created

**Cause**: Multiple possible causes.

**Solution**: Check the workflow logs:
1. Go to Actions tab
2. Click on the failed workflow run
3. Expand the "Create Pull Request" step
4. Review the error message for specific details

## Alternative Solutions

### Option 1: Repository Settings (Not Recommended)

You can allow GitHub Actions to create PRs by changing repository settings:

1. Go to Repository **Settings** → **Actions** → **General**
2. Scroll to **Workflow permissions**
3. Enable **"Allow GitHub Actions to create and approve pull requests"**

**Note**: This is less secure than using a PAT and may conflict with other security policies.

### Option 2: Manual Release Process

If automated PR creation is not desired:

1. Manually create a branch: `git checkout -b release/vX.Y.Z`
2. Update version in `CMakeLists.txt`
3. Commit and push: `git commit -am "Bump version to X.Y.Z" && git push origin release/vX.Y.Z`
4. Create PR manually through GitHub UI

## Support

For issues with this setup, please:
1. Check the [GitHub Actions documentation](https://docs.github.com/en/actions)
2. Review workflow logs for specific error messages
3. Open an issue in this repository with the workflow run URL
