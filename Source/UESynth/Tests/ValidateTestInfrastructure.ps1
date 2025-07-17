# Copyright (c) 2025 UESynth Project
# SPDX-License-Identifier: MIT

# Test Infrastructure Validation Script
# Validates that the UE Automation Testing infrastructure is properly configured

Write-Host "UESynth Test Infrastructure Validation" -ForegroundColor Green
Write-Host "======================================" -ForegroundColor Green
Write-Host ""

$ValidationErrors = @()
$ValidationWarnings = @()

# Function to add validation error
function Add-ValidationError {
    param([string]$Message)
    $ValidationErrors += $Message
    Write-Host "ERROR: $Message" -ForegroundColor Red
}

# Function to add validation warning
function Add-ValidationWarning {
    param([string]$Message)
    $ValidationWarnings += $Message
    Write-Host "WARNING: $Message" -ForegroundColor Yellow
}

# Function to validate file exists
function Test-FileExists {
    param([string]$Path, [string]$Description)
    
    if (Test-Path $Path) {
        Write-Host "✓ $Description found: $Path" -ForegroundColor Green
        return $true
    } else {
        Add-ValidationError "$Description not found: $Path"
        return $false
    }
}

# Function to validate directory structure
function Test-DirectoryStructure {
    Write-Host "Validating test directory structure..." -ForegroundColor Cyan
    
    $RequiredDirs = @(
        @{ Path = "Source/UESynth/Tests"; Description = "Tests root directory" },
        @{ Path = "Source/UESynth/Tests/Unit"; Description = "Unit tests directory" },
        @{ Path = "Source/UESynth/Tests/Integration"; Description = "Integration tests directory" },
        @{ Path = "Source/UESynth/Tests/Mocks"; Description = "Mock objects directory" }
    )
    
    $AllDirsExist = $true
    foreach ($Dir in $RequiredDirs) {
        if (!(Test-Path $Dir.Path)) {
            Add-ValidationError "$($Dir.Description) not found: $($Dir.Path)"
            $AllDirsExist = $false
        } else {
            Write-Host "✓ $($Dir.Description) found" -ForegroundColor Green
        }
    }
    
    return $AllDirsExist
}

# Function to validate core test files
function Test-CoreFiles {
    Write-Host "Validating core test files..." -ForegroundColor Cyan
    
    $RequiredFiles = @(
        @{ Path = "Source/UESynth/Tests/UESynthTestBase.h"; Description = "Test base header" },
        @{ Path = "Source/UESynth/Tests/UESynthTestBase.cpp"; Description = "Test base implementation" },
        @{ Path = "Source/UESynth/Tests/UESynthTests.Build.cs"; Description = "Test module build configuration" },
        @{ Path = "Source/UESynth/Tests/UESynthTests.cpp"; Description = "Test module implementation" }
    )
    
    $AllFilesExist = $true
    foreach ($File in $RequiredFiles) {
        if (!(Test-FileExists $File.Path $File.Description)) {
            $AllFilesExist = $false
        }
    }
    
    return $AllFilesExist
}

# Function to validate test execution scripts
function Test-ExecutionScripts {
    Write-Host "Validating test execution scripts..." -ForegroundColor Cyan
    
    $RequiredScripts = @(
        @{ Path = "Source/UESynth/Tests/RunTests.ps1"; Description = "PowerShell test runner" },
        @{ Path = "Source/UESynth/Tests/RunTests.bat"; Description = "Batch test runner" },
        @{ Path = "Source/UESynth/Tests/AutomateTests.ps1"; Description = "Automated test runner" }
    )
    
    $AllScriptsExist = $true
    foreach ($Script in $RequiredScripts) {
        if (!(Test-FileExists $Script.Path $Script.Description)) {
            $AllScriptsExist = $false
        }
    }
    
    return $AllScriptsExist
}

# Function to validate mock objects
function Test-MockObjects {
    Write-Host "Validating mock objects..." -ForegroundColor Cyan
    
    $RequiredMocks = @(
        @{ Path = "Source/UESynth/Tests/Mocks/MockUEWorld.h"; Description = "Mock UE World header" },
        @{ Path = "Source/UESynth/Tests/Mocks/MockUEWorld.cpp"; Description = "Mock UE World implementation" },
        @{ Path = "Source/UESynth/Tests/Mocks/MockGrpcContext.h"; Description = "Mock gRPC Context header" },
        @{ Path = "Source/UESynth/Tests/Mocks/MockGrpcContext.cpp"; Description = "Mock gRPC Context implementation" }
    )
    
    $AllMocksExist = $true
    foreach ($Mock in $RequiredMocks) {
        if (!(Test-FileExists $Mock.Path $Mock.Description)) {
            $AllMocksExist = $false
        }
    }
    
    return $AllMocksExist
}

# Function to validate unit tests
function Test-UnitTests {
    Write-Host "Validating unit test files..." -ForegroundColor Cyan
    
    $RequiredUnitTests = @(
        @{ Path = "Source/UESynth/Tests/Unit/UESynthServiceImplTest.cpp"; Description = "Service implementation tests" },
        @{ Path = "Source/UESynth/Tests/Unit/TransformUtilsTest.cpp"; Description = "Transform utilities tests" },
        @{ Path = "Source/UESynth/Tests/Unit/ImageCaptureTest.cpp"; Description = "Image capture tests" }
    )
    
    $AllTestsExist = $true
    foreach ($Test in $RequiredUnitTests) {
        if (!(Test-FileExists $Test.Path $Test.Description)) {
            $AllTestsExist = $false
        }
    }
    
    return $AllTestsExist
}

# Function to validate integration tests
function Test-IntegrationTests {
    Write-Host "Validating integration test files..." -ForegroundColor Cyan
    
    $RequiredIntegrationTests = @(
        @{ Path = "Source/UESynth/Tests/Integration/GrpcServiceTest.cpp"; Description = "gRPC service integration tests" }
    )
    
    $AllTestsExist = $true
    foreach ($Test in $RequiredIntegrationTests) {
        if (!(Test-FileExists $Test.Path $Test.Description)) {
            $AllTestsExist = $false
        }
    }
    
    return $AllTestsExist
}

# Function to validate test content
function Test-TestContent {
    Write-Host "Validating test content..." -ForegroundColor Cyan
    
    # Check UESynthTestBase.h for required macros
    $TestBaseHeader = "Source/UESynth/Tests/UESynthTestBase.h"
    if (Test-Path $TestBaseHeader) {
        $Content = Get-Content $TestBaseHeader -Raw
        
        if ($Content -match "IMPLEMENT_UESYNTH_UNIT_TEST") {
            Write-Host "✓ IMPLEMENT_UESYNTH_UNIT_TEST macro found" -ForegroundColor Green
        } else {
            Add-ValidationError "IMPLEMENT_UESYNTH_UNIT_TEST macro not found in $TestBaseHeader"
        }
        
        if ($Content -match "class UESynthTestBase") {
            Write-Host "✓ UESynthTestBase class found" -ForegroundColor Green
        } else {
            Add-ValidationError "UESynthTestBase class not found in $TestBaseHeader"
        }
    }
    
    # Check build configuration
    $BuildConfig = "Source/UESynth/Tests/UESynthTests.Build.cs"
    if (Test-Path $BuildConfig) {
        $Content = Get-Content $BuildConfig -Raw
        
        if ($Content -match "AutomationController") {
            Write-Host "✓ AutomationController dependency found" -ForegroundColor Green
        } else {
            Add-ValidationWarning "AutomationController dependency not found in build configuration"
        }
        
        if ($Content -match "UnrealEd") {
            Write-Host "✓ UnrealEd dependency found" -ForegroundColor Green
        } else {
            Add-ValidationWarning "UnrealEd dependency not found in build configuration"
        }
    }
}

# Function to validate test discovery
function Test-TestDiscovery {
    Write-Host "Validating test discovery patterns..." -ForegroundColor Cyan
    
    $TestFiles = Get-ChildItem -Path "Source/UESynth/Tests" -Recurse -Filter "*.cpp" | Where-Object { $_.Name -like "*Test.cpp" }
    
    $TestClassCount = 0
    $TestMacroCount = 0
    
    foreach ($TestFile in $TestFiles) {
        $Content = Get-Content $TestFile.FullName -Raw
        
        # Count test classes
        $ClassMatches = [regex]::Matches($Content, "class F\w+Test\s*:")
        $TestClassCount += $ClassMatches.Count
        
        # Count test macros
        $MacroMatches = [regex]::Matches($Content, "IMPLEMENT_UESYNTH_UNIT_TEST")
        $TestMacroCount += $MacroMatches.Count
    }
    
    Write-Host "✓ Found $TestClassCount test classes" -ForegroundColor Green
    Write-Host "✓ Found $TestMacroCount test implementations" -ForegroundColor Green
    
    if ($TestClassCount -eq 0) {
        Add-ValidationError "No test classes found"
    }
    
    if ($TestMacroCount -eq 0) {
        Add-ValidationError "No test implementations found"
    }
    
    if ($TestClassCount -ne $TestMacroCount) {
        Add-ValidationWarning "Test class count ($TestClassCount) doesn't match implementation count ($TestMacroCount)"
    }
}

# Main validation execution
Write-Host "Starting validation..." -ForegroundColor White
Write-Host ""

$DirectoryValid = Test-DirectoryStructure
Write-Host ""

$CoreFilesValid = Test-CoreFiles
Write-Host ""

$ExecutionScriptsValid = Test-ExecutionScripts
Write-Host ""

$MockObjectsValid = Test-MockObjects
Write-Host ""

$UnitTestsValid = Test-UnitTests
Write-Host ""

$IntegrationTestsValid = Test-IntegrationTests
Write-Host ""

Test-TestContent
Write-Host ""

Test-TestDiscovery
Write-Host ""

# Summary
Write-Host "Validation Summary" -ForegroundColor White
Write-Host "=================" -ForegroundColor White

if ($ValidationErrors.Count -eq 0) {
    Write-Host "✓ All critical validations PASSED!" -ForegroundColor Green
    $ExitCode = 0
} else {
    Write-Host "✗ Validation FAILED with $($ValidationErrors.Count) errors" -ForegroundColor Red
    $ExitCode = 1
}

if ($ValidationWarnings.Count -gt 0) {
    Write-Host "⚠ $($ValidationWarnings.Count) warnings found" -ForegroundColor Yellow
}

Write-Host ""

if ($ValidationErrors.Count -gt 0) {
    Write-Host "Errors:" -ForegroundColor Red
    foreach ($Error in $ValidationErrors) {
        Write-Host "  - $Error" -ForegroundColor Red
    }
    Write-Host ""
}

if ($ValidationWarnings.Count -gt 0) {
    Write-Host "Warnings:" -ForegroundColor Yellow
    foreach ($Warning in $ValidationWarnings) {
        Write-Host "  - $Warning" -ForegroundColor Yellow
    }
    Write-Host ""
}

# Recommendations
Write-Host "Infrastructure Status:" -ForegroundColor White
Write-Host "  Directory Structure: $(if ($DirectoryValid) { 'VALID' } else { 'INVALID' })"
Write-Host "  Core Files: $(if ($CoreFilesValid) { 'VALID' } else { 'INVALID' })"
Write-Host "  Execution Scripts: $(if ($ExecutionScriptsValid) { 'VALID' } else { 'INVALID' })"
Write-Host "  Mock Objects: $(if ($MockObjectsValid) { 'VALID' } else { 'INVALID' })"
Write-Host "  Unit Tests: $(if ($UnitTestsValid) { 'VALID' } else { 'INVALID' })"
Write-Host "  Integration Tests: $(if ($IntegrationTestsValid) { 'VALID' } else { 'INVALID' })"

Write-Host ""
Write-Host "Next Steps:" -ForegroundColor Cyan
Write-Host "  1. Run .\AutomateTests.ps1 to execute all tests"
Write-Host "  2. Check .\RunTests.ps1 for basic test execution"
Write-Host "  3. Use .\ValidateIntegrationTests.ps1 for integration test validation"

exit $ExitCode