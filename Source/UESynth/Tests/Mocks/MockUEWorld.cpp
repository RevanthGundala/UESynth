// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#include "MockUEWorld.h"

MockUEWorld::MockUEWorld()
    : MockCamera(nullptr)
    , bIsWorldValid(true)
    , ViewportWidth(1920)
    , ViewportHeight(1080)
{
}

MockUEWorld::~MockUEWorld()
{
    TeardownMockWorld();
}

void MockUEWorld::SetupMockWorld()
{
    // Initialize mock pixel data with test pattern
    MockPixelData.Empty();
    const int32 TotalPixels = ViewportWidth * ViewportHeight;
    MockPixelData.Reserve(TotalPixels);
    
    for (int32 i = 0; i < TotalPixels; ++i)
    {
        // Create a simple test pattern
        uint8 R = (i % 256);
        uint8 G = ((i / 256) % 256);
        uint8 B = ((i / (256 * 256)) % 256);
        uint8 A = 255;
        
        MockPixelData.Add(FColor(R, G, B, A));
    }
}

void MockUEWorld::TeardownMockWorld()
{
    MockCamera = nullptr;
    MockPixelData.Empty();
}

MockCameraActor::MockCameraActor()
    : ActorTransform(FTransform::Identity)
{
}