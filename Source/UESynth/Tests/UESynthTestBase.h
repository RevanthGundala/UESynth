// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "pb/uesynth.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <google/protobuf/empty.pb.h>

// Forward declarations
class UESynthServiceImpl;
class MockUEWorld;
class MockGrpcContext;

// Include the actual implementation classes
#include "UESynthServiceImpl.h"
#include "Mocks/MockUEWorld.h"
#include "Mocks/MockGrpcContext.h"

/**
 * Base class for UESynth unit tests
 * Provides common setup and utilities for testing UESynthServiceImpl
 */
class UESynthTestBase
{
public:
    UESynthTestBase();
    virtual ~UESynthTestBase();

protected:
    // Test setup and teardown
    virtual void SetUp();
    virtual void TearDown();

    // Test utilities
    void SetupMockEnvironment();
    void TeardownMockEnvironment();

    // Assertion helpers
    void AssertGrpcStatusOk(const grpc::Status& Status, const FString& Context);
    void AssertGrpcStatusError(const grpc::Status& Status, grpc::StatusCode ExpectedCode, const FString& Context);
    void AssertTransformEquals(const FTransform& Expected, const FTransform& Actual, float Tolerance = 0.01f);
    void AssertVectorEquals(const FVector& Expected, const FVector& Actual, float Tolerance = 0.01f);
    void AssertRotatorEquals(const FRotator& Expected, const FRotator& Actual, float Tolerance = 0.01f);

protected:
    // Test objects
    TUniquePtr<UESynthServiceImpl> ServiceImpl;
    TUniquePtr<MockUEWorld> MockWorld;
    TUniquePtr<MockGrpcContext> MockContext;
    
    // Test data
    FAutomationTestBase* CurrentTest;
};

// Macro for creating UESynth unit tests
#define IMPLEMENT_UESYNTH_UNIT_TEST(TestClass, TestName, TestFlags) \
    IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestClass, TestName, TestFlags) \
    bool TestClass::RunTest(const FString& Parameters)

// Macro for UESynth test assertions
#define UESYNTH_TEST_TRUE(Condition, Message) \
    if (!(Condition)) \
    { \
        AddError(FString::Printf(TEXT("Test failed: %s"), TEXT(Message))); \
        return false; \
    }

#define UESYNTH_TEST_FALSE(Condition, Message) \
    if (Condition) \
    { \
        AddError(FString::Printf(TEXT("Test failed: %s"), TEXT(Message))); \
        return false; \
    }

#define UESYNTH_TEST_EQUAL(Expected, Actual, Message) \
    if ((Expected) != (Actual)) \
    { \
        AddError(FString::Printf(TEXT("Test failed: %s - Expected: %s, Actual: %s"), \
            TEXT(Message), *FString::Printf(TEXT("%s"), Expected), *FString::Printf(TEXT("%s"), Actual))); \
        return false; \
    }