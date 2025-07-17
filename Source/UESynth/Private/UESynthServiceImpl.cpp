// Copyright (c) 2025 UESynth Project
// SPDX-License-Identifier: MIT

#include "UESynthServiceImpl.h"
#include "Async/Async.h"
#include "Async/TaskGraphInterfaces.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "UESynth.h" // For module access
#include <future>
#include <grpcpp/server_builder.h>
#include <google/protobuf/empty.pb.h>

// Helper to run on game thread
#define RUN_ON_GAME_THREAD(Code)                                               \
  AsyncTask(ENamedThreads::GameThread, [=]() { Code });

// New bidirectional streaming method implementation
grpc::Status UESynthServiceImpl::ControlStream(
    grpc::ServerContext *context,
    grpc::ServerReaderWriter<uesynth::FrameResponse, uesynth::ActionRequest>
        *stream) {
  uesynth::ActionRequest request;
  uesynth::FrameResponse response;

  // Continuously read requests from the client stream
  while (stream->Read(&request)) {
    // Process the action and generate response
    grpc::Status status = ProcessAction(request, &response);

    if (!status.ok()) {
      // Log error and continue processing other requests
      UE_LOG(LogTemp, Error, TEXT("Error processing action: %s"),
             *FString(status.error_message().c_str()));
      continue;
    }

    // Send response back to client if there's data to send
    if (response.has_image_response() || response.has_command_response() ||
        response.has_camera_transform() || response.has_object_transform() ||
        response.has_objects_list()) {

      if (!stream->Write(response)) {
        // Client disconnected or write failed
        UE_LOG(LogTemp, Warning,
               TEXT("Failed to write response to client stream"));
        break;
      }
    }

    // Clear response for next iteration
    response.Clear();
  }

  return grpc::Status::OK;
}

// Helper method to process individual actions
grpc::Status
UESynthServiceImpl::ProcessAction(const uesynth::ActionRequest &request,
                                  uesynth::FrameResponse *response) {
  response->set_request_id(request.request_id());

  // Handle different action types using switch on oneof case
  switch (request.action_case()) {
  case uesynth::ActionRequest::kSetCameraTransform: {
    uesynth::CommandResponse cmd_response;
    grpc::Status status = SetCameraTransform(
        nullptr, &request.set_camera_transform(), &cmd_response);
    if (status.ok()) {
      response->mutable_command_response()->CopyFrom(cmd_response);
    }
    return status;
  }

  case uesynth::ActionRequest::kGetCameraTransform: {
    uesynth::GetCameraTransformResponse cam_response;
    grpc::Status status = GetCameraTransform(
        nullptr, &request.get_camera_transform(), &cam_response);
    if (status.ok()) {
      response->mutable_camera_transform()->CopyFrom(cam_response);
    }
    return status;
  }

  case uesynth::ActionRequest::kCaptureRgb: {
    uesynth::ImageResponse img_response;
    grpc::Status status =
        CaptureRgbImage(nullptr, &request.capture_rgb(), &img_response);
    if (status.ok()) {
      response->mutable_image_response()->CopyFrom(img_response);
    }
    return status;
  }

  case uesynth::ActionRequest::kCaptureDepth: {
    uesynth::ImageResponse img_response;
    grpc::Status status =
        CaptureDepthMap(nullptr, &request.capture_depth(), &img_response);
    if (status.ok()) {
      response->mutable_image_response()->CopyFrom(img_response);
    }
    return status;
  }

  case uesynth::ActionRequest::kCaptureSegmentation: {
    uesynth::ImageResponse img_response;
    grpc::Status status = CaptureSegmentationMask(
        nullptr, &request.capture_segmentation(), &img_response);
    if (status.ok()) {
      response->mutable_image_response()->CopyFrom(img_response);
    }
    return status;
  }

  case uesynth::ActionRequest::kSetObjectTransform: {
    uesynth::CommandResponse cmd_response;
    grpc::Status status = SetObjectTransform(
        nullptr, &request.set_object_transform(), &cmd_response);
    if (status.ok()) {
      response->mutable_command_response()->CopyFrom(cmd_response);
    }
    return status;
  }

  case uesynth::ActionRequest::kGetObjectTransform: {
    uesynth::GetObjectTransformResponse obj_response;
    grpc::Status status = GetObjectTransform(
        nullptr, &request.get_object_transform(), &obj_response);
    if (status.ok()) {
      response->mutable_object_transform()->CopyFrom(obj_response);
    }
    return status;
  }

  // Add more cases for other action types as needed
  case uesynth::ActionRequest::kListObjects: {
    uesynth::ListObjectsResponse list_response;
    grpc::Status status =
        ListObjects(nullptr, &request.list_objects(), &list_response);
    if (status.ok()) {
      response->mutable_objects_list()->CopyFrom(list_response);
    }
    return status;
  }

  default:
    return grpc::Status(grpc::StatusCode::UNIMPLEMENTED,
                        "Action not implemented");
  }
}

grpc::Status UESynthServiceImpl::SetCameraTransform(
    grpc::ServerContext *context,
    const uesynth::SetCameraTransformRequest *request,
    uesynth::CommandResponse *reply) {
  bool success = false;
  std::string message;
  auto future =
      Async(EAsyncExecution::TaskGraphMainThread,
            [request, &success, &message]() -> bool {
              // Get world from game engine - try multiple approaches
              UWorld *World = nullptr;

              // Method 1: Try to get world from world contexts
              if (GEngine && GEngine->GetWorldContexts().Num() > 0) {
                for (const FWorldContext &WorldContext :
                     GEngine->GetWorldContexts()) {
                  if (WorldContext.World() &&
                      WorldContext.WorldType == EWorldType::Game) {
                    World = WorldContext.World();
                    break;
                  }
                }
              }

              // Method 2: Fallback to PIE world if in editor
              if (!World && GEngine) {
                World = GEngine->GetCurrentPlayWorld();
              }

              // Method 3: Last resort - get any valid world
              if (!World && GEngine && GEngine->GetWorldContexts().Num() > 0) {
                World = GEngine->GetWorldContexts()[0].World();
              }

              if (World) {
                AActor *Camera = UGameplayStatics::GetActorOfClass(
                    World, ACameraActor::StaticClass()); // Placeholder
                if (Camera) {
                  // Convert protobuf types to Unreal Engine types
                  FVector Location(request->transform().location().x(),
                                   request->transform().location().y(),
                                   request->transform().location().z());
                  FRotator Rotation(request->transform().rotation().pitch(),
                                    request->transform().rotation().yaw(),
                                    request->transform().rotation().roll());

                  Camera->SetActorLocationAndRotation(Location, Rotation);
                  message = "Camera transform set successfully";
                  success = true;
                  return true;
                } else {
                  message = "No camera actor found in world";
                }
              } else {
                message = "No valid world found - make sure game is running";
              }
              success = false;
              return false;
            });
  future.Get();
  reply->set_success(success);
  reply->set_message(message);
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::CaptureRgbImage(grpc::ServerContext *context,
                                    const uesynth::CaptureRequest *request,
                                    uesynth::ImageResponse *reply) {
  std::promise<std::vector<uint8>> promise;
  auto future = promise.get_future();
  AsyncTask(ENamedThreads::GameThread, [&promise, request]() {
    // Get world from game engine - try multiple approaches
    UWorld *World = nullptr;

    // Method 1: Try to get world from world contexts
    if (GEngine && GEngine->GetWorldContexts().Num() > 0) {
      for (const FWorldContext &WorldContext : GEngine->GetWorldContexts()) {
        if (WorldContext.World() &&
            WorldContext.WorldType == EWorldType::Game) {
          World = WorldContext.World();
          break;
        }
      }
    }

    // Method 2: Fallback to PIE world if in editor
    if (!World && GEngine) {
      World = GEngine->GetCurrentPlayWorld();
    }

    // Method 3: Last resort - get any valid world
    if (!World && GEngine && GEngine->GetWorldContexts().Num() > 0) {
      World = GEngine->GetWorldContexts()[0].World();
    }

    if (!World) {
      UE_LOG(LogTemp, Error,
             TEXT("UESynth: No world found for capture - make sure game is "
                  "running"));
      std::vector<uint8> emptyData;
      promise.set_value(emptyData);
      return;
    }

    // Get the game viewport - try multiple approaches
    UGameViewportClient *ViewportClient = nullptr;
    
    // Method 1: Try to get viewport from world
    ViewportClient = World->GetGameViewport();
    
    // Method 2: Try to get viewport from engine
    if (!ViewportClient && GEngine) {
      ViewportClient = GEngine->GameViewport;
    }
    
    // Method 3: Try to get viewport from world contexts
    if (!ViewportClient && GEngine) {
      for (const FWorldContext &WorldContext : GEngine->GetWorldContexts()) {
        if (WorldContext.GameViewport) {
          ViewportClient = WorldContext.GameViewport;
          break;
        }
      }
    }
    
    if (!ViewportClient) {
      UE_LOG(LogTemp, Error, TEXT("UESynth: No viewport client found after trying multiple methods"));
      UE_LOG(LogTemp, Error, TEXT("UESynth: World type: %d, World name: %s"), 
             (int32)World->WorldType, *World->GetName());
      std::vector<uint8> emptyData;
      promise.set_value(emptyData);
      return;
    }

    // Get viewport size
    FViewport *Viewport = ViewportClient->Viewport;
    if (!Viewport) {
      UE_LOG(LogTemp, Error, TEXT("UESynth: No viewport found"));
      std::vector<uint8> emptyData;
      promise.set_value(emptyData);
      return;
    }

    // Capture the viewport
    TArray<FColor> ColorBuffer;
    FIntVector Size(
        request->width() > 0 ? request->width() : Viewport->GetSizeXY().X,
        request->height() > 0 ? request->height() : Viewport->GetSizeXY().Y, 0);

    // Read pixels from the viewport
    bool bCaptureSuccess = Viewport->ReadPixels(
        ColorBuffer, FReadSurfaceDataFlags(), FIntRect(0, 0, Size.X, Size.Y));

    if (!bCaptureSuccess || ColorBuffer.Num() == 0) {
      UE_LOG(LogTemp, Error,
             TEXT("UESynth: Failed to capture viewport pixels"));
      std::vector<uint8> emptyData;
      promise.set_value(emptyData);
      return;
    }

    // Convert FColor array to RGBA bytes
    std::vector<uint8> imageData;
    imageData.reserve(ColorBuffer.Num() * 4);

    for (const FColor &Color : ColorBuffer) {
      imageData.push_back(Color.R);
      imageData.push_back(Color.G);
      imageData.push_back(Color.B);
      imageData.push_back(Color.A);
    }

    UE_LOG(LogTemp, Log,
           TEXT("UESynth: Successfully captured %dx%d image with %d pixels"),
           Size.X, Size.Y, ColorBuffer.Num());

    promise.set_value(imageData);
  });

  auto data = future.get();
  if (data.empty()) {
    return grpc::Status(grpc::StatusCode::INTERNAL, "Failed to capture image");
  }

  reply->set_image_data(data.data(), data.size());
  reply->set_width(request->width() > 0 ? request->width()
                                        : 1920); // Default fallback
  reply->set_height(request->height() > 0 ? request->height()
                                          : 1080); // Default fallback
  reply->set_format("rgba");
  return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::GetCameraTransform(
    grpc::ServerContext *context,
    const uesynth::GetCameraTransformRequest *request,
    uesynth::GetCameraTransformResponse *reply) {
  // Get camera transform using proper world access
  auto future =
      Async(EAsyncExecution::TaskGraphMainThread, [request]() -> FTransform {
        // Get world from game engine - try multiple approaches
        UWorld *World = nullptr;

        // Method 1: Try to get world from world contexts
        if (GEngine && GEngine->GetWorldContexts().Num() > 0) {
          for (const FWorldContext &WorldContext :
               GEngine->GetWorldContexts()) {
            if (WorldContext.World() &&
                WorldContext.WorldType == EWorldType::Game) {
              World = WorldContext.World();
              break;
            }
          }
        }

        // Method 2: Fallback to PIE world if in editor
        if (!World && GEngine) {
          World = GEngine->GetCurrentPlayWorld();
        }

        // Method 3: Last resort - get any valid world
        if (!World && GEngine && GEngine->GetWorldContexts().Num() > 0) {
          World = GEngine->GetWorldContexts()[0].World();
        }

        if (World) {
          AActor *Camera = UGameplayStatics::GetActorOfClass(
              World, ACameraActor::StaticClass());
          if (Camera) {
            return Camera->GetActorTransform();
          }
        }
        return FTransform::Identity;
      });

  FTransform CameraTransform = future.Get();

  // Populate the reply with the transform data
  reply->mutable_transform()->mutable_location()->set_x(
      CameraTransform.GetLocation().X);
  reply->mutable_transform()->mutable_location()->set_y(
      CameraTransform.GetLocation().Y);
  reply->mutable_transform()->mutable_location()->set_z(
      CameraTransform.GetLocation().Z);

  FRotator Rotation = CameraTransform.GetRotation().Rotator();
  reply->mutable_transform()->mutable_rotation()->set_pitch(Rotation.Pitch);
  reply->mutable_transform()->mutable_rotation()->set_yaw(Rotation.Yaw);
  reply->mutable_transform()->mutable_rotation()->set_roll(Rotation.Roll);

  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::CaptureDepthMap(grpc::ServerContext *context,
                                    const uesynth::CaptureRequest *request,
                                    uesynth::ImageResponse *reply) {
  // Placeholder implementation
  reply->set_width(request->width());
  reply->set_height(request->height());
  reply->set_format("png");
  std::string dummyData(request->width() * request->height(), '\0');
  reply->set_image_data(dummyData);
  return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::CaptureSegmentationMask(
    grpc::ServerContext *context, const uesynth::CaptureRequest *request,
    uesynth::ImageResponse *reply) {
  // Placeholder implementation
  reply->set_width(request->width());
  reply->set_height(request->height());
  reply->set_format("png");
  std::string dummyData(request->width() * request->height() * 3, '\0');
  reply->set_image_data(dummyData);
  return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::SetObjectTransform(
    grpc::ServerContext *context,
    const uesynth::SetObjectTransformRequest *request,
    uesynth::CommandResponse *reply) {
  // Placeholder implementation
  reply->set_success(true);
  reply->set_message("SetObjectTransform not yet implemented");
  return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::GetObjectTransform(
    grpc::ServerContext *context,
    const uesynth::GetObjectTransformRequest *request,
    uesynth::GetObjectTransformResponse *reply) {
  // Placeholder implementation
  reply->mutable_transform()->mutable_location()->set_x(0.0f);
  reply->mutable_transform()->mutable_location()->set_y(0.0f);
  reply->mutable_transform()->mutable_location()->set_z(0.0f);
  reply->mutable_transform()->mutable_rotation()->set_pitch(0.0f);
  reply->mutable_transform()->mutable_rotation()->set_yaw(0.0f);
  reply->mutable_transform()->mutable_rotation()->set_roll(0.0f);
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::CreateCamera(grpc::ServerContext *context,
                                 const uesynth::CreateCameraRequest *request,
                                 uesynth::CommandResponse *reply) {
  // Placeholder implementation
  reply->set_success(true);
  reply->set_message("CreateCamera not yet implemented");
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::DestroyCamera(grpc::ServerContext *context,
                                  const uesynth::DestroyCameraRequest *request,
                                  uesynth::CommandResponse *reply) {
  // Placeholder implementation
  reply->set_success(true);
  reply->set_message("DestroyCamera not yet implemented");
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::SetResolution(grpc::ServerContext *context,
                                  const uesynth::SetResolutionRequest *request,
                                  uesynth::CommandResponse *reply) {
  // Placeholder implementation
  reply->set_success(true);
  reply->set_message("SetResolution not yet implemented");
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::CaptureNormals(grpc::ServerContext *context,
                                   const uesynth::CaptureRequest *request,
                                   uesynth::ImageResponse *reply) {
  // Placeholder implementation
  reply->set_width(request->width());
  reply->set_height(request->height());
  reply->set_format("png");
  std::string dummyData(request->width() * request->height() * 3, '\0');
  reply->set_image_data(dummyData);
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::CaptureOpticalFlow(grpc::ServerContext *context,
                                       const uesynth::CaptureRequest *request,
                                       uesynth::ImageResponse *reply) {
  // Placeholder implementation
  reply->set_width(request->width());
  reply->set_height(request->height());
  reply->set_format("png");
  std::string dummyData(request->width() * request->height() * 2, '\0');
  reply->set_image_data(dummyData);
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::SpawnObject(grpc::ServerContext *context,
                                const uesynth::SpawnObjectRequest *request,
                                uesynth::CommandResponse *reply) {
  // Placeholder implementation
  reply->set_success(true);
  reply->set_message("SpawnObject not yet implemented");
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::DestroyObject(grpc::ServerContext *context,
                                  const uesynth::DestroyObjectRequest *request,
                                  uesynth::CommandResponse *reply) {
  // Placeholder implementation
  reply->set_success(true);
  reply->set_message("DestroyObject not yet implemented");
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::SetMaterial(grpc::ServerContext *context,
                                const uesynth::SetMaterialRequest *request,
                                uesynth::CommandResponse *reply) {
  // Placeholder implementation
  reply->set_success(true);
  reply->set_message("SetMaterial not yet implemented");
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::ListObjects(grpc::ServerContext *context,
                                const google::protobuf::Empty *request,
                                uesynth::ListObjectsResponse *reply) {
  // Placeholder implementation - add some dummy objects
  reply->add_object_names("DummyObject1");
  reply->add_object_names("DummyObject2");
  return grpc::Status::OK;
}

grpc::Status
UESynthServiceImpl::SetLighting(grpc::ServerContext *context,
                                const uesynth::SetLightingRequest *request,
                                uesynth::CommandResponse *reply) {
  // Placeholder implementation
  reply->set_success(true);
  reply->set_message("SetLighting not yet implemented");
  return grpc::Status::OK;
}