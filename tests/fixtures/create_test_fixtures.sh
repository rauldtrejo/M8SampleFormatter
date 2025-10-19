#!/bin/bash

# Create test fixtures for M8 Sample Formatter
# This script creates various test audio files for testing

set -e

echo "🎵 Creating M8 Sample Formatter Test Fixtures"
echo "============================================="

# Create test directories
mkdir -p tests/fixtures/input/{drums,vocals,strings,loops}
mkdir -p tests/fixtures/output

# Function to create a simple WAV file
create_wav() {
    local filename="$1"
    local duration="$2"
    local sample_rate="$3"
    local channels="$4"
    local bit_depth="$5"
    
    # Create a simple sine wave WAV file
    python3 -c "
import wave
import numpy as np
import struct

# Parameters
sample_rate = $sample_rate
duration = $duration
channels = $channels
bit_depth = $bit_depth
frequency = 440  # A4 note

# Generate sine wave
t = np.linspace(0, duration, int(sample_rate * duration), False)
wave_data = np.sin(2 * np.pi * frequency * t) * 0.1

# Convert to appropriate bit depth
if bit_depth == 16:
    wave_data = (wave_data * 32767).astype(np.int16)
    sample_width = 2
elif bit_depth == 24:
    wave_data = (wave_data * 8388607).astype(np.int32)
    sample_width = 3
else:  # 32-bit
    wave_data = (wave_data * 2147483647).astype(np.int32)
    sample_width = 4

# Interleave channels if stereo
if channels == 2:
    stereo_data = np.zeros((len(wave_data) * 2,), dtype=wave_data.dtype)
    stereo_data[::2] = wave_data  # Left channel
    stereo_data[1::2] = wave_data * 0.8  # Right channel (slightly quieter)
    wave_data = stereo_data

# Write WAV file
with wave.open('$filename', 'wb') as wav_file:
    wav_file.setnchannels($channels)
    wav_file.setsampwidth($sample_width)
    wav_file.setframerate($sample_rate)
    wav_file.writeframes(wave_data.tobytes())
"
}

# Create test audio files
echo "Creating test audio files..."

# Drums
create_wav "tests/fixtures/input/drums/kick_01.wav" 1.0 44100 1 16
create_wav "tests/fixtures/input/drums/snare_01.wav" 0.5 44100 1 16
create_wav "tests/fixtures/input/drums/hat_01.wav" 0.25 44100 1 16
create_wav "tests/fixtures/input/drums/crash_01.wav" 2.0 44100 1 16

# Vocals
create_wav "tests/fixtures/input/vocals/vocal_loop_01.wav" 4.0 44100 2 16
create_wav "tests/fixtures/input/vocals/vocal_loop_02.wav" 4.0 44100 2 16

# Strings
create_wav "tests/fixtures/input/strings/string_01.wav" 2.0 44100 2 16
create_wav "tests/fixtures/input/strings/string_02.wav" 2.0 44100 2 16

# Loops
create_wav "tests/fixtures/input/loops/loop_01.wav" 8.0 44100 2 16
create_wav "tests/fixtures/input/loops/loop_02.wav" 8.0 44100 2 16

echo "✅ Test fixtures created successfully!"
echo "📁 Created files in tests/fixtures/input/"
echo "🎵 Ready for testing!"
