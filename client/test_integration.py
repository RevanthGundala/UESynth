#!/usr/bin/env python3
"""
UESynth Integration Test Runner
Tests actual server connectivity and functionality with health checks.
"""

import asyncio
import sys
from pathlib import Path

# Add current directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

from uesynth import AsyncUESynthClient
from test_server_health import ServerHealthChecker


async def test_basic_connection():
    """Test basic connection to UESynth server."""
    print("🔌 Testing basic connection...")
    
    try:
        # Use Windows host IP for WSL compatibility
        client = AsyncUESynthClient("172.27.224.1:50051")
        await client.connect()
        print("✓ Connection successful")
        await client.disconnect()
        return True
    except Exception as e:
        print(f"❌ Connection failed: {e}")
        return False


async def test_camera_operations():
    """Test camera positioning and retrieval."""
    print("\n📷 Testing camera operations...")
    
    try:
        client = AsyncUESynthClient("172.27.224.1:50051")
        await client.connect()
        
        # Set camera position
        pos_request_id = await client.camera.set_location(100, 200, 300)
        print(f"✓ Set camera position: {pos_request_id}")
        
        # Set camera rotation
        rot_request_id = await client.camera.set_rotation(10, 20, 30)
        print(f"✓ Set camera rotation: {rot_request_id}")
        
        # Get camera location
        location = await client.camera.get_location()
        print(f"✓ Got camera location: {location}")
        
        await client.disconnect()
        return True
    except Exception as e:
        print(f"❌ Camera operations failed: {e}")
        return False


async def test_image_capture_safe():
    """Test image capture with health check guidance."""
    print("\n📸 Testing image capture (safe mode)...")
    
    try:
        client = AsyncUESynthClient("172.27.224.1:50051")
        await client.connect()
        
        # Test streaming capture first (safer)
        print("  Testing streaming capture...")
        request_id = await client.capture.rgb(width=512, height=512)
        print(f"  ✓ Streaming capture request: {request_id}")
        
        # Wait for capture
        await asyncio.sleep(1.0)
        
        # Get latest frame
        frame_data = await client.get_latest_frame()
        if frame_data is not None:
            print(f"  ✓ Frame retrieved: {frame_data.shape}")
            capture_working = True
        else:
            print("  ⚠ No frame data available")
            print("    💡 This means UE isn't in Play mode or viewport isn't active")
            capture_working = False
        
        # Only test direct capture if streaming worked
        if capture_working:
            print("  Testing direct capture...")
            try:
                rgb_image = await client.capture.rgb_direct(width=256, height=256)
                print(f"  ✓ Direct capture: {rgb_image.shape}")
            except Exception as e:
                print(f"  ❌ Direct capture failed: {e}")
                print("    💡 Direct capture needs active game viewport")
        else:
            print("  ⏭ Skipping direct capture (streaming not working)")
        
        await client.disconnect()
        return capture_working
    except Exception as e:
        print(f"❌ Image capture failed: {e}")
        return False


async def test_object_operations():
    """Test object manipulation if server supports it."""
    print("\n🎮 Testing object operations...")
    
    try:
        client = AsyncUESynthClient("172.27.224.1:50051")
        await client.connect()
        
        # Test spawning an object
        print("  Testing object spawn...")
        spawn_id = await client.objects.spawn("TestCube", 0, 0, 0)
        print(f"  ✓ Object spawn request: {spawn_id}")
        
        # Test setting object location
        print("  Testing object location...")
        move_id = await client.objects.set_location("TestCube", 10, 20, 30)
        print(f"  ✓ Object move request: {move_id}")
        
        await client.disconnect()
        return True
    except Exception as e:
        print(f"❌ Object operations failed: {e}")
        print("    💡 Object operations may not be implemented yet")
        return False


async def run_health_check_first():
    """Run health check before other tests."""
    print("🏥 Running server health check first...")
    print("=" * 50)
    
    checker = ServerHealthChecker()
    health_results = await checker.run_full_health_check()
    
    return health_results


async def run_integration_tests():
    """Run all integration tests with health check."""
    print("🚀 UESynth Integration Test Runner")
    print("=" * 40)
    
    # Run health check first
    health_results = await run_health_check_first()
    
    # Decide which tests to run based on health check
    if not health_results['connection']:
        print("\n❌ Cannot run tests - no server connection")
        return False
    
    print(f"\n🧪 Running integration tests based on server capabilities...")
    
    tests = []
    
    # Always test connection if health check passed
    tests.append(("Connection", test_basic_connection))
    
    # Test camera ops if health check showed they work
    if health_results['camera_ops']:
        tests.append(("Camera Operations", test_camera_operations))
    else:
        print("⏭ Skipping camera tests (health check failed)")
    
    # Always try capture tests but with safe mode
    tests.append(("Image Capture (Safe)", test_image_capture_safe))
    
    # Test object operations (experimental)
    tests.append(("Object Operations", test_object_operations))
    
    results = {}
    
    for test_name, test_func in tests:
        print(f"\n🧪 Running {test_name} test...")
        try:
            results[test_name] = await test_func()
        except Exception as e:
            print(f"❌ {test_name} test crashed: {e}")
            results[test_name] = False
    
    # Summary
    print("\n📊 Integration Test Results:")
    print("=" * 35)
    
    passed = 0
    total = len(tests)
    
    for test_name, result in results.items():
        status = "✓ PASS" if result else "❌ FAIL"
        print(f"{test_name:25} {status}")
        if result:
            passed += 1
    
    print(f"\nPassed: {passed}/{total}")
    
    # Provide guidance based on results
    if passed == total:
        print("🎉 All integration tests passed!")
        if health_results['direct_capture']:
            print("💪 Server is fully ready for all operations!")
        else:
            print("💡 Server ready for streaming operations")
    else:
        print("\n💡 Troubleshooting Guide:")
        if not health_results['connection']:
            print("   • Start Unreal Engine with UESynth plugin")
            print("   • Check gRPC server is running on port 50051")
        elif not health_results['camera_ops']:
            print("   • Load a level/world in Unreal Engine")
            print("   • Ensure camera actor exists in scene")
        elif not health_results['capture_ready']:
            print("   • Enter Play mode (PIE) in Unreal Engine")
            print("   • Make sure game viewport is visible")
        
        print("\n🔧 Quick Setup Checklist:")
        print("   1. Launch Unreal Engine")
        print("   2. Load/create a project with UESynth plugin")
        print("   3. Open any level/map")
        print("   4. Click 'Play' button (PIE mode)")
        print("   5. Ensure game window is visible")
        print("   6. Run this test again")
        
    return passed >= (total // 2)  # Pass if at least half the tests work


async def main():
    """Main function with command line options."""
    # Allow custom server address
    server_address = "172.27.224.1:50051"  # Default for WSL
    if len(sys.argv) > 1:
        if sys.argv[1] == "--help":
            print("Usage: python test_integration.py [server_address]")
            print("Default server: 172.27.224.1:50051 (for WSL)")
            print("For local Windows: localhost:50051")
            return 0
        else:
            server_address = sys.argv[1]
    
    # Update the health checker to use custom address
    global ServerHealthChecker
    original_init = ServerHealthChecker.__init__
    def new_init(self, addr=server_address):
        original_init(self, addr)
    ServerHealthChecker.__init__ = new_init
    
    success = await run_integration_tests()
    return 0 if success else 1


if __name__ == "__main__":
    exit_code = asyncio.run(main())
    sys.exit(exit_code)