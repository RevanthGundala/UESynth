#!/usr/bin/env python3
"""Test creating cameras explicitly in UESynth."""

import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import UESynthClient

def test_camera_creation():
    """Test creating and using explicit cameras."""
    windows_host_ip = "172.27.224.1:50051"
    
    print("🎥 Testing Camera Creation")
    print("=" * 30)
    
    try:
        client = UESynthClient(windows_host_ip)
        print("✓ Client connected")
        
        # Create a new camera
        print("\n📷 Creating new camera...")
        try:
            response = client.camera.create(
                camera_name="TestCamera",
                x=0, y=0, z=200,
                pitch=-45, yaw=0, roll=0
            )
            print(f"✓ Camera creation response: {response}")
        except Exception as e:
            print(f"❌ Camera creation failed: {e}")
        
        # Try capturing with the named camera
        print("\n📸 Capturing with named camera...")
        try:
            rgb_image = client.capture.rgb(camera_name="TestCamera", width=512, height=512)
            print(f"✓ Named camera capture shape: {rgb_image.shape}")
            
            # Check pixel values
            import numpy as np
            unique_values = np.unique(rgb_image)
            print(f"✓ Unique pixel values: {len(unique_values)}")
            print(f"✓ Min: {unique_values.min()}, Max: {unique_values.max()}")
            
            # Save image
            try:
                import cv2
                if rgb_image.shape[2] == 4:
                    rgb_only = rgb_image[:, :, :3]
                    bgr_image = cv2.cvtColor(rgb_only, cv2.COLOR_RGB2BGR)
                else:
                    bgr_image = cv2.cvtColor(rgb_image, cv2.COLOR_RGB2BGR)
                
                cv2.imwrite("test_named_camera.png", bgr_image)
                print("✓ Saved as test_named_camera.png")
            except ImportError:
                print("⚠ OpenCV not available")
                
        except Exception as e:
            print(f"❌ Named camera capture failed: {e}")
        
        client.disconnect()
        print("\n✓ Test complete")
        
    except Exception as e:
        print(f"❌ Connection failed: {e}")

if __name__ == "__main__":
    test_camera_creation()