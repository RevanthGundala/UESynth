// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>
#include <vector>

/**
 * Mock gRPC ServerContext for testing
 * Provides controlled environment for testing gRPC service methods
 */
class MockGrpcContext
{
public:
    MockGrpcContext();
    ~MockGrpcContext();

    // Get mock server context
    grpc::ServerContext* GetServerContext() { return &MockServerContext; }

    // Mock context state management
    void SetCancelled(bool bCancelled) { bIsCancelled = bCancelled; }
    bool IsCancelled() const { return bIsCancelled; }

    // Mock deadline management
    void SetDeadlineExceeded(bool bExceeded) { bIsDeadlineExceeded = bExceeded; }
    bool IsDeadlineExceeded() const { return bIsDeadlineExceeded; }

private:
    grpc::ServerContext MockServerContext;
    bool bIsCancelled;
    bool bIsDeadlineExceeded;
};

/**
 * Mock gRPC Stream for testing streaming methods
 */
template<typename ResponseType, typename RequestType>
class MockGrpcStream
{
public:
    MockGrpcStream() : bWriteSuccess(true), bReadSuccess(true) {}

    // Mock stream operations
    bool Write(const ResponseType& response)
    {
        if (bWriteSuccess)
        {
            WrittenResponses.push_back(response);
        }
        return bWriteSuccess;
    }

    bool Read(RequestType* request)
    {
        if (bReadSuccess && !PendingRequests.empty())
        {
            *request = PendingRequests.front();
            PendingRequests.erase(PendingRequests.begin());
            return true;
        }
        return false;
    }

    // Test helpers
    void SetWriteSuccess(bool bSuccess) { bWriteSuccess = bSuccess; }
    void SetReadSuccess(bool bSuccess) { bReadSuccess = bSuccess; }
    void AddPendingRequest(const RequestType& request) { PendingRequests.push_back(request); }
    const std::vector<ResponseType>& GetWrittenResponses() const { return WrittenResponses; }
    size_t GetWrittenResponseCount() const { return WrittenResponses.size(); }

private:
    std::vector<ResponseType> WrittenResponses;
    std::vector<RequestType> PendingRequests;
    bool bWriteSuccess;
    bool bReadSuccess;
};