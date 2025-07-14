# Asynchronous Client API

The `AsyncUESynthClient` provides a high-performance, non-blocking API designed for real-time simulations, batch processing, and scenarios requiring thousands of operations per second.

## Overview

The asynchronous client offers:
- **High throughput** - 100-1000+ requests/second vs 10-50 for sync
- **Low latency** - Minimal waiting between operations
- **Concurrent operations** - Multiple requests in parallel
- **Streaming support** - Bidirectional data streaming
- **Non-blocking** - Doesn't freeze while waiting for responses

## Performance Comparison

| Feature | Sync Client | Async Client |
|---------|-------------|--------------|
| **Throughput** | ~10-50 req/sec | ~100-1000+ req/sec |
| **Latency** | High accumulation | Minimal |
| **Concurrent ops** | Sequential only | Full parallelism |
| **Use case** | Simple scripts | High-performance sims |

## Getting Started

```python
import asyncio
from uesynth import AsyncUESynthClient

async def main():
    # Connect to UESynth
    client = AsyncUESynthClient()
    await client.connect()
    
    try:
        # Your async code here...
        pass
    finally:
        # Always disconnect
        await client.disconnect()

# Run the async function
asyncio.run(main())
```

## Client Configuration

### Basic Connection

```python
# Default connection
client = AsyncUESynthClient()

# Custom connection
client = AsyncUESynthClient(
    host="192.168.1.100",
    port=50052,
    timeout=10.0,
    stream_buffer_size=2048
)
```

### Advanced Options

```python
client = AsyncUESynthClient(
    host="localhost",
    port=50051,
    timeout=5.0,
    max_concurrent_requests=500,
    stream_buffer_size=4096,
    enable_compression=True,
    grpc_options=[
        ('grpc.keepalive_time_ms', 30000),
        ('grpc.http2.max_pings_without_data', 0)
    ]
)
```

## Core Methods

### Connection Management

#### `connect()`
Establish connection and initialize streaming.

```python
client = AsyncUESynthClient()
await client.connect()
```

#### `disconnect()`
Close connection and clean up resources.

```python
await client.disconnect()
```

#### `is_connected()`
Check connection status.

```python
if await client.is_connected():
    print("Connected to UESynth")
```

#### `ping()`
Test server responsiveness.

```python
latency = await client.ping()
print(f"Server latency: {latency}ms")
```

## Request Modes

The async client supports two operation modes:

### 1. Streaming Mode (Non-blocking)
Operations return request IDs immediately and complete asynchronously.

```python
# Send request (non-blocking, returns immediately)
request_id = await client.camera.set_location(x=100, y=200, z=50)

# Send capture request (non-blocking)
capture_id = await client.capture.rgb()

# Get the captured frame when ready
frame = await client.get_latest_frame()
```

### 2. Direct Mode (Blocking)
Operations wait for completion and return results directly.

```python
# Direct operations (wait for completion)
location = await client.camera.get_location_direct()
rgb_image = await client.capture.rgb_direct()
```

## Camera Control (Streaming)

### Position and Rotation

#### `camera.set_location(x, y, z)`
Set camera position (non-blocking).

```python
# Returns request ID immediately
request_id = await client.camera.set_location(x=100, y=200, z=50)
print(f"Camera move request: {request_id}")
```

#### `camera.set_rotation(pitch, yaw, roll)`
Set camera rotation (non-blocking).

```python
request_id = await client.camera.set_rotation(pitch=-30, yaw=45, roll=0)
```

### Camera Settings

#### `camera.set_fov(fov)`
Set field of view (non-blocking).

```python
request_id = await client.camera.set_fov(75.0)
```

## Camera Control (Direct)

### Position and Rotation

#### `camera.get_location_direct()`
Get camera position (blocking, returns result).

```python
location = await client.camera.get_location_direct()
print(f"Camera at: {location.x}, {location.y}, {location.z}")
```

#### `camera.get_rotation_direct()`
Get camera rotation (blocking, returns result).

```python
rotation = await client.camera.get_rotation_direct()
print(f"Camera rotation: {rotation.pitch}, {rotation.yaw}, {rotation.roll}")
```

#### `camera.set_location_direct(x, y, z)`
Set camera position and wait for completion.

```python
await client.camera.set_location_direct(x=100, y=200, z=50)
print("Camera position updated")
```

## Data Capture (Streaming)

### Image Capture

#### `capture.rgb(width=None, height=None)`
Capture RGB image (non-blocking).

```python
# Start capture (returns immediately)
request_id = await client.capture.rgb(width=1920, height=1080)

# Get result when ready
frame = await client.get_latest_frame()
if frame is not None:
    import cv2
    cv2.imwrite("captured.png", frame)
```

#### `capture.depth(width=None, height=None)`
Capture depth map (non-blocking).

```python
request_id = await client.capture.depth()
```

#### `capture.segmentation(width=None, height=None)`
Capture segmentation (non-blocking).

```python
request_id = await client.capture.segmentation()
```

## Data Capture (Direct)

### Image Capture

#### `capture.rgb_direct(width=None, height=None)`
Capture RGB image and return result immediately.

```python
# Wait for capture and get result
rgb_image = await client.capture.rgb_direct(width=1920, height=1080)
print(f"Captured image: {rgb_image.shape}")
```

#### `capture.depth_direct(width=None, height=None)`
Capture depth map directly.

```python
depth_map = await client.capture.depth_direct()
```

#### `capture.all_modalities_direct(width=None, height=None)`
Capture all data types directly.

```python
data = await client.capture.all_modalities_direct(width=1024, height=768)
rgb = data['rgb']
depth = data['depth']
segmentation = data['segmentation']
```

## Frame Management

### Getting Captured Data

#### `get_latest_frame()`
Get the most recently captured frame.

```python
# Start a capture
await client.capture.rgb()

# Wait a bit and get the result
await asyncio.sleep(0.1)
frame = await client.get_latest_frame()

if frame is not None:
    print(f"Got frame: {frame.shape}")
else:
    print("No frame available yet")
```

#### `get_frame_by_id(request_id)`
Get a specific frame by its request ID.

```python
# Start capture and remember the ID
request_id = await client.capture.rgb()

# Get the specific frame
frame = await client.get_frame_by_id(request_id)
```

#### `wait_for_frame(request_id, timeout=5.0)`
Wait for a specific frame to be ready.

```python
# Start capture
request_id = await client.capture.rgb()

# Wait for it to complete
frame = await client.wait_for_frame(request_id, timeout=10.0)
```

## Object Manipulation

### Transform Control (Streaming)

#### `objects.set_location(name, x, y, z)`
Move object (non-blocking).

```python
request_id = await client.objects.set_location("MyActor", x=100, y=200, z=50)
```

#### `objects.set_rotation(name, pitch, yaw, roll)`
Rotate object (non-blocking).

```python
request_id = await client.objects.set_rotation("MyActor", pitch=0, yaw=45, roll=0)
```

### Transform Control (Direct)

#### `objects.set_transform_direct(name, x, y, z, pitch=0, yaw=0, roll=0)`
Set complete transform and wait for completion.

```python
await client.objects.set_transform_direct(
    "MyActor", 
    x=100, y=200, z=50,
    pitch=0, yaw=45, roll=0
)
```

#### `objects.get_transform_direct(name)`
Get object transform directly.

```python
transform = await client.objects.get_transform_direct("MyActor")
print(f"Transform: {transform}")
```

## High-Performance Patterns

### Concurrent Operations

```python
import asyncio

async def capture_multiple_views():
    client = AsyncUESynthClient()
    await client.connect()
    
    try:
        # Start multiple operations concurrently
        tasks = []
        
        for i in range(10):
            x = i * 100
            # Start camera move (non-blocking)
            move_task = client.camera.set_location(x=x, y=0, z=50)
            # Start capture (non-blocking)  
            capture_task = client.capture.rgb()
            
            tasks.extend([move_task, capture_task])
        
        # Wait for all to complete
        await asyncio.gather(*tasks)
        
        # Collect all frames
        frames = []
        for _ in range(10):
            frame = await client.get_latest_frame()
            if frame is not None:
                frames.append(frame)
        
        print(f"Captured {len(frames)} frames")
        
    finally:
        await client.disconnect()
```

### Real-time Simulation Loop

```python
async def real_time_simulation():
    client = AsyncUESynthClient()
    await client.connect()
    
    try:
        timestep = 0
        
        while timestep < 1000:
            # Move camera (non-blocking)
            await client.camera.set_location(
                x=timestep * 10, 
                y=100, 
                z=50
            )
            
            # Move objects (non-blocking)
            await client.objects.set_location(
                "Car_01", 
                x=timestep * 5, 
                y=0, 
                z=0
            )
            
            # Capture frame (non-blocking)
            await client.capture.rgb()
            
            # Get latest frame if available
            frame = await client.get_latest_frame()
            if frame is not None:
                # Process frame...
                pass
            
            # Small delay to prevent overwhelming
            await asyncio.sleep(0.01)
            timestep += 1
            
    finally:
        await client.disconnect()
```

### Batch Processing

```python
async def batch_capture():
    client = AsyncUESynthClient()
    await client.connect()
    
    try:
        # Prepare batch of positions
        positions = [(i*50, j*50, 100) for i in range(10) for j in range(10)]
        
        # Start all operations (non-blocking)
        request_ids = []
        for x, y, z in positions:
            await client.camera.set_location(x=x, y=y, z=z)
            request_id = await client.capture.rgb()
            request_ids.append(request_id)
        
        # Collect all results
        frames = []
        for request_id in request_ids:
            frame = await client.wait_for_frame(request_id)
            frames.append(frame)
        
        print(f"Batch captured {len(frames)} frames")
        
    finally:
        await client.disconnect()
```

## Error Handling

```python
import asyncio
from uesynth import AsyncUESynthClient, UESynthError

async def robust_client():
    client = AsyncUESynthClient()
    
    try:
        await client.connect()
        
        # Your operations here
        await client.capture.rgb_direct()
        
    except UESynthError as e:
        print(f"UESynth error: {e}")
        
    except asyncio.TimeoutError as e:
        print(f"Operation timed out: {e}")
        
    except ConnectionError as e:
        print(f"Connection lost: {e}")
        
    except Exception as e:
        print(f"Unexpected error: {e}")
        
    finally:
        try:
            await client.disconnect()
        except:
            pass  # Ignore cleanup errors
```

## Context Manager

```python
from uesynth import AsyncUESynthClient

async def with_context_manager():
    async with AsyncUESynthClient() as client:
        # Client automatically connects
        frame = await client.capture.rgb_direct()
        # Client automatically disconnects
```

## Performance Tips

### 1. Use Streaming for High Throughput

```python
# ✅ High performance - streaming mode
for i in range(1000):
    await client.camera.set_location(x=i, y=0, z=50)  # Non-blocking
    await client.capture.rgb()  # Non-blocking

# ❌ Lower performance - direct mode
for i in range(1000):
    await client.camera.set_location_direct(x=i, y=0, z=50)  # Blocking
    frame = await client.capture.rgb_direct()  # Blocking
```

### 2. Minimize Await Points

```python
# ✅ Better - batch operations
tasks = []
for i in range(100):
    tasks.append(client.camera.set_location(x=i, y=0, z=50))
await asyncio.gather(*tasks)

# ❌ Slower - individual awaits
for i in range(100):
    await client.camera.set_location(x=i, y=0, z=50)
```

### 3. Configure Buffer Sizes

```python
# For high-frequency operations
client = AsyncUESynthClient(
    stream_buffer_size=8192,      # Large buffer
    max_concurrent_requests=1000  # High concurrency
)
```

## Complete Example

```python
import asyncio
import cv2
from uesynth import AsyncUESynthClient

async def high_performance_data_collection():
    """Collect 1000 frames as fast as possible."""
    
    async with AsyncUESynthClient(
        stream_buffer_size=4096,
        max_concurrent_requests=500
    ) as client:
        
        print("🚀 Starting high-performance data collection...")
        
        # Generate camera trajectory
        trajectory = [
            (i * 10, 0, 50 + i * 2) 
            for i in range(1000)
        ]
        
        # Start all operations (non-blocking)
        request_ids = []
        start_time = asyncio.get_event_loop().time()
        
        for i, (x, y, z) in enumerate(trajectory):
            # Move camera
            await client.camera.set_location(x=x, y=y, z=z)
            
            # Start capture
            request_id = await client.capture.rgb()
            request_ids.append(request_id)
            
            # Small delay to prevent overwhelming
            if i % 100 == 0:
                await asyncio.sleep(0.01)
        
        # Collect all frames
        frames_collected = 0
        for i, request_id in enumerate(request_ids):
            try:
                frame = await client.wait_for_frame(request_id, timeout=1.0)
                if frame is not None:
                    cv2.imwrite(f"frame_{i:06d}.png", frame)
                    frames_collected += 1
            except asyncio.TimeoutError:
                print(f"Frame {i} timed out")
        
        end_time = asyncio.get_event_loop().time()
        duration = end_time - start_time
        
        print(f"✅ Collected {frames_collected}/1000 frames in {duration:.2f}s")
        print(f"📊 Average: {frames_collected/duration:.1f} frames/second")

if __name__ == "__main__":
    asyncio.run(high_performance_data_collection())
```

## Next Steps

- Learn about [Camera Controls](camera.md) for advanced camera features
- Explore [Performance Optimization](../performance/optimization.md) for maximum throughput
- Check out [Async Examples](../examples/async-streaming.md) for more patterns 