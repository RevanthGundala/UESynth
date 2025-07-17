# Copyright (c) 2025 UESynth Project
# SPDX-License-Identifier: MIT

# UESynth C++ Unit Test Runner (PowerShell)
# Executes all UESynth unit and integration tests using UE Automation framework

param(
    [string]$ProjectPath = "UESynth.uproject",
    [string]$OutputDir = "TestResults",
    [string]$LogLevel = "Log",
    [switch]$UnitOnly,
    [switch]$IntegrationOnly,
    [switch]$Verbose
)

Write-Host "UESynth C++ Test Runner" -ForegroundColor Green
Write-Host "======================" -ForegroundColor Green
Write-Host ""

# Create output directory
if (!(Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    Write-Host "Created output directory: $OutputDir" -ForegroundColor Yellow
}

# Function to run tests
function Run-UESynthTests {
    param(
        [string]$TestFilter,
        [string]$TestType
    )
    
    Write-Host "Running $TestType Tests..." -ForegroundColor Cyan
    Write-Host ("=" * (20 + $TestType.Length)) -ForegroundColor Cyan
    
    $Arguments = @(
        $ProjectPath,
        "-ExecCmds=`"Automation RunTests $TestFilter; quit`"",
        "-TestExit=`"Automation Test Queue Empty`"",
        "-ReportOutputPath=`"$OutputDir`"",
        "-log",
        "-LogLevel=$LogLevel"
    )
    
    if ($Verbose) {
        $Arguments += "-Verbose"
    }
    
    try {
        $Process = Start-Process -FilePath "UnrealEditor-Cmd.exe" -ArgumentList $Arguments -Wait -PassThru -NoNewWindow
        
        if ($Process.ExitCode -eq 0) {
            Write-Host "$TestType tests completed successfully!" -ForegroundColor Green
            return $true
        } else {
            Write-Host "$TestType tests failed with exit code: $($Process.ExitCode)" -ForegroundColor Red
            return $false
        }
    }
    catch {
        Write-Host "Error running $TestType tests: $($_.Exception.Message)" -ForegroundColor Red
        return $false
    }
}

# Run tests based on parameters
$AllTestsPassed = $true

if (!$IntegrationOnly) {
    Write-Host "Starting Unit Tests..." -ForegroundColor White
    $UnitTestResult = Run-UESynthTests -TestFilter "UESynth.Unit" -TestType "Unit"
    $AllTestsPassed = $AllTestsPassed -and $UnitTestResult
    Write-Host ""
}

if (!$UnitOnly) {
    Write-Host "Starting Integration Tests..." -ForegroundColor White
    $IntegrationTestResult = Run-UESynthTests -TestFilter "UESynth.Integration" -TestType "Integration"
    $AllTestsPassed = $AllTestsPassed -and $IntegrationTestResult
    Write-Host ""
}

# Summary
Write-Host "Test Execution Summary" -ForegroundColor White
Write-Host "=====================" -ForegroundColor White

if ($AllTestsPassed) {
    Write-Host "All tests PASSED!" -ForegroundColor Green
    Write-Host "Test results saved to: $OutputDir" -ForegroundColor Yellow
    exit 0
} else {
    Write-Host "Some tests FAILED!" -ForegroundColor Red
    Write-Host "Check the logs in: $OutputDir" -ForegroundColor Yellow
    exit 1
}

# Usage examples:
# .\RunTests.ps1                          # Run all tests
# .\RunTests.ps1 -UnitOnly               # Run only unit tests
# .\RunTests.ps1 -IntegrationOnly        # Run only integration tests
# .\RunTests.ps1 -Verbose                # Run with verbose output
# .\RunTests.ps1 -OutputDir "MyResults"  # Custom output directory