# Contributing to UESynth

We welcome contributions from the community! This guide will help you get started with contributing to UESynth, whether you're fixing bugs, adding features, or improving documentation.

## Getting Started

### 1. Development Environment Setup

1. **Fork and clone the repository**
   ```bash
   git clone https://github.com/your-username/uesynth.git
   cd uesynth
   ```

2. **Set up development dependencies**
   ```bash
   # Python client development
   cd python-client
   uv pip install -e ".[dev]"
   
   # Install development tools
   uv pip install pytest black flake8 mypy pre-commit
   ```

3. **Set up pre-commit hooks**
   ```bash
   pre-commit install
   ```

4. **Verify setup**
   ```bash
   pytest tests/
   ```

### 2. Development Workflow

1. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes**
   - Write code following our style guidelines
   - Add tests for new functionality
   - Update documentation as needed

3. **Test your changes**
   ```bash
   # Run tests
   pytest tests/
   
   # Check code style
   black --check .
   flake8 .
   mypy uesynth/
   ```

4. **Commit and push**
   ```bash
   git add .
   git commit -m "feat: add your feature description"
   git push origin feature/your-feature-name
   ```

5. **Create a Pull Request**
   - Use the PR template
   - Provide clear description of changes
   - Link to relevant issues

## Code Style Guidelines

### Python Code Style

We use **Black** for code formatting and **flake8** for linting:

```bash
# Format code
black uesynth/ tests/

# Check linting
flake8 uesynth/ tests/
```

### Code Quality Standards

1. **Type Hints**: All public functions should have type hints
   ```python
   def capture_rgb(self, width: int = None, height: int = None) -> np.ndarray:
       """Capture RGB image with optional resolution."""
       pass
   ```

2. **Docstrings**: Use Google-style docstrings
   ```python
   def set_camera_location(self, x: float, y: float, z: float) -> None:
       """Set camera position in world coordinates.
       
       Args:
           x: X coordinate in world space
           y: Y coordinate in world space
           z: Z coordinate in world space
           
       Raises:
           UESynthError: If camera position cannot be set
       """
       pass
   ```

3. **Error Handling**: Use specific exception types
   ```python
   try:
       response = self._stub.SetCameraLocation(request)
   except grpc.RpcError as e:
       raise UESynthError(f"Failed to set camera location: {e}")
   ```

## Testing Guidelines

### Writing Tests

1. **Test Structure**: Organize tests by functionality
   ```
   tests/
   ├── test_client.py          # Basic client functionality
   ├── test_camera.py          # Camera controls
   ├── test_capture.py         # Data capture
   ├── test_objects.py         # Object manipulation
   └── integration/            # Integration tests
       └── test_workflows.py   # End-to-end workflows
   ```

2. **Test Examples**
   ```python
   import pytest
   from unittest.mock import Mock, patch
   from uesynth import UESynthClient, UESynthError
   
   class TestCameraControls:
       def test_set_location_success(self):
           """Test successful camera location setting."""
           client = UESynthClient()
           
           with patch.object(client, '_stub') as mock_stub:
               mock_stub.SetCameraLocation.return_value = Mock()
               
               # Should not raise
               client.camera.set_location(x=100, y=200, z=50)
               
               # Verify call
               mock_stub.SetCameraLocation.assert_called_once()
       
       def test_set_location_error(self):
           """Test camera location setting with error."""
           client = UESynthClient()
           
           with patch.object(client, '_stub') as mock_stub:
               mock_stub.SetCameraLocation.side_effect = grpc.RpcError("Network error")
               
               with pytest.raises(UESynthError):
                   client.camera.set_location(x=100, y=200, z=50)
   ```

3. **Integration Tests**: Test with actual UE instance
   ```python
   @pytest.mark.integration
   class TestIntegration:
       def test_full_capture_workflow(self):
           """Test complete capture workflow with real UE instance."""
           # Requires UE to be running
           with UESynthClient() as client:
               client.camera.set_location(x=0, y=100, z=50)
               image = client.capture.rgb(width=512, height=512)
               assert image.shape == (512, 512, 3)
   ```

### Running Tests

```bash
# Run all tests
pytest

# Run specific test file
pytest tests/test_camera.py

# Run with coverage
pytest --cov=uesynth tests/

# Run only unit tests (fast)
pytest -m "not integration"

# Run integration tests (requires UE)
pytest -m integration
```

## Documentation Guidelines

### 1. Code Documentation

- **Docstrings**: All public classes and methods must have docstrings
- **Type hints**: Use comprehensive type annotations
- **Examples**: Include usage examples in docstrings

### 2. User Documentation

Documentation is written in Markdown and organized as follows:

```
docs/
├── README.md                    # Main documentation landing page
├── getting-started/            # Installation and quick start
├── guides/                     # User guides and tutorials
├── api-reference/             # Complete API documentation
├── examples/                  # Code examples
├── performance/               # Performance guides
└── contributing.md            # This file
```

### 3. Writing Style

- **Clear and concise**: Use simple, direct language
- **Examples**: Include practical code examples
- **Structure**: Use consistent heading structure
- **Links**: Cross-reference related documentation

## Issue Guidelines

### Reporting Bugs

Use the bug report template and include:

1. **Environment**: OS, Python version, UE version
2. **Reproduction steps**: Minimal code to reproduce
3. **Expected vs actual behavior**: Clear description
4. **Logs**: Relevant error messages and stack traces

Example:
```markdown
## Bug Report

**Environment:**
- OS: Windows 11
- Python: 3.9.7
- UE Version: 5.1
- UESynth Version: 1.2.0

**Reproduction:**
```python
from uesynth import UESynthClient
client = UESynthClient()
client.capture.rgb()  # Fails with timeout
```

**Expected:** Should capture RGB image
**Actual:** TimeoutError after 5 seconds

**Logs:**
```
grpc._channel._InactiveRpcError: <_InactiveRpcError of RPC that terminated with...>
```
```

### Feature Requests

Use the feature request template and include:

1. **Use case**: Why is this feature needed?
2. **Proposed solution**: How should it work?
3. **Alternatives**: Other approaches considered
4. **Additional context**: Examples, mockups, etc.

## Pull Request Guidelines

### PR Checklist

Before submitting a PR, ensure:

- [ ] Code follows style guidelines (Black, flake8)
- [ ] Tests are added for new functionality
- [ ] All tests pass
- [ ] Documentation is updated
- [ ] Commit messages follow conventions
- [ ] PR description is clear and complete

### Commit Message Format

Use conventional commits:

```
type(scope): description

[optional body]

[optional footer]
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Maintenance tasks

**Examples:**
```bash
feat(client): add async streaming support
fix(camera): handle invalid rotation values
docs(api): update capture method documentation
test(integration): add end-to-end workflow tests
```

### PR Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Performance improvement
- [ ] Other (describe):

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] Manual testing performed

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] Tests pass
```

## Development Areas

### Where to Contribute

1. **Python Client Library**
   - New capture modalities
   - Performance optimizations
   - Error handling improvements
   - Async/await patterns

2. **Unreal Engine Plugin**
   - C++ plugin development
   - New UE features integration
   - Performance optimizations
   - Editor tools

3. **Documentation**
   - User guides and tutorials
   - API documentation
   - Examples and workflows
   - Performance guides

4. **Testing**
   - Unit test coverage
   - Integration tests
   - Performance benchmarks
   - Compatibility testing

### Good First Issues

Look for issues labeled:
- `good first issue` - Perfect for new contributors
- `help wanted` - Community help needed
- `documentation` - Documentation improvements
- `testing` - Testing improvements

## Community Guidelines

### Code of Conduct

We follow the [Contributor Covenant](https://www.contributor-covenant.org/):

- **Be respectful**: Treat everyone with respect
- **Be inclusive**: Welcome newcomers and diverse perspectives
- **Be constructive**: Provide helpful feedback
- **Be patient**: Help others learn and grow

### Communication

- **GitHub Issues**: Bug reports, feature requests
- **GitHub Discussions**: General questions, ideas
- **Pull Requests**: Code contributions and reviews

### Recognition

Contributors are recognized in:
- `CONTRIBUTORS.md` file
- Release notes for significant contributions
- Project documentation

## Release Process

### Versioning

We use [Semantic Versioning](https://semver.org/):
- `MAJOR.MINOR.PATCH`
- Breaking changes increment MAJOR
- New features increment MINOR
- Bug fixes increment PATCH

### Release Checklist

1. Update version numbers
2. Update CHANGELOG.md
3. Run full test suite
4. Create release tag
5. Build and publish packages
6. Update documentation

## Getting Help

If you need help contributing:

1. **Check existing documentation**
2. **Search existing issues/discussions**
3. **Ask in GitHub Discussions**
4. **Join community Discord** (if available)

## License

By contributing, you agree that your contributions will be licensed under the same license as the project (GPL v3.0).

---

Thank you for contributing to UESynth! Your efforts help make synthetic data generation more accessible for the computer vision and machine learning community. 🚀 