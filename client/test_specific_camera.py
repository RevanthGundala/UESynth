#!/usr/bin/env python3
"""
Test specific camera positions and PIE states.
"""

import asyncio
import sys
from pathlib import Path

# Add parent directory to path for import
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import AsyncUESynthClient


async def test_specific_scenarios():
    """Test specific scenarios that might work better."""
    print("🎯 Testing Specific PIE Scenarios")
    print("=" * 40)
    
    client = AsyncUESynthClient("172.27.224.1:50051")
    
    try:
        await client.connect()
        print("✓ Connected to server")
        
        # Test 1: Very simple camera position
        print("\n📷 Test 1: Simple camera position (0,0,200)")
        await client.camera.set_location(0, 0, 200)
        await client.camera.set_rotation(0, 0, 0)
        
        # Wait longer for camera to settle
        await asyncio.sleep(2.0)
        
        # Try very small image first
        print("📸 Trying 32x32 image...")
        request_id = await client.capture.rgb(width=32, height=32)
        print(f"✓ Request sent: {request_id}")
        
        await asyncio.sleep(3.0)  # Wait longer
        
        frame_data = await client.get_latest_frame()
        if frame_data is not None:
            print(f"🎉 SUCCESS! Got {frame_data.shape} frame")
            
            # Save it
            try:
                from PIL import Image
                if frame_data.shape[2] == 4:  # RGBA
                    img = Image.fromarray(frame_data, 'RGBA')
                else:  # RGB
                    img = Image.fromarray(frame_data, 'RGB')
                save_path = "test_32x32_success.png"
                img.save(save_path)
                print(f"✓ Saved as {save_path}")
            except Exception as e:
                print(f"⚠ Could not save: {e}")
        else:
            print("❌ No frame data for 32x32")
        
        # Test 2: Different camera angle
        print("\n📷 Test 2: Angled camera (-45 degrees)")
        await client.camera.set_location(100, 100, 150)
        await client.camera.set_rotation(-45, 45, 0)
        
        await asyncio.sleep(2.0)
        
        print("📸 Trying 64x64 image...")
        request_id = await client.capture.rgb(width=64, height=64)
        print(f"✓ Request sent: {request_id}")
        
        await asyncio.sleep(3.0)
        
        frame_data = await client.get_latest_frame()
        if frame_data is not None:
            print(f"🎉 SUCCESS! Got {frame_data.shape} frame")
            
            try:
                from PIL import Image
                if frame_data.shape[2] == 4:  # RGBA
                    img = Image.fromarray(frame_data, 'RGBA')
                else:  # RGB
                    img = Image.fromarray(frame_data, 'RGB')
                save_path = "test_64x64_angled.png"
                img.save(save_path)
                print(f"✓ Saved as {save_path}")
            except Exception as e:
                print(f"⚠ Could not save: {e}")
        else:
            print("❌ No frame data for angled camera")
        
        # Test 3: Try direct capture with small size
        print("\n📷 Test 3: Direct capture 32x32")
        await client.camera.set_location(0, 0, 100)
        await client.camera.set_rotation(0, 0, 0)
        
        await asyncio.sleep(2.0)
        
        try:
            rgb_image = await client.capture.rgb_direct(width=32, height=32)
            if rgb_image is not None:
                print(f"🎉 DIRECT SUCCESS! Got {rgb_image.shape}")
                
                try:
                    from PIL import Image
                    if rgb_image.shape[2] == 4:  # RGBA
                        img = Image.fromarray(rgb_image, 'RGBA')
                    else:  # RGB
                        img = Image.fromarray(rgb_image, 'RGB')
                    save_path = "test_direct_32x32.png"
                    img.save(save_path)
                    print(f"✓ Saved direct capture as {save_path}")
                except Exception as e:
                    print(f"⚠ Could not save direct: {e}")
            else:
                print("❌ Direct capture returned None")
        except Exception as e:
            print(f"❌ Direct capture failed: {e}")
        
    except Exception as e:
        print(f"❌ Test failed: {e}")
    finally:
        await client.disconnect()
        print("\n✓ Test completed")


if __name__ == "__main__":
    asyncio.run(test_specific_scenarios())