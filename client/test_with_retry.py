#!/usr/bin/env python3
"""
Test capture with retry logic to handle intermittent viewport issues.
"""

import asyncio
import sys
from pathlib import Path

# Add parent directory to path for import
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import AsyncUESynthClient


async def capture_with_retry(client, width=64, height=64, max_retries=5, delay=0.5):
    """Attempt capture with retry logic."""
    for attempt in range(max_retries):
        try:
            print(f"  Attempt {attempt + 1}/{max_retries}...")
            
            # Send capture request
            request_id = await client.capture.rgb(width=width, height=height)
            print(f"    ✓ Request sent: {request_id}")
            
            # Wait for processing
            await asyncio.sleep(delay)
            
            # Try to get frame
            frame_data = await client.get_latest_frame()
            
            if frame_data is not None:
                print(f"    ✓ SUCCESS on attempt {attempt + 1}: {frame_data.shape}")
                return frame_data
            else:
                print(f"    ❌ No frame data on attempt {attempt + 1}")
                
        except Exception as e:
            print(f"    ❌ Attempt {attempt + 1} failed: {e}")
        
        # Wait before retry (except on last attempt)
        if attempt < max_retries - 1:
            await asyncio.sleep(delay)
    
    print(f"  ❌ All {max_retries} attempts failed")
    return None


async def direct_capture_with_retry(client, width=64, height=64, max_retries=5, delay=0.5):
    """Attempt direct capture with retry logic."""
    for attempt in range(max_retries):
        try:
            print(f"  Direct attempt {attempt + 1}/{max_retries}...")
            
            # Wait a bit before each attempt
            await asyncio.sleep(delay)
            
            # Try direct capture
            rgb_image = await client.capture.rgb_direct(width=width, height=height)
            
            if rgb_image is not None:
                print(f"    ✓ DIRECT SUCCESS on attempt {attempt + 1}: {rgb_image.shape}")
                return rgb_image
            else:
                print(f"    ❌ Direct capture returned None on attempt {attempt + 1}")
                
        except Exception as e:
            print(f"    ❌ Direct attempt {attempt + 1} failed: {e}")
        
        # Wait before retry (except on last attempt)
        if attempt < max_retries - 1:
            await asyncio.sleep(delay)
    
    print(f"  ❌ All {max_retries} direct attempts failed")
    return None


async def test_robust_capture():
    """Test capture with robust retry logic."""
    print("🔄 Testing Robust Capture with Retries")
    print("=" * 45)
    
    client = AsyncUESynthClient("172.27.224.1:50051")
    
    try:
        await client.connect()
        print("✓ Connected to server")
        
        # Set camera position
        await client.camera.set_location(0, 0, 100)
        await client.camera.set_rotation(0, 0, 0)
        print("✓ Camera positioned")
        
        # Test streaming capture with retries
        print("\n📸 Testing Streaming Capture with Retries:")
        frame_data = await capture_with_retry(client, width=64, height=64, max_retries=10, delay=0.3)
        
        if frame_data is not None:
            print("🎉 Streaming capture succeeded!")
            
            # Save the successful capture
            try:
                from PIL import Image
                if frame_data.shape[2] == 4:  # RGBA
                    img = Image.fromarray(frame_data, 'RGBA')
                else:  # RGB
                    img = Image.fromarray(frame_data, 'RGB')
                save_path = "robust_streaming_capture.png"
                img.save(save_path)
                print(f"✓ Saved as {save_path}")
            except Exception as e:
                print(f"⚠ Could not save: {e}")
        else:
            print("❌ Streaming capture failed after all retries")
        
        # Test direct capture with retries
        print("\n📸 Testing Direct Capture with Retries:")
        direct_data = await direct_capture_with_retry(client, width=64, height=64, max_retries=10, delay=0.5)
        
        if direct_data is not None:
            print("🎉 Direct capture succeeded!")
            
            # Save the successful direct capture
            try:
                from PIL import Image
                if direct_data.shape[2] == 4:  # RGBA
                    img = Image.fromarray(direct_data, 'RGBA')
                else:  # RGB
                    img = Image.fromarray(direct_data, 'RGB')
                save_path = "robust_direct_capture.png"
                img.save(save_path)
                print(f"✓ Saved as {save_path}")
            except Exception as e:
                print(f"⚠ Could not save: {e}")
        else:
            print("❌ Direct capture failed after all retries")
        
        # Test larger image if smaller one worked
        if frame_data is not None or direct_data is not None:
            print("\n📸 Testing Larger Image (256x256):")
            large_frame = await capture_with_retry(client, width=256, height=256, max_retries=5, delay=1.0)
            
            if large_frame is not None:
                print("🎉 Large image capture succeeded!")
                try:
                    from PIL import Image
                    if large_frame.shape[2] == 4:  # RGBA
                        img = Image.fromarray(large_frame, 'RGBA')
                    else:  # RGB
                        img = Image.fromarray(large_frame, 'RGB')
                    save_path = "robust_large_capture.png"
                    img.save(save_path)
                    print(f"✓ Saved large image as {save_path}")
                except Exception as e:
                    print(f"⚠ Could not save large image: {e}")
        
    except Exception as e:
        print(f"❌ Test failed: {e}")
    finally:
        await client.disconnect()
        print("\n✓ Test completed")


if __name__ == "__main__":
    asyncio.run(test_robust_capture())