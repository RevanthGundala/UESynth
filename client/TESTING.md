# Testing Guide for UESynth Client

This guide explains how to run and understand the tests for the UESynth client library.

## Test Architecture

The test suite now covers both client architectures:

1. **`UESynthClient`** - Synchronous client tests
2. **`AsyncUESynthClient`** - Asynchronous client tests

## Setup

### Install Dependencies

First, install the development dependencies:

```bash
cd client
uv sync --group dev
```

### Dependencies Overview

The project now uses:
- **`basedpyright`** for type checking (replaced mypy)
- **`pytest`** for test execution
- **`pytest-asyncio`** for async test support
- **`pytest-cov`** for coverage reporting
- **`pytest-mock`** for enhanced mocking capabilities

## Running Tests

### Quick Start

Use the provided test runner:

```bash
python test_runner.py
```

### Manual Test Execution

#### Run All Tests
```bash
uv run pytest tests/ -v
```

#### Run Only Synchronous Tests
```bash
uv run pytest tests/ -v -k "TestUESynthClient"
```

#### Run Only Asynchronous Tests
```bash
uv run pytest tests/ -v -k "TestAsyncUESynthClient"
```

#### Run with Coverage
```bash
uv run pytest tests/ --cov=uesynth --cov-report=term-missing
```

#### Run with HTML Coverage Report
```bash
uv run pytest tests/ --cov=uesynth --cov-report=html
```

## Type Checking

### basedpyright Configuration

The project uses `basedpyright` (instead of mypy) for type checking:

```bash
uv run basedpyright uesynth/ tests/
```

### Key Configuration Features

- **Strict mode** enabled for the `uesynth` package
- **Excludes** generated protobuf files (`*_pb2.py`, `*_pb2_grpc.py`)
- **Comprehensive reporting** of unused imports, variables, and functions
- **Python 3.12** target version

## Test Structure

### Test Classes

1. **`TestUESynthClient`** - Tests for synchronous client
   - Connection/disconnection
   - Camera operations
   - Capture operations
   - Object operations

2. **`TestAsyncUESynthClient`** - Tests for asynchronous client
   - Async connection/disconnection
   - Async camera operations
   - Async capture operations (both streaming and direct)
   - Async object operations
   - Streaming functionality

3. **Component Tests** - Tests for individual components
   - Camera component initialization
   - Capture component initialization
   - Objects component initialization

### Async Testing Patterns

The async tests use several patterns:

#### 1. AsyncMock for Async Operations
```python
@patch("uesynth.grpc.aio.insecure_channel")
async def test_async_method(self, mock_channel):
    mock_stub = AsyncMock()
    # ... test setup
    await client.some_async_method()
    mock_stub.some_method.assert_called_once()
```

#### 2. Mocking Streaming Operations
```python
async def test_streaming_operation(self):
    client = AsyncUESynthClient()
    
    # Mock internal streaming methods to avoid actual network calls
    with patch.object(client, '_send_action', new_callable=AsyncMock) as mock_send:
        mock_send.return_value = "request_id"
        
        request_id = await client.camera.set_location(x=1.0, y=2.0, z=3.0)
        
        assert request_id == "request_id"
        mock_send.assert_called_once()
```

#### 3. Testing Frame Handling
```python
async def test_get_latest_frame(self):
    client = AsyncUESynthClient()
    client.lock = asyncio.Lock()
    
    # Mock frame data
    mock_image_response = Mock()
    mock_image_response.image_data = b'\x00' * (50 * 50 * 3)
    mock_image_response.height = 50
    mock_image_response.width = 50
    
    client.latest_responses = {'image': mock_image_response}
    
    frame = await client.get_latest_frame()
    
    assert frame is not None
    assert frame.shape == (50, 50, 3)
```

## Code Quality Tools

### Linting
```bash
uv run ruff check uesynth/ tests/
```

### Formatting
```bash
uv run ruff format uesynth/ tests/
```

### Format Check (CI)
```bash
uv run ruff format --check uesynth/ tests/
```

## CI/CD Integration

The test runner script (`test_runner.py`) provides a complete CI pipeline:

1. **Dependency Installation** - `uv sync --group dev`
2. **Type Checking** - `basedpyright`
3. **Linting** - `ruff check`
4. **Format Checking** - `ruff format --check`
5. **Test Execution** - `pytest` with coverage
6. **Separate Test Runs** - Sync and async tests individually

## Test Coverage

The test suite covers:

- ✅ **Client initialization** for both sync and async
- ✅ **Connection/disconnection** handling
- ✅ **Camera operations** (set/get location, rotation, creation)
- ✅ **Capture operations** (RGB, depth, segmentation)
- ✅ **Object operations** (set location, spawn, transform)
- ✅ **Streaming functionality** (async client only)
- ✅ **Frame handling** (async client only)
- ✅ **Component initialization** for all sub-components

## Migration from mypy to basedpyright

### What Changed

1. **Type Checker**: `mypy` → `basedpyright`
2. **Configuration**: `[tool.mypy]` → `[tool.basedpyright]` in `pyproject.toml`
3. **Enhanced Features**: Better IDE integration, more comprehensive type checking
4. **Async Support**: Better handling of async/await patterns

### Benefits

- **Better Performance**: Faster type checking
- **Enhanced IDE Support**: Better integration with VS Code
- **Improved Error Messages**: More detailed type error reporting
- **Better Async Support**: Superior handling of asyncio patterns 