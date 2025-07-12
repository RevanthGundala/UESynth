# Contributing to UESynth

## How to Contribute

### Reporting Bugs
If you find a bug, please open an issue on our GitHub repository. When filing a bug report, please provide the following information:
-   **Steps to reproduce the bug:** A clear and concise description of how to reproduce the bug.
-   **Version numbers:** The version of UESynth, Unreal Engine, and the Python client library you are using.
-   **Logs:** Any relevant logs or error messages.
-   **Screenshots or videos:** If applicable, add screenshots or videos to help explain the problem.

### Suggesting Enhancements
If you have an idea for a new feature or an enhancement to an existing one, please open an issue on our GitHub repository. Please provide a clear and detailed explanation of the feature you are proposing and why you think it would be a valuable addition to UESynth.

### Your First Code Contribution
Unsure where to begin contributing to UESynth? You can start by looking through `good-first-issue` and `help-wanted` issues.

A step-by-step guide for new contributors:
1.  **Fork the repository:** Start by forking the main UESynth repository to your own GitHub account.
2.  **Clone the fork:** Clone your forked repository to your local machine.
3.  **Set up the development environment:** Follow the instructions in the "Development Setup" section below.
4.  **Create a new branch:** Create a new branch for your feature or bugfix. Use a descriptive name for your branch (e.g., `feature/new-camera-api` or `bugfix/fix-depth-map-crash`).
    ```bash
    git checkout -b feature/your-feature-name
    ```
5.  **Write your code and add tests:** Make your changes to the codebase and add corresponding tests.
6.  **Ensure all tests pass:** Run the C++ and Python test suites to make sure your changes haven't broken anything.
7.  **Submit a pull request:** Push your changes to your forked repository and open a pull request to the `main` branch of the original UESynth repository.

## Development Setup

To get started with developing UESynth, you will need the following software:
-   Unreal Engine 5.x
-   A C++ compiler compatible with your version of Unreal Engine (e.g., Visual Studio for Windows).
-   Python 3.8 or higher.
-   `uv` for Python dependency management.

### gRPC Code Generation
To run the gRPC code generation step locally, you will need to have `protoc` and the gRPC plugins for C++ and Python installed. You can then run the generation script located in the `scripts` directory (to be created).

## Style Guides

### C++
Please adhere to the official [Unreal Engine coding standard](https://docs.unrealengine.com/en-US/ProgrammingAndScripting/ProgrammingWithCPP/CodingStandard/index.html).

### Python
We use the `black` code formatter to maintain a consistent code style. Please make sure to format your Python code with `black` before submitting a pull request.

## Testing

### C++
Unit tests for the C++ code are written using the Unreal Engine Automation System. You can run these tests from the Unreal Editor.

### Python
Unit tests for the Python client library are written using `pytest`. You can run the tests by navigating to the `python` directory and running `pytest`. 