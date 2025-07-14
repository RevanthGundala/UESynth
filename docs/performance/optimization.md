# Performance Optimization

This guide covers techniques to maximize UESynth performance for different use cases, from simple data collection to high-throughput real-time simulations.

## Performance Overview

UESynth performance varies significantly based on client type and usage patterns:

| Client Type | Typical Performance | Best Use Case |
|-------------|-------------------|---------------|
| **Sync Client** | 10-50 requests/sec | Simple scripts, prototyping |
| **Async Client (Direct)** | 50-200 requests/sec | Medium-scale batch processing |
| **Async Client (Streaming)** | 100-1000+ requests/sec | High-performance real-time sims |

## Async Client Optimization

### 1. Choose the Right Mode

```python
from uesynth import AsyncUESynthClient

# 🚀 Streaming mode - highest performance
async def streaming_mode():
    async with AsyncUESynthClient() as client:
        # Non-blocking operations
        request_id = await client.capture.rgb()  # Returns immediately
        frame = await client.get_latest_frame()  # Get when ready

# 📊 Direct mode - medium performance, easier to use
async def direct_mode():
    async with AsyncUESynthClient() as client:
        # Blocking operations
        frame = await client.capture.rgb_direct()  # Waits for result
```

### 2. Optimize Buffer Settings

```python
# High-throughput configuration
client = AsyncUESynthClient(
    stream_buffer_size=8192,        # Large buffer for many requests
    max_concurrent_requests=1000,   # High concurrency limit
    enable_compression=True,        # Reduce network overhead
    timeout=1.0                     # Fast timeout for responsiveness
)

# Memory-conscious configuration
client = AsyncUESynthClient(
    stream_buffer_size=1024,        # Smaller buffer
    max_concurrent_requests=100,    # Lower concurrency
    enable_compression=True,        # Still compress
    timeout=5.0                     # Longer timeout
)
```

### 3. Batch Operations

```python
import asyncio

# ✅ Efficient - batch requests
async def batch_capture(client, positions):
    # Start all operations concurrently
    tasks = []
    for x, y, z in positions:
        tasks.append(client.camera.set_location(x=x, y=y, z=z))
        tasks.append(client.capture.rgb())
    
    # Wait for all to complete
    await asyncio.gather(*tasks)

# ❌ Inefficient - sequential operations
async def sequential_capture(client, positions):
    for x, y, z in positions:
        await client.camera.set_location(x=x, y=y, z=z)
        await client.capture.rgb()
```

## Network Optimization

### 1. Compression

```python
# Enable compression for all clients
client = AsyncUESynthClient(enable_compression=True)

# Significant bandwidth reduction:
# - RGB images: ~50-70% smaller
# - Depth maps: ~80-90% smaller
# - Network latency: reduced proportionally
```

### 2. Image Resolution

```python
# Choose appropriate resolution for your use case
# Training data: 512x512 or 640x480 often sufficient
# Evaluation: 1920x1080 or higher
# Real-time: 256x256 to 512x512 for maximum FPS

# High FPS configuration
await client.capture.rgb(width=256, height=256)

# High quality configuration
await client.capture.rgb(width=1920, height=1080)
```

### 3. Network Configuration

```python
# Custom gRPC options for performance
grpc_options = [
    ('grpc.keepalive_time_ms', 30000),         # Keep connections alive
    ('grpc.keepalive_timeout_ms', 5000),        # Quick keepalive timeout
    ('grpc.http2.max_pings_without_data', 0),   # Allow pings without data
    ('grpc.max_receive_message_length', 100 * 1024 * 1024),  # 100MB max message
    ('grpc.max_send_message_length', 100 * 1024 * 1024),     # 100MB max message
]

client = AsyncUESynthClient(grpc_options=grpc_options)
```

## Memory Optimization

### 1. Frame Management

```python
# ✅ Good - process frames immediately
async def process_immediately():
    async with AsyncUESynthClient() as client:
        for i in range(1000):
            await client.capture.rgb()
            frame = await client.get_latest_frame()
            
            if frame is not None:
                # Process immediately
                result = process_frame(frame)
                save_result(result)
                # Frame is automatically garbage collected

# ❌ Bad - accumulate frames in memory
async def accumulate_frames():
    frames = []  # This will consume lots of memory!
    async with AsyncUESynthClient() as client:
        for i in range(1000):
            frame = await client.capture.rgb_direct()
            frames.append(frame)  # Memory grows continuously
```

### 2. Capture Only What You Need

```python
# ✅ Capture specific modalities
rgb_only = await client.capture.rgb_direct()

# ❌ Don't capture everything if you only need RGB
all_data = await client.capture.all_modalities_direct()  # Wasteful
rgb = all_data['rgb']  # Only using RGB
```

## CPU Optimization

### 1. Concurrent Processing

```python
import asyncio
from concurrent.futures import ThreadPoolExecutor

async def concurrent_processing():
    # Use thread pool for CPU-intensive processing
    executor = ThreadPoolExecutor(max_workers=4)
    
    async with AsyncUESynthClient() as client:
        for i in range(100):
            await client.capture.rgb()
            frame = await client.get_latest_frame()
            
            if frame is not None:
                # Process in background thread
                loop = asyncio.get_event_loop()
                result = await loop.run_in_executor(
                    executor, cpu_intensive_processing, frame
                )
                save_result(result)

def cpu_intensive_processing(frame):
    # Your CPU-intensive code here
    return processed_result
```

### 2. Optimize Frame Processing

```python
import cv2
import numpy as np

# ✅ Efficient frame processing
def efficient_processing(frame):
    # Use numpy operations (vectorized)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    resized = cv2.resize(gray, (224, 224))
    normalized = resized.astype(np.float32) / 255.0
    return normalized

# ❌ Inefficient frame processing
def inefficient_processing(frame):
    # Pixel-by-pixel operations (slow)
    height, width = frame.shape[:2]
    result = np.zeros((height, width), dtype=np.float32)
    for y in range(height):
        for x in range(width):
            result[y, x] = frame[y, x, 0] / 255.0  # Very slow!
    return result
```

## Real-Time Optimization

### 1. Frame Rate Management

```python
import time

async def maintain_target_fps(target_fps=60):
    """Maintain target FPS for real-time applications."""
    frame_time = 1.0 / target_fps
    
    async with AsyncUESynthClient() as client:
        frame_count = 0
        start_time = time.time()
        
        while True:
            frame_start = time.time()
            
            # Your frame logic
            await client.capture.rgb()
            frame = await client.get_latest_frame()
            
            if frame is not None:
                process_frame(frame)
                frame_count += 1
            
            # Maintain target FPS
            elapsed = time.time() - frame_start
            if elapsed < frame_time:
                await asyncio.sleep(frame_time - elapsed)
            
            # Performance monitoring
            if frame_count % 60 == 0:
                total_time = time.time() - start_time
                actual_fps = frame_count / total_time
                print(f"FPS: {actual_fps:.1f}")
```

### 2. Predictive Frame Requests

```python
async def predictive_capture():
    """Start next frame capture before processing current one."""
    
    async with AsyncUESynthClient() as client:
        # Start first capture
        await client.capture.rgb()
        
        for i in range(100):
            # Get current frame
            current_frame = await client.get_latest_frame()
            
            # Immediately start next capture (predictive)
            if i < 99:  # Don't start capture on last iteration
                await client.capture.rgb()
            
            # Process current frame while next is capturing
            if current_frame is not None:
                process_frame(current_frame)
```

## Benchmarking and Monitoring

### 1. Performance Measurement

```python
import time
from collections import deque

class PerformanceMonitor:
    def __init__(self, window_size=100):
        self.frame_times = deque(maxlen=window_size)
        self.request_times = deque(maxlen=window_size)
        self.start_time = None
    
    def start_frame(self):
        self.start_time = time.time()
    
    def end_frame(self):
        if self.start_time:
            frame_time = time.time() - self.start_time
            self.frame_times.append(frame_time)
    
    def record_request(self, request_time):
        self.request_times.append(request_time)
    
    def get_stats(self):
        if not self.frame_times:
            return {}
        
        avg_frame_time = sum(self.frame_times) / len(self.frame_times)
        fps = 1.0 / avg_frame_time if avg_frame_time > 0 else 0
        
        return {
            'fps': fps,
            'avg_frame_time': avg_frame_time * 1000,  # ms
            'total_frames': len(self.frame_times)
        }

# Usage
monitor = PerformanceMonitor()

async with AsyncUESynthClient() as client:
    for i in range(100):
        monitor.start_frame()
        
        await client.capture.rgb()
        frame = await client.get_latest_frame()
        
        if frame is not None:
            process_frame(frame)
        
        monitor.end_frame()
        
        if i % 10 == 0:
            stats = monitor.get_stats()
            print(f"FPS: {stats['fps']:.1f}, Avg frame time: {stats['avg_frame_time']:.1f}ms")
```

### 2. Resource Monitoring

```python
import psutil
import asyncio

async def monitor_resources():
    """Monitor CPU and memory usage during operation."""
    
    process = psutil.Process()
    
    async with AsyncUESynthClient() as client:
        for i in range(100):
            # Your operations
            await client.capture.rgb()
            
            if i % 20 == 0:
                cpu_percent = process.cpu_percent()
                memory_mb = process.memory_info().rss / 1024 / 1024
                print(f"CPU: {cpu_percent:.1f}%, Memory: {memory_mb:.1f}MB")
```

## Configuration Recommendations

### Development Environment
```python
# Prioritize ease of debugging
client = AsyncUESynthClient(
    timeout=30.0,              # Long timeout for debugging
    stream_buffer_size=1024,   # Moderate buffer
    max_concurrent_requests=50, # Lower concurrency
    enable_compression=False    # Disable for easier debugging
)
```

### Production Environment
```python
# Prioritize performance and reliability
client = AsyncUESynthClient(
    timeout=2.0,               # Fast timeout
    stream_buffer_size=4096,   # Large buffer
    max_concurrent_requests=500, # High concurrency
    enable_compression=True,    # Reduce bandwidth
    grpc_options=[             # Optimized gRPC settings
        ('grpc.keepalive_time_ms', 30000),
        ('grpc.keepalive_timeout_ms', 5000)
    ]
)
```

### High-Throughput Scenarios
```python
# Maximum performance configuration
client = AsyncUESynthClient(
    timeout=1.0,               # Very fast timeout
    stream_buffer_size=8192,   # Very large buffer
    max_concurrent_requests=1000, # Maximum concurrency
    enable_compression=True,    # Essential for bandwidth
    grpc_options=[
        ('grpc.keepalive_time_ms', 10000),
        ('grpc.keepalive_timeout_ms', 2000),
        ('grpc.http2.max_pings_without_data', 0),
        ('grpc.max_receive_message_length', 200 * 1024 * 1024),
        ('grpc.max_send_message_length', 200 * 1024 * 1024)
    ]
)
```

## Troubleshooting Performance

### Common Issues and Solutions

1. **Low FPS / High Latency**
   - Use async streaming mode instead of direct mode
   - Increase buffer sizes
   - Enable compression
   - Reduce image resolution

2. **Memory Usage Growing**
   - Process frames immediately instead of accumulating
   - Use smaller capture resolutions
   - Reduce buffer sizes

3. **Network Bottlenecks**
   - Enable compression
   - Optimize image formats (JPEG vs PNG)
   - Use appropriate resolution for use case

4. **CPU Bottlenecks**
   - Use thread pools for processing
   - Optimize frame processing algorithms
   - Reduce capture frequency

By following these optimization techniques, you can achieve maximum performance for your specific UESynth use case, whether it's simple data collection or high-throughput real-time simulation.

## Next Steps

- Check out [Benchmarks](benchmarks.md) for performance comparisons
- Learn [Best Practices](best-practices.md) for reliable operation
- Explore [Advanced Examples](../examples/async-streaming.md) for real-world usage 