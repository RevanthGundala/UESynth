#!/usr/bin/env python3
"""
Test streaming capture with the same parameters that work in health check.
"""

import asyncio
import sys
from pathlib import Path

# Add parent directory to path for import
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import AsyncUESynthClient


async def test_streaming_capture():
    """Test streaming capture with known working parameters."""
    print("🚀 Testing Streaming Capture")
    print("=" * 40)
    
    try:
        # Connect to server
        client = AsyncUESynthClient("172.27.224.1:50051")
        await client.connect()
        print("✓ Connected to server")
        
        # Set camera position
        await client.camera.set_location(0, 0, 100)
        await client.camera.set_rotation(0, 0, 0)
        print("✓ Camera positioned")
        
        # Test with small image first (same as health check)
        print("\n📸 Testing small image (64x64)...")
        request_id = await client.capture.rgb(width=64, height=64)
        print(f"✓ Request sent: {request_id}")
        
        # Wait for processing
        await asyncio.sleep(1.0)
        
        # Get frame
        frame_data = await client.get_latest_frame()
        if frame_data is not None:
            print(f"✓ Small frame retrieved: {frame_data.shape}")
            
            # Save it
            try:
                from PIL import Image
                if frame_data.shape[2] == 4:  # RGBA
                    img = Image.fromarray(frame_data, 'RGBA')
                else:  # RGB
                    img = Image.fromarray(frame_data, 'RGB')
                save_path = "test_small_capture.png"
                img.save(save_path)
                print(f"✓ Saved as {save_path}")
            except ImportError:
                print("⚠ PIL not available for saving")
        else:
            print("❌ No frame data for small image")
        
        # Test with larger image
        print("\n📸 Testing larger image (256x256)...")
        request_id = await client.capture.rgb(width=256, height=256)
        print(f"✓ Request sent: {request_id}")
        
        # Wait longer for larger image
        await asyncio.sleep(2.0)
        
        # Get frame
        frame_data = await client.get_latest_frame()
        if frame_data is not None:
            print(f"✓ Large frame retrieved: {frame_data.shape}")
            
            # Save it
            try:
                from PIL import Image
                if frame_data.shape[2] == 4:  # RGBA
                    img = Image.fromarray(frame_data, 'RGBA')
                else:  # RGB
                    img = Image.fromarray(frame_data, 'RGB')
                save_path = "test_large_capture.png"
                img.save(save_path)
                print(f"✓ Saved as {save_path}")
            except ImportError:
                print("⚠ PIL not available for saving")
        else:
            print("❌ No frame data for large image")
        
        # Test original size (512x512)
        print("\n📸 Testing original size (512x512)...")
        request_id = await client.capture.rgb(width=512, height=512)
        print(f"✓ Request sent: {request_id}")
        
        # Wait even longer
        await asyncio.sleep(3.0)
        
        # Get frame
        frame_data = await client.get_latest_frame()
        if frame_data is not None:
            print(f"✓ Original size frame retrieved: {frame_data.shape}")
            
            # Save it
            try:
                from PIL import Image
                if frame_data.shape[2] == 4:  # RGBA
                    img = Image.fromarray(frame_data, 'RGBA')
                else:  # RGB
                    img = Image.fromarray(frame_data, 'RGB')
                save_path = "test_original_capture.png"
                img.save(save_path)
                print(f"✓ Saved as {save_path}")
            except ImportError:
                print("⚠ PIL not available for saving")
        else:
            print("❌ No frame data for original size")
        
        await client.disconnect()
        print("\n✓ Test completed")
        
    except Exception as e:
        print(f"❌ Test failed: {e}")


if __name__ == "__main__":
    asyncio.run(test_streaming_capture())