@echo off
REM Copyright (c) 2025 UESynth Project
REM SPDX-License-Identifier: MIT

REM UESynth C++ Unit Test Runner
REM Executes all UESynth unit and integration tests using UE Automation framework

echo Starting UESynth C++ Unit Tests...
echo.

REM Set test parameters
set TEST_PROJECT=UESynth
set TEST_FILTER=UESynth.Unit
set TEST_OUTPUT_DIR=TestResults
set TEST_LOG_LEVEL=Log

REM Create output directory if it doesn't exist
if not exist %TEST_OUTPUT_DIR% mkdir %TEST_OUTPUT_DIR%

REM Run unit tests
echo Running Unit Tests...
echo ==================
UnrealEditor-Cmd.exe %TEST_PROJECT% -ExecCmds="Automation RunTests %TEST_FILTER%; quit" -TestExit="Automation Test Queue Empty" -ReportOutputPath="%TEST_OUTPUT_DIR%" -log -LogLevel=%TEST_LOG_LEVEL%

if %ERRORLEVEL% NEQ 0 (
    echo Unit tests failed with error code %ERRORLEVEL%
    goto :error
)

echo.
echo Running Integration Tests...
echo ===========================
set TEST_FILTER=UESynth.Integration
UnrealEditor-Cmd.exe %TEST_PROJECT% -ExecCmds="Automation RunTests %TEST_FILTER%; quit" -TestExit="Automation Test Queue Empty" -ReportOutputPath="%TEST_OUTPUT_DIR%" -log -LogLevel=%TEST_LOG_LEVEL%

if %ERRORLEVEL% NEQ 0 (
    echo Integration tests failed with error code %ERRORLEVEL%
    goto :error
)

echo.
echo All tests completed successfully!
echo Test results saved to: %TEST_OUTPUT_DIR%
goto :end

:error
echo.
echo Tests failed! Check the logs for details.
echo Test results saved to: %TEST_OUTPUT_DIR%
exit /b 1

:end
echo.
pause