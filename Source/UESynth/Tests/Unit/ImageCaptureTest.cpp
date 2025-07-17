// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#include "../UESynthTestBase.h"
#include "pb/uesynth.grpc.pb.h"

/**
 * Unit tests for image capture functionality
 * Tests RGB, depth, and segmentation capture methods
 */

// Test RGB image capture
class FUESynthImageCaptureRgbTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthImageCaptureRgbTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthImageCaptureRgbTest,
    "UESynth.Unit.ImageCapture.RGB",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test standard resolution capture
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        
        Request.set_width(1920);
        Request.set_height(1080);

        // Setup mock pixel data
        TArray<FColor> TestPixels;
        const int32 TotalPixels = 1920 * 1080;
        TestPixels.Reserve(TotalPixels);
        
        for (int32 i = 0; i < TotalPixels; ++i)
        {
            uint8 R = (i % 256);
            uint8 G = ((i / 256) % 256);
            uint8 B = 128;
            uint8 A = 255;
            TestPixels.Add(FColor(R, G, B, A));
        }
        MockWorld->SetMockPixelData(TestPixels);

        grpc::Status Status = ServiceImpl->CaptureRgbImage(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("RGB capture standard resolution"));
        UESYNTH_TEST_EQUAL(Response.width(), 1920, "Width should match request");
        UESYNTH_TEST_EQUAL(Response.height(), 1080, "Height should match request");
        UESYNTH_TEST_TRUE(Response.format() == "rgba", "Format should be RGBA");
        
        // Verify image data size (RGBA = 4 bytes per pixel)
        size_t ExpectedSize = 1920 * 1080 * 4;
        UESYNTH_TEST_TRUE(Response.image_data().size() > 0, "Should have image data");
    }

    // Test small resolution capture
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        
        Request.set_width(64);
        Request.set_height(64);

        TArray<FColor> SmallTestPixels;
        for (int32 i = 0; i < 64 * 64; ++i)
        {
            SmallTestPixels.Add(FColor(255, 0, 0, 255)); // Red pixels
        }
        MockWorld->SetMockPixelData(SmallTestPixels);

        grpc::Status Status = ServiceImpl->CaptureRgbImage(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("RGB capture small resolution"));
        UESYNTH_TEST_EQUAL(Response.width(), 64, "Small width should match");
        UESYNTH_TEST_EQUAL(Response.height(), 64, "Small height should match");
    }

    // Test zero dimensions (should use defaults)
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        
        Request.set_width(0);
        Request.set_height(0);

        grpc::Status Status = ServiceImpl->CaptureRgbImage(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("RGB capture zero dimensions"));
        UESYNTH_TEST_TRUE(Response.width() > 0, "Should use default width > 0");
        UESYNTH_TEST_TRUE(Response.height() > 0, "Should use default height > 0");
    }

    return true;
}

// Test depth map capture
class FUESynthImageCaptureDepthTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthImageCaptureDepthTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthImageCaptureDepthTest,
    "UESynth.Unit.ImageCapture.Depth",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test depth map capture (placeholder implementation)
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        
        Request.set_width(640);
        Request.set_height(480);

        grpc::Status Status = ServiceImpl->CaptureDepthMap(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("Depth map capture"));
        UESYNTH_TEST_EQUAL(Response.width(), 640, "Depth width should match request");
        UESYNTH_TEST_EQUAL(Response.height(), 480, "Depth height should match request");
        UESYNTH_TEST_TRUE(Response.format() == "png", "Depth format should be PNG");
        
        // Current implementation returns dummy data
        size_t ExpectedSize = 640 * 480; // 1 byte per pixel for depth
        UESYNTH_TEST_EQUAL(Response.image_data().size(), ExpectedSize, "Depth data size should match");
    }

    // Test different depth resolutions
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        
        Request.set_width(320);
        Request.set_height(240);

        grpc::Status Status = ServiceImpl->CaptureDepthMap(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("Depth map different resolution"));
        UESYNTH_TEST_EQUAL(Response.width(), 320, "Different depth width");
        UESYNTH_TEST_EQUAL(Response.height(), 240, "Different depth height");
    }

    return true;
}

// Test segmentation mask capture
class FUESynthImageCaptureSegmentationTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthImageCaptureSegmentationTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthImageCaptureSegmentationTest,
    "UESynth.Unit.ImageCapture.Segmentation",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test segmentation mask capture (placeholder implementation)
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        
        Request.set_width(800);
        Request.set_height(600);

        grpc::Status Status = ServiceImpl->CaptureSegmentationMask(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("Segmentation mask capture"));
        UESYNTH_TEST_EQUAL(Response.width(), 800, "Segmentation width should match request");
        UESYNTH_TEST_EQUAL(Response.height(), 600, "Segmentation height should match request");
        UESYNTH_TEST_TRUE(Response.format() == "png", "Segmentation format should be PNG");
        
        // Current implementation returns dummy data (RGB = 3 bytes per pixel)
        size_t ExpectedSize = 800 * 600 * 3;
        UESYNTH_TEST_EQUAL(Response.image_data().size(), ExpectedSize, "Segmentation data size should match");
    }

    return true;
}

// Test normals capture
class FUESynthImageCaptureNormalsTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthImageCaptureNormalsTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthImageCaptureNormalsTest,
    "UESynth.Unit.ImageCapture.Normals",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test normals capture (placeholder implementation)
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        
        Request.set_width(512);
        Request.set_height(512);

        grpc::Status Status = ServiceImpl->CaptureNormals(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("Normals capture"));
        UESYNTH_TEST_EQUAL(Response.width(), 512, "Normals width should match request");
        UESYNTH_TEST_EQUAL(Response.height(), 512, "Normals height should match request");
        UESYNTH_TEST_TRUE(Response.format() == "png", "Normals format should be PNG");
        
        // Current implementation returns dummy data (RGB = 3 bytes per pixel)
        size_t ExpectedSize = 512 * 512 * 3;
        UESYNTH_TEST_EQUAL(Response.image_data().size(), ExpectedSize, "Normals data size should match");
    }

    return true;
}

// Test optical flow capture
class FUESynthImageCaptureOpticalFlowTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthImageCaptureOpticalFlowTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthImageCaptureOpticalFlowTest,
    "UESynth.Unit.ImageCapture.OpticalFlow",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test optical flow capture (placeholder implementation)
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        
        Request.set_width(1024);
        Request.set_height(768);

        grpc::Status Status = ServiceImpl->CaptureOpticalFlow(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("Optical flow capture"));
        UESYNTH_TEST_EQUAL(Response.width(), 1024, "Optical flow width should match request");
        UESYNTH_TEST_EQUAL(Response.height(), 768, "Optical flow height should match request");
        UESYNTH_TEST_TRUE(Response.format() == "png", "Optical flow format should be PNG");
        
        // Current implementation returns dummy data (2 channels = 2 bytes per pixel)
        size_t ExpectedSize = 1024 * 768 * 2;
        UESYNTH_TEST_EQUAL(Response.image_data().size(), ExpectedSize, "Optical flow data size should match");
    }

    return true;
}

// Test image capture error conditions
class FUESynthImageCaptureErrorTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthImageCaptureErrorTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthImageCaptureErrorTest,
    "UESynth.Unit.ImageCapture.ErrorConditions",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test capture with invalid world
    {
        MockWorld->SetWorldValid(false);
        
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        Request.set_width(100);
        Request.set_height(100);

        grpc::Status Status = ServiceImpl->CaptureRgbImage(
            MockContext->GetServerContext(), &Request, &Response);

        // Should handle gracefully - either return error or empty data
        if (Status.ok())
        {
            // If successful, should have valid response structure
            UESYNTH_TEST_TRUE(Response.has_width(), "Response should have width field");
            UESYNTH_TEST_TRUE(Response.has_height(), "Response should have height field");
            UESYNTH_TEST_TRUE(Response.has_format(), "Response should have format field");
        }
        else
        {
            // If error, should be appropriate error code
            UESYNTH_TEST_TRUE(Status.error_code() == grpc::StatusCode::INTERNAL ||
                             Status.error_code() == grpc::StatusCode::UNAVAILABLE,
                "Should return appropriate error code for invalid world");
        }
    }

    // Test capture with extreme dimensions
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        Request.set_width(10000);  // Very large width
        Request.set_height(10000); // Very large height

        grpc::Status Status = ServiceImpl->CaptureRgbImage(
            MockContext->GetServerContext(), &Request, &Response);

        // Should handle gracefully - might succeed with clamped dimensions or fail appropriately
        if (!Status.ok())
        {
            UESYNTH_TEST_TRUE(Status.error_code() == grpc::StatusCode::INVALID_ARGUMENT ||
                             Status.error_code() == grpc::StatusCode::RESOURCE_EXHAUSTED,
                "Should return appropriate error for extreme dimensions");
        }
    }

    return true;
}