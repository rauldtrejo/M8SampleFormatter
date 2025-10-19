#!/bin/bash

# Create simple test fixtures for M8 Sample Formatter
# This script creates basic test files without complex audio generation

set -e

echo "🎵 Creating Simple Test Fixtures"
echo "==============================="

# Create test directories
mkdir -p tests/fixtures/input/{drums,vocals,strings,loops}
mkdir -p tests/fixtures/output

# Create simple test files (text files for now, in real implementation these would be audio)
echo "Creating test files..."

# Create some test files
echo "Test audio content" > tests/fixtures/input/drums/kick_01.wav
echo "Test audio content" > tests/fixtures/input/drums/snare_01.wav
echo "Test audio content" > tests/fixtures/input/vocals/vocal_loop_01.wav
echo "Test audio content" > tests/fixtures/input/strings/string_01.wav
echo "Test audio content" > tests/fixtures/input/loops/loop_01.wav

# Create nested structure
mkdir -p tests/fixtures/input/nested/subfolder
echo "Test audio content" > tests/fixtures/input/nested/subfolder/nested_file.wav

echo "✅ Simple test fixtures created successfully!"
echo "📁 Created files in tests/fixtures/input/"
echo "🎵 Ready for testing!"
