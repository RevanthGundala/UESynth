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
#include "Components/PrimitiveComponent.h"
#include "Async/Async.h"
#include "GameFramework/Actor.h"
#include "Async/TaskGraphInterfaces.h"
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
    auto future = Async(EAsyncExecution::TaskGraphMainThread, [=]() -> bool {
        AActor* Camera = UGameplayStatics::GetActorOfClass(GWorld, ACameraActor::StaticClass()); // Placeholder
        if (Camera) {
            // Set transform
            return true;
        }
        return false;
    });
    success = future.get();
    reply->set_success(success);
    reply->set_message(message);
    return grpc::Status::OK;
}

// Implement other methods similarly with placeholders
// e.g., CaptureRgbImage would use UE's screenshot or high-res screenshot request 

grpc::Status UESynthServiceImpl::CaptureRgbImage(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) {
    std::promise<std::vector<uint8>> promise;
    auto future = promise.get_future();
    AsyncTask(ENamedThreads::GameThread, [&]() {
        // Placeholder: Use FHighResScreenshotConfig for capture
        // Simulate image data
        std::vector<uint8> imageData( request->width() * request->height() * 4, 255 );
        promise.set_value(imageData);
    });
    auto data = future.get();
    reply->set_image_data(data.data(), data.size());
    reply->set_width(request->width());
    reply->set_height(request->height());
    reply->set_format("png");
    return grpc::Status::OK;
} 