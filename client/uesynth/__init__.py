# Copyright (c) 2025 UESynth Project
# SPDX-License-Identifier: MIT

"""UESynth Python client library for communicating with Unreal Engine via gRPC."""

import asyncio
import time
import uuid
from collections.abc import Callable
from typing import Any, Dict, Optional

import cv2
import grpc
import grpc.aio
import numpy as np

from uesynth import uesynth_pb2, uesynth_pb2_grpc


class AsyncUESynthClient:
    """Async client for high-performance interaction with UESynth Unreal Engine plugin via bidirectional gRPC streaming."""

    def __init__(self, address: str = "localhost:50051") -> None:
        """Initialize the async UESynth client.

        Args:
            address: The server address in format 'host:port'
        """
        self.address = address
        self.channel = None
        self.stub = None

        # Streaming state
        self.stream = None
        self.request_queue = None
        self.response_handlers = {}  # request_id -> callback
        self.latest_responses = {}  # response_type -> latest_response

        # Async tasks
        self.response_task = None
        self.request_task = None
        self.running = False
        self.lock = asyncio.Lock()

        # Initialize component controllers
        self.camera = self.Camera(self)
        self.capture = self.Capture(self)
        self.objects = self.Objects(self)

    async def connect(self) -> None:
        """Connect to the server and initialize streaming."""
        self.channel = grpc.aio.insecure_channel(self.address)
        self.stub = uesynth_pb2_grpc.UESynthServiceStub(self.channel)

        # Initialize streaming
        await self._start_streaming()

    async def _start_streaming(self) -> None:
        """Start the bidirectional streaming connection."""
        self.running = True
        self.stream = self.stub.ControlStream()
        self.request_queue = asyncio.Queue()

        # Start background tasks
        self.response_task = asyncio.create_task(self._response_handler())
        self.request_task = asyncio.create_task(self._request_handler())

        # Give tasks time to start
        await asyncio.sleep(0.1)

    async def _request_handler(self) -> None:
        """Background task to handle sending requests to the server."""
        try:
            while self.running:
                try:
                    # Get request from queue with timeout
                    request = await asyncio.wait_for(
                        self.request_queue.get(), timeout=1.0
                    )
                    if request is None:  # Shutdown signal
                        break

                    # Send request to server
                    await self.stream.write(request)
                    self.request_queue.task_done()

                except TimeoutError:
                    continue
                except Exception as e:
                    print(f"Error in request handler: {e}")
                    break
        finally:
            # Close the stream
            if self.stream:
                await self.stream.done_writing()

    async def _response_handler(self) -> None:
        """Background task to handle receiving responses from the server."""
        try:
            while self.running:
                try:
                    # Read response from stream
                    response = await self.stream.read()
                    if response == grpc.aio.EOF:
                        break

                    # Store latest response by type
                    async with self.lock:
                        if response.HasField("image_response"):
                            self.latest_responses["image"] = response.image_response
                        elif response.HasField("command_response"):
                            self.latest_responses["command"] = response.command_response
                        elif response.HasField("camera_transform"):
                            self.latest_responses["camera_transform"] = (
                                response.camera_transform
                            )
                        elif response.HasField("object_transform"):
                            self.latest_responses["object_transform"] = (
                                response.object_transform
                            )
                        elif response.HasField("objects_list"):
                            self.latest_responses["objects_list"] = (
                                response.objects_list
                            )

                    # Call specific response handler if registered
                    request_id = response.request_id
                    if request_id in self.response_handlers:
                        callback = self.response_handlers[request_id]
                        if asyncio.iscoroutinefunction(callback):
                            await callback(response)
                        else:
                            callback(response)
                        del self.response_handlers[request_id]

                except Exception as e:
                    print(f"Error in response handler: {e}")
                    break
        finally:
            self.running = False

    async def _send_action(
        self,
        action_request: uesynth_pb2.ActionRequest,
        callback: Callable | None = None,
    ) -> str:
        """Send an action request to the server.

        Args:
            action_request: The action request to send
            callback: Optional callback to handle the response

        Returns:
            Request ID for tracking
        """
        request_id = str(uuid.uuid4())
        action_request.request_id = request_id

        if callback:
            self.response_handlers[request_id] = callback

        # Queue the request for sending
        await self.request_queue.put(action_request)

        return request_id

    async def get_latest_frame(self) -> np.ndarray | None:
        """Get the latest captured frame.

        Returns:
            Latest RGB image as numpy array, or None if no frame available
        """
        async with self.lock:
            if "image" in self.latest_responses:
                img_response = self.latest_responses["image"]
                image = np.frombuffer(img_response.image_data, dtype=np.uint8).reshape(
                    img_response.height, img_response.width, -1
                )
                return image
        return None

    async def disconnect(self) -> None:
        """Close the gRPC channel and disconnect from the server."""
        self.running = False

        # Signal tasks to stop
        if self.request_queue:
            await self.request_queue.put(None)

        # Wait for tasks to finish
        if self.response_task:
            try:
                await asyncio.wait_for(self.response_task, timeout=2.0)
            except TimeoutError:
                self.response_task.cancel()

        if self.request_task:
            try:
                await asyncio.wait_for(self.request_task, timeout=2.0)
            except TimeoutError:
                self.request_task.cancel()

        # Close channel
        if self.channel:
            await self.channel.close()

    # Async versions of unary RPC methods
    async def get_camera_location(
        self, camera_name: str = ""
    ) -> uesynth_pb2.GetCameraTransformResponse:
        """Get camera location (async unary call).

        Args:
            camera_name: Name of the camera (empty for default)

        Returns:
            Camera transform response
        """
        request = uesynth_pb2.GetCameraTransformRequest(camera_name=camera_name)
        return await self.stub.GetCameraTransform(request)

    async def set_object_transform(
        self, object_name: str, x: float, y: float, z: float
    ) -> uesynth_pb2.CommandResponse:
        """Set object transform (async unary call).

        Args:
            object_name: Name of the object
            x: X coordinate
            y: Y coordinate
            z: Z coordinate

        Returns:
            Command response
        """
        transform = uesynth_pb2.Transform(location=uesynth_pb2.Vector3(x=x, y=y, z=z))
        request = uesynth_pb2.SetObjectTransformRequest(
            object_name=object_name, transform=transform
        )
        return await self.stub.SetObjectTransform(request)

    class Camera:
        """Camera control and manipulation methods."""

        def __init__(self, client: "AsyncUESynthClient") -> None:
            """Initialize camera controller.

            Args:
                client: The async client instance
            """
            self.client = client

        async def set_location(
            self, x: float, y: float, z: float, camera_name: str = ""
        ) -> str:
            """Set camera location in 3D space (non-blocking).

            Args:
                x: X coordinate
                y: Y coordinate
                z: Z coordinate
                camera_name: Name of the camera to move (empty for default)

            Returns:
                Request ID for tracking
            """
            transform = uesynth_pb2.Transform(
                location=uesynth_pb2.Vector3(x=x, y=y, z=z)
            )
            request = uesynth_pb2.SetCameraTransformRequest(
                camera_name=camera_name, transform=transform
            )

            action_request = uesynth_pb2.ActionRequest()
            action_request.set_camera_transform.CopyFrom(request)

            return await self.client._send_action(action_request)

        async def set_rotation(
            self, pitch: float, yaw: float, roll: float, camera_name: str = ""
        ) -> str:
            """Set camera rotation (non-blocking).

            Args:
                pitch: Pitch rotation in degrees
                yaw: Yaw rotation in degrees
                roll: Roll rotation in degrees
                camera_name: Name of the camera to rotate (empty for default)

            Returns:
                Request ID for tracking
            """
            transform = uesynth_pb2.Transform(
                rotation=uesynth_pb2.Rotator(pitch=pitch, yaw=yaw, roll=roll)
            )
            request = uesynth_pb2.SetCameraTransformRequest(
                camera_name=camera_name, transform=transform
            )

            action_request = uesynth_pb2.ActionRequest()
            action_request.set_camera_transform.CopyFrom(request)

            return await self.client._send_action(action_request)

        async def create(
            self,
            camera_name: str,
            x: float = 0,
            y: float = 0,
            z: float = 0,
            pitch: float = 0,
            yaw: float = 0,
            roll: float = 0,
        ) -> str:
            """Create a new camera with specified transform (non-blocking).

            Args:
                camera_name: Name for the new camera
                x: Initial X coordinate
                y: Initial Y coordinate
                z: Initial Z coordinate
                pitch: Initial pitch rotation in degrees
                yaw: Initial yaw rotation in degrees
                roll: Initial roll rotation in degrees

            Returns:
                Request ID for tracking
            """
            transform = uesynth_pb2.Transform(
                location=uesynth_pb2.Vector3(x=x, y=y, z=z),
                rotation=uesynth_pb2.Rotator(pitch=pitch, yaw=yaw, roll=roll),
            )
            request = uesynth_pb2.CreateCameraRequest(
                camera_name=camera_name, initial_transform=transform
            )

            action_request = uesynth_pb2.ActionRequest()
            action_request.create_camera.CopyFrom(request)

            return await self.client._send_action(action_request)

        # Async unary method for getting camera location
        async def get_location(
            self, camera_name: str = ""
        ) -> uesynth_pb2.GetCameraTransformResponse:
            """Get camera location (async unary call).

            Args:
                camera_name: Name of the camera (empty for default)

            Returns:
                Camera transform response
            """
            return await self.client.get_camera_location(camera_name)

    class Capture:
        """Image and data capture methods."""

        def __init__(self, client: "AsyncUESynthClient") -> None:
            """Initialize capture controller.

            Args:
                client: The async client instance
            """
            self.client = client

        async def rgb(
            self, camera_name: str = "", width: int = 0, height: int = 0
        ) -> str:
            """Capture RGB image from camera (non-blocking).

            Args:
                camera_name: Name of the camera to capture from (empty for default)
                width: Desired image width (0 for default)
                height: Desired image height (0 for default)

            Returns:
                Request ID for tracking
            """
            request = uesynth_pb2.CaptureRequest(
                camera_name=camera_name, width=width, height=height
            )

            action_request = uesynth_pb2.ActionRequest()
            action_request.capture_rgb.CopyFrom(request)

            return await self.client._send_action(action_request)

        async def depth(
            self, camera_name: str = "", width: int = 0, height: int = 0
        ) -> str:
            """Capture depth map from camera (non-blocking).

            Args:
                camera_name: Name of the camera to capture from (empty for default)
                width: Desired image width (0 for default)
                height: Desired image height (0 for default)

            Returns:
                Request ID for tracking
            """
            request = uesynth_pb2.CaptureRequest(
                camera_name=camera_name, width=width, height=height
            )

            action_request = uesynth_pb2.ActionRequest()
            action_request.capture_depth.CopyFrom(request)

            return await self.client._send_action(action_request)

        async def segmentation(
            self, camera_name: str = "", width: int = 0, height: int = 0
        ) -> str:
            """Capture segmentation mask from camera (non-blocking).

            Args:
                camera_name: Name of the camera to capture from (empty for default)
                width: Desired image width (0 for default)
                height: Desired image height (0 for default)

            Returns:
                Request ID for tracking
            """
            request = uesynth_pb2.CaptureRequest(
                camera_name=camera_name, width=width, height=height
            )

            action_request = uesynth_pb2.ActionRequest()
            action_request.capture_segmentation.CopyFrom(request)

            return await self.client._send_action(action_request)

        # Async unary method for direct RGB capture
        async def rgb_direct(
            self, camera_name: str = "", width: int = 0, height: int = 0
        ) -> np.ndarray:
            """Capture RGB image directly (async unary call).

            Args:
                camera_name: Name of the camera to capture from (empty for default)
                width: Desired image width (0 for default)
                height: Desired image height (0 for default)

            Returns:
                RGB image as numpy array
            """
            request = uesynth_pb2.CaptureRequest(
                camera_name=camera_name, width=width, height=height
            )
            response = await self.client.stub.CaptureRgbImage(request)
            image = np.frombuffer(response.image_data, dtype=np.uint8).reshape(
                response.height, response.width, -1
            )
            return image

    class Objects:
        """Object spawning and manipulation methods."""

        def __init__(self, client: "AsyncUESynthClient") -> None:
            """Initialize objects controller.

            Args:
                client: The async client instance
            """
            self.client = client

        async def set_location(
            self, object_name: str, x: float, y: float, z: float
        ) -> str:
            """Set object location in 3D space (non-blocking).

            Args:
                object_name: Name of the object to move
                x: X coordinate
                y: Y coordinate
                z: Z coordinate

            Returns:
                Request ID for tracking
            """
            transform = uesynth_pb2.Transform(
                location=uesynth_pb2.Vector3(x=x, y=y, z=z)
            )
            request = uesynth_pb2.SetObjectTransformRequest(
                object_name=object_name, transform=transform
            )

            action_request = uesynth_pb2.ActionRequest()
            action_request.set_object_transform.CopyFrom(request)

            return await self.client._send_action(action_request)

        async def spawn(
            self,
            object_name: str,
            asset_path: str,
            x: float = 0,
            y: float = 0,
            z: float = 0,
        ) -> str:
            """Spawn a new object from asset (non-blocking).

            Args:
                object_name: Name for the spawned object
                asset_path: Path to the asset to spawn
                x: Initial X coordinate
                y: Initial Y coordinate
                z: Initial Z coordinate

            Returns:
                Request ID for tracking
            """
            transform = uesynth_pb2.Transform(
                location=uesynth_pb2.Vector3(x=x, y=y, z=z)
            )
            request = uesynth_pb2.SpawnObjectRequest(
                object_name=object_name,
                asset_path=asset_path,
                initial_transform=transform,
            )

            action_request = uesynth_pb2.ActionRequest()
            action_request.spawn_object.CopyFrom(request)

            return await self.client._send_action(action_request)

        # Async unary method for setting object transform
        async def set_transform_direct(
            self, object_name: str, x: float, y: float, z: float
        ) -> uesynth_pb2.CommandResponse:
            """Set object transform directly (async unary call).

            Args:
                object_name: Name of the object
                x: X coordinate
                y: Y coordinate
                z: Z coordinate

            Returns:
                Command response
            """
            return await self.client.set_object_transform(object_name, x, y, z)


class UESynthClient:
    """Synchronous client for simple interactions with UESynth Unreal Engine plugin."""

    def __init__(self, address: str = "localhost:50051") -> None:
        """Initialize the synchronous UESynth client.

        Args:
            address: The server address in format 'host:port'
        """
        self.channel = grpc.insecure_channel(address)
        self.stub = uesynth_pb2_grpc.UESynthServiceStub(self.channel)
        self.camera = self.Camera(self.stub)
        self.capture = self.Capture(self.stub)
        self.objects = self.Objects(self.stub)

    def disconnect(self) -> None:
        """Close the gRPC channel and disconnect from the server."""
        self.channel.close()

    class Camera:
        """Camera control and manipulation methods."""

        def __init__(self, stub: Any) -> None:
            """Initialize camera controller.

            Args:
                stub: The gRPC service stub
            """
            self.stub = stub

        def set_location(
            self, x: float, y: float, z: float, camera_name: str = ""
        ) -> Any:
            """Set camera location in 3D space.

            Args:
                x: X coordinate
                y: Y coordinate
                z: Z coordinate
                camera_name: Name of the camera to move (empty for default)

            Returns:
                gRPC response object
            """
            transform = uesynth_pb2.Transform(
                location=uesynth_pb2.Vector3(x=x, y=y, z=z)
            )
            request = uesynth_pb2.SetCameraTransformRequest(
                camera_name=camera_name, transform=transform
            )
            return self.stub.SetCameraTransform(request)

        def set_rotation(
            self, pitch: float, yaw: float, roll: float, camera_name: str = ""
        ) -> Any:
            """Set camera rotation.

            Args:
                pitch: Pitch rotation in degrees
                yaw: Yaw rotation in degrees
                roll: Roll rotation in degrees
                camera_name: Name of the camera to rotate (empty for default)

            Returns:
                gRPC response object
            """
            transform = uesynth_pb2.Transform(
                rotation=uesynth_pb2.Rotator(pitch=pitch, yaw=yaw, roll=roll)
            )
            request = uesynth_pb2.SetCameraTransformRequest(
                camera_name=camera_name, transform=transform
            )
            return self.stub.SetCameraTransform(request)

        def get_location(self, camera_name: str = "") -> Any:
            """Get camera location.

            Args:
                camera_name: Name of the camera (empty for default)

            Returns:
                gRPC response object with camera transform
            """
            request = uesynth_pb2.GetCameraTransformRequest(camera_name=camera_name)
            return self.stub.GetCameraTransform(request)

        def create(
            self,
            camera_name: str,
            x: float = 0,
            y: float = 0,
            z: float = 0,
            pitch: float = 0,
            yaw: float = 0,
            roll: float = 0,
        ) -> Any:
            """Create a new camera with specified transform.

            Args:
                camera_name: Name for the new camera
                x: Initial X coordinate
                y: Initial Y coordinate
                z: Initial Z coordinate
                pitch: Initial pitch rotation in degrees
                yaw: Initial yaw rotation in degrees
                roll: Initial roll rotation in degrees

            Returns:
                gRPC response object
            """
            transform = uesynth_pb2.Transform(
                location=uesynth_pb2.Vector3(x=x, y=y, z=z),
                rotation=uesynth_pb2.Rotator(pitch=pitch, yaw=yaw, roll=roll),
            )
            request = uesynth_pb2.CreateCameraRequest(
                camera_name=camera_name, initial_transform=transform
            )
            return self.stub.CreateCamera(request)

    class Capture:
        """Image and data capture methods."""

        def __init__(self, stub: Any) -> None:
            """Initialize capture controller.

            Args:
                stub: The gRPC service stub
            """
            self.stub = stub

        def rgb(
            self, camera_name: str = "", width: int = 0, height: int = 0
        ) -> np.ndarray:
            """Capture RGB image from camera.

            Args:
                camera_name: Name of the camera to capture from (empty for default)
                width: Desired image width (0 for default)
                height: Desired image height (0 for default)

            Returns:
                RGB image as numpy array
            """
            request = uesynth_pb2.CaptureRequest(
                camera_name=camera_name, width=width, height=height
            )
            response = self.stub.CaptureRgbImage(request)
            image = np.frombuffer(response.image_data, dtype=np.uint8).reshape(
                response.height, response.width, -1
            )
            return image

        def depth(
            self, camera_name: str = "", width: int = 0, height: int = 0
        ) -> np.ndarray:
            """Capture depth map from camera.

            Args:
                camera_name: Name of the camera to capture from (empty for default)
                width: Desired image width (0 for default)
                height: Desired image height (0 for default)

            Returns:
                Depth map as numpy array
            """
            request = uesynth_pb2.CaptureRequest(
                camera_name=camera_name, width=width, height=height
            )
            response = self.stub.CaptureDepthMap(request)
            image = np.frombuffer(response.image_data, dtype=np.uint8).reshape(
                response.height, response.width, -1
            )
            return image

        def segmentation(
            self, camera_name: str = "", width: int = 0, height: int = 0
        ) -> np.ndarray:
            """Capture segmentation mask from camera.

            Args:
                camera_name: Name of the camera to capture from (empty for default)
                width: Desired image width (0 for default)
                height: Desired image height (0 for default)

            Returns:
                Segmentation mask as numpy array
            """
            request = uesynth_pb2.CaptureRequest(
                camera_name=camera_name, width=width, height=height
            )
            response = self.stub.CaptureSegmentationMask(request)
            image = np.frombuffer(response.image_data, dtype=np.uint8).reshape(
                response.height, response.width, -1
            )
            return image

    class Objects:
        """Object spawning and manipulation methods."""

        def __init__(self, stub: Any) -> None:
            """Initialize objects controller.

            Args:
                stub: The gRPC service stub
            """
            self.stub = stub

        def set_location(self, object_name: str, x: float, y: float, z: float) -> Any:
            """Set object location in 3D space.

            Args:
                object_name: Name of the object to move
                x: X coordinate
                y: Y coordinate
                z: Z coordinate

            Returns:
                gRPC response object
            """
            transform = uesynth_pb2.Transform(
                location=uesynth_pb2.Vector3(x=x, y=y, z=z)
            )
            request = uesynth_pb2.SetObjectTransformRequest(
                object_name=object_name, transform=transform
            )
            return self.stub.SetObjectTransform(request)

        def get_transform(self, object_name: str) -> Any:
            """Get object transform.

            Args:
                object_name: Name of the object

            Returns:
                gRPC response object with object transform
            """
            request = uesynth_pb2.GetObjectTransformRequest(object_name=object_name)
            return self.stub.GetObjectTransform(request)

        def spawn(
            self,
            object_name: str,
            asset_path: str,
            x: float = 0,
            y: float = 0,
            z: float = 0,
        ) -> Any:
            """Spawn a new object from asset.

            Args:
                object_name: Name for the spawned object
                asset_path: Path to the asset to spawn
                x: Initial X coordinate
                y: Initial Y coordinate
                z: Initial Z coordinate

            Returns:
                gRPC response object
            """
            transform = uesynth_pb2.Transform(
                location=uesynth_pb2.Vector3(x=x, y=y, z=z)
            )
            request = uesynth_pb2.SpawnObjectRequest(
                object_name=object_name,
                asset_path=asset_path,
                initial_transform=transform,
            )
            return self.stub.SpawnObject(request)


# Export both clients for different use cases
__all__ = ["UESynthClient", "AsyncUESynthClient"]
