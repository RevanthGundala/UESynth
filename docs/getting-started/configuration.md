# Configuration

UESynth provides various configuration options to customize the plugin behavior and optimize performance for your specific use case.

## Plugin Configuration

### Editor Settings

Access UESynth settings in the Unreal Editor:

1. Go to `Edit → Project Settings`
2. Navigate to `Plugins → UESynth`
3. Adjust the following settings:

#### Network Settings

| Setting | Default | Description |
|---------|---------|-------------|
| **gRPC Port** | `50051` | Port for the gRPC server |
| **Enable SSL** | `false` | Use secure connections (requires certificates) |
| **Max Connections** | `10` | Maximum concurrent client connections |
| **Timeout (ms)** | `5000` | Request timeout in milliseconds |

#### Performance Settings

| Setting | Default | Description |
|---------|---------|-------------|
| **Enable Streaming** | `true` | Allow bidirectional streaming for async clients |
| **Stream Buffer Size** | `1024` | Buffer size for streaming responses |
| **Async Mode** | `true` | Process requests asynchronously |
| **Thread Pool Size** | `4` | Number of worker threads for processing |

#### Capture Settings

| Setting | Default | Description |
|---------|---------|-------------|
| **Default Image Format** | `PNG` | Default format for captured images |
| **Image Quality** | `95` | JPEG quality (0-100) when using JPEG format |
| **Enable Depth Capture** | `true` | Allow depth map generation |
| **Enable Segmentation** | `true` | Allow segmentation mask generation |
| **Max Resolution** | `4096x4096` | Maximum allowed capture resolution |

### Runtime Configuration

You can also modify settings at runtime through the Python client:

```python
from uesynth import UESynthClient

client = UESynthClient()

# Configure capture settings
client.config.set_image_format("JPEG")
client.config.set_image_quality(85)
client.config.set_max_resolution(1920, 1080)

# Configure performance
client.config.set_async_mode(True)
client.config.set_timeout(10000)  # 10 seconds
```

## Python Client Configuration

### Connection Settings

Configure the client connection parameters:

```python
from uesynth import UESynthClient

# Custom connection settings
client = UESynthClient(
    host="localhost",
    port=50051,
    timeout=10.0,
    enable_ssl=False,
    max_retries=3
)
```

### Async Client Configuration

For the async client, additional options are available:

```python
from uesynth import AsyncUESynthClient

client = AsyncUESynthClient(
    host="localhost",
    port=50051,
    stream_buffer_size=2048,
    max_concurrent_requests=100,
    enable_compression=True
)
```

## Environment Variables

UESynth respects the following environment variables:

| Variable | Default | Description |
|----------|---------|-------------|
| `UESYNTH_HOST` | `localhost` | Default host for connections |
| `UESYNTH_PORT` | `50051` | Default port for connections |
| `UESYNTH_TIMEOUT` | `5.0` | Default timeout in seconds |
| `UESYNTH_LOG_LEVEL` | `INFO` | Logging level (DEBUG, INFO, WARNING, ERROR) |
| `UESYNTH_ENABLE_SSL` | `false` | Enable SSL by default |

Example usage:

```bash
export UESYNTH_HOST=192.168.1.100
export UESYNTH_PORT=50052
export UESYNTH_LOG_LEVEL=DEBUG

python my_script.py
```

## Advanced Configuration

### Custom gRPC Options

For advanced users, you can pass custom gRPC channel options:

```python
from uesynth import UESynthClient
import grpc

# Custom gRPC options
options = [
    ('grpc.keepalive_time_ms', 30000),
    ('grpc.keepalive_timeout_ms', 5000),
    ('grpc.http2.max_pings_without_data', 0),
    ('grpc.http2.min_time_between_pings_ms', 10000),
    ('grpc.http2.min_ping_interval_without_data_ms', 300000)
]

client = UESynthClient(grpc_options=options)
```

### Logging Configuration

Configure logging for debugging and monitoring:

```python
import logging
from uesynth import UESynthClient

# Set up detailed logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

# Enable gRPC debug logging
logging.getLogger('grpc').setLevel(logging.DEBUG)

client = UESynthClient()
```

### SSL/TLS Configuration

For secure connections in production environments:

#### Server Side (Unreal Engine)

1. Generate or obtain SSL certificates
2. Place certificates in your project's `Config` folder
3. Enable SSL in plugin settings
4. Specify certificate paths:
   - `server.crt` - Server certificate
   - `server.key` - Private key
   - `ca.crt` - Certificate Authority (optional)

#### Client Side (Python)

```python
from uesynth import UESynthClient

# Connect with SSL
client = UESynthClient(
    host="your-server.com",
    port=50051,
    enable_ssl=True,
    ssl_cert_path="path/to/client.crt",
    ssl_key_path="path/to/client.key",
    ssl_ca_path="path/to/ca.crt"
)
```

## Configuration Best Practices

### Development Environment

```python
# Development settings - prioritize ease of use
client = UESynthClient(
    timeout=30.0,           # Longer timeout for debugging
    max_retries=1,          # Don't retry on errors
    enable_logging=True     # Verbose logging
)
```

### Production Environment

```python
# Production settings - prioritize performance and reliability
client = UESynthClient(
    timeout=5.0,            # Shorter timeout
    max_retries=3,          # Retry on network issues
    enable_ssl=True,        # Secure connections
    enable_compression=True # Reduce network traffic
)
```

### High-Performance Scenarios

```python
# High-performance async setup
client = AsyncUESynthClient(
    stream_buffer_size=4096,       # Large buffer
    max_concurrent_requests=500,   # High concurrency
    enable_compression=True,       # Reduce bandwidth
    timeout=1.0                    # Fast timeout
)
```

## Troubleshooting Configuration

### Validate Settings

Check your current configuration:

```python
client = UESynthClient()

# Print current settings
print(f"Host: {client.host}")
print(f"Port: {client.port}")
print(f"Timeout: {client.timeout}")
print(f"SSL Enabled: {client.ssl_enabled}")

# Test connection with current settings
if client.test_connection():
    print("✅ Configuration is working")
else:
    print("❌ Configuration needs adjustment")
```

### Common Configuration Issues

1. **Port conflicts**: Use `netstat` to check port availability
2. **Firewall blocking**: Ensure the port is open in firewall settings
3. **SSL certificate errors**: Verify certificate paths and validity
4. **Timeout too short**: Increase timeout for complex scenes
5. **Memory issues**: Reduce buffer sizes and concurrent requests

For detailed troubleshooting, see the [Performance Guide](../performance/optimization.md). 