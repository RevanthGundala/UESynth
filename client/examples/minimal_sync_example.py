#!/usr/bin/env python3
"""
Minimal synchronous example for UESynth.
This is the simplest way to test the UESynth client.
"""

import sys
from pathlib import Path

# Add the client directory to the path (temporary solution)
client_path = Path(__file__).parent.parent / "client"
sys.path.insert(0, str(client_path))

try:
    from uesynth import UESynthClient
    print("✓ UESynth client imported successfully")
except ImportError as e:
    print(f"✗ Failed to import UESynth: {e}")
    sys.exit(1)

try:
    import cv2
    print("✓ OpenCV imported successfully")
except ImportError as e:
    print(f"✗ Failed to import OpenCV: {e}")
    print("Install with: uv add opencv-python")
    sys.exit(1)


def main():
    """Simple synchronous UESynth example."""
    print("\nConnecting to UESynth server...")
    
    try:
        # Create client (connects to localhost:50051 by default)
        client = UESynthClient()
        print("✓ Client created")
        
        # Set camera position
        print("Setting camera position...")
        client.camera.set_location(x=0, y=100, z=50)
        client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
        print("✓ Camera positioned")
        
        # Capture RGB image
        print("Capturing RGB image...")
        rgb_image = client.capture.rgb(width=512, height=512)
        
        if rgb_image is not None:
            cv2.imwrite("test_capture.png", rgb_image)
            print("✓ Image saved as test_capture.png")
        else:
            print("⚠ No image data received")
            
        # Cleanup
        client.disconnect()
        print("✓ Disconnected successfully")
        
    except Exception as e:
        print(f"✗ Error: {e}")
        print("\nMake sure:")
        print("1. Unreal Engine is running with UESynth plugin")
        print("2. The gRPC server is listening on localhost:50051")


if __name__ == "__main__":
    main() 