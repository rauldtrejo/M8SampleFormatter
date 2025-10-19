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
