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

#pragma once

#include "Generated/uesynth.grpc.pb.h"
#include <grpcpp/grpcpp.h>

class UESynthServiceImpl final : public uesynth::UESynthService::Service {
public:
    grpc::Status ControlStream(grpc::ServerContext* context, grpc::ServerReaderWriter<uesynth::FrameResponse, uesynth::ActionRequest>* stream) override;
    grpc::Status SetCameraTransform(grpc::ServerContext* context, const uesynth::SetCameraTransformRequest* request, uesynth::CommandResponse* reply) override;
    grpc::Status GetCameraTransform(grpc::ServerContext* context, const uesynth::GetCameraTransformRequest* request, uesynth::GetCameraTransformResponse* reply) override;
    grpc::Status CaptureRgbImage(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) override;
    grpc::Status CaptureDepthMap(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) override;
    grpc::Status CaptureSegmentationMask(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) override;
    grpc::Status SetObjectTransform(grpc::ServerContext* context, const uesynth::SetObjectTransformRequest* request, uesynth::CommandResponse* reply) override;
    grpc::Status GetObjectTransform(grpc::ServerContext* context, const uesynth::GetObjectTransformRequest* request, uesynth::GetObjectTransformResponse* reply) override;
    grpc::Status CreateCamera(grpc::ServerContext* context, const uesynth::CreateCameraRequest* request, uesynth::CommandResponse* reply) override;
    grpc::Status DestroyCamera(grpc::ServerContext* context, const uesynth::DestroyCameraRequest* request, uesynth::CommandResponse* reply) override;
    grpc::Status SetResolution(grpc::ServerContext* context, const uesynth::SetResolutionRequest* request, uesynth::CommandResponse* reply) override;
    grpc::Status CaptureNormals(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) override;
    grpc::Status CaptureOpticalFlow(grpc::ServerContext* context, const uesynth::CaptureRequest* request, uesynth::ImageResponse* reply) override;
    grpc::Status SpawnObject(grpc::ServerContext* context, const uesynth::SpawnObjectRequest* request, uesynth::CommandResponse* reply) override;
    grpc::Status DestroyObject(grpc::ServerContext* context, const uesynth::DestroyObjectRequest* request, uesynth::CommandResponse* reply) override;
    grpc::Status SetMaterial(grpc::ServerContext* context, const uesynth::SetMaterialRequest* request, uesynth::CommandResponse* reply) override;
    grpc::Status ListObjects(grpc::ServerContext* context, const google::protobuf::Empty* request, uesynth::ListObjectsResponse* reply) override;
    grpc::Status SetLighting(grpc::ServerContext* context, const uesynth::SetLightingRequest* request, uesynth::CommandResponse* reply) override;

private:
    // Helper method to process individual actions
    grpc::Status ProcessAction(const uesynth::ActionRequest& request, uesynth::FrameResponse* response);
}; 