#!/usr/bin/env python3
"""
Quick UESynth Server Status Check
Fast check to see if the server is running and ready.
"""

import asyncio
import sys
from pathlib import Path

# Add current directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

from test_server_health import ServerHealthChecker


async def quick_check():
    """Quick server status check."""
    print("⚡ Quick UESynth Server Check")
    print("=" * 30)
    
    # Allow custom server address
    server_address = "172.27.224.1:50051"  # Default for WSL
    if len(sys.argv) > 1:
        if sys.argv[1] == "--help":
            print("Usage: python check_server.py [server_address]")
            print("Default: 172.27.224.1:50051 (WSL)")
            print("Local:   localhost:50051")
            return 0
        else:
            server_address = sys.argv[1]
    
    print(f"Server: {server_address}")
    
    checker = ServerHealthChecker(server_address)
    
    # Just test connection and basic functionality
    print("\n🔌 Testing connection...")
    if not await checker.check_connection():
        print("\n❌ Server not reachable")
        print("💡 Make sure:")
        print("   • Unreal Engine is running")
        print("   • UESynth plugin is loaded")
        print("   • gRPC server is active on port 50051")
        return 1
    
    print("\n📷 Testing camera operations...")
    camera_ok = await checker.check_camera_operations()
    
    print("\n🎯 Testing capture readiness...")
    capture_ok = await checker.check_capture_readiness()
    
    # Cleanup
    if checker.client:
        await checker.client.disconnect()
    
    # Quick summary
    print(f"\n📋 Quick Status:")
    print(f"   Connection:  {'✓' if True else '❌'}")
    print(f"   Camera Ops:  {'✓' if camera_ok else '❌'}")
    print(f"   Capture:     {'✓' if capture_ok else '❌'}")
    
    if camera_ok and capture_ok:
        print("\n🎉 Server is ready for all operations!")
        return 0
    elif camera_ok:
        print("\n💡 Server ready for camera ops, enter Play mode for capture")
        return 0
    else:
        print("\n⚠ Server needs setup - load a level and enter Play mode")
        return 1


if __name__ == "__main__":
    exit_code = asyncio.run(quick_check())
    sys.exit(exit_code)