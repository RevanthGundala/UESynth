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

"""Setup script for the UESynth Python client library."""

from setuptools import setup, find_packages

setup(
    name='uesynth',
    version='0.1.0',
    packages=find_packages(),
    install_requires=[
        'grpcio',
        'grpcio-tools',
    ],
    description='Python client for UESynth Unreal Engine plugin',
    author='UESynth Community',
    license='GPL-3.0',
)
