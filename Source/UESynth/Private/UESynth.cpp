// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

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
