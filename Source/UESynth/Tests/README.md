# UESynth C++ Unit Tests

This directory contains comprehensive unit and integration tests for the UESynth C++ plugin using Unreal Engine's Automation Testing framework.

## Test Structure

```
Tests/
├── Unit/                           # Unit tests for individual components
│   ├── UESynthServiceImplTest.cpp  # Tests for main service implementation
│   ├── TransformUtilsTest.cpp      # Tests for transform utilities
│   └── ImageCaptureTest.cpp        # Tests for image capture functionality
├── Integration/                    # Integration tests
│   └── GrpcServiceTest.cpp         # Tests for gRPC communication
├── Mocks/                          # Mock objects for testing
│   ├── MockUEWorld.h/.cpp          # Mock UE World and Camera
│   └── MockGrpcContext.h/.cpp      # Mock gRPC contexts
├── UESynthTestBase.h/.cpp          # Base test class with utilities
├── UESynthTests.Build.cs           # Build configuration for test module
├── UESynthTests.cpp                # Test module implementation
├── RunTests.bat                    # Windows batch test runner
├── RunTests.ps1                    # PowerShell test runner
└── README.md                       # This file
```

## Test Categories

### Unit Tests

**UESynthServiceImpl Tests** (`UESynth.Unit.ServiceImpl.*`)
- `SetCameraTransform` - Tests camera transform setting functionality
- `GetCameraTransform` - Tests camera transform retrieval
- `CaptureRgbImage` - Tests RGB image capture
- `ProcessAction` - Tests action processing logic
- `ListObjects` - Tests object listing functionality
- `PlaceholderMethods` - Tests placeholder method implementations

**Transform Utilities Tests** (`UESynth.Unit.TransformUtils.*`)
- `Conversion` - Tests UE ↔ Protobuf transform conversions
- `EdgeCases` - Tests boundary conditions and edge cases

**Image Capture Tests** (`UESynth.Unit.ImageCapture.*`)
- `RGB` - Tests RGB image capture functionality
- `Depth` - Tests depth map capture
- `Segmentation` - Tests segmentation mask capture
- `Normals` - Tests normal map capture
- `OpticalFlow` - Tests optical flow capture
- `ErrorConditions` - Tests error handling in capture methods

### Integration Tests

**gRPC Service Tests** (`UESynth.Integration.GrpcService.*`)
- `ControlStream` - Tests bidirectional streaming functionality
- `ProtobufSerialization` - Tests protocol buffer serialization/deserialization
- `ErrorHandling` - Tests error handling across gRPC boundaries
- `Concurrency` - Tests concurrent request handling

## Running Tests

### Prerequisites

1. Unreal Engine 5.1+ installed
2. UESynth project built successfully
3. gRPC and protobuf dependencies configured

### Command Line

**Windows Batch:**
```batch
cd Source\UESynth\Tests
RunTests.bat
```

**PowerShell (Basic):**
```powershell
cd Source\UESynth\Tests
.\RunTests.ps1                          # Run all tests
.\RunTests.ps1 -UnitOnly               # Run only unit tests
.\RunTests.ps1 -IntegrationOnly        # Run only integration tests
.\RunTests.ps1 -Verbose                # Run with verbose output
.\RunTests.ps1 -OutputDir "MyResults"  # Custom output directory
```

**PowerShell (Advanced Automation):**
```powershell
cd Source\UESynth\Tests
.\AutomateTests.ps1                                    # Run all tests with automation
.\AutomateTests.ps1 -CIMode -GenerateReport           # CI mode with HTML report
.\AutomateTests.ps1 -TestFilter "UESynth.Unit.ServiceImpl" # Run specific test category
.\AutomateTests.ps1 -Verbose -TimeoutMinutes 45       # Verbose output with custom timeout
```

**Infrastructure Validation:**
```powershell
cd Source\UESynth\Tests
.\ValidateTestInfrastructure.ps1        # Validate test setup
.\ValidateIntegrationTests.ps1          # Validate integration tests
```

**Manual UE Command:**
```
UnrealEditor-Cmd.exe UESynth.uproject -ExecCmds="Automation RunTests UESynth.Unit; quit" -TestExit="Automation Test Queue Empty" -ReportOutputPath="TestResults" -log
```

### UE Editor

1. Open UE Editor with UESynth project
2. Go to **Window > Developer Tools > Session Frontend**
3. Switch to **Automation** tab
4. Expand **UESynth** test category
5. Select tests to run and click **Start Tests**

## Test Results

Test results are saved to the `TestResults` directory in various formats:
- **JSON** - Machine-readable test results
- **XML** - JUnit-compatible format for CI systems
- **HTML** - Human-readable test report
- **Logs** - Detailed execution logs

## Mock Objects

The test suite uses mock objects to isolate functionality:

**MockUEWorld**
- Simulates UE World environment
- Provides controlled camera and viewport state
- Generates test pixel data for image capture tests

**MockGrpcContext**
- Simulates gRPC server contexts
- Controls cancellation and deadline states
- Provides mock streaming interfaces

**MockCameraActor**
- Simulates UE Camera Actor behavior
- Provides controllable transform state
- Enables testing without actual UE runtime

## Writing New Tests

### Unit Test Template

```cpp
#include "../UESynthTestBase.h"

class FMyNewTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FMyNewTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FMyNewTest,
    "UESynth.Unit.MyCategory.MyTest",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test implementation
    UESYNTH_TEST_TRUE(true, "This test should pass");
    return true;
}
```

### Test Assertions

Use the provided assertion macros:
- `UESYNTH_TEST_TRUE(condition, message)` - Assert condition is true
- `UESYNTH_TEST_FALSE(condition, message)` - Assert condition is false
- `UESYNTH_TEST_EQUAL(expected, actual, message)` - Assert values are equal
- `AssertGrpcStatusOk(status, context)` - Assert gRPC status is OK
- `AssertTransformEquals(expected, actual, tolerance)` - Assert transforms match

## CI/CD Integration

These tests are designed to integrate with the CI/CD pipeline:

1. **Build Validation** - Tests run after successful C++ compilation
2. **Automated Execution** - PowerShell runner supports CI environments
3. **Result Reporting** - JUnit XML format for CI system integration
4. **Failure Analysis** - Detailed logs for debugging failed tests

## Troubleshooting

**Common Issues:**

1. **Tests not discovered**
   - Ensure UESynthTests module is built
   - Check that test files are included in build
   - Verify IMPLEMENT_UESYNTH_UNIT_TEST macro usage

2. **Mock object failures**
   - Check MockUEWorld setup in test SetUp()
   - Verify mock data matches test expectations
   - Ensure proper cleanup in TearDown()

3. **gRPC-related test failures**
   - Verify gRPC libraries are linked correctly
   - Check protobuf message definitions
   - Ensure mock contexts are properly configured

4. **UE Editor crashes during tests**
   - Run tests in command-line mode first
   - Check for memory leaks in test cleanup
   - Verify UE API usage in mock objects

## Performance Considerations

- Tests use mock objects to avoid expensive UE operations
- Image capture tests use small test data sets
- Concurrent tests are simulated rather than truly parallel
- Test execution time should be under 30 seconds total

## Requirements Coverage

These tests cover the following requirements from the design document:

- **Requirement 2.1** - Automated unit tests for C++ components
- **Requirement 2.2** - Integration tests for gRPC service functionality  
- **Requirement 2.3** - Detailed test results and failure reporting
- **Requirement 4.1-4.3** - Integration testing for client-server communication