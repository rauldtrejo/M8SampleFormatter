#!/bin/bash

# M8 Sample Formatter - Release Script
# This script helps create a GitHub release with the DMG file

set -e

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

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    print_error "Not in a git repository"
    exit 1
fi

# Check if DMG exists
if [ ! -f "dist/M8SampleFormatter.dmg" ]; then
    print_error "DMG file not found. Please run ./scripts/build_complete.sh first"
    exit 1
fi

# Get version from git tag or ask user
VERSION=$(git describe --tags --exact-match 2>/dev/null || echo "")

if [ -z "$VERSION" ]; then
    print_warning "No git tag found. Please create a version tag first:"
    echo "  git tag v1.0.0"
    echo "  git push origin v1.0.0"
    exit 1
fi

print_status "Creating release for version: $VERSION"

# Check if release already exists
if gh release view "$VERSION" >/dev/null 2>&1; then
    print_warning "Release $VERSION already exists"
    read -p "Do you want to update it? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_status "Release creation cancelled"
        exit 0
    fi
fi

# Create or update release
print_status "Creating GitHub release..."

if gh release view "$VERSION" >/dev/null 2>&1; then
    # Update existing release
    gh release upload "$VERSION" dist/M8SampleFormatter.dmg --clobber
    print_success "Updated release $VERSION with new DMG"
else
    # Create new release
    gh release create "$VERSION" \
        --title "M8 Sample Formatter $VERSION" \
        --notes "## What's New

- Fast audio sample pack formatting for M8 tracker
- Folder flattening support (inspired by M8 Sample Organizer)
- 16-bit WAV conversion
- Multi-threaded processing
- Native macOS SwiftUI interface

## Installation

1. Download \`M8SampleFormatter.dmg\`
2. Open the DMG file
3. Drag \`M8SampleFormatter.app\` to Applications
4. Launch from Applications folder

## Features

- **Multi-Folder Selection** - Process multiple sample packs at once
- **Filename Shortening** - Converts to camelCase
- **Format Conversion** - Converts to 16-bit WAV
- **Folder Flattening** - Optional structure flattening
- **Apple Silicon Optimized** - High-performance processing" \
        dist/M8SampleFormatter.dmg
    print_success "Created release $VERSION"
fi

print_success "Release $VERSION is now available at:"
echo "https://github.com/$(gh repo view --json owner,name -q '.owner.login + "/" + .name')/releases/tag/$VERSION"
