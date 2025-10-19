#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>
#include <fstream>

// Test fixture for setting up test environment
class M8SampleFormatterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directories
        testDir = std::filesystem::temp_directory_path() / "m8_formatter_test";
        outputDir = testDir / "output";
        
        std::filesystem::create_directories(testDir);
        std::filesystem::create_directories(outputDir);
        
        // Create test audio files
        createTestAudioFiles();
    }
    
    void TearDown() override {
        // Clean up test directories
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
    
    void createTestAudioFiles() {
        // Create a simple test WAV file
        std::string wavFile = (testDir / "test.wav").string();
        createSimpleWavFile(wavFile, 44100, 1, 16, 1.0); // 1 second, 44.1kHz, mono, 16-bit
        
        // Create a test AIFF file
        std::string aiffFile = (testDir / "test.aiff").string();
        createSimpleAiffFile(aiffFile, 44100, 1, 16, 1.0);
        
        // Create nested directory structure
        std::filesystem::create_directories(testDir / "subfolder" / "nested");
        std::string nestedWav = (testDir / "subfolder" / "nested" / "nested_test.wav").string();
        createSimpleWavFile(nestedWav, 44100, 2, 16, 0.5); // 0.5 second, stereo
    }
    
    void createSimpleWavFile(const std::string& filename, int sampleRate, int channels, int bitDepth, double duration) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) return;
        
        // WAV header
        int samples = static_cast<int>(sampleRate * duration);
        int dataSize = samples * channels * (bitDepth / 8);
        int fileSize = 36 + dataSize;
        
        // RIFF header
        file.write("RIFF", 4);
        file.write(reinterpret_cast<const char*>(&fileSize), 4);
        file.write("WAVE", 4);
        
        // fmt chunk
        file.write("fmt ", 4);
        int fmtSize = 16;
        file.write(reinterpret_cast<const char*>(&fmtSize), 4);
        short audioFormat = 1; // PCM
        file.write(reinterpret_cast<const char*>(&audioFormat), 2);
        file.write(reinterpret_cast<const char*>(&channels), 2);
        file.write(reinterpret_cast<const char*>(&sampleRate), 4);
        int byteRate = sampleRate * channels * (bitDepth / 8);
        file.write(reinterpret_cast<const char*>(&byteRate), 4);
        short blockAlign = channels * (bitDepth / 8);
        file.write(reinterpret_cast<const char*>(&blockAlign), 2);
        file.write(reinterpret_cast<const char*>(&bitDepth), 2);
        
        // data chunk
        file.write("data", 4);
        file.write(reinterpret_cast<const char*>(&dataSize), 4);
        
        // Generate simple sine wave data
        for (int i = 0; i < samples; ++i) {
            for (int ch = 0; ch < channels; ++ch) {
                double sample = 0.1 * sin(2.0 * M_PI * 440.0 * i / sampleRate); // 440Hz sine wave
                if (bitDepth == 16) {
                    short sample16 = static_cast<short>(sample * 32767);
                    file.write(reinterpret_cast<const char*>(&sample16), 2);
                }
            }
        }
        
        file.close();
    }
    
    void createSimpleAiffFile(const std::string& filename, int sampleRate, int channels, int bitDepth, double duration) {
        // Simplified AIFF creation - in real implementation, this would be more complex
        // For testing purposes, we'll create a minimal AIFF file
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) return;
        
        // AIFF header (simplified)
        file.write("FORM", 4);
        int fileSize = 46; // Simplified size
        file.write(reinterpret_cast<const char*>(&fileSize), 4);
        file.write("AIFF", 4);
        
        // Common chunk
        file.write("COMM", 4);
        int commSize = 18;
        file.write(reinterpret_cast<const char*>(&commSize), 4);
        file.write(reinterpret_cast<const char*>(&channels), 2);
        int numSampleFrames = static_cast<int>(sampleRate * duration);
        file.write(reinterpret_cast<const char*>(&numSampleFrames), 4);
        file.write(reinterpret_cast<const char*>(&bitDepth), 2);
        
        // Sound data chunk (simplified)
        file.write("SSND", 4);
        int ssndSize = 8;
        file.write(reinterpret_cast<const char*>(&ssndSize), 4);
        int offset = 0;
        int blockSize = 0;
        file.write(reinterpret_cast<const char*>(&offset), 4);
        file.write(reinterpret_cast<const char*>(&blockSize), 4);
        
        file.close();
    }
    
    std::filesystem::path testDir;
    std::filesystem::path outputDir;
};

// Test cases will be defined in separate files
