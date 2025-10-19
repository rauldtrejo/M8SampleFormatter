#!/bin/bash

# Create minimal audio files for testing
# These are the smallest possible valid WAV files

create_minimal_wav() {
    local filename="$1"
    # Create a minimal WAV file (44 bytes header + 0 data)
    printf "RIFF\x1c\x00\x00\x00WAVEfmt \x10\x00\x00\x00\x01\x00\x01\x00\x44\xac\x00\x00\x88X\x01\x00\x02\x00\x10\x00data\x00\x00\x00\x00" > "$filename"
}

create_minimal_aiff() {
    local filename="$1"
    # Create a minimal AIFF file (46 bytes header + 0 data)
    printf "FORM\x00\x00\x00\x1aAIFFCOMM\x00\x00\x00\x12\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x10@SSND\x00\x00\x00\x00\x00\x00\x00\x00" > "$filename"
}

# Create test files
mkdir -p tests/fixtures/input
mkdir -p tests/fixtures/output

echo "Creating minimal audio files for testing..."

# Create minimal WAV files
create_minimal_wav "tests/fixtures/input/test1.wav"
create_minimal_wav "tests/fixtures/input/test2.wav"
create_minimal_wav "tests/fixtures/input/sample.wav"
create_minimal_wav "tests/fixtures/input/audio.wav"

# Create minimal AIFF files
create_minimal_aiff "tests/fixtures/input/test1.aiff"
create_minimal_aiff "tests/fixtures/input/test2.aiff"

# Create some non-audio files for testing
echo "This is not an audio file" > "tests/fixtures/input/readme.txt"
echo '{"config": "test"}' > "tests/fixtures/input/config.json"

# Create subdirectories with files
mkdir -p "tests/fixtures/input/subfolder"
create_minimal_wav "tests/fixtures/input/subfolder/nested.wav"
echo "Subfolder readme" > "tests/fixtures/input/subfolder/readme.txt"

# Create ignored directories
mkdir -p "tests/fixtures/input/.DS_Store"
mkdir -p "tests/fixtures/input/.Trashes"
create_minimal_wav "tests/fixtures/input/.DS_Store/hidden.wav"

echo "✅ Minimal audio files created successfully!"
echo "Files created:"
ls -la tests/fixtures/input/
