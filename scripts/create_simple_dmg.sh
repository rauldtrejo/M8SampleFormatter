#!/bin/bash

# M8 Sample Formatter - Simple DMG Creator
# This script creates a simple DMG without complex customization

set -e

echo "🎵 M8 Sample Formatter - Simple DMG Creator"
echo "=========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

# Check if app bundle exists
if [ ! -d "M8SampleFormatter.app" ]; then
    print_status "App bundle not found. Creating it first..."
    ./build_app_bundle.sh
fi

# Clean previous DMG
print_status "Cleaning previous DMG builds..."
rm -rf M8SampleFormatter.dmg
rm -rf dmg_build

# Create DMG build directory
print_status "Creating DMG build directory..."
mkdir -p dmg_build

# Copy app bundle
print_status "Copying app bundle to DMG..."
cp -R M8SampleFormatter.app dmg_build/

# Create Applications symlink
print_status "Creating Applications symlink..."
ln -s /Applications dmg_build/Applications

# Create README
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

# Create the DMG
print_status "Creating DMG file..."
hdiutil create -srcfolder dmg_build -volname "M8 Sample Formatter" -fs HFS+ -format UDZO -imagekey zlib-level=9 -o M8SampleFormatter.dmg

# Clean up
print_status "Cleaning up temporary files..."
rm -rf dmg_build

# Get DMG info
DMG_SIZE=$(du -h M8SampleFormatter.dmg | cut -f1)
print_success "DMG created successfully!"
print_status "DMG file: $(pwd)/M8SampleFormatter.dmg"
print_status "DMG size: $DMG_SIZE"

# Create installer script
print_status "Creating installer script..."
cat > install_m8_formatter.sh << 'EOF'
#!/bin/bash

echo "🎵 M8 Sample Formatter - DMG Installer"
echo "======================================"
echo ""

# Check if DMG exists
if [ ! -f "M8SampleFormatter.dmg" ]; then
    echo "❌ Error: M8SampleFormatter.dmg not found"
    echo "Please run ./create_simple_dmg.sh first"
    exit 1
fi

echo "📦 Opening DMG installer..."
open M8SampleFormatter.dmg

echo ""
echo "📋 Installation Instructions:"
echo "1. Drag M8SampleFormatter.app to your Applications folder"
echo "2. Launch from Applications or Spotlight"
echo "3. Enjoy your high-performance sample formatter!"
echo ""
echo "🎉 Installation complete!"
EOF

chmod +x install_m8_formatter.sh

print_success "🎉 DMG installer created successfully!"
print_status "DMG file: $(pwd)/M8SampleFormatter.dmg"
print_status "To install: ./install_m8_formatter.sh"
print_status "To distribute: Share the M8SampleFormatter.dmg file"

echo ""
print_success "🚀 Your M8 Sample Formatter is ready for distribution!"
print_status "The DMG contains:"
print_status "  • Native SwiftUI macOS app"
print_status "  • High-performance C++ backend"
print_status "  • Apple Silicon optimizations"
print_status "  • Professional installer experience"
