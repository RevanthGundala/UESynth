#!/usr/bin/env python3
"""Test async connection to UESynth server with Windows host IP."""

import asyncio
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import AsyncUESynthClient

async def test_async_connection():
    """Test async connection to UESynth server running on Windows host."""
    windows_host_ip = "172.27.224.1:50051"
    
    print(f"🚀 Testing async connection to Windows host: {windows_host_ip}")
    
    try:
        client = AsyncUESynthClient(windows_host_ip)
        print("✓ Async client created")
        
        print("🔌 Connecting...")
        await client.connect()
        print("✓ Connected successfully!")
        
        # Test camera operations
        print("📷 Setting camera position...")
        pos_id = await client.camera.set_location(0, 100, 50)
        rot_id = await client.camera.set_rotation(-15, 0, 0)
        print(f"✓ Camera requests sent: pos={pos_id}, rot={rot_id}")
        
        # Test direct capture
        print("📸 Capturing RGB image (direct)...")
        rgb_image = await client.capture.rgb_direct(width=512, height=512)
        print(f"✅ Direct capture successful! Shape: {rgb_image.shape}")
        
        # Test streaming capture
        print("🌊 Testing streaming capture...")
        request_id = await client.capture.rgb(width=512, height=512)
        print(f"✓ Streaming request sent: {request_id}")
        
        # Wait and get frame
        await asyncio.sleep(0.5)
        frame = await client.get_latest_frame()
        if frame is not None:
            print(f"✓ Streaming frame received: {frame.shape}")
        else:
            print("⚠ No streaming frame available")
        
        await client.disconnect()
        print("✅ All async tests passed!")
        
    except Exception as e:
        print(f"❌ Async connection failed: {e}")
        print(f"   Error type: {type(e).__name__}")

if __name__ == "__main__":
    asyncio.run(test_async_connection())