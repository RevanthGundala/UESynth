// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/AutomationTest.h"

/**
 * UESynth Tests Module
 * Registers and manages all UESynth unit and integration tests
 */
class FUESynthTestsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("UESynth Tests Module Started"));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Log, TEXT("UESynth Tests Module Shutdown"));
    }
};

IMPLEMENT_MODULE(FUESynthTestsModule, UESynthTests)

// Test discovery and registration
DEFINE_LOG_CATEGORY_STATIC(LogUESynthTests, Log, All);

/**
 * Test suite registration for automation framework
 */
class FUESynthTestSuite
{
public:
    static void RegisterTests()
    {
        UE_LOG(LogUESynthTests, Log, TEXT("Registering UESynth test suite"));
        
        // Unit tests are automatically registered via IMPLEMENT_UESYNTH_UNIT_TEST macro
        // Integration tests are automatically registered via IMPLEMENT_UESYNTH_UNIT_TEST macro
        
        // Log test categories for debugging
        UE_LOG(LogUESynthTests, Log, TEXT("Available test categories:"));
        UE_LOG(LogUESynthTests, Log, TEXT("  - UESynth.Unit.ServiceImpl.*"));
        UE_LOG(LogUESynthTests, Log, TEXT("  - UESynth.Unit.TransformUtils.*"));
        UE_LOG(LogUESynthTests, Log, TEXT("  - UESynth.Unit.ImageCapture.*"));
        UE_LOG(LogUESynthTests, Log, TEXT("  - UESynth.Integration.GrpcService.*"));
        
        UE_LOG(LogUESynthTests, Log, TEXT("UESynth test suite registration complete"));
    }
    
    static void ValidateTestEnvironment()
    {
        UE_LOG(LogUESynthTests, Log, TEXT("Validating test environment..."));
        
        // Check if automation framework is available
        if (FAutomationTestFramework::Get().IsAvailable())
        {
            UE_LOG(LogUESynthTests, Log, TEXT("Automation framework is available"));
        }
        else
        {
            UE_LOG(LogUESynthTests, Warning, TEXT("Automation framework is not available"));
        }
        
        // Log test discovery status
        TArray<FAutomationTestInfo> TestInfos;
        FAutomationTestFramework::Get().GetValidTestNames(TestInfos);
        
        int32 UESynthTestCount = 0;
        for (const FAutomationTestInfo& TestInfo : TestInfos)
        {
            if (TestInfo.GetDisplayName().Contains(TEXT("UESynth")))
            {
                UESynthTestCount++;
            }
        }
        
        UE_LOG(LogUESynthTests, Log, TEXT("Discovered %d UESynth tests"), UESynthTestCount);
    }
};

// Auto-register tests when module loads
static struct FUESynthTestAutoRegister
{
    FUESynthTestAutoRegister()
    {
        FUESynthTestSuite::RegisterTests();
        FUESynthTestSuite::ValidateTestEnvironment();
    }
} GUESynthTestAutoRegister;