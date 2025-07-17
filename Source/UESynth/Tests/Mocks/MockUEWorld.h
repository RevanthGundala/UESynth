// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Actor.h"

/**
 * Mock UWorld for testing purposes
 * Provides controlled environment for testing UESynthServiceImpl methods
 */
class MockUEWorld
{
public:
    MockUEWorld();
    ~MockUEWorld();

    // Mock world setup
    void SetupMockWorld();
    void TeardownMockWorld();

    // Mock camera management
    void SetMockCamera(ACameraActor* Camera) { MockCamera = Camera; }
    ACameraActor* GetMockCamera() const { return MockCamera; }

    // Mock world state
    void SetWorldValid(bool bValid) { bIsWorldValid = bValid; }
    bool IsWorldValid() const { return bIsWorldValid; }

    // Mock viewport settings
    void SetViewportSize(int32 Width, int32 Height) 
    { 
        ViewportWidth = Width; 
        ViewportHeight = Height; 
    }
    FIntPoint GetViewportSize() const { return FIntPoint(ViewportWidth, ViewportHeight); }

    // Mock pixel data for capture testing
    void SetMockPixelData(const TArray<FColor>& PixelData) { MockPixelData = PixelData; }
    const TArray<FColor>& GetMockPixelData() const { return MockPixelData; }

private:
    ACameraActor* MockCamera;
    bool bIsWorldValid;
    int32 ViewportWidth;
    int32 ViewportHeight;
    TArray<FColor> MockPixelData;
};

/**
 * Mock Camera Actor for testing
 */
class MockCameraActor
{
public:
    MockCameraActor();
    
    void SetActorTransform(const FTransform& Transform) { ActorTransform = Transform; }
    FTransform GetActorTransform() const { return ActorTransform; }
    
    void SetActorLocationAndRotation(const FVector& Location, const FRotator& Rotation)
    {
        ActorTransform.SetLocation(Location);
        ActorTransform.SetRotation(Rotation.Quaternion());
    }

private:
    FTransform ActorTransform;
};