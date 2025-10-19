#include <gtest/gtest.h>
#include "AudioProcessor.h"
#include <filesystem>
#include <vector>
#include <fstream>
#include <cmath>

// Helper functions for creating test files
void createTestWavFile(const std::string& filename) {
    // Create a simple WAV file header and some dummy audio data
    std::ofstream file(filename, std::ios::binary);
    
    // WAV header (44 bytes)
    file.write("RIFF", 4);
    uint32_t fileSize = 44 + 1024 - 8; // Header + data - 8
    file.write(reinterpret_cast<const char*>(&fileSize), 4);
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    uint32_t fmtSize = 16;
    file.write(reinterpret_cast<const char*>(&fmtSize), 4);
    uint16_t audioFormat = 1; // PCM
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    uint16_t numChannels = 1;
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    uint32_t sampleRate = 44100;
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    uint32_t byteRate = sampleRate * numChannels * 2; // 16-bit
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    uint16_t blockAlign = numChannels * 2;
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    uint16_t bitsPerSample = 16;
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
    file.write("data", 4);
    uint32_t dataSize = 1024;
    file.write(reinterpret_cast<const char*>(&dataSize), 4);
    
    // Write some dummy audio data
    for (int i = 0; i < 256; i++) {
        int16_t sample = static_cast<int16_t>(sin(2.0 * M_PI * 440.0 * i / 44100.0) * 16384);
        file.write(reinterpret_cast<const char*>(&sample), 2);
    }
    
    file.close();
}

void createTestTextFile(const std::string& filename) {
    std::ofstream file(filename);
    file << "This is not a WAV file";
    file.close();
}

class AudioProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        processor = std::make_unique<AudioProcessor>();
    }
    
    std::unique_ptr<AudioProcessor> processor;
};

TEST_F(AudioProcessorTest, LoadValidWavFile) {
    // Create a test WAV file
    std::string testFile = "/tmp/test_audio.wav";
    createTestWavFile(testFile);
    
    std::vector<float> audioData;
    AudioInfo info;
    
    bool result = processor->loadAudioFile(testFile, audioData, info);
    
    EXPECT_TRUE(result);
    EXPECT_GT(audioData.size(), 0);
    EXPECT_EQ(info.sampleRate, 44100);
    EXPECT_EQ(info.channels, 1);
    EXPECT_EQ(info.bitDepth, 16);
    
    // Clean up
    std::filesystem::remove(testFile);
}

TEST_F(AudioProcessorTest, LoadInvalidFile) {
    std::string invalidFile = "/tmp/nonexistent.wav";
    std::vector<float> audioData;
    AudioInfo info;
    
    bool result = processor->loadAudioFile(invalidFile, audioData, info);
    
    EXPECT_FALSE(result);
}

TEST_F(AudioProcessorTest, LoadMinimalWavFile) {
    // Test with a minimal WAV file with some audio data
    std::string minimalFile = "/tmp/minimal.wav";
    
    // Create minimal WAV file with 100 samples (200 bytes of 16-bit data)
    std::ofstream file(minimalFile, std::ios::binary);
    
    // WAV header
    file.write("RIFF", 4);
    uint32_t fileSize = 44 + 200 - 8; // Header + data - 8
    file.write(reinterpret_cast<const char*>(&fileSize), 4);
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    uint32_t fmtSize = 16;
    file.write(reinterpret_cast<const char*>(&fmtSize), 4);
    uint16_t audioFormat = 1; // PCM
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    uint16_t numChannels = 1;
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    uint32_t sampleRate = 44100;
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    uint32_t byteRate = sampleRate * numChannels * 2; // 16-bit
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    uint16_t blockAlign = numChannels * 2;
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    uint16_t bitsPerSample = 16;
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
    file.write("data", 4);
    uint32_t dataSize = 200;
    file.write(reinterpret_cast<const char*>(&dataSize), 4);
    
    // Write some minimal audio data (100 samples of silence)
    for (int i = 0; i < 100; i++) {
        int16_t sample = 0; // Silence
        file.write(reinterpret_cast<const char*>(&sample), 2);
    }
    
    file.close();
    
    std::vector<float> audioData;
    AudioInfo info;
    
    bool result = processor->loadAudioFile(minimalFile, audioData, info);
    
    // Should succeed with some data
    EXPECT_TRUE(result);
    EXPECT_EQ(audioData.size(), 100);
    EXPECT_EQ(info.sampleRate, 44100);
    EXPECT_EQ(info.channels, 1);
    
    // Clean up
    std::filesystem::remove(minimalFile);
}

TEST_F(AudioProcessorTest, SaveAudioFile) {
    // Create test data
    std::vector<float> audioData(44100, 0.1f); // 1 second of audio
    AudioInfo info;
    info.sampleRate = 44100;
    info.channels = 1;
    info.bitDepth = 16;
    info.frameCount = 44100;
    info.isStereo = false;
    info.isPCM = true;
    
    std::string outputFile = "/tmp/test_output.wav";
    bool result = processor->saveAudioFile(outputFile, audioData, info);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists(outputFile));
    
    // Clean up
    std::filesystem::remove(outputFile);
}

TEST_F(AudioProcessorTest, ConvertBitDepth) {
    std::vector<float> inputData(1000, 0.5f);
    std::vector<float> outputData;
    
    bool result = processor->convertBitDepth(inputData, outputData, 16);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(outputData.size(), inputData.size());
}

TEST_F(AudioProcessorTest, ConvertToMono) {
    // Create stereo data (interleaved)
    std::vector<float> stereoData;
    for (int i = 0; i < 1000; ++i) {
        stereoData.push_back(0.5f); // Left channel
        stereoData.push_back(0.3f); // Right channel
    }
    
    std::vector<float> monoData;
    bool result = processor->convertToMono(stereoData, monoData);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(monoData.size(), 1000); // Should be half the size
}

TEST_F(AudioProcessorTest, CalculateRMS) {
    std::vector<float> audioData = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    float rms = processor->calculateRMS(audioData);
    
    EXPECT_GT(rms, 0.0f);
    EXPECT_LT(rms, 1.0f);
}

TEST_F(AudioProcessorTest, IsSilent) {
    std::vector<float> silentData(1000, 0.001f); // Very quiet
    std::vector<float> loudData(1000, 0.5f);    // Loud
    
    EXPECT_TRUE(processor->isSilent(silentData, 0.01f));
    EXPECT_FALSE(processor->isSilent(loudData, 0.01f));
}

TEST_F(AudioProcessorTest, IsValidAudioFile) {
    std::string validFile = "/tmp/valid_audio.wav";
    std::string invalidFile = "/tmp/invalid.txt";
    
    createTestWavFile(validFile);
    createTestTextFile(invalidFile);
    
    EXPECT_TRUE(processor->isValidAudioFile(validFile));
    EXPECT_FALSE(processor->isValidAudioFile(invalidFile));
    
    // Clean up
    std::filesystem::remove(validFile);
    std::filesystem::remove(invalidFile);
}

TEST_F(AudioProcessorTest, IsSupportedFormat) {
    EXPECT_TRUE(processor->isSupportedFormat("test.wav"));
    EXPECT_TRUE(processor->isSupportedFormat("test.aiff"));
    EXPECT_TRUE(processor->isSupportedFormat("test.aif"));
    EXPECT_TRUE(processor->isSupportedFormat("test.flac"));
    EXPECT_TRUE(processor->isSupportedFormat("test.ogg"));
    EXPECT_TRUE(processor->isSupportedFormat("test.mp3"));
    EXPECT_FALSE(processor->isSupportedFormat("test.txt"));
    EXPECT_FALSE(processor->isSupportedFormat("test.unknown"));
}
