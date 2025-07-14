#!/usr/bin/env python3
# Copyright (C) 2025 UESynth Project
# SPDX-License-Identifier: GPL-3.0-or-later

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

"""Test runner script for UESynth client tests.
Demonstrates running both sync and async tests with the new architecture.
"""

import subprocess
import sys
from pathlib import Path


def run_command(cmd: list[str], description: str) -> bool:
    """Run a command and return whether it succeeded."""
    print(f"\n🔍 {description}")
    print(f"Running: {' '.join(cmd)}")

    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        print(f"✅ {description} - SUCCESS")
        if result.stdout:
            print(f"Output:\n{result.stdout}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ {description} - FAILED")
        if e.stdout:
            print(f"Stdout:\n{e.stdout}")
        if e.stderr:
            print(f"Stderr:\n{e.stderr}")
        return False


def main():
    """Main test runner function."""
    print("🚀 UESynth Client Test Runner")
    print("=" * 50)

    # Change to the client directory
    client_dir = Path(__file__).parent
    print(f"Working directory: {client_dir}")

    # Install dependencies
    print("\n📦 Installing dependencies...")
    if not run_command(["uv", "sync", "--group", "dev"], "Installing dev dependencies"):
        print("❌ Failed to install dependencies. Make sure uv is installed.")
        return 1

    # Run type checking with basedpyright
    print("\n🔍 Running type checking with basedpyright...")
    run_command(["uv", "run", "basedpyright", "uesynth/", "tests/"], "Type checking")

    # Run linting with ruff
    print("\n🧹 Running linting with ruff...")
    run_command(["uv", "run", "ruff", "check", "uesynth/", "tests/"], "Linting")

    # Run formatting check with ruff
    print("\n📐 Checking formatting with ruff...")
    run_command(
        ["uv", "run", "ruff", "format", "--check", "uesynth/", "tests/"],
        "Format checking",
    )

    # Run all tests
    print("\n🧪 Running all tests...")
    if not run_command(["uv", "run", "pytest", "tests/", "-v"], "Running all tests"):
        return 1

    # Run only sync tests
    print("\n🔄 Running synchronous tests only...")
    run_command(
        ["uv", "run", "pytest", "tests/", "-v", "-k", "TestUESynthClient"], "Sync tests"
    )

    # Run only async tests
    print("\n⚡ Running asynchronous tests only...")
    run_command(
        ["uv", "run", "pytest", "tests/", "-v", "-k", "TestAsyncUESynthClient"],
        "Async tests",
    )

    # Run with coverage
    print("\n📊 Running tests with coverage...")
    run_command(
        ["uv", "run", "pytest", "tests/", "--cov=uesynth", "--cov-report=term-missing"],
        "Coverage report",
    )

    print("\n🎉 Test run complete!")
    print("=" * 50)

    return 0


if __name__ == "__main__":
    sys.exit(main())
