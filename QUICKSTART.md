# 🚀 Quick Start Guide

Get M8 Sample Formatter up and running in minutes!

---

## For Users

### Installation

1. **Download** `M8SampleFormatter.dmg` from [Releases](https://github.com/yourusername/m8-sample-formatter/releases)
2. **Open** the DMG
3. **Drag** `M8SampleFormatter.app` to Applications
4. **Launch** from Applications folder
5. **Done!**

### First Use

```
1. Click "Add Folders..."
2. ⌘-click multiple sample pack folders
3. Click "Select Folders"
4. Click "Browse..." for output location
5. Click "Start Processing"
6. Wait for completion
7. Your samples are ready! 🎵
```

---

## For Developers

### One-Line Setup

```bash
# Clone, install dependencies, and build
git clone https://github.com/yourusername/m8-sample-formatter.git && \
cd m8-sample-formatter/M8SampleFormatter_Complete && \
brew install cmake pkg-config libsndfile && \
./scripts/build_complete.sh
```

### Step-by-Step Setup

#### 1. Prerequisites

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install build dependencies
brew install cmake pkg-config libsndfile
```

#### 2. Clone Repository

```bash
git clone https://github.com/yourusername/m8-sample-formatter.git
cd m8-sample-formatter/M8SampleFormatter_Complete
```

#### 3. Build Everything

```bash
# Build C++ backend, Swift GUI, app bundle, and DMG
./scripts/build_complete.sh

# Output files:
# - build/M8SampleFormatter (C++ executable)
# - .build/release/M8FormatterGUI (Swift executable)
# - M8SampleFormatter.app (App bundle)
# - dist/M8SampleFormatter.dmg (DMG installer)
```

#### 4. Run the App

```bash
# Launch the app
open M8SampleFormatter.app

# Or install from DMG
open dist/M8SampleFormatter.dmg
# Drag to Applications folder
```

---

## Build Options

### Full Build (Recommended)

```bash
./scripts/build_complete.sh
# Builds everything: backend + GUI + app bundle + DMG
```

### Backend Only

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(sysctl -n hw.ncpu)
./M8SampleFormatter /path/to/samples /path/to/output
```

### GUI Only

```bash
swift build --package-path . -c release --product M8FormatterGUI
.build/release/M8FormatterGUI
```

### App Bundle Only

```bash
# Requires backend and GUI to be built first
./scripts/build_app_bundle.sh
open M8SampleFormatter.app
```

### DMG Only

```bash
# Requires app bundle to exist
./scripts/create_simple_dmg.sh
open dist/M8SampleFormatter.dmg
```

---

## Testing

### Quick Test

```bash
# Test backend with sample files
./build/M8SampleFormatter ~/Music/Samples ~/Desktop/M8Output

# Test with folder flattening
./build/M8SampleFormatter ~/Music/Samples ~/Desktop/M8Output --flatten-folders
```

### GUI Test

```bash
# Run GUI directly
.build/release/M8FormatterGUI

# Or launch app bundle
open M8SampleFormatter.app
```

### Performance Test

```bash
# Time processing of sample folder
time ./build/M8SampleFormatter ~/Music/Samples ~/Desktop/M8Output
```

---

## Common Commands

### Clean Build

```bash
# Clean all build artifacts
rm -rf build .build M8SampleFormatter.app dist/*.dmg

# Rebuild from scratch
./scripts/build_complete.sh
```

### Update Dependencies

```bash
# Update Homebrew packages
brew update && brew upgrade cmake pkg-config libsndfile
```

### Check Build Requirements

```bash
# Check CMake version
cmake --version  # Need 3.20+

# Check Swift version
swift --version  # Need 5.5+

# Check Xcode tools
xcode-select -p
```

---

## Project Structure

```
M8SampleFormatter_Complete/
├── src/
│   ├── cpp/                    # C++ Backend
│   │   ├── audio/             # Audio processing
│   │   ├── filesystem/        # File operations
│   │   ├── utils/             # Utilities
│   │   └── main.cpp           # Entry point
│   └── swift/                  # Swift Frontend
│       └── M8FormatterGUI/
│           ├── ContentView.swift
│           ├── ProcessingViewModel.swift
│           └── M8FormatterBackend.swift
├── scripts/
│   ├── build_complete.sh      # Master build script
│   ├── build_app_bundle.sh    # App bundle creator
│   └── create_simple_dmg.sh   # DMG creator
├── CMakeLists.txt             # C++ build config
├── Package.swift              # Swift package config
├── README.md                  # Full documentation
├── CONTRIBUTING.md            # Contribution guide
└── LICENSE                    # MIT License
```

---

## Troubleshooting

### "libsndfile not found"

```bash
# Make sure libsndfile is installed
brew install libsndfile

# Set PKG_CONFIG_PATH
export PKG_CONFIG_PATH="/opt/homebrew/opt/libsndfile/lib/pkgconfig:$PKG_CONFIG_PATH"

# Rebuild
cd build && cmake .. && make
```

### "xcrun: error: unable to find utility"

```bash
# Install Xcode Command Line Tools
xcode-select --install

# If already installed, reset
sudo xcode-select --reset
```

### "No such file or directory: M8SampleFormatterBackend"

```bash
# Rebuild app bundle
./scripts/build_app_bundle.sh
```

### Build hangs or fails

```bash
# Clean everything
rm -rf build .build

# Try building components separately
mkdir -p build && cd build
cmake ..
make -j1  # Single-threaded for debugging

cd ..
swift build --product M8FormatterGUI
```

---

## Next Steps

### Learn More
- Read [README.md](README.md) for full documentation
- Check [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines
- See [CHANGELOG.md](CHANGELOG.md) for version history

### Get Involved
- Report bugs: [GitHub Issues](https://github.com/yourusername/m8-sample-formatter/issues)
- Request features: [GitHub Discussions](https://github.com/yourusername/m8-sample-formatter/discussions)
- Contribute code: See [CONTRIBUTING.md](CONTRIBUTING.md)

### Resources
- [Dirtywave M8](https://dirtywave.com/) - The M8 tracker
- [libsndfile](http://www.mega-nerd.com/libsndfile/) - Audio I/O library
- [Apple Accelerate](https://developer.apple.com/documentation/accelerate) - Performance framework

---

**Happy formatting! 🎵✨**

