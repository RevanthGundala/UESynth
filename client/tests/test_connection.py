#!/usr/bin/env python3
"""Test connection to UESynth server with Windows host IP."""

import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import UESynthClient

def test_windows_host_connection():
    """Test connection to UESynth server running on Windows host."""
    windows_host_ip = "172.27.224.1"
    
    print(f"🔌 Testing connection to Windows host: {windows_host_ip}:50051")
    
    try:
        client = UESynthClient(f"{windows_host_ip}:50051")
        print("✓ Client created")
        
        # Test a simple camera operation
        print("📷 Testing camera set location...")
        response = client.camera.set_location(0, 100, 50)
        print(f"✅ Camera operation successful! Response: {response}")
        
        # Test image capture
        print("📸 Testing RGB capture...")
        rgb_image = client.capture.rgb(width=512, height=512)
        print(f"✅ RGB capture successful! Shape: {rgb_image.shape}")
        
        client.disconnect()
        print("✅ All tests passed!")
        
    except Exception as e:
        print(f"❌ Connection failed: {e}")
        print(f"   Error type: {type(e).__name__}")
        
        # Try localhost as fallback
        print("\n🔄 Trying localhost as fallback...")
        try:
            client = UESynthClient("localhost:50051")
            response = client.camera.set_location(0, 100, 50)
            print("✅ Localhost connection worked!")
            client.disconnect()
        except Exception as e2:
            print(f"❌ Localhost also failed: {e2}")

if __name__ == "__main__":
    test_windows_host_connection()