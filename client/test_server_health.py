#!/usr/bin/env python3
"""
UESynth Server Health Check Script
Tests server connectivity and readiness before attempting captures.
"""

import asyncio
import sys
from pathlib import Path

# Add parent directory to path for import
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import AsyncUESynthClient


class ServerHealthChecker:
    def __init__(self, server_address="172.27.224.1:50051"):
        self.server_address = server_address
        self.client = None
        
    async def check_connection(self):
        """Test basic gRPC connection."""
        print("🔌 Testing gRPC connection...")
        try:
            self.client = AsyncUESynthClient(self.server_address)
            await self.client.connect()
            print("✓ gRPC connection successful")
            return True
        except Exception as e:
            print(f"❌ Connection failed: {e}")
            return False
    
    async def check_camera_operations(self):
        """Test camera positioning (doesn't require viewport)."""
        print("\n📷 Testing camera operations...")
        try:
            # Test camera positioning
            pos_id = await self.client.camera.set_location(0, 0, 100)
            rot_id = await self.client.camera.set_rotation(0, 0, 0)
            print(f"✓ Camera position set: {pos_id}")
            print(f"✓ Camera rotation set: {rot_id}")
            
            # Test getting camera transform
            location = await self.client.camera.get_location()
            print(f"✓ Camera location retrieved: {location}")
            return True
        except Exception as e:
            print(f"❌ Camera operations failed: {e}")
            return False
    
    async def check_capture_readiness(self):
        """Test if server is ready for image capture."""
        print("\n🎯 Testing capture readiness...")
        
        # Test streaming capture (safer, doesn't fail immediately)
        try:
            request_id = await self.client.capture.rgb(width=64, height=64)
            print(f"✓ Streaming capture request sent: {request_id}")
            
            # Wait briefly for processing
            await asyncio.sleep(0.5)
            
            # Try to get frame data
            frame_data = await self.client.get_latest_frame()
            if frame_data is not None:
                print(f"✓ Frame data available: {frame_data.shape}")
                return True
            else:
                print("⚠ No frame data available - UE may not be in Play mode")
                return False
        except Exception as e:
            print(f"❌ Streaming capture failed: {e}")
            return False
    
    async def test_direct_capture(self):
        """Test direct capture (will fail if viewport not ready)."""
        print("\n🎯 Testing direct capture...")
        try:
            rgb_image = await self.client.capture.rgb_direct(width=64, height=64)
            if rgb_image is not None:
                print(f"✓ Direct capture successful: {rgb_image.shape}")
                return True
            else:
                print("❌ Direct capture returned None")
                return False
        except Exception as e:
            print(f"❌ Direct capture failed: {e}")
            print("   This is expected if UE isn't in Play mode with active viewport")
            return False
    
    async def run_full_health_check(self):
        """Run complete server health check."""
        print("🚀 UESynth Server Health Check")
        print("=" * 50)
        print(f"Server: {self.server_address}")
        
        results = {
            'connection': False,
            'camera_ops': False,
            'capture_ready': False,
            'direct_capture': False
        }
        
        # Test connection
        results['connection'] = await self.check_connection()
        if not results['connection']:
            print("\n❌ Cannot proceed - no connection to server")
            return results
        
        # Test camera operations
        results['camera_ops'] = await self.check_camera_operations()
        
        # Test capture readiness
        results['capture_ready'] = await self.check_capture_readiness()
        
        # Test direct capture
        results['direct_capture'] = await self.test_direct_capture()
        
        # Cleanup
        if self.client:
            await self.client.disconnect()
        
        # Summary
        print("\n📋 Health Check Summary:")
        print("=" * 30)
        print(f"🔌 Connection:      {'✓' if results['connection'] else '❌'}")
        print(f"📷 Camera Ops:      {'✓' if results['camera_ops'] else '❌'}")
        print(f"🎯 Capture Ready:   {'✓' if results['capture_ready'] else '❌'}")
        print(f"📸 Direct Capture:  {'✓' if results['direct_capture'] else '❌'}")
        
        # Recommendations
        print("\n💡 Recommendations:")
        if not results['connection']:
            print("   • Start Unreal Engine with UESynth plugin")
            print("   • Ensure gRPC server is running on port 50051")
        elif not results['camera_ops']:
            print("   • Check UE world/level is loaded")
            print("   • Verify camera actor exists in scene")
        elif not results['capture_ready']:
            print("   • Enter Play mode (PIE) in Unreal Engine")
            print("   • Ensure game viewport is visible and active")
        elif results['capture_ready'] and not results['direct_capture']:
            print("   • Direct capture may need additional viewport setup")
            print("   • Use streaming capture methods for now")
        else:
            print("   • All systems ready! 🎉")
        
        return results


async def main():
    """Main health check function."""
    # Allow custom server address
    server_address = "172.27.224.1:50051"  # Default for WSL
    if len(sys.argv) > 1:
        server_address = sys.argv[1]
    
    checker = ServerHealthChecker(server_address)
    results = await checker.run_full_health_check()
    
    # Exit code based on results
    if results['connection'] and results['camera_ops']:
        sys.exit(0)  # Basic functionality working
    else:
        sys.exit(1)  # Critical issues


if __name__ == "__main__":
    asyncio.run(main())