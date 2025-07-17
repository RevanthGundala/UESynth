#!/usr/bin/env python3
"""
Async UESynth example showing both streaming and direct capture methods.
"""

import asyncio
import os
import numpy as np

# Add parent directory to path for import
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent))

from uesynth import AsyncUESynthClient


async def test_direct_capture(client):
    """Test direct capture methods (immediate results)."""
    print("\n🎯 Testing Direct Capture Methods:")

    try:
        # Direct RGB capture - returns numpy array immediately
        print("📸 Capturing RGB image (direct)...")
        rgb_image = await client.capture.rgb_direct(width=512, height=512)

        if isinstance(rgb_image, np.ndarray):
            print(f"✓ RGB image captured: {rgb_image.shape}, dtype: {rgb_image.dtype}")

            # Save the captured image
            try:
                import cv2
                # Convert RGBA to BGR for OpenCV (if it's RGBA)
                if rgb_image.shape[2] == 4:
                    # Convert RGBA to RGB, then to BGR
                    rgb_only = rgb_image[:, :, :3]  # Drop alpha channel
                    bgr_image = cv2.cvtColor(rgb_only, cv2.COLOR_RGB2BGR)
                else:
                    bgr_image = cv2.cvtColor(rgb_image, cv2.COLOR_RGB2BGR)
                
                save_path = os.path.join(os.path.dirname(__file__), "captured_rgb_direct.png")
                cv2.imwrite(save_path, bgr_image)
                print(f"✓ Image saved as {save_path}")
            except ImportError:
                # Fallback to PIL if cv2 not available
                try:
                    from PIL import Image
                    # Handle RGBA images properly
                    if rgb_image.shape[2] == 4:
                        img = Image.fromarray(rgb_image, 'RGBA')
                    else:
                        img = Image.fromarray(rgb_image, 'RGB')
                    save_path = os.path.join(os.path.dirname(__file__), "captured_rgb_direct.png")
                    img.save(save_path)
                    print(f"✓ Image saved as {save_path}")
                except ImportError:
                    print("⚠ Neither cv2 nor PIL available - install with: uv add opencv-python pillow")
        else:
            print(f"⚠ Unexpected data type: {type(rgb_image)}")

    except Exception as e:
        print(f"❌ Direct capture failed: {e}")


async def test_streaming_capture(client):
    """Test streaming capture methods (request ID + get_latest_frame)."""
    print("\n🌊 Testing Streaming Capture Methods:")

    try:
        # Streaming RGB capture - returns request ID
        print("📸 Sending RGB capture request (streaming)...")
        request_id = await client.capture.rgb(width=512, height=512)
        print(f"✓ Request sent, ID: {request_id}")

        # Wait a bit for response
        await asyncio.sleep(0.5)

        # Get the latest captured frame
        print("🔍 Retrieving latest frame...")
        image_data = await client.get_latest_frame()

        if image_data is not None:
            print(f"✓ Frame retrieved: {image_data.shape}, dtype: {image_data.dtype}")
            
            # Save the streaming frame
            try:
                import cv2
                # Convert RGBA to BGR for OpenCV (if it's RGBA)
                if image_data.shape[2] == 4:
                    # Convert RGBA to RGB, then to BGR
                    rgb_only = image_data[:, :, :3]  # Drop alpha channel
                    bgr_image = cv2.cvtColor(rgb_only, cv2.COLOR_RGB2BGR)
                else:
                    bgr_image = cv2.cvtColor(image_data, cv2.COLOR_RGB2BGR)
                
                save_path = os.path.join(os.path.dirname(__file__), "captured_rgb_streaming.png")
                cv2.imwrite(save_path, bgr_image)
                print(f"✓ Streaming image saved as {save_path}")
            except ImportError:
                # Fallback to PIL if cv2 not available
                try:
                    from PIL import Image
                    # Handle RGBA images properly
                    if image_data.shape[2] == 4:
                        img = Image.fromarray(image_data, 'RGBA')
                    else:
                        img = Image.fromarray(image_data, 'RGB')
                    save_path = os.path.join(os.path.dirname(__file__), "captured_rgb_streaming.png")
                    img.save(save_path)
                    print(f"✓ Streaming image saved as {save_path}")
                except ImportError:
                    print("⚠ Neither cv2 nor PIL available for streaming image save")
        else:
            print("⚠ No frame data available (normal if server not running)")

    except Exception as e:
        print(f"❌ Streaming capture failed: {e}")


async def main():
    """Main async example function."""
    print("🚀 UESynth Async Client Example")
    print("=" * 50)

    try:
        # Create and connect client with Windows host IP for WSL compatibility
        # Use "localhost:50051" if running directly on Windows
        # Use "172.27.224.1:50051" when running from WSL to connect to Windows UE
        windows_host_ip = "172.27.224.1:50051"
        client = AsyncUESynthClient(windows_host_ip)
        print("✓ Client created")

        print(f"🔌 Connecting to UESynth server ({windows_host_ip})...")
        await client.connect()
        print("✓ Connection setup initiated")

        # Set camera position
        print("\n📷 Setting camera position...")
        pos_request_id = await client.camera.set_location(0, 0, 100)
        rot_request_id = await client.camera.set_rotation(0, 0, 0)
        print(f"✓ Camera position request: {pos_request_id}")
        print(f"✓ Camera rotation request: {rot_request_id}")

        # Test both capture methods
        await test_direct_capture(client)
        await test_streaming_capture(client)

        # Show available methods
        print("\n📋 Available AsyncUESynthClient methods:")
        print(
            "🎥 Camera methods:",
            [m for m in dir(client.camera) if not m.startswith("_")],
        )
        print(
            "📸 Capture methods:",
            [m for m in dir(client.capture) if not m.startswith("_")],
        )
        print(
            "🎮 Object methods:",
            [m for m in dir(client.objects) if not m.startswith("_")],
        )

        # Cleanup
        await client.disconnect()
        print("\n✓ Disconnected successfully")

    except Exception as e:
        print(f"\n❌ Error: {e}")
        print("💡 This is normal if Unreal Engine isn't running with UESynth plugin")
        print("\nTo use UESynth:")
        print("1. Launch Unreal Engine")
        print("2. Load a project with UESynth plugin enabled")
        print("3. Make sure gRPC server is running on localhost:50051")


if __name__ == "__main__":
    asyncio.run(main())
