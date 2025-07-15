// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include <grpcpp/grpcpp.h>
#include <thread>
#include <memory>

class FUESynthModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	std::unique_ptr<grpc::Server> GRPCServer;
	std::thread GRPCServerThread;
};
