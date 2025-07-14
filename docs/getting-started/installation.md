# Installation

This guide will walk you through installing UESynth plugin and the Python client library.

## System Requirements

### Unreal Engine Plugin
- Unreal Engine 5.0 or later
- Windows 10/11 or Linux (Ubuntu 20.04+)
- 8GB RAM minimum, 16GB recommended
- Graphics card with DirectX 11/12 or Vulkan support

### Python Client
- Python 3.8 or later
- Windows, macOS, or Linux
- pip or uv package manager

## Plugin Installation

### Method 1: Download Release (Recommended)

1. Go to the [GitHub Releases](https://github.com/your-username/uesynth/releases) page
2. Download the latest `UESynth-vX.X.X.zip` file
3. Extract the `UESynth` folder into your Unreal Engine project's `Plugins` directory
   - If a `Plugins` directory doesn't exist, create one in your project root
4. Restart Unreal Engine
5. Enable the plugin in `Edit → Plugins → Search for "UESynth" → Check the box`

### Method 2: Build from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/uesynth.git
   ```

2. Copy the `UESynth` folder to your project's `Plugins` directory

3. Generate project files:
   ```bash
   # Windows
   path/to/UnrealBuildTool.exe -projectfiles -project="YourProject.uproject" -game -rocket -progress

   # Linux
   /path/to/UnrealBuildTool -projectfiles -project="YourProject.uproject" -game -rocket -progress
   ```

4. Build the project in Unreal Engine or your IDE

## Python Client Installation

### Using pip
```bash
pip install uesynth
```

### Using uv (Recommended)
```bash
uv pip install uesynth
```

### From Source
```bash
git clone https://github.com/your-username/uesynth.git
cd uesynth/python-client
pip install -e .
```

## Verification

### Plugin Verification
1. Open your Unreal Engine project
2. Go to `Edit → Plugins`
3. Search for "UESynth" - it should appear as enabled
4. Check the Output Log for "UESynth: Plugin loaded successfully"

### Python Client Verification
```python
import uesynth
print(f"UESynth version: {uesynth.__version__}")

# Test connection (requires UE to be running)
try:
    client = uesynth.UESynthClient()
    print("✅ Successfully connected to UESynth!")
    client.disconnect()
except Exception as e:
    print(f"❌ Connection failed: {e}")
```

## Network Configuration

UESynth uses gRPC on port `50051` by default. Make sure this port is available:

### Windows
```powershell
netstat -an | findstr :50051
```

### Linux/macOS
```bash
netstat -an | grep :50051
```

If the port is in use, you can configure a different port in the plugin settings.

## Next Steps

- [Quick Start Guide](quick-start.md) - Get up and running with basic examples
- [Configuration](configuration.md) - Learn about plugin configuration options
- [Basic Usage](../guides/basic-usage.md) - Understand the core concepts 