# Branch Protection Policy

This repository enforces branch protection on the `main` branch to ensure code quality and security.

## Protection Rules

The following rules should be enforced for the `main` branch:

1. **Required Pull Request Reviews**: At least **1 approving review** is required before merging
2. **Code Owners Review**: Reviews from code owners (defined in `.github/CODEOWNERS`) are required
3. **No Direct Pushes**: Changes must be made through pull requests
4. **Automatic Branch Deletion**: Merged branches are automatically deleted to keep the repository clean

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

## Configuring Automatic Branch Deletion (Repository Administrator Required)

To automatically delete branches after pull requests are merged:

1. Go to **Settings** → **General**
2. Scroll down to the **"Pull Requests"** section
3. Enable: ✅ **Automatically delete head branches**
4. This setting will automatically delete feature branches after their pull requests are merged

**Note**: This is a repository-level setting and affects all pull requests. Deleted branches can still be restored if needed from the closed PR page.

## Developer Workflow

1. Create a feature branch from `main`
2. Make your changes and commit them
3. Push your branch and create a pull request to `main`
4. Wait for at least 1 approval from a code owner
5. Once approved and all checks pass, merge the pull request

## Questions?

If you have questions about the branch protection policy, please contact the repository administrators.
