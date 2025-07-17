#!/usr/bin/env python3
"""
Simple synchronous UESynth example for testing.
"""

import sys
from pathlib import Path

# Add the parent directory to sys.path to import uesynth
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    from uesynth import UESynthClient
    print("✓ UESynth imported successfully")
except ImportError as e:
    print(f"❌ Failed to import UESynth: {e}")
    sys.exit(1)


def test_connection():
    """Test basic connection to UESynth server."""
    print("\n🔌 Testing UESynth connection...")
    
    try:
        # Create client with Windows host IP for WSL compatibility
        # Use "localhost:50051" if running directly on Windows
        # Use "172.27.224.1:50051" when running from WSL to connect to Windows UE
        windows_host_ip = "172.27.224.1:50051"
        client = UESynthClient(windows_host_ip)
        print("✓ Client created")
        
        # Test basic methods (these will fail gracefully if no server)
        print("📷 Testing camera controls...")
        client.camera.set_location(x=0, y=100, z=50)
        print("✓ Camera location set")
        
        client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
        print("✓ Camera rotation set")
        
        # This will likely fail without a server, but that's fine
        print("📸 Testing image capture...")
        rgb_image = client.capture.rgb(width=512, height=512)
        
        if rgb_image is not None:
            print(f"✓ RGB capture successful - shape: {rgb_image.shape}, type: {type(rgb_image)}")
            
            # Save the captured image
            try:
                import cv2
                # Convert RGBA to BGR for OpenCV (if it's RGBA)
                if rgb_image.shape[2] == 4:
                    # Convert RGBA to RGB, then to BGR
                    rgb_only = rgb_image[:, :, :3]  # Drop alpha channel
                    bgr_image = cv2.cvtColor(rgb_only, cv2.COLOR_RGB2BGR)
                else:
                    bgr_image = cv2.cvtColor(rgb_image, cv2.COLOR_RGB2BGR)
                
                save_path = "examples/captured_rgb_sync.png"
                cv2.imwrite(save_path, bgr_image)
                print(f"✓ Image saved as {save_path}")
            except ImportError:
                # Fallback to PIL if cv2 not available
                try:
                    from PIL import Image
                    # Handle RGBA images
                    if rgb_image.shape[2] == 4:
                        img = Image.fromarray(rgb_image, 'RGBA')
                    else:
                        img = Image.fromarray(rgb_image, 'RGB')
                    save_path = "examples/captured_rgb_sync.png"
                    img.save(save_path)
                    print(f"✓ Image saved as {save_path}")
                except ImportError:
                    print("⚠ Neither cv2 nor PIL available - install with: uv add opencv-python pillow")
        else:
            print("⚠ RGB capture returned None (server not running)")
            
        client.disconnect()
        print("✓ Client disconnected")
        
    except Exception as e:
        print(f"❌ Error: {e}")
        print(f"   Type: {type(e).__name__}")
        print("\n💡 This is normal if UE isn't running with UESynth plugin")


def show_available_methods():
    """Show available methods on the client."""
    print("\n📋 Available UESynth methods:")
    
    # Use Windows host IP for WSL compatibility
    # Use "localhost:50051" if running directly on Windows
    # Use "172.27.224.1:50051" when running from WSL to connect to Windows UE
    windows_host_ip = "172.27.224.1:50051"
    client = UESynthClient(windows_host_ip)
    
    print("\n🎥 Camera methods:")
    camera_methods = [method for method in dir(client.camera) if not method.startswith('_')]
    for method in camera_methods:
        print(f"   - client.camera.{method}")
    
    print("\n📸 Capture methods:")
    capture_methods = [method for method in dir(client.capture) if not method.startswith('_')]
    for method in capture_methods:
        print(f"   - client.capture.{method}")
    
    print("\n🎮 Object methods:")
    object_methods = [method for method in dir(client.objects) if not method.startswith('_')]
    for method in object_methods:
        print(f"   - client.objects.{method}")


if __name__ == "__main__":
    print("🚀 UESynth Sync Client Test")
    show_available_methods()
    test_connection() 