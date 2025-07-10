# UESynth: A Synthetic Data Generation Plugin for Unreal Engine

[![Build Status](https://github.com/your-username/uesynth/actions/workflows/ci.yml/badge.svg)](https://github.com/your-username/uesynth/actions)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Overview
UESynth is a powerful and user-friendly Unreal Engine plugin for synthetic data generation, inspired by UnrealCV. It provides a comprehensive API for controlling scenes, manipulating objects, and capturing a wide variety of data modalities for use in computer vision and machine learning research.

## Key Features
-   **Multi-modal Data Capture:** Capture RGB, depth, segmentation masks, surface normals, and more.
-   **Scene and Object Manipulation:** Programmatically control objects, lighting, and materials.
-   **gRPC-based API:** A modern, robust, and easy-to-use API for external clients (Python).
-   **Scene Randomization:** Tools for domain randomization to create diverse datasets.
-   **Editor Integration:** User-friendly interface within the Unreal Editor.

## Installation

### Plugin
1.  Download the latest release from the [GitHub Releases](https://github.com/your-username/uesynth/releases) page.
2.  Extract the `UESynth` folder into your Unreal Engine project's `Plugins` directory. If a `Plugins` directory does not exist, create one.

### Python Client
To install the Python client library, run the following command:
```bash
uv pip install uesynth
```

## Quick Start
Here is a short example of how to use the Python client to connect to Unreal Engine, capture an image, and save it.

```python
from uesynth import UESynthClient
import cv2

# Connect to the Unreal Engine instance
client = UESynthClient()

# Set camera location and rotation
client.camera.set_location(x=0, y=100, z=50)
client.camera.set_rotation(pitch=-15, yaw=0, roll=0)

# Capture an RGB image
rgb_image = client.capture.rgb()

# Save the image
cv2.imwrite("rgb_image.png", rgb_image)

# Disconnect from the client
client.disconnect()
```

## Documentation
For more detailed information, please see the full documentation (link to be added).

## Contributing
We welcome contributions from the community! Please read our [CONTRIBUTING.md](CONTRIBUTING.md) file for guidelines on how to contribute.

## License
This project is licensed under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details. 