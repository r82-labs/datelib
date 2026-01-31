# GitHub Copilot Agent Instructions

## Repository Overview
This is a C++ library of date utilities. The repository follows modern C++ best practices and conventions.

## Available Custom Agents

### Architect Agent
**Role:** Strategic Architect & Orchestrator
**Purpose:** Lead Technical Architect and Project Manager that analyzes issues, features, and PRs to delegate work to specialist agents or architect new agent profiles.

**When to use the Architect agent:**
- Breaking down complex issues into implementation tasks
- Analyzing Pull Requests for architectural consistency
- Creating new specialized agent profiles for specific tasks
- High-level project planning and task delegation

**How to invoke:**
Since GitHub Copilot doesn't currently support direct assignment of custom agents to issues via the web UI, you can work with the architect agent in the following ways:

1. **Via Copilot Chat (VS Code, JetBrains, etc.):**
   - Type `@architect` in Copilot Chat to invoke the architect agent
   - Provide context about the issue or task you want analyzed

2. **Via GitHub Copilot CLI:**
   - Use `gh copilot` commands with custom agent selection
   - Reference the architect agent when starting a new session

3. **In Issue Descriptions:**
   - Mention `@copilot` and specify you want architectural analysis
   - Reference this AGENTS.md file for agent context

### C++ Code Quality Reviewer Agent
**Role:** Expert C++20 Code Reviewer
**Purpose:** Ensures highest quality C++20 standards compliance, comprehensive edge case coverage, and rigorous testing practices for all C++ code changes.

**When to use the C++ Code Reviewer agent:**
- Reviewing Pull Requests with C++ code changes
- Validating C++20 standards compliance
- Ensuring comprehensive edge case test coverage
- Checking for memory safety, exception safety, and performance issues
- Verifying code quality before merging

**Key Focus Areas:**
- C++20 feature usage and modern C++ idioms
- Memory safety (RAII, smart pointers, no leaks)
- Exception safety and error handling
- Edge case testing (especially for date/time operations)
- Code clarity, maintainability, and documentation
- Performance optimization opportunities

**How to invoke:**
1. **Via Copilot Chat:**
   - Reference the C++ code reviewer agent when reviewing code
   - Request specific edge case analysis for date/time operations

2. **In Pull Request Descriptions:**
   - Mention `@copilot` and request C++20 code quality review
   - Ask for edge case coverage verification

3. **For Issue Analysis:**
   - Request code quality assessment using the reviewer agent's standards
   - Ask for edge case identification before implementation

## General Project Conventions

When working on this repository, all agents should:
- Follow C++ best practices and modern standards (C++17 or later)
- Ensure code is properly documented
- Write or update tests for any code changes
- Maintain backwards compatibility unless explicitly breaking it
- Follow existing code style and formatting

## Architecture Principles

1. **Modularity:** Keep date utilities focused and single-purpose
2. **Performance:** Optimize for common date operations
3. **Correctness:** Handle edge cases (leap years, time zones, etc.)
4. **Compatibility:** Support multiple platforms and compilers
