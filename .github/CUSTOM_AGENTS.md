# Custom Copilot Agents - Usage Guide

## Current Limitation

As of January 2026, GitHub Copilot does **not** support directly assigning custom agents (defined in `.github/agents/*.agent.md`) to issues through the GitHub web UI or API. You can only assign the default `@copilot` agent to issues.

This is a known limitation being tracked in the GitHub Community:
- [Discussion: Ability to add custom agents as the assignee of issues/PRs](https://github.com/orgs/community/discussions/179820)

## Available Custom Agents

### Architect Agent
- **File:** `.github/agents/architect.agent.md`
- **Name:** Strategic Architect & Orchestrator
- **Purpose:** Analyzes issues, features, and PRs to delegate work or architect new agent profiles

### C++ Code Quality Reviewer Agent
- **File:** `.github/agents/cpp-code-reviewer.agent.md`
- **Name:** C++ Code Quality Reviewer
- **Purpose:** Expert C++20 code reviewer ensuring highest quality standards, comprehensive edge case coverage, and rigorous testing practices

## Workarounds

### Option 1: Use AGENTS.md (Recommended)

We've created an `AGENTS.md` file at the repository root. This file provides project-wide instructions and defines role-based agents that Copilot will automatically use when working on issues.

**How it works:**
1. Assign an issue to `@copilot` as usual
2. The Copilot coding agent will automatically read `AGENTS.md` and understand the architect role
3. In your issue description, you can reference the architect agent or specify you want architectural analysis

**Example issue description:**
```markdown
## Issue
We need to refactor the date parsing module.

@copilot Please use the architect agent approach to analyze this issue and create a plan.
```

### Option 2: Use Copilot Chat in Your IDE

1. Open VS Code, JetBrains IDE, or another supported editor
2. Open Copilot Chat
3. Reference the custom agent: `@workspace Can you analyze this using the architect agent principles from .github/agents/architect.agent.md?`

### Option 3: Use GitHub Copilot CLI

```bash
# Start a conversation with context about the architect agent
gh copilot explain --agent-context=".github/agents/architect.agent.md"
```

## Future Support

GitHub is actively working on enabling custom agent assignment to issues. Once available, you will be able to:
- Select custom agents from a dropdown when assigning issues
- Use the API to assign specific custom agents to issues
- Automate agent assignment based on issue labels or templates

## For Repository Maintainers

### How Custom Agents Are Defined

Custom agents are markdown files with YAML frontmatter in `.github/agents/`:

```yaml
---
name: Agent Name
description: "Agent description"
target: github-copilot
infer: true
---

# Agent Instructions

Your agent's detailed instructions go here...
```

**Key fields:**
- `name`: Display name of the agent
- `description`: Brief summary of the agent's purpose (required)
- `target`: Set to `github-copilot` for coding agents
- `infer`: When `true`, Copilot can auto-select this agent based on context

### Testing Your Custom Agent

Even though you can't assign custom agents to issues directly, you can test them:

1. **Via IDE Copilot Chat:** Reference the agent in your chat conversations
2. **Via AGENTS.md:** Include instructions from the custom agent in the root AGENTS.md file
3. **Via Issue Context:** Mention the agent and its purpose in issue descriptions

## Additional Resources

- [GitHub Docs: Creating Custom Agents](https://docs.github.com/en/copilot/how-tos/use-copilot-agents/coding-agent/create-custom-agents)
- [GitHub Docs: Custom Agents Configuration](https://docs.github.com/en/copilot/reference/custom-agents-configuration)
- [How to Write a Great AGENTS.md](https://github.blog/ai-and-ml/github-copilot/how-to-write-a-great-agents-md-lessons-from-over-2500-repositories/)
