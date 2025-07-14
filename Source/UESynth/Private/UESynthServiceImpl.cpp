// Copyright (C) 2025 UESynth Project
// SPDX-License-Identifier: GPL-3.0-or-later

/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "UESynthServiceImpl.h"
#include "UESynth.h" // For module access
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraActor.h"
#include "Components/PrimitiveComponent.h"
#include "Async/Async.h"
#include "GameFramework/Actor.h"
#include "Async/TaskGraphInterfaces.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include <grpcpp/server_builder.h>
#include <future>

// Helper to run on game thread
#define RUN_ON_GAME_THREAD(Code) \
    AsyncTask(ENamedThreads::GameThread, [=]() { Code });

// New bidirectional streaming method implementation
grpc::Status UESynthServiceImpl::ControlStream(grpc::ServerContext* context, grpc::ServerReaderWriter<uesynth::FrameResponse, uesynth::ActionRequest>* stream) {
    uesynth::ActionRequest request;
    uesynth::FrameResponse response;
    
    // Continuously read requests from the client stream
    while (stream->Read(&request)) {
        // Process the action and generate response
        grpc::Status status = ProcessAction(request, &response);
        
        if (!status.ok()) {
            // Log error and continue processing other requests
            UE_LOG(LogTemp, Error, TEXT("Error processing action: %s"), *FString(status.error_message().c_str()));
            continue;
        }
        
        // Send response back to client if there's data to send
        if (response.has_image_response() || response.has_command_response() || 
            response.has_camera_transform() || response.has_object_transform() || 
            response.has_objects_list()) {
            
            if (!stream->Write(response)) {
                // Client disconnected or write failed
                UE_LOG(LogTemp, Warning, TEXT("Failed to write response to client stream"));
                break;
            }
        }
        
        // Clear response for next iteration
        response.Clear();
    }
    
    return grpc::Status::OK;
}

// Helper method to process individual actions
grpc::Status UESynthServiceImpl::ProcessAction(const uesynth::ActionRequest& request, uesynth::FrameResponse* response) {
    response->set_request_id(request.request_id());
    
    // Handle different action types using switch on oneof case
    switch (request.action_case()) {
        case uesynth::ActionRequest::kSetCameraTransform: {
            uesynth::CommandResponse cmd_response;
            grpc::Status status = SetCameraTransform(nullptr, &request.set_camera_transform(), &cmd_response);
            if (status.ok()) {
                response->mutable_command_response()->CopyFrom(cmd_response);
            }
            return status;
        }
        
        case uesynth::ActionRequest::kGetCameraTransform: {
            uesynth::GetCameraTransformResponse cam_response;
            grpc::Status status = GetCameraTransform(nullptr, &request.get_camera_transform(), &cam_response);
            if (status.ok()) {
                response->mutable_camera_transform()->CopyFrom(cam_response);
            }
            return status;
        }
        
        case uesynth::ActionRequest::kCaptureRgb: {
            uesynth::ImageResponse img_response;
            grpc::Status status = CaptureRgbImage(nullptr, &request.capture_rgb(), &img_response);
            if (status.ok()) {
                response->mutable_image_response()->CopyFrom(img_response);
            }
            return status;
        }
        
        case uesynth::ActionRequest::kCaptureDepth: {
            uesynth::ImageResponse img_response;
            grpc::Status status = CaptureDepthMap(nullptr, &request.capture_depth(), &img_response);
            if (status.ok()) {
                response->mutable_image_response()->CopyFrom(img_response);
            }
            return status;
        }
        
        case uesynth::ActionRequest::kCaptureSegmentation: {
            uesynth::ImageResponse img_response;
            grpc::Status status = CaptureSegmentationMask(nullptr, &request.capture_segmentation(), &img_response);
            if (status.ok()) {
                response->mutable_image_response()->CopyFrom(img_response);
            }
            return status;
        }
        
        case uesynth::ActionRequest::kSetObjectTransform: {
            uesynth::CommandResponse cmd_response;
            grpc::Status status = SetObjectTransform(nullptr, &request.set_object_transform(), &cmd_response);
            if (status.ok()) {
                response->mutable_command_response()->CopyFrom(cmd_response);
            }
            return status;
        }
        
        case uesynth::ActionRequest::kGetObjectTransform: {
            uesynth::GetObjectTransformResponse obj_response;
            grpc::Status status = GetObjectTransform(nullptr, &request.get_object_transform(), &obj_response);
            if (status.ok()) {
                response->mutable_object_transform()->CopyFrom(obj_response);
            }
            return status;
        }
        
        // Add more cases for other action types as needed
        case uesynth::ActionRequest::kListObjects: {
            uesynth::ListObjectsResponse list_response;
            grpc::Status status = ListObjects(nullptr, &request.list_objects(), &list_response);
            if (status.ok()) {
                response->mutable_objects_list()->CopyFrom(list_response);
            }
            return status;
        }
        
        default:
            return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Action not implemented");
    }
}

grpc::Status UESynthServiceImpl::SetCameraTransform(grpc::ServerContext* context, const uesynth::SetCameraTransformRequest* request, uesynth::CommandResponse* reply) {
    bool success = false;
    std::string message;
    auto future = Async(EAsyncExecution::TaskGraphMainThread, [request, &success, &message]() -> bool {
        UWorld* World = GEngine->GetWorldFromContextObject(nullptr, EGetWorldErrorMode::LogAndReturnNull);
        if (World) {
            AActor* Camera = UGameplayStatics::GetActorOfClass(World, ACameraActor::StaticClass()); // Placeholder
            if (Camera) {
                // Convert protobuf types to Unreal Engine types
                FVector Location(
                    request->transform().location().x(),
                    request->transform().location().y(),
                    request->transform().location().z()
                );
                FRotator Rotation(
                    request->transform().rotation().pitch(),
                    request->transform().rotation().yaw(),
                    request->transform().rotation().roll()
                );
                
                Camera->SetActorLocationAndRotation(Location, Rotation);
                message = "Camera transform set successfully";
                success = true;
                return true;
            }
        }
        message = "Camera not found";
        success = false;
        return false;
    });
    future.Get();
    reply->set_success(success);
    reply->set_message(message);
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::CaptureRgbImage(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) {
    std::promise<std::vector<uint8>> promise;
    auto future = promise.get_future();
    AsyncTask(ENamedThreads::GameThread, [&promise, request]() {
        // Placeholder: Use FHighResScreenshotConfig for capture
        // Simulate image data
        std::vector<uint8> imageData(request->width() * request->height() * 4, 255);
        // Note: FHighResScreenshot::CaptureScreen would need proper implementation
        // For now, just return dummy data
        promise.set_value(imageData);
    });
    auto data = future.get();
    reply->set_image_data(data.data(), data.size());
    reply->set_width(request->width());
    reply->set_height(request->height());
    reply->set_format("png");
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::GetCameraTransform(
    grpc::ServerContext* context,
    const uesynth::GetCameraTransformRequest* request,
    uesynth::GetCameraTransformResponse* reply
) {
    // Placeholder implementation - get camera transform
    auto future = Async(EAsyncExecution::TaskGraphMainThread, [request]() -> FTransform {
        UWorld* World = GEngine->GetWorldFromContextObject(nullptr, EGetWorldErrorMode::LogAndReturnNull);
        if (World) {
            AActor* Camera = UGameplayStatics::GetActorOfClass(World, ACameraActor::StaticClass());
            if (Camera) {
                return Camera->GetActorTransform();
            }
        }
        return FTransform::Identity;
    });
    
    FTransform CameraTransform = future.Get();
    
    // Populate the reply with the transform data
    reply->mutable_transform()->mutable_location()->set_x(CameraTransform.GetLocation().X);
    reply->mutable_transform()->mutable_location()->set_y(CameraTransform.GetLocation().Y);
    reply->mutable_transform()->mutable_location()->set_z(CameraTransform.GetLocation().Z);

    FRotator Rotation = CameraTransform.GetRotation().Rotator();
    reply->mutable_transform()->mutable_rotation()->set_pitch(Rotation.Pitch);
    reply->mutable_transform()->mutable_rotation()->set_yaw(Rotation.Yaw);
    reply->mutable_transform()->mutable_rotation()->set_roll(Rotation.Roll);

    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::CaptureDepthMap(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) {
    // Placeholder implementation
    reply->set_width(request->width());
    reply->set_height(request->height());
    reply->set_format("png");
    std::string dummyData(request->width() * request->height(), '\0');
    reply->set_image_data(dummyData);
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::CaptureSegmentationMask(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) {
    // Placeholder implementation
    reply->set_width(request->width());
    reply->set_height(request->height());
    reply->set_format("png");
    std::string dummyData(request->width() * request->height() * 3, '\0');
    reply->set_image_data(dummyData);
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::SetObjectTransform(grpc::ServerContext* context, const uesynth::SetObjectTransformRequest* request, uesynth::CommandResponse* reply) {
    // Placeholder implementation
    reply->set_success(true);
    reply->set_message("SetObjectTransform not yet implemented");
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::GetObjectTransform(grpc::ServerContext* context, const uesynth::GetObjectTransformRequest* request, uesynth::GetObjectTransformResponse* reply) {
    // Placeholder implementation
    reply->mutable_transform()->mutable_location()->set_x(0.0f);
    reply->mutable_transform()->mutable_location()->set_y(0.0f);
    reply->mutable_transform()->mutable_location()->set_z(0.0f);
    reply->mutable_transform()->mutable_rotation()->set_pitch(0.0f);
    reply->mutable_transform()->mutable_rotation()->set_yaw(0.0f);
    reply->mutable_transform()->mutable_rotation()->set_roll(0.0f);
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::CreateCamera(grpc::ServerContext* context, const uesynth::CreateCameraRequest* request, uesynth::CommandResponse* reply) {
    // Placeholder implementation
    reply->set_success(true);
    reply->set_message("CreateCamera not yet implemented");
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::DestroyCamera(grpc::ServerContext* context, const uesynth::DestroyCameraRequest* request, uesynth::CommandResponse* reply) {
    // Placeholder implementation
    reply->set_success(true);
    reply->set_message("DestroyCamera not yet implemented");
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::SetResolution(grpc::ServerContext* context, const uesynth::SetResolutionRequest* request, uesynth::CommandResponse* reply) {
    // Placeholder implementation
    reply->set_success(true);
    reply->set_message("SetResolution not yet implemented");
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::CaptureNormals(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) {
    // Placeholder implementation
    reply->set_width(request->width());
    reply->set_height(request->height());
    reply->set_format("png");
    std::string dummyData(request->width() * request->height() * 3, '\0');
    reply->set_image_data(dummyData);
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::CaptureOpticalFlow(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) {
    // Placeholder implementation
    reply->set_width(request->width());
    reply->set_height(request->height());
    reply->set_format("png");
    std::string dummyData(request->width() * request->height() * 2, '\0');
    reply->set_image_data(dummyData);
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::SpawnObject(grpc::ServerContext* context, const uesynth::SpawnObjectRequest* request, uesynth::CommandResponse* reply) {
    // Placeholder implementation
    reply->set_success(true);
    reply->set_message("SpawnObject not yet implemented");
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::DestroyObject(grpc::ServerContext* context, const uesynth::DestroyObjectRequest* request, uesynth::CommandResponse* reply) {
    // Placeholder implementation
    reply->set_success(true);
    reply->set_message("DestroyObject not yet implemented");
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::SetMaterial(grpc::ServerContext* context, const uesynth::SetMaterialRequest* request, uesynth::CommandResponse* reply) {
    // Placeholder implementation
    reply->set_success(true);
    reply->set_message("SetMaterial not yet implemented");
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::ListObjects(grpc::ServerContext* context, const google::protobuf::Empty* request, uesynth::ListObjectsResponse* reply) {
    // Placeholder implementation - add some dummy objects
    reply->add_object_names("DummyObject1");
    reply->add_object_names("DummyObject2");
    return grpc::Status::OK;
}

grpc::Status UESynthServiceImpl::SetLighting(grpc::ServerContext* context, const uesynth::SetLightingRequest* request, uesynth::CommandResponse* reply) {
    // Placeholder implementation
    reply->set_success(true);
    reply->set_message("SetLighting not yet implemented");
    return grpc::Status::OK;
} 