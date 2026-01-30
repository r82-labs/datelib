# Branch Protection Policy

This repository enforces branch protection on the `main` branch to ensure code quality and security.

> **⚠️ Important for Solo Maintainers**: If you're the only code owner and seeing "Still needs 1 approver" after approving your own PR, see the [troubleshooting section](#important-note-about-self-approvals) below. GitHub prevents self-approval when CODEOWNERS review is required.

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

## Important Note About Self-Approvals

⚠️ **GitHub's security model prevents you from approving your own pull requests** when "Require review from Code Owners" is enabled, even if you are listed as a code owner in the CODEOWNERS file.

### Why Does This Happen?

When branch protection is configured with both:
- "Require approvals: 1" 
- "Require review from Code Owners"

GitHub requires at least 1 approval from someone **other than the PR author**. This is an intentional security feature to prevent authors from merging their own code without independent review.

### For Solo Maintainers

If you are the sole maintainer of this repository (@r82labs-ronan), you have several options:

#### Option 1: Temporarily Disable CODEOWNERS Review (Recommended for Solo Projects)
1. Go to **Settings** → **Branches** → **Branch protection rules** for `main`
2. **Uncheck** "Require review from Code Owners"
3. Keep "Require approvals: 1" enabled
4. This allows you to approve your own PRs while still requiring explicit approval

#### Option 2: Add a Second Approver
- Add another GitHub user or a bot account to the CODEOWNERS file
- This provides actual independent review before merging

#### Option 3: Use Administrator Bypass (Not Recommended)
- Repository administrators can force-merge PRs
- This bypasses all protection rules and should be used sparingly

### Troubleshooting: "Still needs 1 approver" Message

**Symptom**: After approving your own PR, GitHub still shows "This pull request is blocked: 1 approving review required."

**Cause**: You are the PR author and also the only code owner. GitHub does not count your approval.

**Solution**: See "For Solo Maintainers" options above.

## Questions?

If you have questions about the branch protection policy, please contact the repository administrators.
