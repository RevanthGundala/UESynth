# Check Scripts

This directory contains scripts for running various code quality checks locally, similar to Google Quantum Cirq's approach.

## Available Scripts

### `check/format`
Formats Python code using `ruff`.

```bash
# Format all code
./check/format

# Check formatting without making changes
./check/format --check

# Format specific files
./check/format uesynth/client.py
```

### `check/lint`
Runs linting using `ruff`.

```bash
# Run linter
./check/lint

# Run linter with auto-fix
./check/lint --fix

# Lint specific files
./check/lint uesynth/client.py
```

### `check/typecheck`
Runs type checking using `mypy`.

```bash
# Run type checker
./check/typecheck

# Type check specific files
./check/typecheck uesynth/client.py
```

### `check/test`
Runs tests using `pytest`.

```bash
# Run all tests
./check/test

# Run tests with coverage
./check/test --cov

# Run specific test file
./check/test tests/test_client.py
```

### `check/all`
Runs all checks in sequence.

```bash
# Run all checks
./check/all

# Run all checks with auto-fix where possible
./check/all --fix
```

## Pre-commit Hook

To run these checks before every commit, you can create a pre-commit hook:

```bash
#!/bin/sh
# .git/hooks/pre-commit
./check/all
```

Make it executable:
```bash
chmod +x .git/hooks/pre-commit
```

## CI/CD Integration

The GitHub Actions workflow in `.github/workflows/ci.yml` uses these same scripts to ensure consistency between local and CI environments.

## Configuration

All tool configurations are in `client/pyproject.toml`:
- `[tool.ruff]` - Ruff linter and formatter settings
- `[tool.mypy]` - Type checker settings
- `[tool.pytest.ini_options]` - Test runner settings

## Generated Files

All scripts automatically exclude protobuf-generated files (`*_pb2.py`, `*_pb2_grpc.py`) from checks since they are auto-generated and should not be modified manually.

## Why Ruff?

We use Ruff as our primary code quality tool because it:
- Combines linting and formatting in one fast tool
- Supports hundreds of linting rules (including pycodestyle, pyflakes, isort, etc.)
- Is 10-100x faster than traditional tools
- Has excellent error messages and auto-fix capabilities
- Integrates seamlessly with modern Python development workflows 