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

"""Example demonstrating the difference between UESynthClient and AsyncUESynthClient.
This shows how to use both clients for different use cases.
"""

import asyncio
import time

import cv2
import numpy as np

from uesynth import AsyncUESynthClient, UESynthClient


def synchronous_example():
    """Example using the synchronous UESynthClient."""
    print("=== Synchronous UESynthClient Example ===")

    # Connect to the Unreal Engine instance
    client = UESynthClient()

    # Simple operations - blocking calls
    print("Setting camera location...")
    client.camera.set_location(x=0, y=100, z=50)

    print("Setting camera rotation...")
    client.camera.set_rotation(pitch=-15, yaw=0, roll=0)

    print("Capturing RGB image...")
    rgb_image = client.capture.rgb()
    print(f"Captured image shape: {rgb_image.shape}")

    # Save the image
    cv2.imwrite("sync_image.png", rgb_image)
    print("Image saved as sync_image.png")

    # Disconnect
    client.disconnect()
    print("Disconnected from server")


async def async_example():
    """Example using the async AsyncUESynthClient."""
    print("\n=== Async AsyncUESynthClient Example ===")

    # Connect to the Unreal Engine instance
    client = AsyncUESynthClient()
    await client.connect()

    # High-performance streaming operations
    print("Starting high-frequency simulation...")

    start_time = time.time()
    num_frames = 100

    for frame_idx in range(num_frames):
        # Move camera in a circle (non-blocking)
        x = 100 * np.cos(frame_idx * 0.1)
        y = 100 * np.sin(frame_idx * 0.1) + 100
        z = 50

        await client.camera.set_location(x=x, y=y, z=z)

        # Capture frame (non-blocking)
        await client.capture.rgb()

        # You can also use direct capture for immediate results
        if frame_idx % 10 == 0:  # Every 10th frame
            direct_image = await client.capture.rgb_direct()
            cv2.imwrite(f"async_frame_{frame_idx:03d}.png", direct_image)

        # Small delay to prevent overwhelming the system
        await asyncio.sleep(0.01)

    # Get the latest frame from streaming
    latest_frame = await client.get_latest_frame()
    if latest_frame is not None:
        cv2.imwrite("async_latest.png", latest_frame)
        print(f"Latest frame shape: {latest_frame.shape}")

    end_time = time.time()
    fps = num_frames / (end_time - start_time)
    print(
        f"Processed {num_frames} frames in {end_time - start_time:.2f}s ({fps:.1f} FPS)"
    )

    # Disconnect
    await client.disconnect()
    print("Disconnected from server")


async def mixed_operations_example():
    """Example showing mixed streaming and unary operations."""
    print("\n=== Mixed Operations Example ===")

    client = AsyncUESynthClient()
    await client.connect()

    # Use unary call to get camera location
    camera_transform = await client.camera.get_location()
    print(f"Current camera location: {camera_transform}")

    # Use streaming for rapid position changes
    positions = [(0, 100, 50), (50, 150, 75), (100, 200, 100)]

    for i, (x, y, z) in enumerate(positions):
        # Non-blocking streaming call
        await client.camera.set_location(x=x, y=y, z=z)

        # Non-blocking capture
        await client.capture.rgb()

        # Direct unary call for object manipulation
        await client.objects.set_transform_direct(
            f"Object_{i}", x=x / 2, y=y / 2, z=z / 2
        )

    await client.disconnect()
    print("Mixed operations completed")


if __name__ == "__main__":
    # Run synchronous example
    synchronous_example()

    # Run async examples
    asyncio.run(async_example())
    asyncio.run(mixed_operations_example())
