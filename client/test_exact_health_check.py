#!/usr/bin/env python3
"""
Test that exactly mimics the health check streaming capture.
"""

import asyncio
import sys
from pathlib import Path

# Add parent directory to path for import
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import AsyncUESynthClient


async def mimic_health_check():
    """Exactly mimic what the health check does for streaming capture."""
    print("🔍 Mimicking Health Check Streaming Capture")
    print("=" * 50)
    
    client = AsyncUESynthClient("172.27.224.1:50051")
    
    try:
        # Connect (same as health check)
        await client.connect()
        print("✓ Connected")
        
        # Test streaming capture (exact same as health check)
        print("📸 Sending RGB capture request (streaming)...")
        request_id = await client.capture.rgb(width=64, height=64)
        print(f"✓ Streaming capture request sent: {request_id}")
        
        # Wait briefly for processing (same as health check)
        await asyncio.sleep(0.5)
        
        # Try to get frame data (same as health check)
        print("🔍 Retrieving latest frame...")
        frame_data = await client.get_latest_frame()
        
        if frame_data is not None:
            print(f"✓ Frame data available: {frame_data.shape}")
            
            # Try to save it
            try:
                from PIL import Image
                if frame_data.shape[2] == 4:  # RGBA
                    img = Image.fromarray(frame_data, 'RGBA')
                else:  # RGB
                    img = Image.fromarray(frame_data, 'RGB')
                save_path = "health_check_mimic.png"
                img.save(save_path)
                print(f"✓ Saved frame as {save_path}")
            except ImportError:
                print("⚠ PIL not available")
            except Exception as e:
                print(f"⚠ Could not save: {e}")
                
            return True
        else:
            print("❌ No frame data available")
            return False
            
    except Exception as e:
        print(f"❌ Test failed: {e}")
        return False
    finally:
        if client:
            await client.disconnect()
            print("✓ Disconnected")


async def test_multiple_attempts():
    """Test multiple streaming attempts to see if it's consistent."""
    print("\n🔄 Testing Multiple Streaming Attempts")
    print("=" * 40)
    
    success_count = 0
    total_attempts = 3
    
    for i in range(total_attempts):
        print(f"\n--- Attempt {i+1}/{total_attempts} ---")
        success = await mimic_health_check()
        if success:
            success_count += 1
        
        # Wait between attempts
        if i < total_attempts - 1:
            await asyncio.sleep(1.0)
    
    print(f"\n📊 Results: {success_count}/{total_attempts} successful")
    
    if success_count > 0:
        print("✓ Streaming capture works sometimes!")
        print("💡 This suggests the server state affects capture availability")
    else:
        print("❌ Streaming capture not working consistently")
        print("💡 Server might need specific setup for streaming")


if __name__ == "__main__":
    asyncio.run(test_multiple_attempts())