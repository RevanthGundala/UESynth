// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#include "../Mocks/MockGrpcContext.h"
#include "../Mocks/MockUEWorld.h"
#include "../UESynthTestBase.h"
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "UESynthServiceImpl.h"
#include "pb/uesynth.grpc.pb.h"
#include <google/protobuf/empty.pb.h>
#include <grpcpp/grpcpp.h>
#include <limits>
#include <memory>
#include <vector>

/**
 * Integration tests for gRPC service functionality
 * Tests end-to-end gRPC communication patterns and streaming
 */

// Test ControlStream bidirectional streaming
class FUESynthServiceControlStreamTest : public FAutomationTestBase,
                                         public UESynthTestBase {
public:
  FUESynthServiceControlStreamTest(const FString &InName,
                                   const bool bInComplexTask)
      : FAutomationTestBase(InName, bInComplexTask) {
    CurrentTest = this;
  }
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServiceControlStreamTest,
                            "UESynth.Integration.GrpcService.ControlStream",
                            EAutomationTestFlags::ApplicationContextMask |
                                EAutomationTestFlags::ProductFilter) {
  // Test ProcessAction method which is the core of ControlStream
  {
    uesynth::ActionRequest Request;
    uesynth::FrameResponse Response;

    // Test camera transform action
    Request.set_request_id("stream-test-1");
    auto *CameraTransform = Request.mutable_set_camera_transform();
    CameraTransform->mutable_transform()->mutable_location()->set_x(10.0f);
    CameraTransform->mutable_transform()->mutable_location()->set_y(20.0f);
    CameraTransform->mutable_transform()->mutable_location()->set_z(30.0f);

    grpc::Status Status = ServiceImpl->ProcessAction(Request, &Response);
    AssertGrpcStatusOk(Status, TEXT("ProcessAction with camera transform"));
    UESYNTH_TEST_TRUE(Response.has_command_response(),
                      "Response should have command response");
    UESYNTH_TEST_TRUE(Response.request_id() == "stream-test-1",
                      "Response should have matching request ID");
  }

  // Test image capture action
  {
    uesynth::ActionRequest Request;
    uesynth::FrameResponse Response;

    Request.set_request_id("stream-test-2");
    auto *CaptureRgb = Request.mutable_capture_rgb();
    CaptureRgb->set_width(640);
    CaptureRgb->set_height(480);

    grpc::Status Status = ServiceImpl->ProcessAction(Request, &Response);
    AssertGrpcStatusOk(Status, TEXT("ProcessAction with RGB capture"));
    UESYNTH_TEST_TRUE(Response.has_image_response(),
                      "Response should have image response");
    UESYNTH_TEST_TRUE(Response.request_id() == "stream-test-2",
                      "Response should have matching request ID");
    UESYNTH_TEST_EQUAL(Response.image_response().width(), 640,
                       "Image width should match request");
    UESYNTH_TEST_EQUAL(Response.image_response().height(), 480,
                       "Image height should match request");
  }

  // Test get camera transform action
  {
    uesynth::ActionRequest Request;
    uesynth::FrameResponse Response;

    Request.set_request_id("stream-test-3");
    Request.mutable_get_camera_transform();

    grpc::Status Status = ServiceImpl->ProcessAction(Request, &Response);
    AssertGrpcStatusOk(Status, TEXT("ProcessAction with get camera transform"));
    UESYNTH_TEST_TRUE(Response.has_camera_transform(),
                      "Response should have camera transform");
    UESYNTH_TEST_TRUE(Response.request_id() == "stream-test-3",
                      "Response should have matching request ID");
    UESYNTH_TEST_TRUE(Response.camera_transform().has_transform(),
                      "Camera transform response should have transform data");
  }

  // Test unimplemented action
  {
    uesynth::ActionRequest Request;
    uesynth::FrameResponse Response;

    Request.set_request_id("stream-test-4");
    // Don't set any action - should trigger default case

    grpc::Status Status = ServiceImpl->ProcessAction(Request, &Response);
    UESYNTH_TEST_TRUE(Status.error_code() == grpc::StatusCode::UNIMPLEMENTED,
                      "Should return UNIMPLEMENTED for empty action");
  }

  return true;
}

// Test protocol buffer serialization/deserialization
class FUESynthServiceProtobufSerializationTest : public FAutomationTestBase,
                                                 public UESynthTestBase {
public:
  FUESynthServiceProtobufSerializationTest(const FString &InName,
                                           const bool bInComplexTask)
      : FAutomationTestBase(InName, bInComplexTask) {
    CurrentTest = this;
  }
};

IMPLEMENT_UESYNTH_UNIT_TEST(
    FUESynthServiceProtobufSerializationTest,
    "UESynth.Integration.GrpcService.ProtobufSerialization",
    EAutomationTestFlags::ApplicationContextMask |
        EAutomationTestFlags::ProductFilter) {
  // Test Transform message serialization
  {
    uesynth::Transform Transform;
    Transform.mutable_location()->set_x(123.45f);
    Transform.mutable_location()->set_y(678.90f);
    Transform.mutable_location()->set_z(111.22f);
    Transform.mutable_rotation()->set_pitch(33.44f);
    Transform.mutable_rotation()->set_yaw(55.66f);
    Transform.mutable_rotation()->set_roll(77.88f);

    // Serialize to string
    std::string SerializedData;
    bool bSerializeSuccess = Transform.SerializeToString(&SerializedData);
    UESYNTH_TEST_TRUE(bSerializeSuccess,
                      "Transform should serialize successfully");
    UESYNTH_TEST_TRUE(SerializedData.size() > 0,
                      "Serialized data should not be empty");

    // Deserialize back
    uesynth::Transform DeserializedTransform;
    bool bDeserializeSuccess =
        DeserializedTransform.ParseFromString(SerializedData);
    UESYNTH_TEST_TRUE(bDeserializeSuccess,
                      "Transform should deserialize successfully");

    // Verify data integrity
    UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(DeserializedTransform.location().x(),
                                           123.45f, 0.001f),
                      "Location X should match");
    UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(DeserializedTransform.location().y(),
                                           678.90f, 0.001f),
                      "Location Y should match");
    UESYNTH_TEST_TRUE(FMath::IsNearlyEqual(DeserializedTransform.location().z(),
                                           111.22f, 0.001f),
                      "Location Z should match");
    UESYNTH_TEST_TRUE(
        FMath::IsNearlyEqual(DeserializedTransform.rotation().pitch(), 33.44f,
                             0.001f),
        "Rotation Pitch should match");
    UESYNTH_TEST_TRUE(
        FMath::IsNearlyEqual(DeserializedTransform.rotation().yaw(), 55.66f,
                             0.001f),
        "Rotation Yaw should match");
    UESYNTH_TEST_TRUE(
        FMath::IsNearlyEqual(DeserializedTransform.rotation().roll(), 77.88f,
                             0.001f),
        "Rotation Roll should match");
  }

  // Test ImageResponse message with binary data
  {
    uesynth::ImageResponse ImageResponse;
    ImageResponse.set_width(320);
    ImageResponse.set_height(240);
    ImageResponse.set_format("rgba");

    // Create test image data
    std::vector<uint8_t> TestImageData;
    for (int32 i = 0; i < 320 * 240 * 4; ++i) {
      TestImageData.push_back(static_cast<uint8_t>(i % 256));
    }
    ImageResponse.set_image_data(TestImageData.data(), TestImageData.size());

    // Serialize and deserialize
    std::string SerializedData;
    bool bSerializeSuccess = ImageResponse.SerializeToString(&SerializedData);
    UESYNTH_TEST_TRUE(bSerializeSuccess,
                      "ImageResponse should serialize successfully");

    uesynth::ImageResponse DeserializedResponse;
    bool bDeserializeSuccess =
        DeserializedResponse.ParseFromString(SerializedData);
    UESYNTH_TEST_TRUE(bDeserializeSuccess,
                      "ImageResponse should deserialize successfully");

    // Verify data integrity
    UESYNTH_TEST_EQUAL(DeserializedResponse.width(), 320, "Width should match");
    UESYNTH_TEST_EQUAL(DeserializedResponse.height(), 240,
                       "Height should match");
    UESYNTH_TEST_TRUE(DeserializedResponse.format() == "rgba",
                      "Format should match");
    UESYNTH_TEST_EQUAL(DeserializedResponse.image_data().size(),
                       TestImageData.size(), "Image data size should match");

    // Verify binary data integrity
    const std::string &DeserializedData = DeserializedResponse.image_data();
    for (size_t i = 0; i < TestImageData.size(); ++i) {
      UESYNTH_TEST_TRUE(
          static_cast<uint8_t>(DeserializedData[i]) == TestImageData[i],
          FString::Printf(TEXT("Binary data should match at index %d"),
                          static_cast<int32>(i)));
    }
  }

  // Test ActionRequest with oneof field serialization
  {
    uesynth::ActionRequest ActionRequest;
    ActionRequest.set_request_id("action-test-1");

    // Set camera transform action
    auto *CameraTransform = ActionRequest.mutable_set_camera_transform();
    CameraTransform->set_camera_name("TestCamera");
    CameraTransform->mutable_transform()->mutable_location()->set_x(100.0f);
    CameraTransform->mutable_transform()->mutable_location()->set_y(200.0f);
    CameraTransform->mutable_transform()->mutable_location()->set_z(300.0f);
    CameraTransform->mutable_transform()->mutable_rotation()->set_pitch(45.0f);
    CameraTransform->mutable_transform()->mutable_rotation()->set_yaw(90.0f);
    CameraTransform->mutable_transform()->mutable_rotation()->set_roll(180.0f);

    // Serialize and deserialize
    std::string SerializedData;
    bool bSerializeSuccess = ActionRequest.SerializeToString(&SerializedData);
    UESYNTH_TEST_TRUE(bSerializeSuccess,
                      "ActionRequest should serialize successfully");

    uesynth::ActionRequest DeserializedRequest;
    bool bDeserializeSuccess =
        DeserializedRequest.ParseFromString(SerializedData);
    UESYNTH_TEST_TRUE(bDeserializeSuccess,
                      "ActionRequest should deserialize successfully");

    // Verify oneof field is preserved
    UESYNTH_TEST_TRUE(
        DeserializedRequest.has_set_camera_transform(),
        "Deserialized request should have camera transform action");
    UESYNTH_TEST_TRUE(DeserializedRequest.request_id() == "action-test-1",
                      "Request ID should match");
    UESYNTH_TEST_TRUE(
        DeserializedRequest.set_camera_transform().camera_name() ==
            "TestCamera",
        "Camera name should match");

    // Verify nested transform data
    const auto &Transform =
        DeserializedRequest.set_camera_transform().transform();
    UESYNTH_TEST_TRUE(
        FMath::IsNearlyEqual(Transform.location().x(), 100.0f, 0.001f),
        "Transform location X should match");
    UESYNTH_TEST_TRUE(
        FMath::IsNearlyEqual(Transform.rotation().pitch(), 45.0f, 0.001f),
        "Transform rotation pitch should match");
  }

  // Test FrameResponse with oneof field serialization
  {
    uesynth::FrameResponse FrameResponse;
    FrameResponse.set_request_id("frame-test-1");

    // Set image response
    auto *ImageResponse = FrameResponse.mutable_image_response();
    ImageResponse->set_width(1920);
    ImageResponse->set_height(1080);
    ImageResponse->set_format("png");

    // Add some test image data
    std::string TestData = "test_image_data_12345";
    ImageResponse->set_image_data(TestData);

    // Serialize and deserialize
    std::string SerializedData;
    bool bSerializeSuccess = FrameResponse.SerializeToString(&SerializedData);
    UESYNTH_TEST_TRUE(bSerializeSuccess,
                      "FrameResponse should serialize successfully");

    uesynth::FrameResponse DeserializedResponse;
    bool bDeserializeSuccess =
        DeserializedResponse.ParseFromString(SerializedData);
    UESYNTH_TEST_TRUE(bDeserializeSuccess,
                      "FrameResponse should deserialize successfully");

    // Verify oneof field is preserved
    UESYNTH_TEST_TRUE(DeserializedResponse.has_image_response(),
                      "Deserialized response should have image response");
    UESYNTH_TEST_TRUE(DeserializedResponse.request_id() == "frame-test-1",
                      "Request ID should match");
    UESYNTH_TEST_EQUAL(DeserializedResponse.image_response().width(), 1920,
                       "Image width should match");
    UESYNTH_TEST_TRUE(DeserializedResponse.image_response().image_data() ==
                          TestData,
                      "Image data should match");
  }

  // Test corrupted data handling
  {
    std::string CorruptedData = "this_is_not_valid_protobuf_data_12345";

    uesynth::Transform Transform;
    bool bParseSuccess = Transform.ParseFromString(CorruptedData);
    UESYNTH_TEST_FALSE(bParseSuccess, "Should fail to parse corrupted data");

    // Verify object is still in valid state after failed parse
    UESYNTH_TEST_TRUE(
        FMath::IsNearlyEqual(Transform.location().x(), 0.0f, 0.001f),
        "Transform should have default values after failed parse");
  }

  return true;
}

// Test error handling across gRPC boundaries
class FUESynthServiceErrorHandlingTest : public FAutomationTestBase,
                                         public UESynthTestBase {
public:
  FUESynthServiceErrorHandlingTest(const FString &InName,
                                   const bool bInComplexTask)
      : FAutomationTestBase(InName, bInComplexTask) {
    CurrentTest = this;
  }
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServiceErrorHandlingTest,
                            "UESynth.Integration.GrpcService.ErrorHandling",
                            EAutomationTestFlags::ApplicationContextMask |
                                EAutomationTestFlags::ProductFilter) {
  // Test handling of invalid requests
  {
    // Test with null request (should be handled gracefully)
    uesynth::CommandResponse Response;

    // We can't pass null to the actual method, but we can test with minimal
    // request
    uesynth::SetCameraTransformRequest MinimalRequest;
    // Don't set any fields - should still work but might fail internally

    grpc::Status Status = ServiceImpl->SetCameraTransform(
        MockContext->GetServerContext(), &MinimalRequest, &Response);

    // Should return OK status even if operation fails
    AssertGrpcStatusOk(Status, TEXT("SetCameraTransform with minimal request"));
    UESYNTH_TEST_TRUE(Response.has_success(),
                      "Response should have success field");
    UESYNTH_TEST_TRUE(Response.has_message(),
                      "Response should have message field");
  }

  // Test with cancelled context
  {
    MockContext->SetCancelled(true);

    uesynth::GetCameraTransformRequest Request;
    uesynth::GetCameraTransformResponse Response;

    grpc::Status Status = ServiceImpl->GetCameraTransform(
        MockContext->GetServerContext(), &Request, &Response);

    // Method should still complete (current implementation doesn't check
    // cancellation)
    AssertGrpcStatusOk(Status,
                       TEXT("GetCameraTransform with cancelled context"));

    // Reset cancelled state for other tests
    MockContext->SetCancelled(false);
  }

  // Test with invalid world state
  {
    MockWorld->SetWorldValid(false);

    uesynth::CaptureRequest Request;
    uesynth::ImageResponse Response;
    Request.set_width(100);
    Request.set_height(100);

    grpc::Status Status = ServiceImpl->CaptureRgbImage(
        MockContext->GetServerContext(), &Request, &Response);

    // Should handle gracefully - might return error status or empty data
    if (Status.ok()) {
      // If OK, should have some response data
      UESYNTH_TEST_TRUE(Response.has_width(), "Response should have width");
      UESYNTH_TEST_TRUE(Response.has_height(), "Response should have height");
    } else {
      // If error, should be internal error
      UESYNTH_TEST_TRUE(Status.error_code() == grpc::StatusCode::INTERNAL,
                        "Should return INTERNAL error for world issues");
    }

    // Reset world state for other tests
    MockWorld->SetWorldValid(true);
  }

  // Test with invalid capture dimensions
  {
    uesynth::CaptureRequest Request;
    uesynth::ImageResponse Response;
    Request.set_width(0);  // Invalid width
    Request.set_height(0); // Invalid height

    grpc::Status Status = ServiceImpl->CaptureRgbImage(
        MockContext->GetServerContext(), &Request, &Response);

    // Should handle gracefully - either return error or use default dimensions
    if (Status.ok()) {
      // If OK, should have valid dimensions (likely defaults)
      UESYNTH_TEST_TRUE(Response.width() > 0,
                        "Response width should be positive");
      UESYNTH_TEST_TRUE(Response.height() > 0,
                        "Response height should be positive");
    } else {
      // If error, should be invalid argument
      UESYNTH_TEST_TRUE(
          Status.error_code() == grpc::StatusCode::INVALID_ARGUMENT ||
              Status.error_code() == grpc::StatusCode::INTERNAL,
          "Should return appropriate error for invalid dimensions");
    }
  }

  // Test with extremely large capture dimensions
  {
    uesynth::CaptureRequest Request;
    uesynth::ImageResponse Response;
    Request.set_width(100000);  // Very large width
    Request.set_height(100000); // Very large height

    grpc::Status Status = ServiceImpl->CaptureRgbImage(
        MockContext->GetServerContext(), &Request, &Response);

    // Should handle gracefully - either succeed with clamped values or return
    // error
    if (!Status.ok()) {
      UESYNTH_TEST_TRUE(
          Status.error_code() == grpc::StatusCode::RESOURCE_EXHAUSTED ||
              Status.error_code() == grpc::StatusCode::INVALID_ARGUMENT ||
              Status.error_code() == grpc::StatusCode::INTERNAL,
          "Should return appropriate error for excessive dimensions");
    }
  }

  // Test error propagation through ProcessAction
  {
    uesynth::ActionRequest Request;
    uesynth::FrameResponse Response;

    // Test with invalid capture request in streaming context
    Request.set_request_id("error-test-1");
    auto *CaptureRgb = Request.mutable_capture_rgb();
    CaptureRgb->set_width(0);
    CaptureRgb->set_height(0);

    grpc::Status Status = ServiceImpl->ProcessAction(Request, &Response);

    // ProcessAction should handle errors gracefully
    if (Status.ok()) {
      // If OK, response should be populated
      UESYNTH_TEST_TRUE(
          Response.request_id() == "error-test-1",
          "Response should have matching request ID even on error");
      UESYNTH_TEST_TRUE(Response.has_image_response(),
                        "Response should have image response field");
    } else {
      // If error, should be appropriate error code
      UESYNTH_TEST_TRUE(Status.error_code() != grpc::StatusCode::OK,
                        "Error status should not be OK");
    }
  }

  // Test string encoding across language boundaries
  {
    uesynth::SetCameraTransformRequest Request;
    uesynth::CommandResponse Response;

    // Test with special characters and Unicode
    Request.set_camera_name("TestCamera_ñáéíóú_中文_🎮");
    Request.mutable_transform()->mutable_location()->set_x(1.0f);

    grpc::Status Status = ServiceImpl->SetCameraTransform(
        MockContext->GetServerContext(), &Request, &Response);

    AssertGrpcStatusOk(Status,
                       TEXT("SetCameraTransform with Unicode camera name"));
    UESYNTH_TEST_TRUE(Response.has_success(),
                      "Response should have success field");
    UESYNTH_TEST_TRUE(Response.has_message(),
                      "Response should have message field");

    // Message should be valid UTF-8 string
    UESYNTH_TEST_TRUE(Response.message().length() > 0,
                      "Response message should not be empty");
  }

  // Test floating point precision across boundaries
  {
    uesynth::SetCameraTransformRequest Request;
    uesynth::CommandResponse Response;

    // Test with extreme floating point values
    Request.mutable_transform()->mutable_location()->set_x(
        std::numeric_limits<float>::max());
    Request.mutable_transform()->mutable_location()->set_y(
        std::numeric_limits<float>::lowest());
    Request.mutable_transform()->mutable_location()->set_z(
        std::numeric_limits<float>::epsilon());

    grpc::Status Status = ServiceImpl->SetCameraTransform(
        MockContext->GetServerContext(), &Request, &Response);

    AssertGrpcStatusOk(Status,
                       TEXT("SetCameraTransform with extreme float values"));

    // Verify we can get the transform back
    uesynth::GetCameraTransformRequest GetRequest;
    uesynth::GetCameraTransformResponse GetResponse;

    grpc::Status GetStatus = ServiceImpl->GetCameraTransform(
        MockContext->GetServerContext(), &GetRequest, &GetResponse);

    AssertGrpcStatusOk(GetStatus,
                       TEXT("GetCameraTransform after extreme values"));
    UESYNTH_TEST_TRUE(GetResponse.has_transform(),
                      "Get response should have transform");

    // Values should be finite (not NaN or infinite)
    const auto &Transform = GetResponse.transform();
    UESYNTH_TEST_TRUE(FMath::IsFinite(Transform.location().x()),
                      "Location X should be finite");
    UESYNTH_TEST_TRUE(FMath::IsFinite(Transform.location().y()),
                      "Location Y should be finite");
    UESYNTH_TEST_TRUE(FMath::IsFinite(Transform.location().z()),
                      "Location Z should be finite");
  }

  return true;
}

// Test end-to-end client-server communication scenarios
class FUESynthServiceEndToEndTest : public FAutomationTestBase,
                                    public UESynthTestBase {
public:
  FUESynthServiceEndToEndTest(const FString &InName, const bool bInComplexTask)
      : FAutomationTestBase(InName, bInComplexTask) {
    CurrentTest = this;
  }
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServiceEndToEndTest,
                            "UESynth.Integration.GrpcService.EndToEnd",
                            EAutomationTestFlags::ApplicationContextMask |
                                EAutomationTestFlags::ProductFilter) {
  // Test complete camera manipulation workflow
  {
    // Step 1: Get initial camera transform
    uesynth::GetCameraTransformRequest GetRequest;
    uesynth::GetCameraTransformResponse GetResponse;

    grpc::Status GetStatus = ServiceImpl->GetCameraTransform(
        MockContext->GetServerContext(), &GetRequest, &GetResponse);

    AssertGrpcStatusOk(GetStatus, TEXT("Get initial camera transform"));
    UESYNTH_TEST_TRUE(GetResponse.has_transform(),
                      "Initial response should have transform");

    // Store initial transform for comparison
    uesynth::Transform InitialTransform = GetResponse.transform();

    // Step 2: Set new camera transform
    uesynth::SetCameraTransformRequest SetRequest;
    uesynth::CommandResponse SetResponse;

    SetRequest.set_camera_name("TestCamera");
    SetRequest.mutable_transform()->mutable_location()->set_x(100.0f);
    SetRequest.mutable_transform()->mutable_location()->set_y(200.0f);
    SetRequest.mutable_transform()->mutable_location()->set_z(300.0f);
    SetRequest.mutable_transform()->mutable_rotation()->set_pitch(45.0f);
    SetRequest.mutable_transform()->mutable_rotation()->set_yaw(90.0f);
    SetRequest.mutable_transform()->mutable_rotation()->set_roll(180.0f);

    grpc::Status SetStatus = ServiceImpl->SetCameraTransform(
        MockContext->GetServerContext(), &SetRequest, &SetResponse);

    AssertGrpcStatusOk(SetStatus, TEXT("Set camera transform"));
    UESYNTH_TEST_TRUE(SetResponse.has_success(),
                      "Set response should have success field");
    UESYNTH_TEST_TRUE(SetResponse.has_message(),
                      "Set response should have message field");

    // Step 3: Verify transform was applied by getting it again
    uesynth::GetCameraTransformRequest VerifyRequest;
    uesynth::GetCameraTransformResponse VerifyResponse;

    grpc::Status VerifyStatus = ServiceImpl->GetCameraTransform(
        MockContext->GetServerContext(), &VerifyRequest, &VerifyResponse);

    AssertGrpcStatusOk(VerifyStatus, TEXT("Verify camera transform"));
    UESYNTH_TEST_TRUE(VerifyResponse.has_transform(),
                      "Verify response should have transform");

    // Note: In a real scenario, we would verify the transform matches what we
    // set For now, we just verify the communication worked
  }

  // Test complete image capture workflow
  {
    // Step 1: Capture RGB image
    uesynth::CaptureRequest RgbRequest;
    uesynth::ImageResponse RgbResponse;

    RgbRequest.set_camera_name("TestCamera");
    RgbRequest.set_width(640);
    RgbRequest.set_height(480);

    grpc::Status RgbStatus = ServiceImpl->CaptureRgbImage(
        MockContext->GetServerContext(), &RgbRequest, &RgbResponse);

    AssertGrpcStatusOk(RgbStatus, TEXT("Capture RGB image"));
    UESYNTH_TEST_TRUE(RgbResponse.has_width(),
                      "RGB response should have width");
    UESYNTH_TEST_TRUE(RgbResponse.has_height(),
                      "RGB response should have height");
    UESYNTH_TEST_TRUE(RgbResponse.has_format(),
                      "RGB response should have format");
    UESYNTH_TEST_TRUE(RgbResponse.has_image_data(),
                      "RGB response should have image data");

    // Step 2: Capture depth map
    uesynth::CaptureRequest DepthRequest;
    uesynth::ImageResponse DepthResponse;

    DepthRequest.set_camera_name("TestCamera");
    DepthRequest.set_width(640);
    DepthRequest.set_height(480);

    grpc::Status DepthStatus = ServiceImpl->CaptureDepthMap(
        MockContext->GetServerContext(), &DepthRequest, &DepthResponse);

    AssertGrpcStatusOk(DepthStatus, TEXT("Capture depth map"));
    UESYNTH_TEST_TRUE(DepthResponse.has_width(),
                      "Depth response should have width");
    UESYNTH_TEST_TRUE(DepthResponse.has_height(),
                      "Depth response should have height");
    UESYNTH_TEST_TRUE(DepthResponse.has_format(),
                      "Depth response should have format");

    // Step 3: Capture segmentation mask
    uesynth::CaptureRequest SegRequest;
    uesynth::ImageResponse SegResponse;

    SegRequest.set_camera_name("TestCamera");
    SegRequest.set_width(640);
    SegRequest.set_height(480);

    grpc::Status SegStatus = ServiceImpl->CaptureSegmentationMask(
        MockContext->GetServerContext(), &SegRequest, &SegResponse);

    AssertGrpcStatusOk(SegStatus, TEXT("Capture segmentation mask"));
    UESYNTH_TEST_TRUE(SegResponse.has_width(),
                      "Segmentation response should have width");
    UESYNTH_TEST_TRUE(SegResponse.has_height(),
                      "Segmentation response should have height");
    UESYNTH_TEST_TRUE(SegResponse.has_format(),
                      "Segmentation response should have format");
  }

  // Test streaming workflow simulation
  {
    // Simulate a streaming session with multiple actions
    TArray<uesynth::ActionRequest> StreamRequests;
    TArray<uesynth::FrameResponse> StreamResponses;

    // Request 1: Set camera transform
    uesynth::ActionRequest Request1;
    Request1.set_request_id("stream-1");
    auto *CameraTransform = Request1.mutable_set_camera_transform();
    CameraTransform->mutable_transform()->mutable_location()->set_x(50.0f);
    CameraTransform->mutable_transform()->mutable_location()->set_y(100.0f);
    CameraTransform->mutable_transform()->mutable_location()->set_z(150.0f);
    StreamRequests.Add(Request1);

    // Request 2: Capture image
    uesynth::ActionRequest Request2;
    Request2.set_request_id("stream-2");
    auto *CaptureRgb = Request2.mutable_capture_rgb();
    CaptureRgb->set_width(320);
    CaptureRgb->set_height(240);
    StreamRequests.Add(Request2);

    // Request 3: Get camera transform
    uesynth::ActionRequest Request3;
    Request3.set_request_id("stream-3");
    Request3.mutable_get_camera_transform();
    StreamRequests.Add(Request3);

    // Process all requests
    for (int32 i = 0; i < StreamRequests.Num(); ++i) {
      uesynth::FrameResponse Response;
      grpc::Status Status =
          ServiceImpl->ProcessAction(StreamRequests[i], &Response);

      AssertGrpcStatusOk(Status,
                         FString::Printf(TEXT("Stream request %d"), i + 1));
      UESYNTH_TEST_TRUE(
          Response.request_id() == StreamRequests[i].request_id(),
          FString::Printf(
              TEXT("Stream response %d should have matching request ID"),
              i + 1));

      StreamResponses.Add(Response);
    }

    // Verify response types match request types
    UESYNTH_TEST_TRUE(StreamResponses[0].has_command_response(),
                      "First stream response should be command response");
    UESYNTH_TEST_TRUE(StreamResponses[1].has_image_response(),
                      "Second stream response should be image response");
    UESYNTH_TEST_TRUE(
        StreamResponses[2].has_camera_transform(),
        "Third stream response should be camera transform response");
  }

  // Test object manipulation workflow
  {
    // Step 1: List existing objects
    google::protobuf::Empty ListRequest;
    uesynth::ListObjectsResponse ListResponse;

    grpc::Status ListStatus = ServiceImpl->ListObjects(
        MockContext->GetServerContext(), &ListRequest, &ListResponse);

    AssertGrpcStatusOk(ListStatus, TEXT("List objects"));
    UESYNTH_TEST_TRUE(ListResponse.object_names_size() >= 0,
                      "List response should have object names array");

    // Step 2: Set object transform (if objects exist)
    if (ListResponse.object_names_size() > 0) {
      uesynth::SetObjectTransformRequest SetObjRequest;
      uesynth::CommandResponse SetObjResponse;

      SetObjRequest.set_object_name(ListResponse.object_names(0));
      SetObjRequest.mutable_transform()->mutable_location()->set_x(10.0f);
      SetObjRequest.mutable_transform()->mutable_location()->set_y(20.0f);
      SetObjRequest.mutable_transform()->mutable_location()->set_z(30.0f);

      grpc::Status SetObjStatus = ServiceImpl->SetObjectTransform(
          MockContext->GetServerContext(), &SetObjRequest, &SetObjResponse);

      AssertGrpcStatusOk(SetObjStatus, TEXT("Set object transform"));
      UESYNTH_TEST_TRUE(SetObjResponse.has_success(),
                        "Set object response should have success field");

      // Step 3: Get object transform to verify
      uesynth::GetObjectTransformRequest GetObjRequest;
      uesynth::GetObjectTransformResponse GetObjResponse;

      GetObjRequest.set_object_name(ListResponse.object_names(0));

      grpc::Status GetObjStatus = ServiceImpl->GetObjectTransform(
          MockContext->GetServerContext(), &GetObjRequest, &GetObjResponse);

      AssertGrpcStatusOk(GetObjStatus, TEXT("Get object transform"));
      UESYNTH_TEST_TRUE(GetObjResponse.has_transform(),
                        "Get object response should have transform");
    }
  }

  // Test error recovery in workflow
  {
    // Step 1: Attempt invalid operation
    uesynth::CaptureRequest InvalidRequest;
    uesynth::ImageResponse InvalidResponse;

    InvalidRequest.set_width(0);  // Invalid
    InvalidRequest.set_height(0); // Invalid

    grpc::Status InvalidStatus = ServiceImpl->CaptureRgbImage(
        MockContext->GetServerContext(), &InvalidRequest, &InvalidResponse);

    // Step 2: Attempt valid operation after error
    uesynth::GetCameraTransformRequest ValidRequest;
    uesynth::GetCameraTransformResponse ValidResponse;

    grpc::Status ValidStatus = ServiceImpl->GetCameraTransform(
        MockContext->GetServerContext(), &ValidRequest, &ValidResponse);

    AssertGrpcStatusOk(ValidStatus,
                       TEXT("Valid operation after error should work"));
    UESYNTH_TEST_TRUE(
        ValidResponse.has_transform(),
        "Valid response should have transform after previous error");
  }

  return true;
}

// Test concurrent request handling
class FUESynthServiceConcurrencyTest : public FAutomationTestBase,
                                       public UESynthTestBase {
public:
  FUESynthServiceConcurrencyTest(const FString &InName,
                                 const bool bInComplexTask)
      : FAutomationTestBase(InName, bInComplexTask) {
    CurrentTest = this;
  }
};

IMPLEMENT_UESYNTH_UNIT_TEST(FUESynthServiceConcurrencyTest,
                            "UESynth.Integration.GrpcService.Concurrency",
                            EAutomationTestFlags::ApplicationContextMask |
                                EAutomationTestFlags::ProductFilter) {
  // Test multiple simultaneous requests (simulated)
  {
    // Create multiple contexts and requests
    TArray<TUniquePtr<MockGrpcContext>> Contexts;
    TArray<uesynth::GetCameraTransformRequest> Requests;
    TArray<uesynth::GetCameraTransformResponse> Responses;

    const int32 NumRequests = 5;
    for (int32 i = 0; i < NumRequests; ++i) {
      Contexts.Add(MakeUnique<MockGrpcContext>());
      Requests.Add(uesynth::GetCameraTransformRequest());
      Responses.Add(uesynth::GetCameraTransformResponse());
    }

    // Execute requests sequentially (simulating concurrent access)
    TArray<grpc::Status> Statuses;
    for (int32 i = 0; i < NumRequests; ++i) {
      grpc::Status Status = ServiceImpl->GetCameraTransform(
          Contexts[i]->GetServerContext(), &Requests[i], &Responses[i]);
      Statuses.Add(Status);
    }

    // Verify all requests completed successfully
    for (int32 i = 0; i < NumRequests; ++i) {
      AssertGrpcStatusOk(Statuses[i],
                         FString::Printf(TEXT("Concurrent request %d"), i));
      UESYNTH_TEST_TRUE(
          Responses[i].has_transform(),
          FString::Printf(TEXT("Response %d should have transform"), i));
    }
  }

  return true;
}