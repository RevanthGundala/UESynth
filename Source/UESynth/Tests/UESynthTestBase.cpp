// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#include "UESynthTestBase.h"
#include "Mocks/MockUEWorld.h"
#include "Mocks/MockGrpcContext.h"
#include "UESynthServiceImpl.h"

UESynthTestBase::UESynthTestBase()
    : CurrentTest(nullptr)
{
}

UESynthTestBase::~UESynthTestBase()
{
    TearDown();
}

void UESynthTestBase::SetUp()
{
    // Create service implementation
    ServiceImpl = MakeUnique<UESynthServiceImpl>();
    
    // Setup mock environment
    SetupMockEnvironment();
}

void UESynthTestBase::TearDown()
{
    TeardownMockEnvironment();
    
    ServiceImpl.Reset();
    MockWorld.Reset();
    MockContext.Reset();
}

void UESynthTestBase::SetupMockEnvironment()
{
    // Create mock world
    MockWorld = MakeUnique<MockUEWorld>();
    MockWorld->SetupMockWorld();
    
    // Create mock gRPC context
    MockContext = MakeUnique<MockGrpcContext>();
}

void UESynthTestBase::TeardownMockEnvironment()
{
    if (MockWorld.IsValid())
    {
        MockWorld->TeardownMockWorld();
    }
}

void UESynthTestBase::AssertGrpcStatusOk(const grpc::Status& Status, const FString& Context)
{
    if (!Status.ok())
    {
        if (CurrentTest)
        {
            CurrentTest->AddError(FString::Printf(
                TEXT("gRPC Status not OK in %s: Code=%d, Message=%s"), 
                *Context, 
                static_cast<int32>(Status.error_code()),
                *FString(Status.error_message().c_str())
            ));
        }
    }
}

void UESynthTestBase::AssertGrpcStatusError(const grpc::Status& Status, grpc::StatusCode ExpectedCode, const FString& Context)
{
    if (Status.ok())
    {
        if (CurrentTest)
        {
            CurrentTest->AddError(FString::Printf(
                TEXT("Expected gRPC error in %s but got OK status"), 
                *Context
            ));
        }
    }
    else if (Status.error_code() != ExpectedCode)
    {
        if (CurrentTest)
        {
            CurrentTest->AddError(FString::Printf(
                TEXT("Wrong gRPC error code in %s: Expected=%d, Actual=%d"), 
                *Context,
                static_cast<int32>(ExpectedCode),
                static_cast<int32>(Status.error_code())
            ));
        }
    }
}

void UESynthTestBase::AssertTransformEquals(const FTransform& Expected, const FTransform& Actual, float Tolerance)
{
    AssertVectorEquals(Expected.GetLocation(), Actual.GetLocation(), Tolerance);
    AssertRotatorEquals(Expected.GetRotation().Rotator(), Actual.GetRotation().Rotator(), Tolerance);
    AssertVectorEquals(Expected.GetScale3D(), Actual.GetScale3D(), Tolerance);
}

void UESynthTestBase::AssertVectorEquals(const FVector& Expected, const FVector& Actual, float Tolerance)
{
    if (!Expected.Equals(Actual, Tolerance))
    {
        if (CurrentTest)
        {
            CurrentTest->AddError(FString::Printf(
                TEXT("Vector mismatch: Expected=(%f,%f,%f), Actual=(%f,%f,%f)"),
                Expected.X, Expected.Y, Expected.Z,
                Actual.X, Actual.Y, Actual.Z
            ));
        }
    }
}

void UESynthTestBase::AssertRotatorEquals(const FRotator& Expected, const FRotator& Actual, float Tolerance)
{
    if (!Expected.Equals(Actual, Tolerance))
    {
        if (CurrentTest)
        {
            CurrentTest->AddError(FString::Printf(
                TEXT("Rotator mismatch: Expected=(%f,%f,%f), Actual=(%f,%f,%f)"),
                Expected.Pitch, Expected.Yaw, Expected.Roll,
                Actual.Pitch, Actual.Yaw, Actual.Roll
            ));
        }
    }
}