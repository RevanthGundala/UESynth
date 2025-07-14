// Copyright (C) 2025 UESynth Project
// SPDX-License-Identifier: GPL-3.0-or-later

/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

using UnrealBuildTool;
using System.IO;

public class UESynth : ModuleRules
{
	public UESynth(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		bEnableExceptions = true;
		bUseRTTI = true;
		
		// Add preprocessor definitions for OpenSSL and gRPC compatibility
		PublicDefinitions.AddRange(new string[]
		{
			"OPENSSL_NO_ASM=1",
			"GRPC_ALLOW_EXCEPTIONS=1"
		});

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Projects",
				"TurboLinkGrpc"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"ToolMenus",
				"EditorStyle",
				"EditorWidgets",
				"UnrealEd",
				"LevelEditor",
				"TurboLinkGrpc"
			}
		);

		// Add OpenSSL and zlib dependencies to support gRPC SSL functionality
		AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenSSL");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");
		
		// Force static linking of OpenSSL libraries on Windows
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
			string OpenSSLPath = Path.Combine(EngineDir, "Source", "ThirdParty", "OpenSSL");
			string ZlibPath = Path.Combine(EngineDir, "Source", "ThirdParty", "zlib");
			
			// Try to find and link OpenSSL libraries directly
			string[] OpenSSLLibs = {
				"libssl.lib",
				"libcrypto.lib"
			};
			
			string[] ZlibLibs = {
				"zlibstatic.lib"
			};
			
			// Add OpenSSL libraries
			foreach (string lib in OpenSSLLibs)
			{
				string[] PossiblePaths = {
					Path.Combine(OpenSSLPath, "1.1.1t", "lib", "Win64", "VS2015", "Release", lib),
					Path.Combine(OpenSSLPath, "1.1.1", "lib", "Win64", "VS2015", "Release", lib),
					Path.Combine(OpenSSLPath, "lib", "Win64", lib)
				};
				
				foreach (string LibPath in PossiblePaths)
				{
					if (File.Exists(LibPath))
					{
						PublicAdditionalLibraries.Add(LibPath);
						break;
					}
				}
			}
			
			// Add zlib libraries
			foreach (string lib in ZlibLibs)
			{
				string[] PossiblePaths = {
					Path.Combine(ZlibPath, "v1.2.8", "lib", "Win64", "Release", lib),
					Path.Combine(ZlibPath, "lib", "Win64", lib)
				};
				
				foreach (string LibPath in PossiblePaths)
				{
					if (File.Exists(LibPath))
					{
						PublicAdditionalLibraries.Add(LibPath);
						break;
					}
				}
			}
		}

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
		
		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ModuleDirectory, "Private/Generated")
			}
		);

		// Add TurboLinkGrpc protobuf include paths since they're private in TurboLinkGrpc
		// NOTE: ModuleDirectory points at <Project>/Plugins/UESynth/Source/UESynth.
		// TurboLink lives beside UESynth inside the Plugins folder, so we need to go up
		// three levels to reach the Plugins directory and then down into TurboLink.
		string TurboLinkGrpcPath = Path.Combine(ModuleDirectory, "../../../TurboLink/Source/TurboLinkGrpc");
		string TurboLinkThirdPartyPath = Path.Combine(ModuleDirectory, "../../../TurboLink/Source/ThirdParty");
		
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
		else
		{
			// Fallback to common system paths
			if (Target.Platform == UnrealTargetPlatform.Win64)
			{
				string[] PossibleProtobufPaths = {
					"C:/vcpkg/installed/x64-windows/include",
					"C:/Program Files/protobuf/include",
					"C:/protobuf/include"
				};
				
				foreach (string ProtobufPath in PossibleProtobufPaths)
				{
					if (Directory.Exists(ProtobufPath))
					{
						PrivateIncludePaths.Add(ProtobufPath);
						break;
					}
				}
			}
		}
	}
}
