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
            
            # Save with PIL since cv2 might not be available
            try:
                from PIL import Image
                if len(rgb_image.shape) == 3:
                    # Convert BGR to RGB if needed
                    if rgb_image.shape[2] == 3:
                        rgb_image = rgb_image[:, :, ::-1]  # BGR to RGB
                    img = Image.fromarray(rgb_image)
                    save_path = os.path.join(os.path.dirname(__file__), "captured_rgb_direct.png")
                    img.save(save_path)
                    print(f"✓ Image saved as {save_path}")
                else:
                    print(f"⚠ Unexpected image shape: {rgb_image.shape}")
            except ImportError:
                print("⚠ PIL not available - install with: uv add pillow")
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
        else:
            print("⚠ No frame data available (normal if server not running)")
            
    except Exception as e:
        print(f"❌ Streaming capture failed: {e}")


async def main():
    """Main async example function."""
    print("🚀 UESynth Async Client Example")
    print("=" * 50)
    
    try:
        # Create and connect client
        client = AsyncUESynthClient()
        print("✓ Client created")
        
        print("🔌 Connecting to UESynth server (localhost:50051)...")
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
        print("🎥 Camera methods:", [m for m in dir(client.camera) if not m.startswith('_')])
        print("📸 Capture methods:", [m for m in dir(client.capture) if not m.startswith('_')])
        print("🎮 Object methods:", [m for m in dir(client.objects) if not m.startswith('_')])
        
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
