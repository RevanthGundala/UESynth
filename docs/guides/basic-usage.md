# Basic Usage

This guide covers the fundamental concepts and common workflows for using UESynth effectively in your computer vision and machine learning projects.

## Core Concepts

### 1. Client Types

UESynth provides two client types for different use cases:

```python
# Synchronous client - simple, blocking operations
from uesynth import UESynthClient
client = UESynthClient()

# Asynchronous client - high-performance, non-blocking
from uesynth import AsyncUESynthClient
client = AsyncUESynthClient()
```

**When to use each:**
- **Sync Client**: Prototyping, simple scripts, single-shot captures
- **Async Client**: Real-time simulations, batch processing, high-throughput scenarios

### 2. Data Modalities

UESynth can capture multiple types of data from your Unreal Engine scene:

```python
# RGB images - standard color images
rgb_image = client.capture.rgb()

# Depth maps - distance from camera to objects
depth_map = client.capture.depth()

# Segmentation masks - object instance IDs
segmentation = client.capture.segmentation()

# Surface normals - object surface orientations
normals = client.capture.normals()

# All at once - efficient multi-modal capture
data = client.capture.all_modalities()
```

## Common Workflows

### 1. Single Image Capture

The simplest workflow - capture one image:

```python
from uesynth import UESynthClient
import cv2

with UESynthClient() as client:
    # Position camera
    client.camera.set_location(x=0, y=100, z=50)
    client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
    
    # Capture and save
    image = client.capture.rgb(width=1920, height=1080)
    cv2.imwrite("captured_image.png", image)
```

### 2. Multi-View Capture

Capture the same scene from multiple viewpoints:

```python
from uesynth import UESynthClient
import cv2

def multi_view_capture():
    with UESynthClient() as client:
        # Define viewpoints
        viewpoints = [
            {"pos": (0, 200, 100), "rot": (0, 0, 0), "name": "front"},
            {"pos": (200, 0, 100), "rot": (0, 90, 0), "name": "right"},
            {"pos": (0, -200, 100), "rot": (0, 180, 0), "name": "back"},
            {"pos": (-200, 0, 100), "rot": (0, -90, 0), "name": "left"},
            {"pos": (0, 0, 300), "rot": (-90, 0, 0), "name": "top"}
        ]
        
        for i, view in enumerate(viewpoints):
            # Position camera
            client.camera.set_location(*view["pos"])
            client.camera.set_rotation(*view["rot"])
            
            # Capture RGB and depth
            rgb = client.capture.rgb()
            depth = client.capture.depth()
            
            # Save with descriptive names
            cv2.imwrite(f"view_{view['name']}_rgb.png", rgb)
            
            # Normalize depth for visualization
            depth_norm = cv2.normalize(depth, None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U)
            cv2.imwrite(f"view_{view['name']}_depth.png", depth_norm)
            
            print(f"Captured view {i+1}/{len(viewpoints)}: {view['name']}")

multi_view_capture()
```

### 3. Object Manipulation Workflow

Move objects around and capture the results:

```python
from uesynth import UESynthClient
import cv2

def object_manipulation_dataset():
    with UESynthClient() as client:
        # Get list of objects in scene
        objects = client.objects.list_all()
        print(f"Found {len(objects)} objects in scene")
        
        # Find a specific object (e.g., a car)
        car_objects = [obj for obj in objects if "car" in obj.name.lower()]
        
        if not car_objects:
            print("No car objects found!")
            return
        
        car_name = car_objects[0].name
        print(f"Using object: {car_name}")
        
        # Position camera to view the scene
        client.camera.set_location(x=0, y=300, z=150)
        client.camera.set_rotation(pitch=-20, yaw=0, roll=0)
        
        # Generate dataset with object in different positions
        positions = [
            (0, 0, 0),      # Center
            (100, 0, 0),    # Right
            (-100, 0, 0),   # Left
            (0, 100, 0),    # Forward
            (0, -100, 0),   # Backward
        ]
        
        for i, (x, y, z) in enumerate(positions):
            # Move object
            client.objects.set_location(car_name, x=x, y=y, z=z)
            
            # Capture all modalities
            data = client.capture.all_modalities(width=1024, height=768)
            
            # Save each modality
            cv2.imwrite(f"car_pos_{i}_rgb.png", data['rgb'])
            cv2.imwrite(f"car_pos_{i}_depth.png", 
                       cv2.normalize(data['depth'], None, 0, 255, cv2.NORM_MINMAX, dtype=cv2.CV_8U))
            cv2.imwrite(f"car_pos_{i}_seg.png", data['segmentation'].astype('uint8'))
            
            print(f"Captured position {i+1}/{len(positions)}")

object_manipulation_dataset()
```

### 4. Time-of-Day Dataset

Create a dataset with different lighting conditions:

```python
from uesynth import UESynthClient
import cv2

def time_of_day_dataset():
    with UESynthClient() as client:
        # Position camera
        client.camera.set_location(x=0, y=200, z=100)
        client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
        
        # Different times of day
        time_settings = [
            {"hour": 6, "name": "dawn"},
            {"hour": 9, "name": "morning"},
            {"hour": 12, "name": "noon"},
            {"hour": 15, "name": "afternoon"},
            {"hour": 18, "name": "sunset"},
            {"hour": 21, "name": "night"}
        ]
        
        for time_setting in time_settings:
            # Set time of day
            client.scene.set_time_of_day(time_setting["hour"])
            
            # Capture image
            image = client.capture.rgb(width=1920, height=1080)
            
            # Save with time label
            filename = f"scene_{time_setting['name']}.png"
            cv2.imwrite(filename, image)
            
            print(f"Captured {time_setting['name']} scene")

time_of_day_dataset()
```

## Data Processing Patterns

### 1. Real-time Processing

Process frames as they're captured:

```python
import cv2
import numpy as np
from uesynth import UESynthClient

def real_time_processing():
    with UESynthClient() as client:
        # Setup camera
        client.camera.set_location(x=0, y=100, z=50)
        
        for i in range(100):
            # Move camera slightly each frame
            client.camera.set_location(x=i*2, y=100, z=50)
            
            # Capture frame
            frame = client.capture.rgb(width=640, height=480)
            
            # Process immediately (example: edge detection)
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            edges = cv2.Canny(gray, 100, 200)
            
            # Save processed result
            cv2.imwrite(f"processed_frame_{i:03d}.png", edges)
            
            print(f"Processed frame {i+1}/100")

real_time_processing()
```

### 2. Batch Processing

Collect data first, then process:

```python
import cv2
import numpy as np
from uesynth import UESynthClient

def batch_processing():
    # Step 1: Collect all data
    print("Collecting data...")
    frames = []
    
    with UESynthClient() as client:
        client.camera.set_location(x=0, y=100, z=50)
        
        for i in range(50):
            client.camera.set_rotation(pitch=-15, yaw=i*7.2, roll=0)  # 360° rotation
            frame = client.capture.rgb(width=512, height=512)
            frames.append(frame)
    
    print(f"Collected {len(frames)} frames")
    
    # Step 2: Process all frames
    print("Processing data...")
    processed_frames = []
    
    for i, frame in enumerate(frames):
        # Example processing: blur and resize
        blurred = cv2.GaussianBlur(frame, (15, 15), 0)
        resized = cv2.resize(blurred, (256, 256))
        processed_frames.append(resized)
        
        if i % 10 == 0:
            print(f"Processed {i+1}/{len(frames)} frames")
    
    # Step 3: Save results
    print("Saving results...")
    for i, processed_frame in enumerate(processed_frames):
        cv2.imwrite(f"batch_processed_{i:03d}.png", processed_frame)

batch_processing()
```

### 3. Dataset Creation

Create structured datasets for ML training:

```python
import cv2
import json
import os
from uesynth import UESynthClient

def create_ml_dataset():
    # Create dataset directory structure
    os.makedirs("dataset/images", exist_ok=True)
    os.makedirs("dataset/annotations", exist_ok=True)
    
    dataset_info = {
        "images": [],
        "annotations": [],
        "metadata": {
            "created_with": "UESynth",
            "capture_settings": {
                "width": 1024,
                "height": 768
            }
        }
    }
    
    with UESynthClient() as client:
        # Get scene objects for annotations
        objects = client.objects.list_all()
        
        for i in range(100):
            # Random camera position
            import random
            x = random.uniform(-200, 200)
            y = random.uniform(-200, 200)
            z = random.uniform(50, 150)
            
            client.camera.set_location(x=x, y=y, z=z)
            client.camera.set_rotation(pitch=-15, yaw=random.uniform(0, 360), roll=0)
            
            # Capture data
            rgb = client.capture.rgb(width=1024, height=768)
            segmentation = client.capture.segmentation(width=1024, height=768)
            
            # Save image
            image_filename = f"image_{i:06d}.png"
            cv2.imwrite(f"dataset/images/{image_filename}", rgb)
            
            # Create annotation
            annotation = {
                "image_id": i,
                "filename": image_filename,
                "camera_position": {"x": x, "y": y, "z": z},
                "objects": []
            }
            
            # Analyze segmentation for object info
            unique_ids = np.unique(segmentation)
            for obj_id in unique_ids:
                if obj_id > 0:  # Skip background
                    mask = (segmentation == obj_id)
                    if mask.sum() > 100:  # Only include objects with enough pixels
                        # Find bounding box
                        coords = np.where(mask)
                        y_min, y_max = coords[0].min(), coords[0].max()
                        x_min, x_max = coords[1].min(), coords[1].max()
                        
                        annotation["objects"].append({
                            "object_id": int(obj_id),
                            "bbox": [int(x_min), int(y_min), int(x_max), int(y_max)],
                            "area": int(mask.sum())
                        })
            
            dataset_info["images"].append({
                "id": i,
                "filename": image_filename,
                "width": 1024,
                "height": 768
            })
            dataset_info["annotations"].append(annotation)
            
            # Save individual annotation
            with open(f"dataset/annotations/annotation_{i:06d}.json", 'w') as f:
                json.dump(annotation, f, indent=2)
            
            if i % 10 == 0:
                print(f"Created {i+1}/100 samples")
    
    # Save dataset metadata
    with open("dataset/dataset_info.json", 'w') as f:
        json.dump(dataset_info, f, indent=2)
    
    print("Dataset creation complete!")

create_ml_dataset()
```

## Best Practices

### 1. Error Handling

Always handle potential errors gracefully:

```python
from uesynth import UESynthClient, UESynthError

def robust_capture():
    try:
        with UESynthClient() as client:
            # Test connection
            if not client.is_connected():
                raise ConnectionError("Failed to connect to UESynth")
            
            # Your capture code here
            image = client.capture.rgb()
            
    except UESynthError as e:
        print(f"UESynth specific error: {e}")
    except ConnectionError as e:
        print(f"Connection error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")

robust_capture()
```

### 2. Resource Management

Use context managers for automatic cleanup:

```python
# ✅ Good - automatic cleanup
with UESynthClient() as client:
    image = client.capture.rgb()
    # Client automatically disconnected

# ❌ Manual cleanup required
client = UESynthClient()
try:
    image = client.capture.rgb()
finally:
    client.disconnect()  # Must remember to disconnect
```

### 3. Progress Tracking

For long-running operations, show progress:

```python
from tqdm import tqdm
from uesynth import UESynthClient

def capture_with_progress():
    with UESynthClient() as client:
        positions = [(i*10, 0, 50) for i in range(100)]
        
        for i, (x, y, z) in enumerate(tqdm(positions, desc="Capturing")):
            client.camera.set_location(x=x, y=y, z=z)
            image = client.capture.rgb()
            cv2.imwrite(f"frame_{i:03d}.png", image)

capture_with_progress()
```

### 4. Configuration Validation

Validate your setup before starting:

```python
from uesynth import UESynthClient

def validate_setup():
    try:
        with UESynthClient() as client:
            # Test basic functionality
            location = client.camera.get_location()
            print(f"✅ Camera accessible at: {location}")
            
            # Test capture
            test_image = client.capture.rgb(width=256, height=256)
            print(f"✅ Capture working: {test_image.shape}")
            
            # Test object access
            objects = client.objects.list_all()
            print(f"✅ Found {len(objects)} objects in scene")
            
            return True
    except Exception as e:
        print(f"❌ Setup validation failed: {e}")
        return False

if validate_setup():
    print("Setup validation passed - ready to proceed!")
else:
    print("Please fix setup issues before proceeding")
```

This guide covers the fundamental usage patterns for UESynth. As you become more comfortable with these basics, you can explore advanced features like async streaming, scene randomization, and performance optimization.

## Next Steps

- Learn about [Advanced Features](advanced-features.md)
- Explore [Scene Randomization](scene-randomization.md)
- Check out [Data Collection Workflows](data-collection.md) 