# UESynth: A Synthetic Data Generation Plugin for Unreal Engine

[![Build Status](https://github.com/your-username/uesynth/actions/workflows/ci.yml/badge.svg)](https://github.com/your-username/uesynth/actions)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Overview
UESynth is a powerful and user-friendly Unreal Engine plugin for synthetic data generation, inspired by UnrealCV. It provides a comprehensive API for controlling scenes, manipulating objects, and capturing a wide variety of data modalities for use in computer vision and machine learning research.

## Key Features
-   **Multi-modal Data Capture:** Capture RGB, depth, segmentation masks, surface normals, and more.
-   **Scene and Object Manipulation:** Programmatically control objects, lighting, and materials.
-   **gRPC-based API:** A modern, robust, and easy-to-use API for external clients (Python).
-   **Bidirectional Streaming:** High-performance streaming for real-time simulation and data capture.
-   **Scene Randomization:** Tools for domain randomization to create diverse datasets.
-   **Editor Integration:** User-friendly interface within the Unreal Editor.

## Installation

### Plugin
1.  Download the latest release from the [GitHub Releases](https://github.com/your-username/uesynth/releases) page.
2.  Extract the `UESynth` folder into your Unreal Engine project's `Plugins` directory. If a `Plugins` directory does not exist, create one.

### Python Client
To install the Python client library, run the following command:
```bash
uv pip install uesynth
```

## Quick Start
Here is a short example of how to use the Python client to connect to Unreal Engine, capture an image, and save it.

```python
from uesynth import UESynthClient
import cv2

# Connect to the Unreal Engine instance
client = UESynthClient()

# Set camera location and rotation
client.camera.set_location(x=0, y=100, z=50)
client.camera.set_rotation(pitch=-15, yaw=0, roll=0)

# Capture an RGB image
rgb_image = client.capture.rgb()

# Save the image
cv2.imwrite("rgb_image.png", rgb_image)

# Disconnect from the client
client.disconnect()
```

### New Async Streaming API
For high-performance scenarios requiring thousands of timesteps, use the new async streaming client:

```python
from uesynth import AsyncUESynthClient
import cv2
import asyncio

async def main():
    # Connect to the Unreal Engine instance with async streaming
    client = AsyncUESynthClient()
    await client.connect()

    # Example: High-frequency simulation loop
    for timestep in range(1000):
        # Move camera to new position (non-blocking)
        await client.camera.set_location(x=timestep * 10, y=100, z=50)
        
        # Capture frame (non-blocking)
        await client.capture.rgb()
        
        # Get the latest captured frame
        frame = await client.get_latest_frame()
        if frame is not None:
            cv2.imwrite(f"frame_{timestep:06d}.png", frame)
        
        # Small delay to prevent overwhelming the system
        await asyncio.sleep(0.01)

    # Disconnect from the client
    await client.disconnect()

# Run the async function
asyncio.run(main())
```

### Advanced Async Streaming Example
Here's a more advanced example demonstrating callback-based response handling:

```python
from uesynth import AsyncUESynthClient
import cv2
import asyncio

async def on_image_captured(response):
    """Callback function to handle captured images."""
    if response.HasField('image_response'):
        print(f"Received image: {response.image_response.width}x{response.image_response.height}")

async def main():
    # Connect with async streaming
    client = AsyncUESynthClient()
    await client.connect()

    # Set up camera path
    camera_positions = [
        (0, 100, 50),
        (50, 150, 75),
        (100, 200, 100),
        (-50, 50, 25)
    ]

    for i, (x, y, z) in enumerate(camera_positions):
        # Move camera (non-blocking)
        await client.camera.set_location(x=x, y=y, z=z)
        
        # Capture with callback (non-blocking)
        await client.capture.rgb()
        
        # Move an object while capturing
        await client.objects.set_location("MyObject", x=x/2, y=y/2, z=z/2)

    # Wait a bit for all responses
    await asyncio.sleep(2.0)

    # Get the latest frame
    latest_frame = await client.get_latest_frame()
    if latest_frame is not None:
        cv2.imwrite("latest_capture.png", latest_frame)

    # Disconnect
    await client.disconnect()

# Run the async function
asyncio.run(main())
```

## Performance Comparison

### Synchronous UESynthClient
- **Use case:** Simple scripts, prototyping, single-shot captures
- **Performance:** ~10-50 requests/second depending on network latency
- **Pros:** Simple, synchronous, easy to debug
- **Cons:** High latency accumulation for multiple requests

### Async AsyncUESynthClient
- **Use case:** High-frequency simulations, real-time data collection, batch processing
- **Performance:** ~100-1000+ requests/second with minimal latency
- **Pros:** High throughput, low latency, concurrent operations
- **Cons:** Slightly more complex, asynchronous patterns

## API Reference

### UESynthClient (Synchronous)
- `client.camera.set_location(x, y, z)` - Set camera position (blocking)
- `client.camera.set_rotation(pitch, yaw, roll)` - Set camera rotation (blocking)
- `client.camera.get_location()` - Get camera location (blocking)
- `client.capture.rgb()` - Capture RGB image (blocking, returns numpy array)
- `client.capture.depth()` - Capture depth map (blocking, returns numpy array)
- `client.capture.segmentation()` - Capture segmentation mask (blocking, returns numpy array)
- `client.objects.set_location(name, x, y, z)` - Move object (blocking)
- `client.objects.get_transform(name)` - Get object transform (blocking)

### AsyncUESynthClient (Asynchronous)
- `await client.connect()` - Connect to server and initialize streaming
- `await client.camera.set_location(x, y, z)` - Set camera position (non-blocking, returns request ID)
- `await client.camera.set_rotation(pitch, yaw, roll)` - Set camera rotation (non-blocking, returns request ID)
- `await client.camera.get_location()` - Get camera location (async unary call)
- `await client.capture.rgb()` - Capture RGB image (non-blocking, returns request ID)
- `await client.capture.depth()` - Capture depth map (non-blocking, returns request ID)
- `await client.capture.segmentation()` - Capture segmentation mask (non-blocking, returns request ID)
- `await client.capture.rgb_direct()` - Capture RGB image directly (async unary call, returns numpy array)
- `await client.objects.set_location(name, x, y, z)` - Move object (non-blocking, returns request ID)
- `await client.objects.set_transform_direct(name, x, y, z)` - Set object transform directly (async unary call)
- `await client.get_latest_frame()` - Get most recent captured frame (returns numpy array or None)
- `await client.disconnect()` - Disconnect from server

## Documentation
For more detailed information, please see the full documentation (link to be added).

## Contributing
We welcome contributions from the community! Please read our [CONTRIBUTING.md](CONTRIBUTING.md) file for guidelines on how to contribute.

## License
This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details. 