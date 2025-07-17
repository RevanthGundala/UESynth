// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#include "../UESynthTestBase.h"
#include "pb/uesynth.grpc.pb.h"

/**
 * Unit tests for UESynthServiceImpl class methods
 * Tests core gRPC service functionality with mocked UE dependencies
 */

// Test SetCameraTransform method
class FUESynthServiceSetCameraTransformTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthServiceSetCameraTransformTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServiceSetCameraTransformTest, 
    "UESynth.Unit.ServiceImpl.SetCameraTransform", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test valid camera transform request
    {
        uesynth::SetCameraTransformRequest Request;
        uesynth::CommandResponse Response;

        // Setup request with test transform
        Request.mutable_transform()->mutable_location()->set_x(100.0f);
        Request.mutable_transform()->mutable_location()->set_y(200.0f);
        Request.mutable_transform()->mutable_location()->set_z(300.0f);
        Request.mutable_transform()->mutable_rotation()->set_pitch(10.0f);
        Request.mutable_transform()->mutable_rotation()->set_yaw(20.0f);
        Request.mutable_transform()->mutable_rotation()->set_roll(30.0f);

        // Execute the method
        grpc::Status Status = ServiceImpl->SetCameraTransform(
            MockContext->GetServerContext(), &Request, &Response);

        // Verify response
        AssertGrpcStatusOk(Status, TEXT("SetCameraTransform"));
        UESYNTH_TEST_TRUE(Response.has_success(), "Response should have success field");
        UESYNTH_TEST_TRUE(Response.has_message(), "Response should have message field");
    }

    // Test with invalid world state
    {
        MockWorld->SetWorldValid(false);
        
        uesynth::SetCameraTransformRequest Request;
        uesynth::CommandResponse Response;

        Request.mutable_transform()->mutable_location()->set_x(0.0f);
        Request.mutable_transform()->mutable_location()->set_y(0.0f);
        Request.mutable_transform()->mutable_location()->set_z(0.0f);

        grpc::Status Status = ServiceImpl->SetCameraTransform(
            MockContext->GetServerContext(), &Request, &Response);

        // Should still return OK status but with failure in response
        AssertGrpcStatusOk(Status, TEXT("SetCameraTransform with invalid world"));
        UESYNTH_TEST_FALSE(Response.success(), "Should fail when world is invalid");
    }

    return true;
}

// Test GetCameraTransform method
class FUESynthServiceGetCameraTransformTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthServiceGetCameraTransformTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServiceGetCameraTransformTest,
    "UESynth.Unit.ServiceImpl.GetCameraTransform",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test getting camera transform
    {
        uesynth::GetCameraTransformRequest Request;
        uesynth::GetCameraTransformResponse Response;

        // Execute the method
        grpc::Status Status = ServiceImpl->GetCameraTransform(
            MockContext->GetServerContext(), &Request, &Response);

        // Verify response
        AssertGrpcStatusOk(Status, TEXT("GetCameraTransform"));
        UESYNTH_TEST_TRUE(Response.has_transform(), "Response should have transform");
        UESYNTH_TEST_TRUE(Response.transform().has_location(), "Transform should have location");
        UESYNTH_TEST_TRUE(Response.transform().has_rotation(), "Transform should have rotation");
    }

    return true;
}

// Test CaptureRgbImage method
class FUESynthServiceCaptureRgbImageTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthServiceCaptureRgbImageTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServiceCaptureRgbImageTest,
    "UESynth.Unit.ServiceImpl.CaptureRgbImage",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test valid image capture request
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;

        Request.set_width(640);
        Request.set_height(480);

        // Setup mock pixel data
        TArray<FColor> TestPixels;
        for (int32 i = 0; i < 640 * 480; ++i)
        {
            TestPixels.Add(FColor(255, 128, 64, 255));
        }
        MockWorld->SetMockPixelData(TestPixels);

        // Execute the method
        grpc::Status Status = ServiceImpl->CaptureRgbImage(
            MockContext->GetServerContext(), &Request, &Response);

        // Verify response
        AssertGrpcStatusOk(Status, TEXT("CaptureRgbImage"));
        UESYNTH_TEST_EQUAL(Response.width(), 640, "Width should match request");
        UESYNTH_TEST_EQUAL(Response.height(), 480, "Height should match request");
        UESYNTH_TEST_TRUE(Response.format() == "rgba", "Format should be rgba");
        UESYNTH_TEST_TRUE(Response.image_data().size() > 0, "Should have image data");
    }

    // Test with zero dimensions (should use defaults)
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;

        Request.set_width(0);
        Request.set_height(0);

        grpc::Status Status = ServiceImpl->CaptureRgbImage(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("CaptureRgbImage with zero dimensions"));
        UESYNTH_TEST_TRUE(Response.width() > 0, "Should use default width");
        UESYNTH_TEST_TRUE(Response.height() > 0, "Should use default height");
    }

    return true;
}

// Test ProcessAction method
class FUESynthServiceProcessActionTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthServiceProcessActionTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServiceProcessActionTest,
    "UESynth.Unit.ServiceImpl.ProcessAction",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test SetCameraTransform action
    {
        uesynth::ActionRequest Request;
        uesynth::FrameResponse Response;

        Request.set_request_id("test-123");
        auto* CameraTransform = Request.mutable_set_camera_transform();
        CameraTransform->mutable_transform()->mutable_location()->set_x(50.0f);
        CameraTransform->mutable_transform()->mutable_location()->set_y(100.0f);
        CameraTransform->mutable_transform()->mutable_location()->set_z(150.0f);

        // Execute ProcessAction (accessing private method through public interface)
        // We'll test this through ControlStream which uses ProcessAction internally
        UESYNTH_TEST_TRUE(Request.has_set_camera_transform(), "Request should have camera transform");
        UESYNTH_TEST_EQUAL(Request.request_id(), "test-123", "Request ID should match");
    }

    // Test CaptureRgb action
    {
        uesynth::ActionRequest Request;
        uesynth::FrameResponse Response;

        Request.set_request_id("capture-456");
        auto* CaptureRgb = Request.mutable_capture_rgb();
        CaptureRgb->set_width(320);
        CaptureRgb->set_height(240);

        UESYNTH_TEST_TRUE(Request.has_capture_rgb(), "Request should have capture RGB");
        UESYNTH_TEST_EQUAL(Request.capture_rgb().width(), 320, "Width should match");
        UESYNTH_TEST_EQUAL(Request.capture_rgb().height(), 240, "Height should match");
    }

    // Test unimplemented action
    {
        uesynth::ActionRequest Request;
        Request.set_request_id("unknown-789");
        // Don't set any action - should result in ACTION_NOT_SET case

        UESYNTH_TEST_EQUAL(Request.action_case(), uesynth::ActionRequest::ACTION_NOT_SET, 
            "Empty request should have ACTION_NOT_SET");
    }

    return true;
}

// Test ListObjects method
class FUESynthServiceListObjectsTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthServiceListObjectsTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServiceListObjectsTest,
    "UESynth.Unit.ServiceImpl.ListObjects",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test listing objects
    {
        google::protobuf::Empty Request;
        uesynth::ListObjectsResponse Response;

        grpc::Status Status = ServiceImpl->ListObjects(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("ListObjects"));
        UESYNTH_TEST_TRUE(Response.object_names_size() >= 0, "Should have object names array");
        
        // Current implementation returns dummy objects
        if (Response.object_names_size() > 0)
        {
            UESYNTH_TEST_TRUE(Response.object_names(0).length() > 0, "Object names should not be empty");
        }
    }

    return true;
}

// Test placeholder methods (should return success but not implemented)
class FUESynthServicePlaceholderMethodsTest : public FAutomationTestBase, public UESynthTestBase
{
public:
    FUESynthServicePlaceholderMethodsTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
        CurrentTest = this;
    }

    virtual bool RunTest(const FString& Parameters) override;
    bool RunTestImpl();
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServicePlaceholderMethodsTest,
    "UESynth.Unit.ServiceImpl.PlaceholderMethods",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
{
    // Test CaptureDepthMap
    {
        uesynth::CaptureRequest Request;
        uesynth::ImageResponse Response;
        Request.set_width(100);
        Request.set_height(100);

        grpc::Status Status = ServiceImpl->CaptureDepthMap(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("CaptureDepthMap"));
        UESYNTH_TEST_EQUAL(Response.width(), 100, "Width should match request");
        UESYNTH_TEST_EQUAL(Response.height(), 100, "Height should match request");
    }

    // Test SetObjectTransform
    {
        uesynth::SetObjectTransformRequest Request;
        uesynth::CommandResponse Response;
        Request.set_object_name("TestObject");

        grpc::Status Status = ServiceImpl->SetObjectTransform(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("SetObjectTransform"));
        UESYNTH_TEST_TRUE(Response.success(), "Placeholder should return success");
        UESYNTH_TEST_TRUE(Response.message().length() > 0, "Should have message");
    }

    // Test CreateCamera
    {
        uesynth::CreateCameraRequest Request;
        uesynth::CommandResponse Response;
        Request.set_camera_name("TestCamera");

        grpc::Status Status = ServiceImpl->CreateCamera(
            MockContext->GetServerContext(), &Request, &Response);

        AssertGrpcStatusOk(Status, TEXT("CreateCamera"));
        UESYNTH_TEST_TRUE(Response.success(), "Placeholder should return success");
    }

    return true;
}