# Copyright (c) 2025 UESynth Project
# SPDX-License-Identifier: MIT

# UESynth Automated Test Execution Script
# Provides comprehensive test automation with CI/CD integration support

param(
    [string]$ProjectPath = "UESynth.uproject",
    [string]$OutputDir = "TestResults",
    [string]$LogLevel = "Log",
    [string]$TestFilter = "",
    [switch]$UnitOnly,
    [switch]$IntegrationOnly,
    [switch]$Verbose,
    [switch]$CIMode,
    [switch]$GenerateReport,
    [int]$TimeoutMinutes = 30
)

# Set error action preference for CI environments
if ($CIMode) {
    $ErrorActionPreference = "Stop"
}

Write-Host "UESynth Automated Test Execution" -ForegroundColor Green
Write-Host "================================" -ForegroundColor Green
Write-Host "Project: $ProjectPath" -ForegroundColor Cyan
Write-Host "Output Directory: $OutputDir" -ForegroundColor Cyan
Write-Host "CI Mode: $CIMode" -ForegroundColor Cyan
Write-Host ""

# Create output directory structure
$Directories = @($OutputDir, "$OutputDir/Logs", "$OutputDir/Reports", "$OutputDir/Screenshots")
foreach ($Dir in $Directories) {
    if (!(Test-Path $Dir)) {
        New-Item -ItemType Directory -Path $Dir -Force | Out-Null
        Write-Host "Created directory: $Dir" -ForegroundColor Yellow
    }
}

# Function to validate test environment
function Test-Environment {
    Write-Host "Validating test environment..." -ForegroundColor Cyan
    
    # Check if UE Editor exists
    $UEEditor = Get-Command "UnrealEditor-Cmd.exe" -ErrorAction SilentlyContinue
    if (!$UEEditor) {
        Write-Host "ERROR: UnrealEditor-Cmd.exe not found in PATH" -ForegroundColor Red
        Write-Host "Please ensure Unreal Engine is installed and added to PATH" -ForegroundColor Red
        return $false
    }
    
    # Check if project file exists
    if (!(Test-Path $ProjectPath)) {
        Write-Host "ERROR: Project file not found: $ProjectPath" -ForegroundColor Red
        return $false
    }
    
    # Check if test module exists
    $TestModulePath = "Source/UESynth/Tests/UESynthTests.Build.cs"
    if (!(Test-Path $TestModulePath)) {
        Write-Host "ERROR: Test module not found: $TestModulePath" -ForegroundColor Red
        return $false
    }
    
    Write-Host "Environment validation passed" -ForegroundColor Green
    return $true
}

# Function to discover available tests
function Get-AvailableTests {
    Write-Host "Discovering available tests..." -ForegroundColor Cyan
    
    $TestDiscoveryArgs = @(
        $ProjectPath,
        "-ExecCmds=`"Automation List; quit`"",
        "-log",
        "-LogLevel=Log",
        "-stdout"
    )
    
    try {
        $Process = Start-Process -FilePath "UnrealEditor-Cmd.exe" -ArgumentList $TestDiscoveryArgs -Wait -PassThru -NoNewWindow -RedirectStandardOutput "$OutputDir/test-discovery.log"
        
        if ($Process.ExitCode -eq 0) {
            $DiscoveryLog = Get-Content "$OutputDir/test-discovery.log" -Raw
            $UESynthTests = $DiscoveryLog | Select-String -Pattern "UESynth\." -AllMatches
            
            Write-Host "Discovered $($UESynthTests.Matches.Count) UESynth tests" -ForegroundColor Green
            return $UESynthTests.Matches.Count -gt 0
        }
    }
    catch {
        Write-Host "Warning: Test discovery failed: $($_.Exception.Message)" -ForegroundColor Yellow
    }
    
    return $true # Continue even if discovery fails
}

# Function to run tests with timeout
function Invoke-TestExecution {
    param(
        [string]$TestCategory,
        [string]$TestName,
        [int]$TimeoutSeconds = 1800
    )
    
    Write-Host "Running $TestName Tests..." -ForegroundColor Cyan
    Write-Host ("=" * (20 + $TestName.Length)) -ForegroundColor Cyan
    
    $LogFile = "$OutputDir/Logs/$TestName-execution.log"
    $ReportFile = "$OutputDir/Reports/$TestName-results.xml"
    
    $Arguments = @(
        $ProjectPath,
        "-ExecCmds=`"Automation RunTests $TestCategory; quit`"",
        "-TestExit=`"Automation Test Queue Empty`"",
        "-ReportOutputPath=`"$OutputDir/Reports`"",
        "-log",
        "-LogLevel=$LogLevel",
        "-stdout"
    )
    
    if ($Verbose) {
        $Arguments += "-Verbose"
    }
    
    if ($CIMode) {
        $Arguments += "-unattended"
        $Arguments += "-nopause"
    }
    
    try {
        $Process = Start-Process -FilePath "UnrealEditor-Cmd.exe" -ArgumentList $Arguments -Wait -PassThru -NoNewWindow -RedirectStandardOutput $LogFile
        
        $Result = @{
            ExitCode = $Process.ExitCode
            TestName = $TestName
            LogFile = $LogFile
            ReportFile = $ReportFile
            Duration = $Process.TotalProcessorTime
        }
        
        if ($Process.ExitCode -eq 0) {
            Write-Host "$TestName tests completed successfully!" -ForegroundColor Green
            $Result.Status = "PASSED"
        } else {
            Write-Host "$TestName tests failed with exit code: $($Process.ExitCode)" -ForegroundColor Red
            $Result.Status = "FAILED"
        }
        
        return $Result
    }
    catch {
        Write-Host "Error running $TestName tests: $($_.Exception.Message)" -ForegroundColor Red
        return @{
            ExitCode = -1
            TestName = $TestName
            Status = "ERROR"
            Error = $_.Exception.Message
        }
    }
}

# Function to generate test report
function New-TestReport {
    param([array]$TestResults)
    
    if (!$GenerateReport) {
        return
    }
    
    Write-Host "Generating test report..." -ForegroundColor Cyan
    
    $ReportPath = "$OutputDir/test-summary.html"
    $TotalTests = $TestResults.Count
    $PassedTests = ($TestResults | Where-Object { $_.Status -eq "PASSED" }).Count
    $FailedTests = ($TestResults | Where-Object { $_.Status -eq "FAILED" }).Count
    $ErrorTests = ($TestResults | Where-Object { $_.Status -eq "ERROR" }).Count
    
    $Html = @"
<!DOCTYPE html>
<html>
<head>
    <title>UESynth Test Results</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 10px; border-radius: 5px; }
        .summary { margin: 20px 0; }
        .passed { color: green; font-weight: bold; }
        .failed { color: red; font-weight: bold; }
        .error { color: orange; font-weight: bold; }
        table { border-collapse: collapse; width: 100%; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
    </style>
</head>
<body>
    <div class="header">
        <h1>UESynth Test Results</h1>
        <p>Generated: $(Get-Date)</p>
        <p>Project: $ProjectPath</p>
    </div>
    
    <div class="summary">
        <h2>Summary</h2>
        <p>Total Tests: $TotalTests</p>
        <p class="passed">Passed: $PassedTests</p>
        <p class="failed">Failed: $FailedTests</p>
        <p class="error">Errors: $ErrorTests</p>
    </div>
    
    <h2>Test Details</h2>
    <table>
        <tr>
            <th>Test Suite</th>
            <th>Status</th>
            <th>Exit Code</th>
            <th>Log File</th>
        </tr>
"@
    
    foreach ($Result in $TestResults) {
        $StatusClass = $Result.Status.ToLower()
        $Html += @"
        <tr>
            <td>$($Result.TestName)</td>
            <td class="$StatusClass">$($Result.Status)</td>
            <td>$($Result.ExitCode)</td>
            <td><a href="$($Result.LogFile)">View Log</a></td>
        </tr>
"@
    }
    
    $Html += @"
    </table>
</body>
</html>
"@
    
    $Html | Out-File -FilePath $ReportPath -Encoding UTF8
    Write-Host "Test report generated: $ReportPath" -ForegroundColor Green
}

# Main execution
try {
    # Validate environment
    if (!(Test-Environment)) {
        exit 1
    }
    
    # Discover tests
    if (!(Get-AvailableTests)) {
        Write-Host "Warning: No tests discovered, continuing anyway..." -ForegroundColor Yellow
    }
    
    # Execute tests
    $TestResults = @()
    $AllTestsPassed = $true
    
    if (!$IntegrationOnly) {
        Write-Host "Starting Unit Tests..." -ForegroundColor White
        $Filter = if ($TestFilter) { $TestFilter } else { "UESynth.Unit" }
        $UnitResult = Invoke-TestExecution -TestCategory $Filter -TestName "Unit"
        $TestResults += $UnitResult
        $AllTestsPassed = $AllTestsPassed -and ($UnitResult.Status -eq "PASSED")
        Write-Host ""
    }
    
    if (!$UnitOnly) {
        Write-Host "Starting Integration Tests..." -ForegroundColor White
        $Filter = if ($TestFilter) { $TestFilter } else { "UESynth.Integration" }
        $IntegrationResult = Invoke-TestExecution -TestCategory $Filter -TestName "Integration"
        $TestResults += $IntegrationResult
        $AllTestsPassed = $AllTestsPassed -and ($IntegrationResult.Status -eq "PASSED")
        Write-Host ""
    }
    
    # Generate report
    New-TestReport -TestResults $TestResults
    
    # Summary
    Write-Host "Test Execution Summary" -ForegroundColor White
    Write-Host "=====================" -ForegroundColor White
    
    foreach ($Result in $TestResults) {
        $Color = if ($Result.Status -eq "PASSED") { "Green" } elseif ($Result.Status -eq "FAILED") { "Red" } else { "Yellow" }
        Write-Host "$($Result.TestName): $($Result.Status)" -ForegroundColor $Color
    }
    
    Write-Host ""
    Write-Host "Test results saved to: $OutputDir" -ForegroundColor Yellow
    
    if ($AllTestsPassed) {
        Write-Host "All tests PASSED!" -ForegroundColor Green
        exit 0
    } else {
        Write-Host "Some tests FAILED!" -ForegroundColor Red
        if ($CIMode) {
            exit 1
        }
    }
}
catch {
    Write-Host "Fatal error during test execution: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host $_.ScriptStackTrace -ForegroundColor Red
    
    if ($CIMode) {
        exit 1
    }
}

# Usage examples:
# .\AutomateTests.ps1                                    # Run all tests
# .\AutomateTests.ps1 -UnitOnly                         # Run only unit tests
# .\AutomateTests.ps1 -IntegrationOnly                  # Run only integration tests
# .\AutomateTests.ps1 -CIMode -GenerateReport           # CI mode with HTML report
# .\AutomateTests.ps1 -TestFilter "UESynth.Unit.ServiceImpl" # Run specific test category
# .\AutomateTests.ps1 -Verbose -TimeoutMinutes 45       # Verbose output with custom timeout