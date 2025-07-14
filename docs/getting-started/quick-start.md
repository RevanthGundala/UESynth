# Quick Start

This guide will help you get started with UESynth in just a few minutes. We'll cover the basic workflow for capturing synthetic data from Unreal Engine.

## Prerequisites

- UESynth plugin installed and enabled in your Unreal Engine project
- Python client library installed (`uv pip install uesynth`)
- Unreal Engine project running with the plugin active

## Your First Capture

### Step 1: Start Unreal Engine

1. Open your Unreal Engine project with UESynth enabled
2. Load a scene or use the default level
3. Press Play or ensure the game is running
4. Check the Output Log for "UESynth: gRPC server started on port 50051"

### Step 2: Connect with Python

Create a new Python script and add the following code:

```python
from uesynth import UESynthClient
import cv2
import numpy as np

# Connect to the running Unreal Engine instance
client = UESynthClient()
print("✅ Connected to UESynth!")

# Check if connection is successful
if client.is_connected():
    print("🎮 UE instance is responsive")
else:
    print("❌ Failed to connect")
    exit(1)
```

### Step 3: Position the Camera

```python
# Set camera to a good viewing position
client.camera.set_location(x=0, y=100, z=50)
client.camera.set_rotation(pitch=-15, yaw=0, roll=0)

# Verify the camera position
location = client.camera.get_location()
print(f"📍 Camera position: {location}")
```

### Step 4: Capture Your First Image

```python
# Capture an RGB image
rgb_image = client.capture.rgb()
print(f"📷 Captured image: {rgb_image.shape}")

# Save the image
cv2.imwrite("my_first_capture.png", rgb_image)
print("💾 Image saved as 'my_first_capture.png'")
```

### Step 5: Capture Additional Data Types

```python
# Capture depth information
depth_map = client.capture.depth()
print(f"🔍 Depth map: {depth_map.shape}")

# Normalize depth for visualization
depth_normalized = cv2.normalize(depth_map, None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U)
cv2.imwrite("depth_map.png", depth_normalized)

# Capture segmentation mask
segmentation = client.capture.segmentation()
print(f"🎯 Segmentation: {segmentation.shape}")
cv2.imwrite("segmentation.png", segmentation)
```

### Step 6: Clean Up

```python
# Always disconnect when done
client.disconnect()
print("👋 Disconnected from UESynth")
```

## Complete Example Script

Here's the complete script you can run:

```python
from uesynth import UESynthClient
import cv2
import numpy as np

def main():
    # Connect to UESynth
    client = UESynthClient()
    
    try:
        print("🚀 Starting UESynth quick start example...")
        
        # Position camera
        client.camera.set_location(x=0, y=100, z=50)
        client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
        
        # Capture multiple data types
        print("📷 Capturing RGB image...")
        rgb_image = client.capture.rgb()
        cv2.imwrite("rgb_capture.png", rgb_image)
        
        print("🔍 Capturing depth map...")
        depth_map = client.capture.depth()
        depth_normalized = cv2.normalize(depth_map, None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U)
        cv2.imwrite("depth_capture.png", depth_normalized)
        
        print("🎯 Capturing segmentation...")
        segmentation = client.capture.segmentation()
        cv2.imwrite("segmentation_capture.png", segmentation)
        
        print("✅ Quick start complete! Check the saved images.")
        
    except Exception as e:
        print(f"❌ Error: {e}")
    
    finally:
        # Always disconnect
        client.disconnect()
        print("👋 Disconnected successfully")

if __name__ == "__main__":
    main()
```

## What's Next?

Now that you've captured your first synthetic data, you can:

- **Learn about object manipulation:** [Basic Usage Guide](../guides/basic-usage.md)
- **Explore high-performance capture:** [Async Streaming Examples](../examples/async-streaming.md)
- **Set up automated data collection:** [Data Collection Workflows](../guides/data-collection.md)
- **Optimize for performance:** [Performance Guide](../performance/optimization.md)

## Troubleshooting

### Common Issues

**Connection Failed**
- Ensure Unreal Engine is running and the game/PIE mode is active
- Check that port 50051 is not blocked by firewall
- Verify the plugin is enabled in the Plugins menu

**Empty or Black Images**
- Make sure the camera is positioned with a clear view
- Check that lighting exists in your scene
- Verify the camera is not inside an object

**Import Errors**
- Reinstall the Python client: `uv pip install --force-reinstall uesynth`
- Check Python version compatibility (3.8+)

For more detailed troubleshooting, see our [FAQ](../guides/faq.md) or create an issue on GitHub. 