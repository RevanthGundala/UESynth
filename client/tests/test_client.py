# Copyright (c) 2025 UESynth Project
# SPDX-License-Identifier: MIT

"""Tests for UESynth client."""

import asyncio
from unittest.mock import AsyncMock, Mock, patch

from uesynth import AsyncUESynthClient, UESynthClient


class TestUESynthClient:
    """Test cases for UESynthClient (synchronous) class."""

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_init(self, mock_stub_class: Mock, mock_channel: Mock) -> None:
        """Test client initialization."""
        mock_channel_instance = Mock()
        mock_channel.return_value = mock_channel_instance
        mock_stub_instance = Mock()
        mock_stub_class.return_value = mock_stub_instance

        client = UESynthClient("test:1234")

        mock_channel.assert_called_once_with("test:1234")
        mock_stub_class.assert_called_once_with(mock_channel_instance)
        assert client.channel == mock_channel_instance
        assert client.stub == mock_stub_instance
        assert client.camera is not None
        assert client.capture is not None
        assert client.objects is not None

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_disconnect(self, mock_stub_class: Mock, mock_channel: Mock) -> None:
        """Test client disconnect."""
        mock_channel_instance = Mock()
        mock_channel.return_value = mock_channel_instance

        client = UESynthClient()
        client.disconnect()

        mock_channel_instance.close.assert_called_once()

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_default_address(self, mock_stub_class: Mock, mock_channel: Mock) -> None:
        """Test client uses default address."""
        UESynthClient()
        mock_channel.assert_called_once_with("localhost:50051")

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_camera_set_location(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test camera set location."""
        mock_stub_instance = Mock()
        mock_stub_class.return_value = mock_stub_instance

        client = UESynthClient()
        client.camera.set_location(x=1.0, y=2.0, z=3.0)

        mock_stub_instance.SetCameraTransform.assert_called_once()

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_camera_get_location(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test camera get location."""
        mock_stub_instance = Mock()
        mock_stub_class.return_value = mock_stub_instance

        client = UESynthClient()
        client.camera.get_location()

        mock_stub_instance.GetCameraTransform.assert_called_once()

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_capture_rgb(self, mock_stub_class: Mock, mock_channel: Mock) -> None:
        """Test RGB capture."""
        mock_stub_instance = Mock()
        mock_stub_class.return_value = mock_stub_instance

        # Mock the response
        mock_response = Mock()
        mock_response.image_data = b"\x00" * (100 * 100 * 3)  # 100x100 RGB image
        mock_response.height = 100
        mock_response.width = 100
        mock_stub_instance.CaptureRgbImage.return_value = mock_response

        client = UESynthClient()
        image = client.capture.rgb()

        mock_stub_instance.CaptureRgbImage.assert_called_once()
        assert image.shape == (100, 100, 3)

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_objects_set_location(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test objects set location."""
        mock_stub_instance = Mock()
        mock_stub_class.return_value = mock_stub_instance

        client = UESynthClient()
        client.objects.set_location("test_object", x=1.0, y=2.0, z=3.0)

        mock_stub_instance.SetObjectTransform.assert_called_once()


class TestAsyncUESynthClient:
    """Test cases for AsyncUESynthClient class."""

    def test_init(self) -> None:
        """Test async client initialization."""
        client = AsyncUESynthClient("test:1234")

        assert client.address == "test:1234"
        assert client.channel is None
        assert client.stub is None
        assert client.camera is not None
        assert client.capture is not None
        assert client.objects is not None

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_connect(self, mock_stub_class: Mock, mock_channel: Mock) -> None:
        """Test async client connect."""
        mock_channel_instance = AsyncMock()
        mock_channel.return_value = mock_channel_instance
        mock_stub_instance = AsyncMock()
        mock_stub_class.return_value = mock_stub_instance

        # Mock the ControlStream
        mock_stream = AsyncMock()
        mock_stub_instance.ControlStream.return_value = mock_stream

        client = AsyncUESynthClient("test:1234")

        # Mock the streaming methods to avoid actual streaming in tests
        with patch.object(
            client, "_start_streaming", new_callable=AsyncMock
        ) as mock_start_streaming:
            await client.connect()

        mock_channel.assert_called_once_with("test:1234")
        mock_stub_class.assert_called_once_with(mock_channel_instance)
        mock_start_streaming.assert_called_once()
        assert client.channel == mock_channel_instance
        assert client.stub == mock_stub_instance

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_disconnect(self, mock_stub_class: Mock, mock_channel: Mock) -> None:
        """Test async client disconnect."""
        mock_channel_instance = AsyncMock()
        mock_channel.return_value = mock_channel_instance

        client = AsyncUESynthClient()
        client.channel = mock_channel_instance
        client.request_queue = AsyncMock()
        client.response_task = AsyncMock()
        client.request_task = AsyncMock()

        await client.disconnect()

        mock_channel_instance.close.assert_called_once()

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_get_camera_location(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test async get camera location."""
        mock_stub_instance = AsyncMock()
        mock_stub_class.return_value = mock_stub_instance

        client = AsyncUESynthClient()
        client.stub = mock_stub_instance

        await client.get_camera_location("test_camera")

        mock_stub_instance.GetCameraTransform.assert_called_once()

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_set_object_transform(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test async set object transform."""
        mock_stub_instance = AsyncMock()
        mock_stub_class.return_value = mock_stub_instance

        client = AsyncUESynthClient()
        client.stub = mock_stub_instance

        await client.set_object_transform("test_object", x=1.0, y=2.0, z=3.0)

        mock_stub_instance.SetObjectTransform.assert_called_once()

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_camera_set_location(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test async camera set location."""
        client = AsyncUESynthClient()
        client.request_queue = AsyncMock()

        # Mock _send_action to avoid actual streaming
        with patch.object(
            client, "_send_action", new_callable=AsyncMock
        ) as mock_send_action:
            mock_send_action.return_value = "test_request_id"

            request_id = await client.camera.set_location(x=1.0, y=2.0, z=3.0)

            assert request_id == "test_request_id"
            mock_send_action.assert_called_once()

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_camera_get_location(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test async camera get location."""
        client = AsyncUESynthClient()

        # Mock get_camera_location
        with patch.object(
            client, "get_camera_location", new_callable=AsyncMock
        ) as mock_get_camera_location:
            mock_response = Mock()
            mock_get_camera_location.return_value = mock_response

            response = await client.camera.get_location("test_camera")

            assert response == mock_response
            mock_get_camera_location.assert_called_once_with("test_camera")

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_capture_rgb(self, mock_stub_class: Mock, mock_channel: Mock) -> None:
        """Test async RGB capture."""
        client = AsyncUESynthClient()
        client.request_queue = AsyncMock()

        # Mock _send_action to avoid actual streaming
        with patch.object(
            client, "_send_action", new_callable=AsyncMock
        ) as mock_send_action:
            mock_send_action.return_value = "test_request_id"

            request_id = await client.capture.rgb()

            assert request_id == "test_request_id"
            mock_send_action.assert_called_once()

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_capture_rgb_direct(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test async RGB capture direct."""
        mock_stub_instance = AsyncMock()
        mock_stub_class.return_value = mock_stub_instance

        # Mock the response
        mock_response = Mock()
        mock_response.image_data = b"\x00" * (100 * 100 * 3)  # 100x100 RGB image
        mock_response.height = 100
        mock_response.width = 100
        mock_stub_instance.CaptureRgbImage.return_value = mock_response

        client = AsyncUESynthClient()
        client.stub = mock_stub_instance

        image = await client.capture.rgb_direct()

        mock_stub_instance.CaptureRgbImage.assert_called_once()
        assert image.shape == (100, 100, 3)

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_objects_set_location(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test async objects set location."""
        client = AsyncUESynthClient()
        client.request_queue = AsyncMock()

        # Mock _send_action to avoid actual streaming
        with patch.object(
            client, "_send_action", new_callable=AsyncMock
        ) as mock_send_action:
            mock_send_action.return_value = "test_request_id"

            request_id = await client.objects.set_location(
                "test_object", x=1.0, y=2.0, z=3.0
            )

            assert request_id == "test_request_id"
            mock_send_action.assert_called_once()

    @patch("uesynth.grpc.aio.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    async def test_objects_set_transform_direct(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test async objects set transform direct."""
        client = AsyncUESynthClient()

        # Mock set_object_transform
        with patch.object(
            client, "set_object_transform", new_callable=AsyncMock
        ) as mock_set_object_transform:
            mock_response = Mock()
            mock_set_object_transform.return_value = mock_response

            response = await client.objects.set_transform_direct(
                "test_object", x=1.0, y=2.0, z=3.0
            )

            assert response == mock_response
            mock_set_object_transform.assert_called_once_with(
                "test_object", x=1.0, y=2.0, z=3.0
            )

    async def test_get_latest_frame_no_frame(self) -> None:
        """Test get latest frame when no frame is available."""
        client = AsyncUESynthClient()

        frame = await client.get_latest_frame()

        assert frame is None

    async def test_get_latest_frame_with_frame(self) -> None:
        """Test get latest frame when frame is available."""
        client = AsyncUESynthClient()
        client.lock = asyncio.Lock()

        # Mock image response
        mock_image_response = Mock()
        mock_image_response.image_data = b"\x00" * (50 * 50 * 3)  # 50x50 RGB image
        mock_image_response.height = 50
        mock_image_response.width = 50

        client.latest_responses = {"image": mock_image_response}

        frame = await client.get_latest_frame()

        assert frame is not None
        assert frame.shape == (50, 50, 3)


class TestCameraComponents:
    """Test cases for Camera component classes."""

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_sync_camera_initialization(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test sync camera is properly initialized."""
        mock_stub_instance = Mock()
        mock_stub_class.return_value = mock_stub_instance

        client = UESynthClient()

        assert client.camera.stub == mock_stub_instance

    def test_async_camera_initialization(self) -> None:
        """Test async camera is properly initialized."""
        client = AsyncUESynthClient()

        assert client.camera.client == client


class TestCaptureComponents:
    """Test cases for Capture component classes."""

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_sync_capture_initialization(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test sync capture is properly initialized."""
        mock_stub_instance = Mock()
        mock_stub_class.return_value = mock_stub_instance

        client = UESynthClient()

        assert client.capture.stub == mock_stub_instance

    def test_async_capture_initialization(self) -> None:
        """Test async capture is properly initialized."""
        client = AsyncUESynthClient()

        assert client.capture.client == client


class TestObjectsComponents:
    """Test cases for Objects component classes."""

    @patch("uesynth.grpc.insecure_channel")
    @patch("uesynth.uesynth_pb2_grpc.UESynthServiceStub")
    def test_sync_objects_initialization(
        self, mock_stub_class: Mock, mock_channel: Mock
    ) -> None:
        """Test sync objects is properly initialized."""
        mock_stub_instance = Mock()
        mock_stub_class.return_value = mock_stub_instance

        client = UESynthClient()

        assert client.objects.stub == mock_stub_instance

    def test_async_objects_initialization(self) -> None:
        """Test async objects is properly initialized."""
        client = AsyncUESynthClient()

        assert client.objects.client == client
