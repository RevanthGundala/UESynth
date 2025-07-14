# UESynth: A Synthetic Data Generation Plugin for Unreal Engine

[![Build Status](https://github.com/your-username/uesynth/actions/workflows/ci.yml/badge.svg)](https://github.com/your-username/uesynth/actions)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Documentation](https://img.shields.io/badge/docs-gitbook-blue.svg)](https://your-username.gitbook.io/uesynth/)
[![Python Package](https://img.shields.io/pypi/v/uesynth.svg)](https://pypi.org/project/uesynth/)

## Overview
UESynth is a powerful and user-friendly Unreal Engine plugin for synthetic data generation, inspired by UnrealCV. It provides a comprehensive API for controlling scenes, manipulating objects, and capturing a wide variety of data modalities for use in computer vision and machine learning research.

## Key Features
-   **Multi-modal Data Capture:** Capture RGB, depth, segmentation masks, surface normals, and more
-   **High-Performance API:** Both synchronous and asynchronous Python clients with streaming support  
-   **Scene and Object Manipulation:** Programmatically control objects, lighting, and materials
-   **gRPC-based Communication:** Modern, robust, and high-performance networking
-   **Real-time Capabilities:** Support for 100-1000+ requests/second with async streaming
-   **Scene Randomization:** Tools for domain randomization to create diverse datasets
-   **Editor Integration:** User-friendly interface within the Unreal Editor

## Quick Start

### Installation

1. **Install the Python client:**
   ```bash
   uv pip install uesynth
   ```

2. **Install the UE plugin:**
   - Download from [GitHub Releases](https://github.com/your-username/uesynth/releases)
   - Extract to your project's `Plugins/` directory
   - Enable in the UE Editor

3. **First capture:**
   ```python
   from uesynth import UESynthClient
   import cv2
   
   with UESynthClient() as client:
       client.camera.set_location(x=0, y=100, z=50)
       image = client.capture.rgb()
       cv2.imwrite("my_first_capture.png", image)
   ```

📖 **[Complete Installation Guide](docs/getting-started/installation.md)**

## Performance Comparison

UESynth provides both synchronous and asynchronous clients for different use cases:

| Client Type | Performance | Best For |
|-------------|-------------|----------|
| **Sync Client** | 10-50 req/sec | Simple scripts, prototyping |
| **Async Client** | 100-1000+ req/sec | Real-time sims, batch processing |

### Synchronous Client
```python
from uesynth import UESynthClient
import cv2

with UESynthClient() as client:
    client.camera.set_location(x=0, y=100, z=50)
    image = client.capture.rgb()
    cv2.imwrite("captured.png", image)
```

### Asynchronous Client
For high-performance scenarios requiring thousands of operations:

```python
import asyncio
from uesynth import AsyncUESynthClient

async def high_performance_capture():
    async with AsyncUESynthClient() as client:
        # High-frequency simulation loop
        for timestep in range(1000):
            # Non-blocking operations
            await client.camera.set_location(x=timestep * 10, y=100, z=50)
            await client.capture.rgb()
            
            # Get latest frame when ready
            frame = await client.get_latest_frame()
            if frame is not None:
                cv2.imwrite(f"frame_{timestep:06d}.png", frame)
            
            await asyncio.sleep(0.01)  # 100 FPS target

asyncio.run(high_performance_capture())
```

## Data Modalities

UESynth supports multiple data types for comprehensive scene understanding:

- **RGB Images** - Standard color imagery
- **Depth Maps** - Distance information  
- **Segmentation Masks** - Object instance IDs
- **Surface Normals** - Object surface orientations
- **Multi-modal Capture** - All data types simultaneously

```python
# Capture all data types at once
data = client.capture.all_modalities(width=1024, height=768)
rgb = data['rgb']
depth = data['depth'] 
segmentation = data['segmentation']
normals = data['normals']
```

## Documentation

📚 **[Complete Documentation](docs/)** - Comprehensive guides and API reference

### Quick Links
- **[Installation Guide](docs/getting-started/installation.md)** - Detailed setup instructions
- **[Quick Start Tutorial](docs/getting-started/quick-start.md)** - Get up and running in minutes  
- **[API Reference](docs/api-reference/)** - Complete API documentation
- **[Performance Guide](docs/performance/optimization.md)** - Maximize throughput and efficiency
- **[Examples](docs/examples/)** - Practical code examples and patterns
- **[Contributing Guide](docs/contributing.md)** - How to contribute to the project

### Key Documentation Sections

| Section | Description |
|---------|-------------|
| **[Getting Started](docs/getting-started/)** | Installation, quick start, configuration |
| **[User Guides](docs/guides/)** | Basic usage, advanced features, workflows |
| **[API Reference](docs/api-reference/)** | Complete client API documentation |
| **[Examples](docs/examples/)** | Practical code examples and patterns |
| **[Performance](docs/performance/)** | Optimization guides and benchmarks |

## Community & Support

- **📖 Documentation:** [docs/](docs/)
- **🐛 Bug Reports:** [GitHub Issues](https://github.com/your-username/uesynth/issues)
- **💡 Feature Requests:** [GitHub Issues](https://github.com/your-username/uesynth/issues)
- **🤝 Contributing:** [Contributing Guide](docs/contributing.md)

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details. 