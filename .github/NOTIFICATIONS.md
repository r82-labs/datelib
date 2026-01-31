# GitHub Actions Email Notifications - How to Disable

## Overview
GitHub sends email notifications for Actions workflows based on your personal or organization notification settings. These notifications cannot be disabled directly in the workflow YAML files - they must be configured at the GitHub account level.

## Solutions to Stop Email Notifications

### Option 1: Configure Personal GitHub Notification Settings (Recommended)

1. Go to **GitHub Settings** → **Notifications** → **Actions**
   - URL: https://github.com/settings/notifications

2. Under "Actions" section, uncheck or configure:
   - ✅ **Uncheck "Send notifications for failed workflows only"**
   - ✅ **Uncheck "Send notifications for all workflows"**

3. Alternatively, customize notification delivery:
   - Choose "Only participating or @mentions" instead of "All activity"
   - This reduces notifications to only when you're directly involved

### Option 2: Repository Watch Settings

1. Navigate to this repository: https://github.com/r82labs/datelib

2. Click the **"Watch"** or **"Unwatch"** button at the top

3. Select one of these options:
   - **Participating and @mentions** - Only notified when participating or mentioned
   - **Ignore** - Never be notified (not recommended for maintainers)
   - **Custom** - Fine-tune what types of events trigger notifications
     - Uncheck "Actions" to stop all Actions notifications for this repo

### Option 3: Email Filtering (Temporary Solution)

If you still want to keep notifications but reduce email clutter:

1. Set up an email filter to automatically archive/label GitHub Actions emails:
   - **Gmail**: Create a filter for emails containing `[r82labs/datelib]` and `workflow run`
   - **Outlook**: Create a rule to move these emails to a specific folder
   - **Other email clients**: Use similar filtering capabilities

Example Gmail filter criteria:
```
from:(notifications@github.com)
subject:([r82labs/datelib])
subject:(workflow run OR Actions)
```

### Option 4: Organization-Level Settings (For Organization Owners)

If this is an organization repository and you're an owner:

1. Go to **Organization Settings** → **Code, planning, and automation** → **Actions** → **General**

2. Configure default notification settings for all members

## Understanding GitHub Actions Notifications

GitHub Actions notifications are sent in these scenarios:
- ✅ Workflow run **fails** (most common)
- ✅ Workflow run **succeeds** after previous failure
- ✅ You're explicitly **@mentioned** in a workflow or commit
- ✅ You **manually triggered** the workflow

## Current Workflow Configuration

This repository has the following workflow(s):
- **build-and-test.yml** - Runs on push to main/master and pull requests

The workflow does not contain any notification configuration because:
- GitHub Actions workflows **do not support email notification configuration** in YAML
- All notification settings are managed through GitHub's web UI

## Verification

After changing your settings:

1. Make a test commit to trigger the workflow
2. Check your email inbox
3. If you still receive notifications, verify:
   - Your GitHub notification settings are saved
   - You're checking the correct email address associated with your GitHub account
   - Email filters aren't interfering with the test

## Additional Resources

- [GitHub Docs: About Notifications](https://docs.github.com/en/account-and-profile/managing-subscriptions-and-notifications-on-github/setting-up-notifications/about-notifications)
- [GitHub Docs: Configuring Notifications](https://docs.github.com/en/account-and-profile/managing-subscriptions-and-notifications-on-github/setting-up-notifications/configuring-notifications)
- [GitHub Actions Notifications](https://docs.github.com/en/actions/monitoring-and-troubleshooting-workflows/notifications-for-workflow-runs)

## Summary

**No code changes are needed in this repository.** Email notifications are controlled entirely through GitHub's web interface settings. Follow **Option 1** above to stop receiving email notifications for GitHub Actions in this repository.
