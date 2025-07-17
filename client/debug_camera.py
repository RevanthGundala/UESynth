#!/usr/bin/env python3
"""
Debug script to test camera and capture functionality step by step.
"""

import asyncio
import sys
from pathlib import Path

# Add parent directory to path for import
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import AsyncUESynthClient


async def debug_capture_pipeline():
    """Debug the capture pipeline step by step."""
    print("🔍 UESynth Capture Pipeline Debug")
    print("=" * 40)
    
    client = AsyncUESynthClient("172.27.224.1:50051")
    
    try:
        await client.connect()
        print("✓ Connected to server")
        
        # Test multiple camera positions to see if that affects capture
        positions = [
            (0, 0, 100),
            (100, 100, 200),
            (0, 0, 500),
            (-100, -100, 150)
        ]
        
        for i, (x, y, z) in enumerate(positions):
            print(f"\n--- Test {i+1}: Camera at ({x}, {y}, {z}) ---")
            
            # Set camera position
            await client.camera.set_location(x, y, z)
            await client.camera.set_rotation(0, 0, 0)
            print(f"✓ Camera positioned at ({x}, {y}, {z})")
            
            # Wait for camera to settle
            await asyncio.sleep(0.5)
            
            # Try streaming capture with different sizes
            sizes = [(64, 64), (128, 128), (256, 256)]
            
            for width, height in sizes:
                print(f"  📸 Testing {width}x{height} capture...")
                
                try:
                    # Send capture request
                    request_id = await client.capture.rgb(width=width, height=height)
                    print(f"    ✓ Request sent: {request_id}")
                    
                    # Wait for processing
                    await asyncio.sleep(1.0)
                    
                    # Try to get frame
                    frame_data = await client.get_latest_frame()
                    
                    if frame_data is not None:
                        print(f"    ✓ SUCCESS: Got {frame_data.shape} frame")
                        
                        # Save successful capture
                        try:
                            from PIL import Image
                            if frame_data.shape[2] == 4:  # RGBA
                                img = Image.fromarray(frame_data, 'RGBA')
                            else:  # RGB
                                img = Image.fromarray(frame_data, 'RGB')
                            save_path = f"debug_position_{i+1}_{width}x{height}.png"
                            img.save(save_path)
                            print(f"    ✓ Saved as {save_path}")
                        except Exception as e:
                            print(f"    ⚠ Could not save: {e}")
                    else:
                        print(f"    ❌ No frame data")
                        
                except Exception as e:
                    print(f"    ❌ Capture failed: {e}")
                
                # Small delay between size tests
                await asyncio.sleep(0.5)
        
        # Test direct capture at different positions
        print(f"\n--- Testing Direct Capture ---")
        for i, (x, y, z) in enumerate(positions[:2]):  # Just test first 2 positions
            print(f"\nDirect capture test {i+1}: Camera at ({x}, {y}, {z})")
            
            await client.camera.set_location(x, y, z)
            await asyncio.sleep(0.5)
            
            try:
                rgb_image = await client.capture.rgb_direct(width=128, height=128)
                if rgb_image is not None:
                    print(f"✓ Direct capture SUCCESS: {rgb_image.shape}")
                    
                    # Save direct capture
                    try:
                        from PIL import Image
                        if rgb_image.shape[2] == 4:  # RGBA
                            img = Image.fromarray(rgb_image, 'RGBA')
                        else:  # RGB
                            img = Image.fromarray(rgb_image, 'RGB')
                        save_path = f"debug_direct_{i+1}.png"
                        img.save(save_path)
                        print(f"✓ Saved direct capture as {save_path}")
                    except Exception as e:
                        print(f"⚠ Could not save direct: {e}")
                else:
                    print("❌ Direct capture returned None")
            except Exception as e:
                print(f"❌ Direct capture failed: {e}")
        
    except Exception as e:
        print(f"❌ Debug failed: {e}")
    finally:
        await client.disconnect()
        print("\n✓ Debug completed")


if __name__ == "__main__":
    asyncio.run(debug_capture_pipeline())