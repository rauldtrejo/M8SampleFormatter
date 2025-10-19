#!/bin/bash

# M8 Sample Formatter - App Bundle Builder
# This script creates a proper macOS app bundle with both GUI and C++ backend

set -e

echo "🎵 M8 Sample Formatter - App Bundle Builder"
echo "=========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
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

# Check if we're in the right directory
if [ ! -d "M8SampleFormatter" ] || [ ! -d "M8FormatterGUI" ]; then
    print_error "Please run this script from the m8FileFormatter root directory"
    exit 1
fi

# Clean previous builds
print_status "Cleaning previous builds..."
rm -rf M8SampleFormatter.app
rm -rf M8FormatterGUI/.build
rm -rf build

# Build the C++ backend
print_status "Building C++ backend..."
cd M8SampleFormatter
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ../..

# Build the SwiftUI GUI
print_status "Building SwiftUI GUI..."
cd M8FormatterGUI
swift build -c release
cd ..

# Create app bundle structure
print_status "Creating app bundle structure..."
mkdir -p M8SampleFormatter.app/Contents/{MacOS,Resources,Frameworks}

# Copy the C++ executable
print_status "Copying C++ backend..."
cp M8SampleFormatter/build/M8SampleFormatter M8SampleFormatter.app/Contents/MacOS/

# Build the SwiftUI app as a standalone executable
print_status "Building SwiftUI app as standalone executable..."
cd M8FormatterGUI
swift build -c release --static-swift-stdlib
cd ..

# Copy the SwiftUI executable
print_status "Copying SwiftUI GUI..."
cp M8FormatterGUI/.build/release/M8FormatterGUI M8SampleFormatter.app/Contents/MacOS/M8SampleFormatterGUI

# Copy the Dirtywave header image
print_status "Copying Dirtywave header image..."
cp dirtywaveHeader.png M8SampleFormatter.app/Contents/Resources/

# Create a launcher script that runs the SwiftUI app
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

# Copy Info.plist (already created)
print_status "App bundle structure created"

# Create app icon (placeholder)
print_status "Creating app icon..."
# For now, we'll use a system icon, but you can replace this with a custom icon
# sips -s format icns /System/Library/CoreServices/CoreTypes.bundle/Contents/Resources/GenericApplicationIcon.icns --out M8SampleFormatter.app/Contents/Resources/AppIcon.icns

# Create README for the app bundle
print_status "Creating app documentation..."
cat > M8SampleFormatter.app/Contents/Resources/README.txt << 'EOF'
M8 Sample Formatter
==================

A high-performance sample pack formatter for Dirtywave M8, optimized for Apple Silicon.

Features:
- Convert stereo drums to mono
- Convert to 16-bit PCM WAV
- Organize samples by type and BPM
- Apple Silicon optimized (144+ files/second)
- Native macOS GUI

Usage:
1. Double-click M8SampleFormatter.app to launch
2. Select your source folder containing sample packs
3. Select output folder for processed samples
4. Configure processing options
5. Click "Start Processing"

Requirements:
- macOS 14.0 or later
- Apple Silicon (M1/M2/M3) or Intel Mac

Performance:
- Processes 144+ files per second
- Optimized for Apple Silicon with Accelerate framework
- Multi-threaded processing for maximum performance

For support and updates, visit the project repository.
EOF

# Create a simple installer script
print_status "Creating installer script..."
cat > install_m8_formatter.sh << 'EOF'
#!/bin/bash

echo "Installing M8 Sample Formatter..."

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: This installer is for macOS only"
    exit 1
fi

# Check macOS version
MACOS_VERSION=$(sw_vers -productVersion)
REQUIRED_VERSION="14.0"

if [ "$(printf '%s\n' "$REQUIRED_VERSION" "$MACOS_VERSION" | sort -V | head -n1)" != "$REQUIRED_VERSION" ]; then
    echo "Error: macOS 14.0 or later is required. You have $MACOS_VERSION"
    exit 1
fi

# Install to Applications folder
echo "Installing to /Applications..."
sudo cp -R M8SampleFormatter.app /Applications/

# Set permissions
sudo chmod -R 755 /Applications/M8SampleFormatter.app

echo "Installation complete!"
echo "You can now find M8 Sample Formatter in your Applications folder."
EOF

chmod +x install_m8_formatter.sh

print_success "App bundle created successfully!"
print_status "App bundle location: $(pwd)/M8SampleFormatter.app"
print_status "Installer script: $(pwd)/install_m8_formatter.sh"

# Show app bundle contents
print_status "App bundle contents:"
ls -la M8SampleFormatter.app/Contents/MacOS/

echo ""
print_success "🎉 M8 Sample Formatter app bundle is ready!"
print_status "To install: ./install_m8_formatter.sh"
print_status "To run: open M8SampleFormatter.app"
