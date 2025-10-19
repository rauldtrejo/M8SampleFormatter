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
