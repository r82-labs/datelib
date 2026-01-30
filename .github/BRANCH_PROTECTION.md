# Branch Protection Policy

This repository enforces branch protection on the `main` branch to ensure code quality and security.

## Protection Rules

The following rules should be enforced for the `main` branch:

1. **Required Pull Request Reviews**: At least **1 approving review** is required before merging
2. **Code Owners Review**: Reviews from code owners (defined in `.github/CODEOWNERS`) are required
3. **No Direct Pushes**: Changes must be made through pull requests

## How This Works

### CODEOWNERS File
The `.github/CODEOWNERS` file defines who is responsible for code in this repository. When branch protection is enabled with "Require review from Code Owners", these individuals or teams are automatically requested for review on pull requests.

## Configuring Branch Protection (Repository Administrator Required)

Branch protection must be configured by a repository administrator through GitHub's repository settings. Follow these steps:

1. Go to **Settings** → **Branches** → **Branch protection rules**
2. Click **Add rule** or **Add branch protection rule**
3. In "Branch name pattern", enter: `main`
4. Enable the following settings:
   - ✅ **Require a pull request before merging**
     - ✅ **Require approvals**: Set to at least **1**
     - ✅ **Require review from Code Owners**
   - ✅ **Do not allow bypassing the above settings** (recommended)
5. Click **Create** or **Save changes**

## Developer Workflow

1. Create a feature branch from `main`
2. Make your changes and commit them
3. Push your branch and create a pull request to `main`
4. Wait for at least 1 approval from a code owner
5. Once approved and all checks pass, merge the pull request

## Questions?

If you have questions about the branch protection policy, please contact the repository administrators.
