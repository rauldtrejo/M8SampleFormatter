# 🎵 M8 Sample Formatter

**Fast, simple audio sample pack formatter for the Dirtywave M8 tracker.**

[![macOS](https://img.shields.io/badge/macOS-11.0+-blue.svg)](https://www.apple.com/macos/)
[![Apple Silicon](https://img.shields.io/badge/Apple%20Silicon-Optimized-green.svg)](https://www.apple.com/mac/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

M8 Sample Formatter converts and organizes audio sample packs for the Dirtywave M8 tracker by shortening filenames to camelCase and converting all audio formats to 16-bit WAV.

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

## ✨ Features

- **📁 Multi-Folder Selection** - Process multiple sample packs at once
- **✂️ Intelligent Name Shortening** - Hyphenated format preserves casing for M8 readability
- **🔄 Smart Duplicate Removal** - Fuzzy substring matching removes pack name duplicates
- **🏷️ Acronym Removal** - Automatically removes pack-specific prefixes (ESE-, NRE-, HL-, etc.)
- **🎯 Redundant Category Removal** - Removes category words already in folder path
- **🧹 Filler Word Cleanup** - Removes marketing words, years, WAV, SERUM, and more
- **📏 Path Length Management** - Keeps filenames under M8's 128-character limit
- **⚡ Always-On Abbreviations** - Drum→Drm, Vocal→Vox, Bass→Bs, etc.
- **🎵 Format Conversion** - Converts WAV, AIFF, FLAC, OGG, MP3 to 16-bit WAV
- **📂 Structure Preservation** - Maintains your folder organization
- **🗂️ Folder Flattening** - Optional folder structure flattening (inspired by M8 Sample Organizer)
- **⚡ Ultra Fast** - 200-300 files/second with multi-threading
- **🖥️ Native macOS UI** - Clean SwiftUI interface
- **🍎 Apple Silicon Optimized** - Uses Accelerate framework for vectorized operations

## 📸 Screenshot

![M8 Sample Formatter Interface](interface.png)

*Clean, modern interface with Dirtywave branding and intuitive workflow for processing sample packs.*

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
└── KSHMR-Presents-Zafrir-s-World-Sounds-Vol.-1/
    ├── Drum-Fills/
    │   └── Drum-Fill-01-125-Cm.wav
    ├── Vocals/
    │   └── Loops-Solo/
    │       └── Vocal-Loop-01-130-F#m.wav
    └── Strings/
        └── Pizz-Strings-01-80-F#m.wav
```

### Transformations Applied

1. **Parent Folder Cleanup**:
   - `Ghosthack - Ultimate Techno Bundle 2025` → `Ghosthack-Melodic-Techno`
   - Removes: Years (2020-2029), marketing words, content in () and []
   
2. **Folder Name Formatting**:
   - `Drum Fills` → `Drum-Fills` (hyphenated, preserves casing)
   - Removes: Duplicates, filler words, acronyms
   
3. **Filename Transformations**:
   - `KSHMR Zafrir - Drum Fill 01.wav` → `Fill-01.wav`
   - Removes: Pack name (KSHMR, Zafrir), category words (Drum in folder)
   - Applies: Abbreviations (Drum→Drm, Vocal→Vox)
   
4. **Audio Format**: All formats → 16-bit PCM WAV

5. **Preserved Elements**: Musical keys (F#m, C#), BPM, meaningful content

6. **Structure**: Folder hierarchy maintained

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

## 🎯 Comprehensive Name Formatting

M8 Sample Formatter applies multiple intelligent formatting strategies to create clean, readable, M8-compatible file names.

---

### 1️⃣ Parent Folder Cleanup

The top-level pack folder name is cleaned before processing:

#### Removes:
- **Content in parentheses/brackets**: `(WAV, SERUM) [2024]` → removed entirely
- **Year numbers**: `2020`, `2021`, `2022`, `2023`, `2024`, `2025`, etc.
- **Marketing words**: `Ultimate`, `Essentials`, `Bundle`, `Exclusive`, `Collection`, `Series`, `Legends`, `Hero`, `Edition`
- **Filler words**: `Sample`, `Pack`, `WAV`, `SERUM`

#### Examples:
| Original | Cleaned |
|----------|---------|
| `Ghosthack - Ultimate Techno Bundle 2025` | `Ghosthack-Melodic-Techno` |
| `Dropgun Samples - Melodic Techno 2 (WAV, SERUM) [2024]` | `Dropgun-Samples-Melodic-Techno-2` |
| `Producer Pack Essentials (WAV) 2023` | `Producer` |

---

### 2️⃣ Smart Duplicate Removal (Substring Matching)

Uses intelligent fuzzy matching to remove duplicate words from filenames.

#### How It Works:
1. **Extract pack name words** from the cleaned folder name
2. **Fuzzy substring matching** - case-insensitive comparison
3. **Remove all matches** from subfolders and filenames

#### Examples:

**Pack**: `KSHMR Presents Zafrir World Sounds`

| Original Filename | Output | Removed |
|-------------------|--------|---------|
| `KSHMR-Zafrir-Drum-Fill-01.wav` | `Drm-Fill-01.wav` | KSHMR, Zafrir, Drum→Drm |
| `ghosthack-Techno-Kick.wav` | `Kick.wav` | ghosthack, Techno |

#### Fuzzy Matching Examples:

| Pack Name | Filename | Matches? | Reason |
|-----------|----------|----------|--------|
| `KSHMR` | `kshmr_kick.wav` | ✅ Yes | Case-insensitive |
| `KSHMR` | `K_S_H_M_R_kick.wav` | ✅ Yes | Normalized match |
| `ghosthack` | `GhostHack-Kick.wav` | ✅ Yes | Case-insensitive substring |
| `Lo-Fi` | `lofi_beat.wav` | ✅ Yes | Hyphen normalization |

---

### 3️⃣ Pack Acronym Removal

Automatically removes 2-4 letter acronyms at the start of filenames.

#### Examples:
| Original | Output | Acronym Removed |
|----------|--------|-----------------|
| `ESE-Reverse-Breath.wav` | `Reverse-Breath.wav` | ESE- |
| `NRE-Vox-Shot-Drop.wav` | `Vox-Shot-Drop.wav` | NRE- |
| `HL-Kit-Levels-D#m.wav` | `Kit-Levels-D#m.wav` | HL- |
| `UE-Clap-Melt.wav` | `Clap-Melt.wav` | UE- |
| `DH4-Kick-01.wav` | `Kick-01.wav` | DH4- |

---

### 4️⃣ Redundant Category Word Removal

If a word appears in the folder path, it's removed from the filename.

#### Examples:

**Folder**: `Drm-Hero-4-OS-Kicks-Acoustic`

| Original Filename | Output | Removed |
|-------------------|--------|---------|
| `Kick-01-Hard.wav` | `01-Hard.wav` | Kick (in folder) |
| `Acoustic-Kick-02.wav` | `02.wav` | Acoustic, Kick |

**Folder**: `EDM-Sound-Effects-Reverses`

| Original Filename | Output | Removed |
|-------------------|--------|---------|
| `Reverse-Breath.wav` | `Breath.wav` | Reverse (in folder) |
| `EDM-Transition.wav` | `Transition.wav` | EDM (in folder) |

---

### 5️⃣ Always-On Abbreviations

Common audio terms are abbreviated to save space:

| Full Word | Abbreviation |
|-----------|--------------|
| Drum/Drums | Drm/Drms |
| Vocal/Vocals | Vox |
| Percussion | Perc |
| Bass | Bs |
| Guitar | Gtr |
| String/Strings | Str/Strs |
| Synthesizer | Synth |
| One-Shot/OneShot | OS |
| Loop/Loops | Lp/Lps |
| Melody | Mel |
| Chord/Chords | Chd/Chds |
| Atmosphere | Atm |
| Texture | Txt |
| Instrument | Inst |

#### Example:
`Vocal-Loop-Bass-Drum.wav` → `Vox-Lp-Bs-Drm.wav`

---

## 📏 Path Length Management

M8 tracker has a **128-character filename limit**. M8 Sample Formatter automatically ensures compliance.

### Automatic Strategy:

1. ✅ **Parent folder cleanup** - Remove years, marketing words, parentheses content
2. ✅ **Duplicate removal** - Remove pack name from filenames
3. ✅ **Acronym removal** - Remove pack-specific prefixes (ESE-, NRE-, etc.)
4. ✅ **Category word removal** - Remove words already in folder path
5. ✅ **Abbreviations** - Always apply (Drum→Drm, Vocal→Vox, etc.)
6. ⚠️ **Truncation** - Last resort if still too long (with warning logged)

### Result:

**Before**: `Ghosthack-Ultimate-Techno-Bundle-2025/EDM-Sound-Effects-Essentials/Ghosthack-ESE-Drum-Sample-Long-Name-With-Many-Words.wav` (120+ chars)

**After**: `Ghosthack-Melodic-Techno/EDM-Sound-Effects/Long-Name-With-Many-Words.wav` (70 chars)

---

## 📊 Complete Real-World Example

### Input:
```
Ghosthack - Ultimate Techno Bundle 2025/
├── Ghosthack - EDM Sound Effects/
│   ├── Miscellaneous/
│   │   └── Ghosthack-ESE_Misc_Kick_Ultimate.wav
│   └── Reverses/
│       └── ESE-Reverse-Breath.wav
└── Ghosthack - Drum Hero 4/
    └── Loops/
        └── DH4-Drum-Loop-Techno-155BPM.wav
```

### Output:
```
Ghosthack-Melodic-Techno/
├── EDM-Sound-Effects/
│   ├── Miscellaneous/
│   │   └── Misc-Kick.wav
│   └── Reverses/
│       └── Breath.wav
└── Drm-Hero-4/
    └── Lps/
        └── Lp-155BPM.wav
```

### What Was Removed:
- ✅ Parent folder: `Ultimate`, `Bundle`, `2025`
- ✅ Subfolders: `Ghosthack` (duplicate), `Drum→Drm`, `Loops→Lps`
- ✅ Filenames: `Ghosthack`, `ESE`, `DH4` (acronyms), category words, duplicates
- ✅ Filler words: `Ultimate`, `Techno` (from folder)
- ✅ Applied abbreviations throughout

### What Was Preserved:
- ✅ Musical information: `155BPM`
- ✅ Meaningful content: `Breath`, `Kick`, `Misc`
- ✅ Folder hierarchy
- ✅ Original casing

---

## 📋 Quick Reference: All Formatting Rules

### 🧹 Removed Automatically:

| Category | Examples |
|----------|----------|
| **Marketing Words** | Ultimate, Essentials, Bundle, Exclusive, Collection, Series, Legends, Hero, Edition |
| **Filler Words** | Sample, Pack, Label, Process, Edit, Final, Version, Copy, Backup |
| **Years** | 2020, 2021, 2022, 2023, 2024, 2025, 2026, 2027, 2028, 2029 |
| **Audio Formats** | WAV, SERUM, Splice, CPA |
| **Parentheses/Brackets** | Everything inside `()` and `[]` removed from parent folder |
| **Pack Acronyms** | ESE-, NRE-, HL-, UE-, DH4- (2-4 letter prefixes) |
| **Duplicate Words** | Pack name words removed from subfolders/filenames |
| **Category Words** | Words in folder path removed from filenames |

### ✏️ Always Abbreviated:

| Original | Short | Original | Short |
|----------|-------|----------|-------|
| Drum/Drums | Drm/Drms | Vocal/Vocals | Vox |
| Percussion | Perc | Bass | Bs |
| Guitar | Gtr | String/Strings | Str/Strs |
| Synthesizer | Synth | One-Shot | OS |
| Loop/Loops | Lp/Lps | Melody | Mel |
| Chord/Chords | Chd/Chds | Atmosphere | Atm |
| Texture | Txt | Instrument | Inst |
| Sample/Samples | Smp/Smps | | |

### ✅ Always Preserved:

- Musical keys: `F#m`, `C#`, `Db`, `G#`
- BPM: `120BPM`, `155BPM`
- Numbers: `01`, `02`, `03`
- Original casing: `KSHMR`, `LoFi`, `MySound`
- Meaningful content words
- Folder hierarchy

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

Contributions are welcome! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details on how to contribute to this project.

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

