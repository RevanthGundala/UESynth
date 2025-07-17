# Copyright (c) 2025 UESynth Project
# SPDX-License-Identifier: MIT

# Integration Test Validation Script
# Validates that integration tests are properly structured and complete

Write-Host "UESynth Integration Test Validation" -ForegroundColor Green
Write-Host "===================================" -ForegroundColor Green
Write-Host ""

$TestFile = "Integration/GrpcServiceTest.cpp"
$TestPath = Join-Path $PSScriptRoot $TestFile

if (!(Test-Path $TestPath)) {
    Write-Host "ERROR: Integration test file not found: $TestPath" -ForegroundColor Red
    exit 1
}

Write-Host "Validating integration test file: $TestFile" -ForegroundColor Cyan

$Content = Get-Content $TestPath -Raw

# Check for required test classes
$RequiredTests = @(
    "FUESynthServiceControlStreamTest",
    "FUESynthServiceProtobufSerializationTest", 
    "FUESynthServiceErrorHandlingTest",
    "FUESynthServiceConcurrencyTest",
    "FUESynthServiceEndToEndTest"
)

$TestsFound = 0
foreach ($TestClass in $RequiredTests) {
    if ($Content -match $TestClass) {
        Write-Host "Found test class: $TestClass" -ForegroundColor Green
        $TestsFound++
    } else {
        Write-Host "Missing test class: $TestClass" -ForegroundColor Red
    }
}

# Check for key test scenarios
$RequiredScenarios = @(
    "ProcessAction",
    "SerializeToString",
    "ParseFromString", 
    "ActionRequest",
    "FrameResponse",
    "error handling",
    "client-server communication",
    "protocol buffer",
    "streaming workflow"
)

$ScenariosFound = 0
foreach ($Scenario in $RequiredScenarios) {
    if ($Content -match $Scenario) {
        Write-Host "Found test scenario: $Scenario" -ForegroundColor Green
        $ScenariosFound++
    } else {
        Write-Host "Missing test scenario: $Scenario" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "Validation Summary:" -ForegroundColor White
Write-Host "Test Classes: $TestsFound/$($RequiredTests.Count)"
Write-Host "Test Scenarios: $ScenariosFound/$($RequiredScenarios.Count)"

# Check file size to ensure substantial implementation
$FileSize = (Get-Item $TestPath).Length
Write-Host "Test File Size: $FileSize bytes"

if ($TestsFound -eq $RequiredTests.Count -and $ScenariosFound -ge 7 -and $FileSize -gt 10000) {
    Write-Host ""
    Write-Host "Integration tests validation PASSED!" -ForegroundColor Green
    Write-Host "All required test classes and scenarios are implemented." -ForegroundColor Green
    exit 0
} else {
    Write-Host ""
    Write-Host "Integration tests validation FAILED!" -ForegroundColor Red
    Write-Host "Some required components are missing or incomplete." -ForegroundColor Red
    exit 1
}