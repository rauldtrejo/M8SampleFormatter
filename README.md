# 🎵 M8 Sample Formatter

**Fast, simple audio sample pack formatter for the Dirtywave M8 tracker.**

[![macOS](https://img.shields.io/badge/macOS-11.0+-blue.svg)](https://www.apple.com/macos/)
[![Apple Silicon](https://img.shields.io/badge/Apple%20Silicon-Optimized-green.svg)](https://www.apple.com/mac/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

M8 Sample Formatter converts and organizes audio sample packs for the Dirtywave M8 tracker by shortening filenames to camelCase and converting all audio formats to 16-bit WAV.

---

## ✨ Features

- **📁 Multi-Folder Selection** - Process multiple sample packs at once
- **✂️ Filename Shortening** - Converts spaces, hyphens, underscores to camelCase
- **🎵 Format Conversion** - Converts WAV, AIFF, FLAC, OGG, MP3 to 16-bit WAV
- **📂 Structure Preservation** - Maintains your folder organization
- **🗂️ Folder Flattening** - Optional folder structure flattening (inspired by M8 Sample Organizer)
- **⚡ Ultra Fast** - 200-300 files/second with multi-threading
- **🖥️ Native macOS UI** - Clean SwiftUI interface
- **🍎 Apple Silicon Optimized** - Uses Accelerate framework for vectorized operations

---

## 📥 Installation

### Download Release (Recommended)

1. Go to the [Releases](https://github.com/rauldtrejo/M8SampleFormatter/releases) page
2. Download `M8SampleFormatter.dmg` from the latest release
3. Open the DMG file
4. Drag `M8SampleFormatter.app` to your Applications folder
5. Done!

### Build from Source

See [Building from Source](#-building-from-source) below.

---

## 🚀 Quick Start

1. **Launch** the application
2. **Click** "Add Folders..." and select your sample pack folders (⌘-click for multiple)
3. **Click** "Browse..." to select an output destination
4. **Click** "Start Processing"
5. **Done!** Your samples are ready for M8

---

## 📖 How It Works

### Input
```
KSHMR Presents Zafrir_s World Sounds Vol. 1/
├── Drum Fills/
│   └── KSHMR Zafrir - Drum Fill 01 (125, Cm).wav
├── Vocals/
│   └── Loops - Solo/
│       └── KSHMR Zafrir - Vocal Loop 01 (130, F#m).aiff
└── Strings/
    └── KSHMR Zafrir - Pizz Strings 01 (80, F#m).flac
```

### Output
```
output/
└── kSHMRPresentsZafrirSWorldSoundsVol.1/
    ├── drumFills/
    │   └── kSHMRZafrirDrumFill01125Cm.wav
    ├── vocals/
    │   └── loopsSolo/
    │       └── kSHMRZafrirVocalLoop01130F#m.wav
    └── strings/
        └── kSHMRZafrirPizzStrings0180F#m.wav
```

### Transformations Applied

- **Folders**: `Drum Fills` → `drumFills`
- **Filenames**: `KSHMR Zafrir - Drum Fill 01.wav` → `kSHMRZafrirDrumFill01.wav`
- **Format**: All audio → 16-bit PCM WAV
- **Musical Keys**: `F#m`, `C#` preserved
- **Structure**: Folder hierarchy maintained

### Folder Flattening (Optional)

When the "Flatten folder structure" option is enabled, nested folders are combined into a single flattened directory:

#### Input (Nested Structure)
```
Capsun - Lo-Fi Soul & Future Beats/
├── CPA_CAPSUN_Lo_Fi_Soul___Future_Beats/
│   └── Drums___Percussion/
│       └── Drum___Perc_One_Shots/
│           └── Snare/
│               └── CLF_Snare_Chunk.wav
```

#### Output (Flattened Structure)
```
output/
└── capsunLoFiSoulFutureBeats/
    └── CPA_CAPSUN_Lo_Fi_Soul___Future_Beats_Drums___Percussion_Drum___Perc_One_Shots_Snare/
        └── CLF_Snare_Chunk.wav
```

**Benefits of Flattening:**
- **Simplified Navigation** - No more deep folder hierarchies
- **Duplicate Word Removal** - Removes redundant words like "processed", "final", "version"
- **Filler Word Cleanup** - Removes common filler words like "sample", "sound", "audio"
- **M8-Friendly** - Easier to navigate on the M8 tracker's limited screen

---

## 🏷️ Releases

### Creating a Release

1. **Tag a version**:
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```

2. **GitHub Actions will automatically**:
   - Build the application
   - Create a GitHub Release
   - Upload the DMG file
   - Make it available for download

### Manual Release (Alternative)

If you prefer to create releases manually:

1. Build the DMG locally: `./scripts/build_complete.sh`
2. Go to GitHub → Releases → Create a new release
3. Upload `dist/M8SampleFormatter.dmg` as an asset

---

## 🛠️ Building from Source

### Prerequisites

- **macOS 11.0+** (Big Sur or later)
- **Xcode Command Line Tools**: `xcode-select --install`
- **CMake 3.20+**: `brew install cmake`
- **pkg-config**: `brew install pkg-config`
- **libsndfile**: `brew install libsndfile`

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/rauldtrejo/M8SampleFormatter.git
cd m8-sample-formatter/M8SampleFormatter_Complete

# Build everything (C++ backend, SwiftUI GUI, and DMG)
./scripts/build_complete.sh

# The DMG will be created in the dist/ folder
open dist/M8SampleFormatter.dmg
```

### Manual Build

If you want to build components separately:

```bash
# 1. Build C++ Backend
mkdir -p build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
cd ..

# 2. Build SwiftUI GUI
swift build --package-path . -c release --product M8FormatterGUI

# 3. Create App Bundle
./scripts/build_app_bundle.sh

# 4. Create DMG (optional)
./scripts/create_simple_dmg.sh
```

### Build Output

- **`build/M8SampleFormatter`** - C++ backend executable
- **`.build/release/M8FormatterGUI`** - SwiftUI GUI executable
- **`M8SampleFormatter.app`** - macOS app bundle
- **`dist/M8SampleFormatter.dmg`** - DMG installer

---

## 🏗️ Project Structure

```
M8SampleFormatter_Complete/
├── src/
│   ├── cpp/                    # C++ Backend
│   │   ├── audio/             # Audio processing (libsndfile, Accelerate)
│   │   ├── filesystem/        # File operations, path management
│   │   ├── utils/             # Logger, ThreadPool
│   │   └── main.cpp           # Entry point
│   └── swift/                  # SwiftUI Frontend
│       └── M8FormatterGUI/
│           ├── ContentView.swift          # Main UI
│           ├── ProcessingViewModel.swift  # State management
│           ├── M8FormatterBackend.swift   # C++ bridge
│           └── main.swift                 # App entry
├── scripts/
│   ├── build_complete.sh      # Master build script
│   ├── build_app_bundle.sh    # Create .app bundle
│   ├── create_simple_dmg.sh   # Create DMG installer
│   └── install_m8_formatter.sh # Installation helper
├── CMakeLists.txt             # C++ build configuration
├── Package.swift              # Swift package manifest
└── README.md                  # This file
```

---

## 🙏 Acknowledgments

This project was inspired by and builds upon the excellent work of the [M8 Sample Organizer](https://github.com/birds-inc/m8-sample-organizer) by [birds-inc](https://github.com/birds-inc). 

We extend our gratitude to the original developers for:
- **Folder flattening algorithm** - The core logic for reducing nested folder structures
- **Strike word removal** - The intelligent approach to removing redundant words
- **M8 optimization concepts** - Understanding the specific needs of M8 tracker users

Our implementation adds:
- **Native macOS GUI** - SwiftUI interface for better user experience
- **Apple Silicon optimization** - High-performance processing using Accelerate framework
- **Multi-threading** - Parallel processing for faster conversion
- **Real-time progress tracking** - Live updates during processing

Thank you to the M8 community and the original M8 Sample Organizer team for the inspiration and foundation! 🎵

---

## 🔧 Development

### Requirements

- **C++17** compiler (Clang recommended)
- **Swift 5.5+**
- **macOS SDK 11.0+**

### Technologies Used

#### Backend
- **C++17**: High-performance processing
- **libsndfile**: Audio file I/O (WAV, AIFF, FLAC, OGG, MP3)
- **Apple Accelerate**: Vectorized audio operations
- **std::thread**: Multi-threaded file processing

#### Frontend
- **SwiftUI**: Native macOS UI
- **AppKit**: File pickers and native dialogs
- **Combine**: Reactive state management

### Running Tests

```bash
# Test C++ backend directly
./build/M8SampleFormatter /path/to/source /path/to/output

# Test with options
./build/M8SampleFormatter /path/to/source /path/to/output --no-bitdepth
./build/M8SampleFormatter /path/to/source /path/to/output --flatten-folders
```

### Debugging

```bash
# Build with debug symbols
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(sysctl -n hw.ncpu)

# Run with lldb
lldb ./M8SampleFormatter
```

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

### Reporting Issues

1. Check [existing issues](https://github.com/rauldtrejo/M8SampleFormatter/issues)
2. Create a new issue with:
   - Clear description
   - Steps to reproduce
   - Expected vs actual behavior
   - macOS version and hardware
   - Sample files (if applicable)

### Submitting Pull Requests

1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature/amazing-feature`
3. **Make** your changes
4. **Test** thoroughly
5. **Commit** with clear messages: `git commit -m 'Add amazing feature'`
6. **Push** to your fork: `git push origin feature/amazing-feature`
7. **Open** a Pull Request

### Development Guidelines

- **Code Style**: Follow existing patterns
- **Comments**: Document complex logic
- **Testing**: Test on Apple Silicon and Intel Macs
- **Performance**: Profile before and after changes
- **UI**: Ensure responsiveness and native feel

### Areas for Contribution

- [ ] Intel Mac testing and optimization
- [ ] Additional audio format support
- [ ] Batch processing improvements
- [ ] UI/UX enhancements
- [ ] Documentation improvements
- [ ] Unit tests
- [ ] CI/CD pipeline
- [ ] Windows/Linux ports (via CLI)

---

## 📊 Performance

### Benchmarks (M1 MacBook Pro)

| Files | Time | Speed |
|-------|------|-------|
| 262 | 1.0s | 262 files/s |
| 1,000 | 4.2s | 238 files/s |
| 10,000 | 42s | 238 files/s |

### Optimizations

- **Multi-threading**: Uses all CPU cores (2x hardware concurrency)
- **Vectorized operations**: Apple Accelerate framework
- **Streaming I/O**: Memory-efficient file processing
- **Native ARM64**: Compiled for Apple Silicon

---

## 🐛 Known Issues

- None currently! 🎉

Report issues at: https://github.com/rauldtrejo/M8SampleFormatter/issues

---

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 M8 Sample Formatter Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 🙏 Acknowledgments

- **Dirtywave** - For creating the amazing M8 tracker
- **libsndfile** - For comprehensive audio format support
- **Apple** - For the Accelerate framework
- **Open Source Community** - For inspiration and support

---

## 📧 Contact

- **Issues**: [GitHub Issues](https://github.com/rauldtrejo/M8SampleFormatter/issues)
- **Discussions**: [GitHub Discussions](https://github.com/rauldtrejo/M8SampleFormatter/discussions)

---

## ⭐ Show Your Support

If you find this project useful, please consider:
- ⭐ Starring the repository
- 🐛 Reporting bugs
- 💡 Suggesting features
- 🤝 Contributing code
- 📢 Sharing with the M8 community

---

**Made with ❤️ for the M8 community**

