# Synchronous Client API

The `UESynthClient` provides a synchronous, blocking API that's perfect for simple scripts, prototyping, and single-shot data capture tasks.

## Overview

The synchronous client offers:
- **Simple, linear execution** - Each call blocks until completion
- **Easy error handling** - Exceptions are raised immediately
- **Straightforward debugging** - Stack traces point to exact issues
- **Perfect for scripting** - Ideal for batch processing and automation

## Getting Started

```python
from uesynth import UESynthClient

# Connect to UESynth
client = UESynthClient()

# Your code here...

# Always disconnect when done
client.disconnect()
```

## Client Configuration

### Basic Connection

```python
# Default connection (localhost:50051)
client = UESynthClient()

# Custom connection
client = UESynthClient(
    host="192.168.1.100",
    port=50052,
    timeout=10.0
)
```

### Advanced Options

```python
client = UESynthClient(
    host="localhost",
    port=50051,
    timeout=5.0,
    max_retries=3,
    enable_ssl=False,
    compression=True,
    grpc_options=[
        ('grpc.keepalive_time_ms', 30000),
        ('grpc.keepalive_timeout_ms', 5000)
    ]
)
```

## Core Methods

### Connection Management

#### `connect()`
Establish connection to the UESynth server.

```python
client = UESynthClient()
client.connect()
```

#### `disconnect()`
Close the connection and clean up resources.

```python
client.disconnect()
```

#### `is_connected()`
Check if the client is currently connected.

```python
if client.is_connected():
    print("Connected to UESynth")
else:
    print("Not connected")
```

#### `ping()`
Test server responsiveness.

```python
try:
    latency = client.ping()
    print(f"Server response time: {latency}ms")
except Exception as e:
    print(f"Server not responding: {e}")
```

## Camera Control

### Position and Rotation

#### `camera.set_location(x, y, z)`
Set the camera position in world coordinates.

```python
# Move camera to specific position
client.camera.set_location(x=100, y=200, z=50)

# Using keyword arguments
client.camera.set_location(x=0, y=0, z=100)
```

**Parameters:**
- `x` (float): X coordinate in world space
- `y` (float): Y coordinate in world space  
- `z` (float): Z coordinate in world space

#### `camera.set_rotation(pitch, yaw, roll)`
Set the camera rotation in degrees.

```python
# Look down at 30 degrees
client.camera.set_rotation(pitch=-30, yaw=0, roll=0)

# Rotate 90 degrees left
client.camera.set_rotation(pitch=0, yaw=90, roll=0)
```

**Parameters:**
- `pitch` (float): Pitch rotation in degrees (-90 to 90)
- `yaw` (float): Yaw rotation in degrees (0 to 360)
- `roll` (float): Roll rotation in degrees (-180 to 180)

#### `camera.get_location()`
Get the current camera position.

```python
location = client.camera.get_location()
print(f"Camera at: {location.x}, {location.y}, {location.z}")
```

**Returns:** `Location` object with `.x`, `.y`, `.z` attributes

#### `camera.get_rotation()`
Get the current camera rotation.

```python
rotation = client.camera.get_rotation()
print(f"Camera rotation: {rotation.pitch}, {rotation.yaw}, {rotation.roll}")
```

**Returns:** `Rotation` object with `.pitch`, `.yaw`, `.roll` attributes

### Camera Settings

#### `camera.set_fov(fov)`
Set the camera field of view.

```python
# Wide angle view
client.camera.set_fov(90.0)

# Narrow telephoto view
client.camera.set_fov(35.0)
```

**Parameters:**
- `fov` (float): Field of view in degrees (10-180)

#### `camera.get_fov()`
Get the current field of view.

```python
current_fov = client.camera.get_fov()
print(f"Current FOV: {current_fov} degrees")
```

## Data Capture

### Image Capture

#### `capture.rgb(width=None, height=None)`
Capture an RGB image from the current camera view.

```python
# Capture at default resolution
rgb_image = client.capture.rgb()

# Capture at specific resolution
rgb_image = client.capture.rgb(width=1920, height=1080)

# Save the image
import cv2
cv2.imwrite("captured_image.png", rgb_image)
```

**Parameters:**
- `width` (int, optional): Image width in pixels
- `height` (int, optional): Image height in pixels

**Returns:** `numpy.ndarray` with shape `(height, width, 3)` and dtype `uint8`

#### `capture.depth(width=None, height=None)`
Capture a depth map from the current camera view.

```python
# Capture depth map
depth_map = client.capture.depth()

# Normalize for visualization
import cv2
depth_normalized = cv2.normalize(depth_map, None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U)
cv2.imwrite("depth_map.png", depth_normalized)
```

**Returns:** `numpy.ndarray` with shape `(height, width)` and dtype `float32`

#### `capture.segmentation(width=None, height=None)`
Capture a segmentation mask with object IDs.

```python
# Capture segmentation
seg_mask = client.capture.segmentation()

# Each pixel value represents an object ID
unique_objects = np.unique(seg_mask)
print(f"Scene contains {len(unique_objects)} objects")
```

**Returns:** `numpy.ndarray` with shape `(height, width)` and dtype `uint32`

#### `capture.normals(width=None, height=None)`
Capture surface normals.

```python
# Capture surface normals
normals = client.capture.normals()

# Normals are in world space, range [-1, 1]
print(f"Normal map shape: {normals.shape}")  # (height, width, 3)
```

**Returns:** `numpy.ndarray` with shape `(height, width, 3)` and dtype `float32`

### Multi-Modal Capture

#### `capture.all_modalities(width=None, height=None)`
Capture all available data types in a single call.

```python
# Efficient multi-modal capture
data = client.capture.all_modalities(width=1024, height=768)

rgb = data['rgb']
depth = data['depth'] 
segmentation = data['segmentation']
normals = data['normals']
```

**Returns:** Dictionary with keys: `'rgb'`, `'depth'`, `'segmentation'`, `'normals'`

## Object Manipulation

### Transform Control

#### `objects.set_location(name, x, y, z)`
Move an object to a specific world position.

```python
# Move a specific object
client.objects.set_location("MyActor", x=100, y=200, z=50)

# Move multiple objects
objects_to_move = ["Car_01", "Car_02", "Car_03"]
for obj_name in objects_to_move:
    client.objects.set_location(obj_name, x=0, y=0, z=100)
```

#### `objects.set_rotation(name, pitch, yaw, roll)`
Set an object's rotation.

```python
# Rotate an object
client.objects.set_rotation("MyActor", pitch=0, yaw=45, roll=0)
```

#### `objects.set_scale(name, x, y, z)`
Set an object's scale.

```python
# Make object twice as large
client.objects.set_scale("MyActor", x=2.0, y=2.0, z=2.0)

# Uniform scaling
client.objects.set_scale("MyActor", x=1.5, y=1.5, z=1.5)
```

#### `objects.get_transform(name)`
Get an object's complete transform information.

```python
transform = client.objects.get_transform("MyActor")
print(f"Location: {transform.location}")
print(f"Rotation: {transform.rotation}")
print(f"Scale: {transform.scale}")
```

### Object Queries

#### `objects.list_all()`
Get a list of all available objects in the scene.

```python
all_objects = client.objects.list_all()
print(f"Scene contains {len(all_objects)} objects:")
for obj in all_objects:
    print(f"  - {obj.name} ({obj.class_name})")
```

#### `objects.find_by_class(class_name)`
Find objects by their Unreal Engine class.

```python
# Find all static mesh actors
static_meshes = client.objects.find_by_class("StaticMeshActor")

# Find all lighting actors  
lights = client.objects.find_by_class("Light")
```

## Scene Control

### Lighting

#### `scene.set_time_of_day(hour, minute=0)`
Control the time of day for dynamic lighting.

```python
# Set to noon
client.scene.set_time_of_day(12, 0)

# Set to sunset
client.scene.set_time_of_day(18, 30)

# Set to midnight
client.scene.set_time_of_day(0, 0)
```

#### `scene.set_sun_intensity(intensity)`
Control the directional light intensity.

```python
# Bright daylight
client.scene.set_sun_intensity(10.0)

# Dim lighting
client.scene.set_sun_intensity(1.0)

# No sun
client.scene.set_sun_intensity(0.0)
```

### Weather and Environment

#### `scene.set_weather(weather_type)`
Set weather conditions.

```python
# Available weather types
client.scene.set_weather("clear")
client.scene.set_weather("cloudy")
client.scene.set_weather("rainy")
client.scene.set_weather("foggy")
client.scene.set_weather("stormy")
```

## Error Handling

The synchronous client uses standard Python exceptions:

```python
from uesynth import UESynthClient, UESynthError

client = UESynthClient()

try:
    # This might fail if object doesn't exist
    client.objects.set_location("NonExistentActor", 0, 0, 0)
    
except UESynthError as e:
    print(f"UESynth error: {e}")
    
except ConnectionError as e:
    print(f"Connection error: {e}")
    
except TimeoutError as e:
    print(f"Request timed out: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
    
finally:
    # Always clean up
    client.disconnect()
```

## Performance Considerations

### Batching Operations

For better performance, minimize round trips:

```python
# ❌ Inefficient - multiple round trips
for i in range(100):
    client.camera.set_location(i, 0, 50)
    image = client.capture.rgb()

# ✅ Better - batch similar operations
positions = [(i, 0, 50) for i in range(100)]
images = []

for pos in positions:
    client.camera.set_location(*pos)
    images.append(client.capture.rgb())
```

### Context Manager

Use the context manager for automatic cleanup:

```python
from uesynth import UESynthClient

with UESynthClient() as client:
    # Client automatically connects
    image = client.capture.rgb()
    # Client automatically disconnects
```

## Complete Example

```python
from uesynth import UESynthClient
import cv2
import numpy as np

def capture_scene_from_multiple_angles():
    """Capture a scene from multiple camera angles."""
    
    with UESynthClient() as client:
        # Define camera positions
        positions = [
            (0, 100, 50),      # Front view
            (100, 0, 50),      # Side view  
            (-100, 0, 50),     # Other side
            (0, -100, 50),     # Back view
            (0, 0, 150)        # Top-down view
        ]
        
        # Capture from each position
        for i, (x, y, z) in enumerate(positions):
            print(f"Capturing view {i+1}/5...")
            
            # Position camera
            client.camera.set_location(x=x, y=y, z=z)
            client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
            
            # Capture all data types
            rgb = client.capture.rgb(width=1920, height=1080)
            depth = client.capture.depth(width=1920, height=1080)
            
            # Save images
            cv2.imwrite(f"view_{i+1}_rgb.png", rgb)
            
            # Normalize and save depth
            depth_norm = cv2.normalize(depth, None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U)
            cv2.imwrite(f"view_{i+1}_depth.png", depth_norm)
            
        print("✅ Multi-angle capture complete!")

if __name__ == "__main__":
    capture_scene_from_multiple_angles()
```

## Next Steps

- Learn about [Asynchronous Client](async-client.md) for high-performance scenarios
- Explore [Camera Controls](camera.md) for advanced camera manipulation
- Check out [Examples](../examples/basic-examples.md) for more usage patterns 