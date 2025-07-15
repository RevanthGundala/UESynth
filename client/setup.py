# Copyright (c) 2025 UESynth Project
# SPDX-License-Identifier: MIT

"""Setup script for the UESynth Python client library."""

from setuptools import find_packages, setup

setup(
    name="uesynth",
    version="0.1.0",
    packages=find_packages(),
    install_requires=[
        "grpcio",
        "grpcio-tools",
    ],
    description="Python client for UESynth Unreal Engine plugin",
    author="UESynth Community",
    license="MIT",
)
