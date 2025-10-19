# Contributing to M8 Sample Formatter

Thank you for your interest in contributing to M8 Sample Formatter! This document provides guidelines and instructions for contributing.

## 🤝 How to Contribute

### Reporting Bugs

1. **Check existing issues** to avoid duplicates
2. **Create a new issue** with:
   - Clear, descriptive title
   - Detailed description of the problem
   - Steps to reproduce
   - Expected behavior
   - Actual behavior
   - Screenshots (if applicable)
   - System information:
     - macOS version
     - Hardware (Intel/Apple Silicon)
     - Sample files used (if relevant)

### Suggesting Features

1. **Check existing feature requests**
2. **Create an issue** with:
   - Clear description of the feature
   - Use case and benefits
   - Possible implementation approach
   - Any relevant mockups or examples

### Submitting Code

#### 1. Fork and Clone

```bash
# Fork the repository on GitHub
# Then clone your fork
git clone https://github.com/YOUR_USERNAME/m8-sample-formatter.git
cd m8-sample-formatter/M8SampleFormatter_Complete
```

#### 2. Create a Branch

```bash
git checkout -b feature/your-feature-name
# or
git checkout -b fix/issue-description
```

#### 3. Set Up Development Environment

```bash
# Install dependencies
brew install cmake pkg-config libsndfile

# Build the project
./scripts/build_complete.sh
```

#### 4. Make Your Changes

- Follow existing code style
- Add comments for complex logic
- Update documentation if needed
- Test thoroughly

#### 5. Test Your Changes

```bash
# Build and test C++ backend
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
./M8SampleFormatter /path/to/test/samples /path/to/output

# Build and test Swift GUI
cd ..
swift build --package-path . -c release --product M8FormatterGUI
```

#### 6. Commit Your Changes

```bash
# Stage your changes
git add .

# Commit with a clear message
git commit -m "Add feature: brief description"

# Follow conventional commits format:
# feat: New feature
# fix: Bug fix
# docs: Documentation changes
# style: Code style changes
# refactor: Code refactoring
# perf: Performance improvements
# test: Adding tests
# chore: Build process or auxiliary tool changes
```

#### 7. Push and Create Pull Request

```bash
git push origin feature/your-feature-name
```

Then open a Pull Request on GitHub with:
- Clear title
- Description of changes
- Link to related issues
- Screenshots (for UI changes)

## 📝 Code Guidelines

### C++ Code Style

```cpp
// Use clear, descriptive names
void processAudioFile(const std::string& inputPath);

// Comment complex algorithms
// Apply camelCase transformation to filename
std::string result = toCamelCase(filename);

// Use modern C++ features
auto files = std::filesystem::directory_iterator(path);

// Prefer RAII and smart pointers
std::unique_ptr<Processor> processor = std::make_unique<Processor>();
```

### Swift Code Style

```swift
// Follow Swift naming conventions
func processFolder(at path: String) { }

// Use clear property names
@Published var selectedFolders: [SelectedFolder] = []

// Document public APIs
/// Starts processing the selected folders
/// - Returns: true if processing started successfully
func startProcessing() -> Bool { }
```

### File Organization

- **C++ Backend**: `src/cpp/`
  - `audio/` - Audio processing
  - `filesystem/` - File operations
  - `utils/` - Utilities (logger, threadpool)
  - `main.cpp` - Entry point

- **Swift Frontend**: `src/swift/M8FormatterGUI/`
  - `ContentView.swift` - UI
  - `ProcessingViewModel.swift` - State management
  - `M8FormatterBackend.swift` - Backend bridge

## 🧪 Testing

### Manual Testing

1. **Test with various sample packs**:
   - Different audio formats (WAV, AIFF, FLAC, OGG, MP3)
   - Different naming conventions
   - Various folder structures
   - Large sample sets (1000+ files)

2. **Test UI interactions**:
   - Multi-folder selection
   - Folder removal
   - Progress updates
   - Error handling

3. **Test edge cases**:
   - Empty folders
   - Invalid audio files
   - Permission issues
   - Path length limits

### Performance Testing

```bash
# Benchmark with different sample sizes
time ./build/M8SampleFormatter /path/to/samples /path/to/output

# Profile memory usage
leaks --atExit -- ./build/M8SampleFormatter /path/to/samples /path/to/output
```

## 🎨 UI/UX Guidelines

- **Native feel**: Use standard macOS patterns
- **Responsive**: Test window resizing
- **Clear feedback**: Show progress and errors
- **Accessibility**: Support keyboard navigation
- **Consistency**: Match macOS design language

## 📚 Documentation

When contributing, please update:

- **README.md** - For user-facing changes
- **Code comments** - For implementation details
- **This file** - For process changes

## ✅ Checklist Before Submitting PR

- [ ] Code builds without warnings
- [ ] Tested on actual hardware
- [ ] Tested with various sample packs
- [ ] UI changes tested at different window sizes
- [ ] No performance regressions
- [ ] Documentation updated
- [ ] Commit messages are clear
- [ ] Branch is up to date with main

## 🐛 Debugging Tips

### C++ Backend

```bash
# Build with debug symbols
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with lldb
lldb ./M8SampleFormatter
> run /path/to/samples /path/to/output
```

### Swift GUI

```bash
# Build and run with debugging
swift build --configuration debug
lldb .build/debug/M8FormatterGUI
```

### Common Issues

**Issue**: libsndfile not found
```bash
brew install libsndfile
export PKG_CONFIG_PATH="/opt/homebrew/opt/libsndfile/lib/pkgconfig:$PKG_CONFIG_PATH"
```

**Issue**: CMake can't find compiler
```bash
xcode-select --install
sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer
```

## 🎯 Priority Areas for Contribution

### High Priority
- Intel Mac testing and fixes
- Unit tests for core functionality
- Performance optimizations
- Bug fixes

### Medium Priority
- Additional audio format support
- UI improvements
- Error handling enhancements
- Documentation improvements

### Low Priority
- Code refactoring
- Style improvements
- Minor feature additions

## 📞 Getting Help

- **Questions**: Open a GitHub Discussion
- **Bugs**: Create an Issue
- **Ideas**: Start a Discussion or Issue

## 🙏 Thank You!

Every contribution, no matter how small, is valuable and appreciated. Thank you for helping make M8 Sample Formatter better!

