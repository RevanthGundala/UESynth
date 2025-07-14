# Basic Examples

This page provides practical examples for common UESynth usage patterns. These examples are perfect for getting started and understanding the core workflows.

## Simple Image Capture

### Single RGB Capture

```python
from uesynth import UESynthClient
import cv2

# Basic image capture
with UESynthClient() as client:
    # Position camera
    client.camera.set_location(x=0, y=100, z=50)
    client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
    
    # Capture image
    image = client.capture.rgb(width=1920, height=1080)
    
    # Save image
    cv2.imwrite("captured_image.png", image)
    print("✅ Image captured and saved!")
```

### Multi-Modal Capture

```python
from uesynth import UESynthClient
import cv2
import numpy as np

# Capture all data types
with UESynthClient() as client:
    client.camera.set_location(x=0, y=100, z=50)
    
    # Capture all modalities at once
    data = client.capture.all_modalities(width=1024, height=768)
    
    # Save RGB
    cv2.imwrite("rgb.png", data['rgb'])
    
    # Save depth (normalized for visualization)
    depth_norm = cv2.normalize(data['depth'], None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U)
    cv2.imwrite("depth.png", depth_norm)
    
    # Save segmentation
    cv2.imwrite("segmentation.png", data['segmentation'].astype('uint8'))
    
    # Save normals (convert to 0-255 range)
    normals_vis = ((data['normals'] + 1) * 127.5).astype('uint8')
    cv2.imwrite("normals.png", normals_vis)
    
    print("✅ All modalities captured!")
```

## Camera Movement

### Camera Orbit

```python
from uesynth import UESynthClient
import cv2
import math

def camera_orbit_capture():
    """Capture images while orbiting around a central point."""
    
    with UESynthClient() as client:
        center_x, center_y, center_z = 0, 0, 0  # Orbit center
        radius = 200  # Orbit radius
        height = 100  # Camera height
        
        # Capture 36 images (10-degree intervals)
        for i in range(36):
            angle = i * 10 * math.pi / 180  # Convert to radians
            
            # Calculate camera position
            cam_x = center_x + radius * math.cos(angle)
            cam_y = center_y + radius * math.sin(angle)
            cam_z = center_z + height
            
            # Position camera
            client.camera.set_location(x=cam_x, y=cam_y, z=cam_z)
            
            # Look at center
            yaw = math.degrees(angle) + 180  # Look inward
            client.camera.set_rotation(pitch=-15, yaw=yaw, roll=0)
            
            # Capture image
            image = client.capture.rgb(width=1024, height=768)
            cv2.imwrite(f"orbit_{i:02d}.png", image)
            
            print(f"Captured view {i+1}/36")

camera_orbit_capture()
```

### Camera Path

```python
from uesynth import UESynthClient
import cv2

def camera_path_capture():
    """Capture images along a predefined camera path."""
    
    # Define camera path (position and rotation)
    camera_path = [
        {"pos": (0, 0, 100), "rot": (0, 0, 0), "name": "start"},
        {"pos": (100, 100, 100), "rot": (-10, 45, 0), "name": "corner"},
        {"pos": (200, 0, 150), "rot": (-20, 90, 0), "name": "side"},
        {"pos": (100, -100, 200), "rot": (-30, 135, 0), "name": "high"},
        {"pos": (0, 0, 250), "rot": (-45, 180, 0), "name": "top"}
    ]
    
    with UESynthClient() as client:
        for i, waypoint in enumerate(camera_path):
            # Set camera position and rotation
            client.camera.set_location(*waypoint["pos"])
            client.camera.set_rotation(*waypoint["rot"])
            
            # Capture image
            image = client.capture.rgb(width=1280, height=720)
            filename = f"path_{i:02d}_{waypoint['name']}.png"
            cv2.imwrite(filename, image)
            
            print(f"Captured waypoint: {waypoint['name']}")

camera_path_capture()
```

## Object Manipulation

### Object Positioning Dataset

```python
from uesynth import UESynthClient
import cv2

def object_positioning_dataset():
    """Create a dataset with objects in different positions."""
    
    with UESynthClient() as client:
        # Setup camera
        client.camera.set_location(x=0, y=300, z=150)
        client.camera.set_rotation(pitch=-20, yaw=0, roll=0)
        
        # Get all objects in scene
        objects = client.objects.list_all()
        print(f"Found {len(objects)} objects in scene")
        
        # Find a moveable object (e.g., car, cube, etc.)
        target_object = None
        for obj in objects:
            if any(keyword in obj.name.lower() for keyword in ['car', 'cube', 'box', 'sphere']):
                target_object = obj.name
                break
        
        if not target_object:
            print("No suitable object found for manipulation")
            return
        
        print(f"Using object: {target_object}")
        
        # Grid of positions
        positions = []
        for x in range(-2, 3):  # -200 to 200 in steps of 100
            for y in range(-2, 3):
                positions.append((x * 100, y * 100, 0))
        
        for i, (x, y, z) in enumerate(positions):
            # Move object
            client.objects.set_location(target_object, x=x, y=y, z=z)
            
            # Capture scene
            image = client.capture.rgb(width=1024, height=768)
            cv2.imwrite(f"object_pos_{i:02d}_x{x}_y{y}.png", image)
            
            print(f"Position {i+1}/{len(positions)}: ({x}, {y}, {z})")

object_positioning_dataset()
```

### Object Rotation Study

```python
from uesynth import UESynthClient
import cv2

def object_rotation_study():
    """Study object appearance from different rotation angles."""
    
    with UESynthClient() as client:
        # Setup camera
        client.camera.set_location(x=0, y=200, z=100)
        client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
        
        # Find target object
        objects = client.objects.list_all()
        target_object = objects[0].name if objects else "DefaultPawn"
        
        print(f"Rotating object: {target_object}")
        
        # Rotate object through 360 degrees
        for angle in range(0, 360, 30):  # Every 30 degrees
            # Set object rotation
            client.objects.set_rotation(target_object, pitch=0, yaw=angle, roll=0)
            
            # Capture image
            image = client.capture.rgb(width=1024, height=768)
            cv2.imwrite(f"rotation_{angle:03d}.png", image)
            
            print(f"Rotation: {angle}°")

object_rotation_study()
```

## Scene Variations

### Lighting Conditions

```python
from uesynth import UESynthClient
import cv2

def lighting_conditions_dataset():
    """Capture scene under different lighting conditions."""
    
    lighting_settings = [
        {"hour": 6, "intensity": 5.0, "name": "dawn"},
        {"hour": 9, "intensity": 8.0, "name": "morning"},
        {"hour": 12, "intensity": 10.0, "name": "noon"},
        {"hour": 15, "intensity": 8.0, "name": "afternoon"},
        {"hour": 18, "intensity": 4.0, "name": "sunset"},
        {"hour": 22, "intensity": 1.0, "name": "night"}
    ]
    
    with UESynthClient() as client:
        # Setup camera
        client.camera.set_location(x=0, y=150, z=75)
        client.camera.set_rotation(pitch=-10, yaw=0, roll=0)
        
        for setting in lighting_settings:
            # Set lighting
            client.scene.set_time_of_day(setting["hour"])
            client.scene.set_sun_intensity(setting["intensity"])
            
            # Capture scene
            image = client.capture.rgb(width=1920, height=1080)
            cv2.imwrite(f"lighting_{setting['name']}.png", image)
            
            print(f"Captured {setting['name']} lighting")

lighting_conditions_dataset()
```

### Weather Variations

```python
from uesynth import UESynthClient
import cv2

def weather_variations_dataset():
    """Capture scene under different weather conditions."""
    
    weather_conditions = ["clear", "cloudy", "rainy", "foggy", "stormy"]
    
    with UESynthClient() as client:
        # Setup camera
        client.camera.set_location(x=0, y=200, z=100)
        client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
        
        for weather in weather_conditions:
            try:
                # Set weather
                client.scene.set_weather(weather)
                
                # Capture scene
                image = client.capture.rgb(width=1920, height=1080)
                cv2.imwrite(f"weather_{weather}.png", image)
                
                print(f"Captured {weather} weather")
                
            except Exception as e:
                print(f"Weather '{weather}' not supported: {e}")

weather_variations_dataset()
```

## Field of View Studies

### FOV Comparison

```python
from uesynth import UESynthClient
import cv2

def fov_comparison():
    """Compare different field of view settings."""
    
    fov_settings = [
        {"fov": 35, "name": "telephoto"},
        {"fov": 50, "name": "normal"},
        {"fov": 75, "name": "wide"},
        {"fov": 90, "name": "very_wide"},
        {"fov": 120, "name": "ultra_wide"}
    ]
    
    with UESynthClient() as client:
        # Setup camera position
        client.camera.set_location(x=0, y=100, z=50)
        client.camera.set_rotation(pitch=-10, yaw=0, roll=0)
        
        for setting in fov_settings:
            # Set field of view
            client.camera.set_fov(setting["fov"])
            
            # Capture image
            image = client.capture.rgb(width=1920, height=1080)
            cv2.imwrite(f"fov_{setting['fov']:03d}_{setting['name']}.png", image)
            
            print(f"Captured FOV {setting['fov']}° ({setting['name']})")

fov_comparison()
```

## Batch Processing

### Systematic Dataset Creation

```python
from uesynth import UESynthClient
import cv2
import json
import os
from itertools import product

def systematic_dataset_creation():
    """Create a systematic dataset with multiple variables."""
    
    # Create output directory
    os.makedirs("systematic_dataset", exist_ok=True)
    
    # Define parameter ranges
    camera_heights = [50, 100, 150]
    camera_distances = [100, 200, 300]
    camera_angles = [0, 45, 90, 135]
    
    dataset_info = {
        "parameters": {
            "camera_heights": camera_heights,
            "camera_distances": camera_distances, 
            "camera_angles": camera_angles
        },
        "samples": []
    }
    
    with UESynthClient() as client:
        sample_id = 0
        
        # Generate all combinations
        for height, distance, angle in product(camera_heights, camera_distances, camera_angles):
            # Calculate camera position
            import math
            angle_rad = math.radians(angle)
            cam_x = distance * math.cos(angle_rad)
            cam_y = distance * math.sin(angle_rad)
            cam_z = height
            
            # Position camera
            client.camera.set_location(x=cam_x, y=cam_y, z=cam_z)
            client.camera.set_rotation(pitch=-15, yaw=angle + 180, roll=0)
            
            # Capture image
            image = client.capture.rgb(width=1024, height=768)
            
            # Save image
            filename = f"sample_{sample_id:04d}.png"
            cv2.imwrite(f"systematic_dataset/{filename}", image)
            
            # Record metadata
            sample_info = {
                "id": sample_id,
                "filename": filename,
                "parameters": {
                    "camera_height": height,
                    "camera_distance": distance,
                    "camera_angle": angle,
                    "camera_position": {"x": cam_x, "y": cam_y, "z": cam_z}
                }
            }
            dataset_info["samples"].append(sample_info)
            
            print(f"Sample {sample_id}: h={height}, d={distance}, a={angle}°")
            sample_id += 1
    
    # Save dataset metadata
    with open("systematic_dataset/dataset_info.json", 'w') as f:
        json.dump(dataset_info, f, indent=2)
    
    print(f"✅ Created {sample_id} samples")

systematic_dataset_creation()
```

## Error Handling Examples

### Robust Capture with Retry

```python
from uesynth import UESynthClient, UESynthError
import cv2
import time

def robust_capture_with_retry():
    """Demonstrate robust image capture with error handling."""
    
    def capture_with_retry(client, max_retries=3):
        """Capture image with automatic retry on failure."""
        
        for attempt in range(max_retries):
            try:
                # Attempt capture
                image = client.capture.rgb(width=1024, height=768)
                print(f"✅ Capture successful on attempt {attempt + 1}")
                return image
                
            except UESynthError as e:
                print(f"❌ UESynth error on attempt {attempt + 1}: {e}")
                if attempt < max_retries - 1:
                    print(f"Retrying in 1 second...")
                    time.sleep(1)
                else:
                    print(f"Failed after {max_retries} attempts")
                    raise
                    
            except Exception as e:
                print(f"❌ Unexpected error on attempt {attempt + 1}: {e}")
                if attempt < max_retries - 1:
                    time.sleep(1)
                else:
                    raise
    
    try:
        with UESynthClient() as client:
            # Test connection
            if not client.is_connected():
                raise ConnectionError("Not connected to UESynth")
            
            # Setup camera
            client.camera.set_location(x=0, y=100, z=50)
            
            # Capture with retry logic
            image = capture_with_retry(client)
            cv2.imwrite("robust_capture.png", image)
            
    except Exception as e:
        print(f"💥 Final error: {e}")

robust_capture_with_retry()
```

These examples demonstrate the fundamental patterns for using UESynth effectively. Start with these basics and then explore more advanced features as your needs grow.

## Next Steps

- **[Async Streaming Examples](async-streaming.md)** - High-performance capture patterns
- **[Real-time Examples](real-time.md)** - Real-time simulation workflows  
- **[Batch Processing Examples](batch-processing.md)** - Large-scale data generation
- **[API Reference](../api-reference/)** - Complete API documentation 