#!/bin/bash

# M8 Sample Formatter - Complete Build Script
# This script builds the entire application from the clean project structure

set -e

echo "🎵 M8 Sample Formatter - Complete Build"
echo "======================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_step() {
    echo -e "${PURPLE}[STEP]${NC} $1"
}

# Check if we're in the right directory
if [ ! -d "src/cpp" ] || [ ! -d "src/swift" ]; then
    print_error "Please run this script from the M8SampleFormatter_Complete directory"
    exit 1
fi

# Clean previous builds
print_status "Cleaning previous builds..."
rm -rf build/
rm -rf M8SampleFormatter.app
rm -rf M8SampleFormatter.dmg
rm -rf temp.dmg
rm -rf dmg_build

# Step 1: Build C++ Backend
print_step "Step 1: Building C++ Backend"
echo "================================="
mkdir -p build
cd build
print_status "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release
print_status "Building C++ backend..."
make -j$(sysctl -n hw.ncpu)
print_success "C++ backend built successfully"
cd ..

# Step 2: Build SwiftUI GUI
print_step "Step 2: Building SwiftUI GUI"
echo "==============================="
print_status "Building SwiftUI GUI..."
swift build -c release
print_success "SwiftUI GUI built successfully"

# Step 3: Create App Bundle
print_step "Step 3: Creating macOS App Bundle"
echo "======================================"
print_status "Creating app bundle structure..."
mkdir -p M8SampleFormatter.app/Contents/{MacOS,Resources,Frameworks}

# Copy executables
print_status "Copying C++ backend..."
cp build/M8SampleFormatter M8SampleFormatter.app/Contents/MacOS/M8SampleFormatterBackend

print_status "Copying SwiftUI GUI..."
cp .build/release/M8FormatterGUI M8SampleFormatter.app/Contents/MacOS/M8SampleFormatterGUI

# Create launcher script
print_status "Creating launcher script..."
cat > M8SampleFormatter.app/Contents/MacOS/M8SampleFormatter << 'EOF'
#!/bin/bash
# M8 Sample Formatter Launcher
# This script launches the SwiftUI GUI

# Get the directory where this script is located
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Launch the SwiftUI GUI
exec "$DIR/M8SampleFormatterGUI"
EOF

chmod +x M8SampleFormatter.app/Contents/MacOS/M8SampleFormatter
chmod +x M8SampleFormatter.app/Contents/MacOS/M8SampleFormatterGUI

# Create Info.plist
print_status "Creating app metadata..."
cat > M8SampleFormatter.app/Contents/Info.plist << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>en</string>
	<key>CFBundleExecutable</key>
	<string>M8SampleFormatter</string>
	<key>CFBundleIdentifier</key>
	<string>com.m8formatter.M8SampleFormatter</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>M8 Sample Formatter</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
	<string>1.0</string>
	<key>CFBundleVersion</key>
	<string>1</string>
	<key>LSMinimumSystemVersion</key>
	<string>14.0</string>
	<key>NSHighResolutionCapable</key>
	<true/>
	<key>NSPrincipalClass</key>
	<string>NSApplication</string>
</dict>
</plist>
EOF

print_success "App bundle created successfully"

# Step 4: Create DMG
print_step "Step 4: Creating DMG Installer"
echo "================================="
print_status "Creating DMG build directory..."
mkdir -p dmg_build

print_status "Copying app bundle to DMG..."
cp -R M8SampleFormatter.app dmg_build/

print_status "Creating Applications symlink..."
ln -s /Applications dmg_build/Applications

# Create README for DMG
print_status "Creating DMG documentation..."
cat > dmg_build/README.txt << 'EOF'
M8 Sample Formatter
==================

🎵 High-Performance Sample Pack Formatter for Dirtywave M8

Features:
• Convert stereo drums to mono automatically
• Convert all samples to 16-bit PCM WAV format
• Organize samples by type and BPM
• Apple Silicon optimized (144+ files/second)
• Native macOS GUI with SwiftUI
• Real-time progress tracking

Requirements:
• macOS 14.0 (Sonoma) or later
• Apple Silicon (M1/M2/M3) or Intel Mac

Installation:
1. Drag M8SampleFormatter.app to Applications folder
2. Launch from Applications or Spotlight
3. Select your sample pack folders
4. Choose output destination
5. Configure processing options
6. Click "Start Processing"

Performance:
• Processes 144+ files per second
• Optimized for Apple Silicon with Accelerate framework
• Multi-threaded processing for maximum performance
• Smart mono conversion for drum samples

Enjoy your perfectly organized sample library! 🎶
EOF

# Calculate DMG size
APP_SIZE=$(du -sk dmg_build | cut -f1)
DMG_SIZE=$((APP_SIZE + 51200)) # Add 50MB overhead
print_status "Calculated DMG size: ${DMG_SIZE}KB"

# Create the DMG
print_status "Creating DMG file..."
hdiutil create -srcfolder dmg_build -volname "M8 Sample Formatter" -fs HFS+ -format UDZO -imagekey zlib-level=9 -o M8SampleFormatter.dmg

# Clean up
print_status "Cleaning up temporary files..."
rm -rf dmg_build

# Get DMG info
DMG_SIZE=$(du -h M8SampleFormatter.dmg | cut -f1)
print_success "DMG created successfully!"

# Move to dist folder
print_status "Moving DMG to dist folder..."
mv M8SampleFormatter.dmg dist/

# Create installer script
print_status "Creating installer script..."
cat > scripts/install_m8_formatter.sh << 'EOF'
#!/bin/bash

echo "🎵 M8 Sample Formatter - DMG Installer"
echo "======================================"
echo ""

# Check if DMG exists
if [ ! -f "dist/M8SampleFormatter.dmg" ]; then
    echo "❌ Error: M8SampleFormatter.dmg not found in dist folder"
    echo "Please run ./scripts/build_complete.sh first"
    exit 1
fi

echo "📦 Opening DMG installer..."
open dist/M8SampleFormatter.dmg

echo ""
echo "📋 Installation Instructions:"
echo "1. Drag M8SampleFormatter.app to your Applications folder"
echo "2. Launch from Applications or Spotlight"
echo "3. Enjoy your high-performance sample formatter!"
echo ""
echo "🎉 Installation complete!"
EOF

chmod +x scripts/install_m8_formatter.sh

# Final summary
print_step "Build Complete!"
echo "================"
print_success "🎉 M8 Sample Formatter complete build finished successfully!"
echo ""
print_status "📦 Generated files:"
print_status "  • M8SampleFormatter.app - macOS app bundle"
print_status "  • dist/M8SampleFormatter.dmg - DMG installer ($DMG_SIZE)"
print_status "  • scripts/install_m8_formatter.sh - Installer script"
echo ""
print_status "🚀 Distribution ready!"
print_status "  • Share dist/M8SampleFormatter.dmg for easy installation"
print_status "  • Users can drag the app to Applications folder"
print_status "  • No additional setup required"
echo ""
print_status "⚡ Performance features:"
print_status "  • Apple Silicon optimized (144+ files/second)"
print_status "  • Native SwiftUI macOS interface"
print_status "  • High-performance C++ backend"
print_status "  • Professional installer experience"
echo ""
print_success "🎵 Your M8 Sample Formatter is ready for distribution!"
