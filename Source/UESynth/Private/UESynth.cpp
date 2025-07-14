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

#include "UESynth.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"
#include "LevelEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Commands/UICommandList.h"
#include "Styling/AppStyle.h"
#include "ToolMenus.h"
#include "UESynthServiceImpl.h"
#include <grpcpp/grpcpp.h>
#include <thread>

#define LOCTEXT_NAMESPACE "FUESynthModule"


void FUESynthModule::StartupModule() {
    UE_LOG(LogTemp, Log, TEXT("Starting gRPC server..."));
    GRPCServerThread = std::thread([&]() {
        std::string server_address("0.0.0.0:50051");
        UESynthServiceImpl service;
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        GRPCServer = builder.BuildAndStart();
        
        if (GRPCServer) {
            UE_LOG(LogTemp, Log, TEXT("gRPC Server listening on %s"), *FString(server_address.c_str()));
            GRPCServer->Wait(); // Block until the server is shutdown
        } else {
            UE_LOG(LogTemp, Error, TEXT("Failed to start gRPC server"));
        }
    });
}

void FUESynthModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("Shutting down gRPC server..."));
    if (GRPCServer) {
        GRPCServer->Shutdown();
    }
    // Join the thread to ensure it's finished
    if (GRPCServerThread.joinable()) {
        GRPCServerThread.join();
    }
    UE_LOG(LogTemp, Log, TEXT("gRPC server shutdown complete"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUESynthModule, UESynth)
