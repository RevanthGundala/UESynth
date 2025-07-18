//Generated by TurboLink CodeGenerator, do not edit!
#include "UesynthMarshaling.h"
#include "SGoogleProtobuf/EmptyMarshaling.h"

void GRPC_TO_TURBOLINK(const ::uesynth::ActionRequest* in, FGrpcUesynthActionRequest* out)
{
    out->RequestId=StringCast<TCHAR>((const UTF8CHAR*)(in->request_id().c_str())).Get();
    switch(in->action_case())
    {
    case ::uesynth::ActionRequest::kSetCameraTransform:
    {
        FGrpcUesynthSetCameraTransformRequest field;
        GRPC_TO_TURBOLINK(&(in->set_camera_transform()), &field);
        out->Action.SetCameraTransform=MakeShareable(new FGrpcUesynthSetCameraTransformRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::SetCameraTransform;
    }
        break;
    case ::uesynth::ActionRequest::kGetCameraTransform:
    {
        FGrpcUesynthGetCameraTransformRequest field;
        GRPC_TO_TURBOLINK(&(in->get_camera_transform()), &field);
        out->Action.GetCameraTransform=MakeShareable(new FGrpcUesynthGetCameraTransformRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::GetCameraTransform;
    }
        break;
    case ::uesynth::ActionRequest::kCaptureRgb:
    {
        FGrpcUesynthCaptureRequest field;
        GRPC_TO_TURBOLINK(&(in->capture_rgb()), &field);
        out->Action.CaptureRgb=MakeShareable(new FGrpcUesynthCaptureRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::CaptureRgb;
    }
        break;
    case ::uesynth::ActionRequest::kCaptureDepth:
    {
        FGrpcUesynthCaptureRequest field;
        GRPC_TO_TURBOLINK(&(in->capture_depth()), &field);
        out->Action.CaptureDepth=MakeShareable(new FGrpcUesynthCaptureRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::CaptureDepth;
    }
        break;
    case ::uesynth::ActionRequest::kCaptureSegmentation:
    {
        FGrpcUesynthCaptureRequest field;
        GRPC_TO_TURBOLINK(&(in->capture_segmentation()), &field);
        out->Action.CaptureSegmentation=MakeShareable(new FGrpcUesynthCaptureRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::CaptureSegmentation;
    }
        break;
    case ::uesynth::ActionRequest::kCaptureNormals:
    {
        FGrpcUesynthCaptureRequest field;
        GRPC_TO_TURBOLINK(&(in->capture_normals()), &field);
        out->Action.CaptureNormals=MakeShareable(new FGrpcUesynthCaptureRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::CaptureNormals;
    }
        break;
    case ::uesynth::ActionRequest::kCaptureOpticalFlow:
    {
        FGrpcUesynthCaptureRequest field;
        GRPC_TO_TURBOLINK(&(in->capture_optical_flow()), &field);
        out->Action.CaptureOpticalFlow=MakeShareable(new FGrpcUesynthCaptureRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::CaptureOpticalFlow;
    }
        break;
    case ::uesynth::ActionRequest::kSetObjectTransform:
    {
        FGrpcUesynthSetObjectTransformRequest field;
        GRPC_TO_TURBOLINK(&(in->set_object_transform()), &field);
        out->Action.SetObjectTransform=MakeShareable(new FGrpcUesynthSetObjectTransformRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::SetObjectTransform;
    }
        break;
    case ::uesynth::ActionRequest::kGetObjectTransform:
    {
        FGrpcUesynthGetObjectTransformRequest field;
        GRPC_TO_TURBOLINK(&(in->get_object_transform()), &field);
        out->Action.GetObjectTransform=MakeShareable(new FGrpcUesynthGetObjectTransformRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::GetObjectTransform;
    }
        break;
    case ::uesynth::ActionRequest::kCreateCamera:
    {
        FGrpcUesynthCreateCameraRequest field;
        GRPC_TO_TURBOLINK(&(in->create_camera()), &field);
        out->Action.CreateCamera=MakeShareable(new FGrpcUesynthCreateCameraRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::CreateCamera;
    }
        break;
    case ::uesynth::ActionRequest::kDestroyCamera:
    {
        FGrpcUesynthDestroyCameraRequest field;
        GRPC_TO_TURBOLINK(&(in->destroy_camera()), &field);
        out->Action.DestroyCamera=MakeShareable(new FGrpcUesynthDestroyCameraRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::DestroyCamera;
    }
        break;
    case ::uesynth::ActionRequest::kSetResolution:
    {
        FGrpcUesynthSetResolutionRequest field;
        GRPC_TO_TURBOLINK(&(in->set_resolution()), &field);
        out->Action.SetResolution=MakeShareable(new FGrpcUesynthSetResolutionRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::SetResolution;
    }
        break;
    case ::uesynth::ActionRequest::kSpawnObject:
    {
        FGrpcUesynthSpawnObjectRequest field;
        GRPC_TO_TURBOLINK(&(in->spawn_object()), &field);
        out->Action.SpawnObject=MakeShareable(new FGrpcUesynthSpawnObjectRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::SpawnObject;
    }
        break;
    case ::uesynth::ActionRequest::kDestroyObject:
    {
        FGrpcUesynthDestroyObjectRequest field;
        GRPC_TO_TURBOLINK(&(in->destroy_object()), &field);
        out->Action.DestroyObject=MakeShareable(new FGrpcUesynthDestroyObjectRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::DestroyObject;
    }
        break;
    case ::uesynth::ActionRequest::kSetMaterial:
    {
        FGrpcUesynthSetMaterialRequest field;
        GRPC_TO_TURBOLINK(&(in->set_material()), &field);
        out->Action.SetMaterial=MakeShareable(new FGrpcUesynthSetMaterialRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::SetMaterial;
    }
        break;
    case ::uesynth::ActionRequest::kListObjects:
        GRPC_TO_TURBOLINK(&(in->list_objects()), &(out->Action.ListObjects));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::ListObjects;
        break;
    case ::uesynth::ActionRequest::kSetLighting:
    {
        FGrpcUesynthSetLightingRequest field;
        GRPC_TO_TURBOLINK(&(in->set_lighting()), &field);
        out->Action.SetLighting=MakeShareable(new FGrpcUesynthSetLightingRequest(field));
        out->Action.ActionCase = EGrpcUesynthActionRequestAction::SetLighting;
    }
        break;
    }
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthActionRequest* in, ::uesynth::ActionRequest* out)
{
    out->set_request_id((const char*)StringCast<UTF8CHAR>(*(in->RequestId)).Get());
    switch (in->Action.ActionCase)
    {
    case EGrpcUesynthActionRequestAction::SetCameraTransform:
        TURBOLINK_TO_GRPC(in->Action.SetCameraTransform.Get(), out->mutable_set_camera_transform());
        break;
    case EGrpcUesynthActionRequestAction::GetCameraTransform:
        TURBOLINK_TO_GRPC(in->Action.GetCameraTransform.Get(), out->mutable_get_camera_transform());
        break;
    case EGrpcUesynthActionRequestAction::CaptureRgb:
        TURBOLINK_TO_GRPC(in->Action.CaptureRgb.Get(), out->mutable_capture_rgb());
        break;
    case EGrpcUesynthActionRequestAction::CaptureDepth:
        TURBOLINK_TO_GRPC(in->Action.CaptureDepth.Get(), out->mutable_capture_depth());
        break;
    case EGrpcUesynthActionRequestAction::CaptureSegmentation:
        TURBOLINK_TO_GRPC(in->Action.CaptureSegmentation.Get(), out->mutable_capture_segmentation());
        break;
    case EGrpcUesynthActionRequestAction::CaptureNormals:
        TURBOLINK_TO_GRPC(in->Action.CaptureNormals.Get(), out->mutable_capture_normals());
        break;
    case EGrpcUesynthActionRequestAction::CaptureOpticalFlow:
        TURBOLINK_TO_GRPC(in->Action.CaptureOpticalFlow.Get(), out->mutable_capture_optical_flow());
        break;
    case EGrpcUesynthActionRequestAction::SetObjectTransform:
        TURBOLINK_TO_GRPC(in->Action.SetObjectTransform.Get(), out->mutable_set_object_transform());
        break;
    case EGrpcUesynthActionRequestAction::GetObjectTransform:
        TURBOLINK_TO_GRPC(in->Action.GetObjectTransform.Get(), out->mutable_get_object_transform());
        break;
    case EGrpcUesynthActionRequestAction::CreateCamera:
        TURBOLINK_TO_GRPC(in->Action.CreateCamera.Get(), out->mutable_create_camera());
        break;
    case EGrpcUesynthActionRequestAction::DestroyCamera:
        TURBOLINK_TO_GRPC(in->Action.DestroyCamera.Get(), out->mutable_destroy_camera());
        break;
    case EGrpcUesynthActionRequestAction::SetResolution:
        TURBOLINK_TO_GRPC(in->Action.SetResolution.Get(), out->mutable_set_resolution());
        break;
    case EGrpcUesynthActionRequestAction::SpawnObject:
        TURBOLINK_TO_GRPC(in->Action.SpawnObject.Get(), out->mutable_spawn_object());
        break;
    case EGrpcUesynthActionRequestAction::DestroyObject:
        TURBOLINK_TO_GRPC(in->Action.DestroyObject.Get(), out->mutable_destroy_object());
        break;
    case EGrpcUesynthActionRequestAction::SetMaterial:
        TURBOLINK_TO_GRPC(in->Action.SetMaterial.Get(), out->mutable_set_material());
        break;
    case EGrpcUesynthActionRequestAction::ListObjects:
        TURBOLINK_TO_GRPC(&(in->Action.ListObjects), out->mutable_list_objects());
        break;
    case EGrpcUesynthActionRequestAction::SetLighting:
        TURBOLINK_TO_GRPC(in->Action.SetLighting.Get(), out->mutable_set_lighting());
        break;
    }
}

void GRPC_TO_TURBOLINK(const ::uesynth::FrameResponse* in, FGrpcUesynthFrameResponse* out)
{
    out->RequestId=StringCast<TCHAR>((const UTF8CHAR*)(in->request_id().c_str())).Get();
    switch(in->response_case())
    {
    case ::uesynth::FrameResponse::kCommandResponse:
    {
        FGrpcUesynthCommandResponse field;
        GRPC_TO_TURBOLINK(&(in->command_response()), &field);
        out->Response.CommandResponse=MakeShareable(new FGrpcUesynthCommandResponse(field));
        out->Response.ResponseCase = EGrpcUesynthFrameResponseResponse::CommandResponse;
    }
        break;
    case ::uesynth::FrameResponse::kCameraTransform:
    {
        FGrpcUesynthGetCameraTransformResponse field;
        GRPC_TO_TURBOLINK(&(in->camera_transform()), &field);
        out->Response.CameraTransform=MakeShareable(new FGrpcUesynthGetCameraTransformResponse(field));
        out->Response.ResponseCase = EGrpcUesynthFrameResponseResponse::CameraTransform;
    }
        break;
    case ::uesynth::FrameResponse::kImageResponse:
    {
        FGrpcUesynthImageResponse field;
        GRPC_TO_TURBOLINK(&(in->image_response()), &field);
        out->Response.ImageResponse=MakeShareable(new FGrpcUesynthImageResponse(field));
        out->Response.ResponseCase = EGrpcUesynthFrameResponseResponse::ImageResponse;
    }
        break;
    case ::uesynth::FrameResponse::kObjectTransform:
    {
        FGrpcUesynthGetObjectTransformResponse field;
        GRPC_TO_TURBOLINK(&(in->object_transform()), &field);
        out->Response.ObjectTransform=MakeShareable(new FGrpcUesynthGetObjectTransformResponse(field));
        out->Response.ResponseCase = EGrpcUesynthFrameResponseResponse::ObjectTransform;
    }
        break;
    case ::uesynth::FrameResponse::kObjectsList:
    {
        FGrpcUesynthListObjectsResponse field;
        GRPC_TO_TURBOLINK(&(in->objects_list()), &field);
        out->Response.ObjectsList=MakeShareable(new FGrpcUesynthListObjectsResponse(field));
        out->Response.ResponseCase = EGrpcUesynthFrameResponseResponse::ObjectsList;
    }
        break;
    }
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthFrameResponse* in, ::uesynth::FrameResponse* out)
{
    out->set_request_id((const char*)StringCast<UTF8CHAR>(*(in->RequestId)).Get());
    switch (in->Response.ResponseCase)
    {
    case EGrpcUesynthFrameResponseResponse::CommandResponse:
        TURBOLINK_TO_GRPC(in->Response.CommandResponse.Get(), out->mutable_command_response());
        break;
    case EGrpcUesynthFrameResponseResponse::CameraTransform:
        TURBOLINK_TO_GRPC(in->Response.CameraTransform.Get(), out->mutable_camera_transform());
        break;
    case EGrpcUesynthFrameResponseResponse::ImageResponse:
        TURBOLINK_TO_GRPC(in->Response.ImageResponse.Get(), out->mutable_image_response());
        break;
    case EGrpcUesynthFrameResponseResponse::ObjectTransform:
        TURBOLINK_TO_GRPC(in->Response.ObjectTransform.Get(), out->mutable_object_transform());
        break;
    case EGrpcUesynthFrameResponseResponse::ObjectsList:
        TURBOLINK_TO_GRPC(in->Response.ObjectsList.Get(), out->mutable_objects_list());
        break;
    }
}

void GRPC_TO_TURBOLINK(const ::uesynth::Vector3* in, FGrpcUesynthVector3* out)
{
    out->X=in->x();
    out->Y=in->y();
    out->Z=in->z();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthVector3* in, ::uesynth::Vector3* out)
{
    out->set_x(in->X);
    out->set_y(in->Y);
    out->set_z(in->Z);
}

void GRPC_TO_TURBOLINK(const ::uesynth::Rotator* in, FGrpcUesynthRotator* out)
{
    out->Pitch=in->pitch();
    out->Yaw=in->yaw();
    out->Roll=in->roll();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthRotator* in, ::uesynth::Rotator* out)
{
    out->set_pitch(in->Pitch);
    out->set_yaw(in->Yaw);
    out->set_roll(in->Roll);
}

void GRPC_TO_TURBOLINK(const ::uesynth::Transform* in, FGrpcUesynthTransform* out)
{
    GRPC_TO_TURBOLINK(&(in->location()), &(out->Location));
    GRPC_TO_TURBOLINK(&(in->rotation()), &(out->Rotation));
    GRPC_TO_TURBOLINK(&(in->scale()), &(out->Scale));
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthTransform* in, ::uesynth::Transform* out)
{
    TURBOLINK_TO_GRPC(&(in->Location), out->mutable_location());
    TURBOLINK_TO_GRPC(&(in->Rotation), out->mutable_rotation());
    TURBOLINK_TO_GRPC(&(in->Scale), out->mutable_scale());
}

void GRPC_TO_TURBOLINK(const ::uesynth::CommandResponse* in, FGrpcUesynthCommandResponse* out)
{
    out->Success=in->success();
    out->Message=StringCast<TCHAR>((const UTF8CHAR*)(in->message().c_str())).Get();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthCommandResponse* in, ::uesynth::CommandResponse* out)
{
    out->set_success(in->Success);
    out->set_message((const char*)StringCast<UTF8CHAR>(*(in->Message)).Get());
}

void GRPC_TO_TURBOLINK(const ::uesynth::SetCameraTransformRequest* in, FGrpcUesynthSetCameraTransformRequest* out)
{
    out->CameraName=StringCast<TCHAR>((const UTF8CHAR*)(in->camera_name().c_str())).Get();
    GRPC_TO_TURBOLINK(&(in->transform()), &(out->Transform));
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthSetCameraTransformRequest* in, ::uesynth::SetCameraTransformRequest* out)
{
    out->set_camera_name((const char*)StringCast<UTF8CHAR>(*(in->CameraName)).Get());
    TURBOLINK_TO_GRPC(&(in->Transform), out->mutable_transform());
}

void GRPC_TO_TURBOLINK(const ::uesynth::GetCameraTransformRequest* in, FGrpcUesynthGetCameraTransformRequest* out)
{
    out->CameraName=StringCast<TCHAR>((const UTF8CHAR*)(in->camera_name().c_str())).Get();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthGetCameraTransformRequest* in, ::uesynth::GetCameraTransformRequest* out)
{
    out->set_camera_name((const char*)StringCast<UTF8CHAR>(*(in->CameraName)).Get());
}

void GRPC_TO_TURBOLINK(const ::uesynth::GetCameraTransformResponse* in, FGrpcUesynthGetCameraTransformResponse* out)
{
    GRPC_TO_TURBOLINK(&(in->transform()), &(out->Transform));
    out->Success=in->success();
    out->Message=StringCast<TCHAR>((const UTF8CHAR*)(in->message().c_str())).Get();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthGetCameraTransformResponse* in, ::uesynth::GetCameraTransformResponse* out)
{
    TURBOLINK_TO_GRPC(&(in->Transform), out->mutable_transform());
    out->set_success(in->Success);
    out->set_message((const char*)StringCast<UTF8CHAR>(*(in->Message)).Get());
}

void GRPC_TO_TURBOLINK(const ::uesynth::CaptureRequest* in, FGrpcUesynthCaptureRequest* out)
{
    out->CameraName=StringCast<TCHAR>((const UTF8CHAR*)(in->camera_name().c_str())).Get();
    out->Width=in->width();
    out->Height=in->height();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthCaptureRequest* in, ::uesynth::CaptureRequest* out)
{
    out->set_camera_name((const char*)StringCast<UTF8CHAR>(*(in->CameraName)).Get());
    out->set_width(in->Width);
    out->set_height(in->Height);
}

void GRPC_TO_TURBOLINK(const ::uesynth::ImageResponse* in, FGrpcUesynthImageResponse* out)
{
    out->ImageData=FBytes((const uint8*)in->image_data().c_str(), in->image_data().length());
    out->Width=in->width();
    out->Height=in->height();
    out->Format=StringCast<TCHAR>((const UTF8CHAR*)(in->format().c_str())).Get();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthImageResponse* in, ::uesynth::ImageResponse* out)
{
    out->set_image_data(std::string((const char*)(in->ImageData.Value.GetData()), (size_t)in->ImageData.Value.Num()));
    out->set_width(in->Width);
    out->set_height(in->Height);
    out->set_format((const char*)StringCast<UTF8CHAR>(*(in->Format)).Get());
}

void GRPC_TO_TURBOLINK(const ::uesynth::SetObjectTransformRequest* in, FGrpcUesynthSetObjectTransformRequest* out)
{
    out->ObjectName=StringCast<TCHAR>((const UTF8CHAR*)(in->object_name().c_str())).Get();
    GRPC_TO_TURBOLINK(&(in->transform()), &(out->Transform));
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthSetObjectTransformRequest* in, ::uesynth::SetObjectTransformRequest* out)
{
    out->set_object_name((const char*)StringCast<UTF8CHAR>(*(in->ObjectName)).Get());
    TURBOLINK_TO_GRPC(&(in->Transform), out->mutable_transform());
}

void GRPC_TO_TURBOLINK(const ::uesynth::GetObjectTransformRequest* in, FGrpcUesynthGetObjectTransformRequest* out)
{
    out->ObjectName=StringCast<TCHAR>((const UTF8CHAR*)(in->object_name().c_str())).Get();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthGetObjectTransformRequest* in, ::uesynth::GetObjectTransformRequest* out)
{
    out->set_object_name((const char*)StringCast<UTF8CHAR>(*(in->ObjectName)).Get());
}

void GRPC_TO_TURBOLINK(const ::uesynth::GetObjectTransformResponse* in, FGrpcUesynthGetObjectTransformResponse* out)
{
    GRPC_TO_TURBOLINK(&(in->transform()), &(out->Transform));
    out->Success=in->success();
    out->Message=StringCast<TCHAR>((const UTF8CHAR*)(in->message().c_str())).Get();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthGetObjectTransformResponse* in, ::uesynth::GetObjectTransformResponse* out)
{
    TURBOLINK_TO_GRPC(&(in->Transform), out->mutable_transform());
    out->set_success(in->Success);
    out->set_message((const char*)StringCast<UTF8CHAR>(*(in->Message)).Get());
}

void GRPC_TO_TURBOLINK(const ::uesynth::CreateCameraRequest* in, FGrpcUesynthCreateCameraRequest* out)
{
    out->CameraName=StringCast<TCHAR>((const UTF8CHAR*)(in->camera_name().c_str())).Get();
    GRPC_TO_TURBOLINK(&(in->initial_transform()), &(out->InitialTransform));
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthCreateCameraRequest* in, ::uesynth::CreateCameraRequest* out)
{
    out->set_camera_name((const char*)StringCast<UTF8CHAR>(*(in->CameraName)).Get());
    TURBOLINK_TO_GRPC(&(in->InitialTransform), out->mutable_initial_transform());
}

void GRPC_TO_TURBOLINK(const ::uesynth::DestroyCameraRequest* in, FGrpcUesynthDestroyCameraRequest* out)
{
    out->CameraName=StringCast<TCHAR>((const UTF8CHAR*)(in->camera_name().c_str())).Get();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthDestroyCameraRequest* in, ::uesynth::DestroyCameraRequest* out)
{
    out->set_camera_name((const char*)StringCast<UTF8CHAR>(*(in->CameraName)).Get());
}

void GRPC_TO_TURBOLINK(const ::uesynth::SetResolutionRequest* in, FGrpcUesynthSetResolutionRequest* out)
{
    out->CameraName=StringCast<TCHAR>((const UTF8CHAR*)(in->camera_name().c_str())).Get();
    out->Width=in->width();
    out->Height=in->height();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthSetResolutionRequest* in, ::uesynth::SetResolutionRequest* out)
{
    out->set_camera_name((const char*)StringCast<UTF8CHAR>(*(in->CameraName)).Get());
    out->set_width(in->Width);
    out->set_height(in->Height);
}

void GRPC_TO_TURBOLINK(const ::uesynth::ListObjectsResponse* in, FGrpcUesynthListObjectsResponse* out)
{
    out->ObjectNames.Empty();
    for (int i=0; i<in->object_names_size(); ++i) {
        out->ObjectNames.Add(StringCast<TCHAR>((const UTF8CHAR*)(in->object_names(i).c_str())).Get());
    }
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthListObjectsResponse* in, ::uesynth::ListObjectsResponse* out)
{
    for(const auto& value : in->ObjectNames) {
        out->add_object_names((const char*)StringCast<UTF8CHAR>(*(value)).Get());
    } 
}

void GRPC_TO_TURBOLINK(const ::uesynth::SpawnObjectRequest* in, FGrpcUesynthSpawnObjectRequest* out)
{
    out->ObjectName=StringCast<TCHAR>((const UTF8CHAR*)(in->object_name().c_str())).Get();
    out->AssetPath=StringCast<TCHAR>((const UTF8CHAR*)(in->asset_path().c_str())).Get();
    GRPC_TO_TURBOLINK(&(in->initial_transform()), &(out->InitialTransform));
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthSpawnObjectRequest* in, ::uesynth::SpawnObjectRequest* out)
{
    out->set_object_name((const char*)StringCast<UTF8CHAR>(*(in->ObjectName)).Get());
    out->set_asset_path((const char*)StringCast<UTF8CHAR>(*(in->AssetPath)).Get());
    TURBOLINK_TO_GRPC(&(in->InitialTransform), out->mutable_initial_transform());
}

void GRPC_TO_TURBOLINK(const ::uesynth::DestroyObjectRequest* in, FGrpcUesynthDestroyObjectRequest* out)
{
    out->ObjectName=StringCast<TCHAR>((const UTF8CHAR*)(in->object_name().c_str())).Get();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthDestroyObjectRequest* in, ::uesynth::DestroyObjectRequest* out)
{
    out->set_object_name((const char*)StringCast<UTF8CHAR>(*(in->ObjectName)).Get());
}

void GRPC_TO_TURBOLINK(const ::uesynth::SetMaterialRequest* in, FGrpcUesynthSetMaterialRequest* out)
{
    out->ObjectName=StringCast<TCHAR>((const UTF8CHAR*)(in->object_name().c_str())).Get();
    out->MaterialProperty=StringCast<TCHAR>((const UTF8CHAR*)(in->material_property().c_str())).Get();
    out->Value=StringCast<TCHAR>((const UTF8CHAR*)(in->value().c_str())).Get();
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthSetMaterialRequest* in, ::uesynth::SetMaterialRequest* out)
{
    out->set_object_name((const char*)StringCast<UTF8CHAR>(*(in->ObjectName)).Get());
    out->set_material_property((const char*)StringCast<UTF8CHAR>(*(in->MaterialProperty)).Get());
    out->set_value((const char*)StringCast<UTF8CHAR>(*(in->Value)).Get());
}

void GRPC_TO_TURBOLINK(const ::uesynth::SetLightingRequest* in, FGrpcUesynthSetLightingRequest* out)
{
    out->LightName=StringCast<TCHAR>((const UTF8CHAR*)(in->light_name().c_str())).Get();
    out->Intensity=in->intensity();
    GRPC_TO_TURBOLINK(&(in->color()), &(out->Color));
    GRPC_TO_TURBOLINK(&(in->transform()), &(out->Transform));
}

void TURBOLINK_TO_GRPC(const FGrpcUesynthSetLightingRequest* in, ::uesynth::SetLightingRequest* out)
{
    out->set_light_name((const char*)StringCast<UTF8CHAR>(*(in->LightName)).Get());
    out->set_intensity(in->Intensity);
    TURBOLINK_TO_GRPC(&(in->Color), out->mutable_color());
    TURBOLINK_TO_GRPC(&(in->Transform), out->mutable_transform());
}

