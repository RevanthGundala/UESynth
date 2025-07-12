#!/usr/bin/env python3
"""
Example demonstrating the new bidirectional streaming functionality of UESynth.

This example shows how to use the UESynthStreamingClient for high-performance
simulation scenarios requiring thousands of timesteps.
"""

import sys
import time
import cv2
import numpy as np
from pathlib import Path

# Add the parent directory to the path so we can import uesynth
sys.path.insert(0, str(Path(__file__).parent.parent / "client"))

from uesynth import UESynthStreamingClient


def basic_streaming_example():
    """Basic example of streaming API usage."""
    print("=== Basic Streaming Example ===")
    
    # Connect to the Unreal Engine instance
    client = UESynthStreamingClient()
    
    print("Connected to UESynth streaming server")
    
    # Set initial camera position
    client.camera.set_location(x=0, y=100, z=50)
    client.camera.set_rotation(pitch=-15, yaw=0, roll=0)
    
    # Capture a few frames
    for i in range(5):
        print(f"Capturing frame {i+1}/5...")
        
        # Capture RGB image (non-blocking)
        client.capture.rgb()
        
        # Move camera slightly
        client.camera.set_location(x=i * 20, y=100, z=50)
        
        # Wait a bit for the response
        time.sleep(0.5)
        
        # Get the latest frame
        frame = client.get_latest_frame()
        if frame is not None:
            print(f"  Received frame: {frame.shape}")
            cv2.imwrite(f"streaming_frame_{i:02d}.png", frame)
        else:
            print("  No frame received yet")
    
    # Disconnect
    client.disconnect()
    print("Disconnected from server")


def high_frequency_simulation():
    """Example of high-frequency simulation loop."""
    print("\n=== High-Frequency Simulation Example ===")
    
    # Connect to the Unreal Engine instance
    client = UESynthStreamingClient()
    
    print("Starting high-frequency simulation...")
    
    # Parameters for circular camera movement
    radius = 200
    height = 100
    num_steps = 100
    
    start_time = time.time()
    
    for step in range(num_steps):
        # Calculate circular path
        angle = 2 * np.pi * step / num_steps
        x = radius * np.cos(angle)
        y = radius * np.sin(angle)
        z = height
        
        # Move camera (non-blocking)
        client.camera.set_location(x=x, y=y, z=z)
        
        # Point camera toward center (non-blocking)
        yaw = np.degrees(angle + np.pi)  # Point inward
        client.camera.set_rotation(pitch=-10, yaw=yaw, roll=0)
        
        # Capture frame (non-blocking)
        client.capture.rgb()
        
        # Optional: Move an object
        if step % 10 == 0:
            obj_x = 50 * np.cos(angle * 2)
            obj_y = 50 * np.sin(angle * 2)
            client.objects.set_location("MyObject", x=obj_x, y=obj_y, z=0)
        
        # Small delay to prevent overwhelming the system
        time.sleep(0.01)
        
        # Periodically save frames
        if step % 20 == 0:
            frame = client.get_latest_frame()
            if frame is not None:
                cv2.imwrite(f"simulation_step_{step:04d}.png", frame)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"Completed {num_steps} steps in {elapsed:.2f} seconds")
    print(f"Average rate: {num_steps / elapsed:.1f} commands/second")
    
    # Get final frame
    final_frame = client.get_latest_frame()
    if final_frame is not None:
        cv2.imwrite("final_simulation_frame.png", final_frame)
        print(f"Final frame saved: {final_frame.shape}")
    
    # Disconnect
    client.disconnect()
    print("Simulation completed")


def callback_based_example():
    """Example using callback functions for response handling."""
    print("\n=== Callback-Based Example ===")
    
    responses_received = []
    
    def on_response(response):
        """Callback function to handle responses."""
        if response.HasField('image_response'):
            img_response = response.image_response
            responses_received.append({
                'type': 'image',
                'size': (img_response.width, img_response.height),
                'request_id': response.request_id
            })
            print(f"  Received image: {img_response.width}x{img_response.height}")
        elif response.HasField('command_response'):
            cmd_response = response.command_response
            responses_received.append({
                'type': 'command',
                'success': cmd_response.success,
                'request_id': response.request_id
            })
            print(f"  Command response: {cmd_response.success}")
    
    # Connect to server
    client = UESynthStreamingClient()
    
    # Camera positions for a simple path
    positions = [
        (0, 100, 50),
        (100, 100, 50),
        (100, 200, 50),
        (0, 200, 50)
    ]
    
    print("Sending commands with callbacks...")
    
    for i, (x, y, z) in enumerate(positions):
        # Send camera movement command (this would need callback support)
        req_id = client.camera.set_location(x=x, y=y, z=z)
        print(f"Sent camera move command {i+1}, request ID: {req_id}")
        
        # Send capture command
        req_id = client.capture.rgb()
        print(f"Sent capture command {i+1}, request ID: {req_id}")
        
        # Small delay between commands
        time.sleep(0.2)
    
    # Wait for all responses
    print("Waiting for responses...")
    time.sleep(2.0)
    
    print(f"Received {len(responses_received)} responses")
    
    # Get latest frame
    latest_frame = client.get_latest_frame()
    if latest_frame is not None:
        cv2.imwrite("callback_example_frame.png", latest_frame)
        print(f"Latest frame saved: {latest_frame.shape}")
    
    # Disconnect
    client.disconnect()
    print("Callback example completed")


def main():
    """Run all examples."""
    print("UESynth Streaming API Examples")
    print("=" * 40)
    
    try:
        # Run basic example
        basic_streaming_example()
        
        # Run high-frequency simulation
        high_frequency_simulation()
        
        # Run callback-based example
        callback_based_example()
        
    except KeyboardInterrupt:
        print("\nExamples interrupted by user")
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main() 