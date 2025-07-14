# Async Streaming Examples

This guide provides practical examples demonstrating the powerful async streaming capabilities of UESynth for high-performance data collection scenarios.

## Overview

Async streaming enables:
- **Non-blocking operations** - Send requests without waiting
- **High throughput** - 100-1000+ operations per second
- **Concurrent execution** - Multiple operations in parallel
- **Real-time responsiveness** - Minimal latency accumulation

## Basic Async Pattern

```python
import asyncio
import cv2
from uesynth import AsyncUESynthClient

async def basic_async_capture():
    """Basic pattern for async data capture."""
    
    async with AsyncUESynthClient() as client:
        # Start a capture (non-blocking)
        request_id = await client.capture.rgb(width=1920, height=1080)
        print(f"Started capture request: {request_id}")
        
        # Do other work while capture is processing
        await client.camera.set_location(x=100, y=200, z=50)
        
        # Get the captured frame
        frame = await client.get_latest_frame()
        if frame is not None:
            cv2.imwrite("async_capture.png", frame)
            print("✅ Frame captured and saved")

asyncio.run(basic_async_capture())
```

## High-Performance Collection

```python
async def rapid_fire_collection():
    """Collect data as fast as possible with minimal latency."""
    
    async with AsyncUESynthClient(
        stream_buffer_size=4096,
        max_concurrent_requests=500
    ) as client:
        
        print("🚀 Starting rapid fire collection...")
        request_ids = []
        
        # Fire off 500 requests as fast as possible
        for i in range(500):
            x = (i % 25) * 20  # Grid pattern
            y = (i // 25) * 20
            await client.camera.set_location(x=x, y=y, z=100)
            
            # Capture (non-blocking)
            request_id = await client.capture.rgb(width=512, height=512)
            request_ids.append(request_id)
            
            if i % 50 == 49:
                await asyncio.sleep(0.01)  # Brief pause
        
        # Collect results
        frames_collected = 0
        for i, request_id in enumerate(request_ids):
            try:
                frame = await client.wait_for_frame(request_id, timeout=2.0)
                if frame is not None:
                    if i % 10 == 0:  # Save every 10th frame
                        cv2.imwrite(f"rapid_{i:03d}.png", frame)
                    frames_collected += 1
            except asyncio.TimeoutError:
                print(f"⏰ Request {i} timed out")
        
        print(f"📊 Collected {frames_collected}/500 frames")

asyncio.run(rapid_fire_collection())
```

## Real-time Simulation

```python
import math
import time

async def real_time_simulation():
    """Simulate real-time data collection with moving objects."""
    
    async with AsyncUESynthClient() as client:
        print("🎮 Starting real-time simulation...")
        
        timestep = 0
        collected_frames = 0
        max_timesteps = 1000
        
        start_time = time.time()
        
        while timestep < max_timesteps:
            # Calculate positions
            angle = timestep * 0.01
            cam_x = 200 * math.cos(angle)
            cam_y = 200 * math.sin(angle)
            car_x = timestep * 10
            
            # Update scene (non-blocking)
            await client.camera.set_location(x=cam_x, y=cam_y, z=100)
            await client.camera.set_rotation(pitch=-15, yaw=math.degrees(angle), roll=0)
            await client.objects.set_location("Car_01", x=car_x, y=0, z=0)
            
            # Capture frame (non-blocking)
            await client.capture.rgb(width=640, height=480)
            
            # Check for completed frames
            frame = await client.get_latest_frame()
            if frame is not None:
                if timestep % 50 == 0:
                    cv2.imwrite(f"sim_frame_{timestep:06d}.png", frame)
                collected_frames += 1
            
            # Real-time delay (60 FPS target)
            await asyncio.sleep(1.0 / 60.0)
            timestep += 1
            
            if timestep % 100 == 0:
                elapsed = time.time() - start_time
                fps = timestep / elapsed
                print(f"📈 Timestep {timestep}: {fps:.1f} sim FPS, {collected_frames} frames")
        
        final_time = time.time() - start_time
        print(f"🏁 Simulation complete: {max_timesteps/final_time:.1f} FPS, {collected_frames} frames")

asyncio.run(real_time_simulation())
```

## Producer-Consumer Pattern

```python
from asyncio import Queue

async def producer_consumer_pattern():
    """Separate data production from processing using queues."""
    
    frame_queue = Queue(maxsize=100)
    
    async def producer(client):
        """Produce frames as fast as possible."""
        for i in range(200):
            x = (i % 20) * 25
            y = (i // 20) * 25
            await client.camera.set_location(x=x, y=y, z=100)
            
            request_id = await client.capture.rgb(width=512, height=512)
            
            try:
                frame = await client.wait_for_frame(request_id, timeout=1.0)
                if frame is not None:
                    await frame_queue.put((i, frame))
            except asyncio.TimeoutError:
                print(f"⏰ Frame {i} timed out")
        
        await frame_queue.put(None)  # End signal
    
    async def consumer():
        """Process frames from the queue."""
        processed = 0
        
        while True:
            try:
                item = await asyncio.wait_for(frame_queue.get(), timeout=5.0)
                if item is None:
                    break
                
                frame_id, frame = item
                
                # Simulate processing
                await asyncio.sleep(0.01)
                
                if frame_id % 10 == 0:
                    cv2.imwrite(f"processed_{frame_id:03d}.png", frame)
                
                processed += 1
                frame_queue.task_done()
                
            except asyncio.TimeoutError:
                break
        
        print(f"📥 Processed {processed} frames")
    
    async with AsyncUESynthClient() as client:
        await asyncio.gather(producer(client), consumer())

asyncio.run(producer_consumer_pattern())
```

## Error Handling & Resilience

```python
async def robust_streaming():
    """Demonstrate robust streaming with error recovery."""
    
    async def resilient_capture_loop(client, total_frames=100):
        successful_captures = 0
        failed_captures = 0
        
        for i in range(total_frames):
            max_retries = 3
            
            for attempt in range(max_retries):
                try:
                    x = i * 10
                    await client.camera.set_location(x=x, y=0, z=100)
                    
                    request_id = await client.capture.rgb()
                    frame = await client.wait_for_frame(request_id, timeout=2.0)
                    
                    if frame is not None:
                        cv2.imwrite(f"robust_frame_{i:03d}.png", frame)
                        successful_captures += 1
                        break
                    else:
                        raise Exception("No frame returned")
                
                except asyncio.TimeoutError:
                    print(f"⏰ Timeout on frame {i}, attempt {attempt + 1}")
                    if attempt < max_retries - 1:
                        await asyncio.sleep(0.5)
                    else:
                        failed_captures += 1
                
                except Exception as e:
                    print(f"❌ Error on frame {i}: {e}")
                    if attempt < max_retries - 1:
                        await asyncio.sleep(0.5)
                    else:
                        failed_captures += 1
        
        return successful_captures, failed_captures
    
    max_connection_attempts = 3
    
    for connection_attempt in range(max_connection_attempts):
        try:
            async with AsyncUESynthClient() as client:
                print(f"🔌 Connected (attempt {connection_attempt + 1})")
                
                success, failures = await resilient_capture_loop(client)
                
                print(f"📊 Results: {success} successful, {failures} failed")
                print(f"Success rate: {success/(success+failures)*100:.1f}%")
                break
                
        except Exception as e:
            print(f"❌ Connection failed: {e}")
            if connection_attempt < max_connection_attempts - 1:
                await asyncio.sleep(2.0)
            else:
                print("💥 Max connection attempts reached")

asyncio.run(robust_streaming())
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

### 2. Batch Operations
```python
# ✅ Better - batch requests
tasks = [client.capture.rgb() for _ in range(100)]
request_ids = await asyncio.gather(*tasks)

# ❌ Less efficient - individual awaits
request_ids = []
for _ in range(100):
    request_id = await client.capture.rgb()
    request_ids.append(request_id)
```

### 3. Configure for Performance
```python
# High-performance setup
client = AsyncUESynthClient(
    stream_buffer_size=4096,      # Large buffer
    max_concurrent_requests=1000, # High concurrency
    enable_compression=True       # Reduce bandwidth
)
```

These examples demonstrate the power and flexibility of UESynth's async streaming capabilities. Choose the pattern that best fits your specific use case.

## Next Steps

- Learn about [Performance Optimization](../performance/optimization.md)
- Explore [Real-time Examples](real-time.md)
- Check out [Batch Processing](batch-processing.md) techniques 