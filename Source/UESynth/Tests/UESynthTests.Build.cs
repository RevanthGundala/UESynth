// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

using UnrealBuildTool;
using System.IO;

public class UESynthTests : ModuleRules
{
    public UESynthTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        bEnableExceptions = true;
        bUseRTTI = true;
        
        // Add preprocessor definitions for OpenSSL and gRPC compatibility
        PublicDefinitions.AddRange(new string[]
        {
            "OPENSSL_NO_ASM=1",
            "GRPC_ALLOW_EXCEPTIONS=1"
        });

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UESynth",
            "AutomationController",
            "UnrealEd",
            "TurboLinkGrpc"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore",
            "ToolMenus",
            "EditorStyle",
            "EditorWidgets",
            "GameplayTags",
            "Camera",
            "RenderCore",
            "RHI",
            "TurboLinkGrpc"
        });

        // Add the same include paths as the main UESynth module
        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "../../../GeneratedCode/Private")
            }
        );

        // Add TurboLinkGrpc protobuf include paths
        string TurboLinkThirdPartyPath = Path.Combine(ModuleDirectory, "../../../../TurboLink/Source/ThirdParty");
        
        if (Directory.Exists(TurboLinkThirdPartyPath))
        {
            PrivateIncludePaths.AddRange(new string[]
            {
                Path.Combine(TurboLinkThirdPartyPath, "protobuf/include"),
                Path.Combine(TurboLinkThirdPartyPath, "grpc/include"),
                Path.Combine(TurboLinkThirdPartyPath, "abseil/include"),
                Path.Combine(TurboLinkThirdPartyPath, "re2/include")
            });
        }

        // Add OpenSSL and zlib dependencies to support gRPC SSL functionality
        AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenSSL");
        AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");

        // Add Windows-specific libraries for gRPC SSL support
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicSystemLibraries.AddRange(new string[]
            {
                "ws2_32.lib",
                "crypt32.lib",
                "secur32.lib"
            });
        }
    }
}