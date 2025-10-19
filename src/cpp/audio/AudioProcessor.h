#pragma once

#include <string>
#include <vector>
#include <memory>

struct AudioInfo {
    int sampleRate;
    int channels;
    int bitDepth;
    size_t frameCount;
    bool isStereo;
    bool isPCM;
};

class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();
    
    // Audio file operations
    bool loadAudioFile(const std::string& filepath, std::vector<float>& audioData, AudioInfo& info);
    bool saveAudioFile(const std::string& filepath, const std::vector<float>& audioData, const AudioInfo& info);
    
    // Format conversions
    bool convertToMono(const std::vector<float>& stereoData, std::vector<float>& monoData);
    bool convertBitDepth(const std::vector<float>& inputData, std::vector<float>& outputData, int targetBitDepth);
    bool convertToPCM(const std::string& inputFile, const std::string& outputFile);
    
    // Validation
    bool isValidAudioFile(const std::string& filepath);
    bool isPCMFormat(const std::string& filepath);
    bool isSupportedFormat(const std::string& filepath);
    
    // Audio analysis
    float calculateRMS(const std::vector<float>& audioData);
    bool isSilent(const std::vector<float>& audioData, float threshold = 0.01f);
    
    // Apple Silicon acceleration (macOS only)
    bool initializeAppleSilicon();
    bool processWithAppleSilicon(const std::vector<float>& inputData, std::vector<float>& outputData, const std::string& operation);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
    
    bool m_appleSiliconInitialized = false;
    
    // Helper functions
    std::vector<float> interleaveChannels(const std::vector<float>& left, const std::vector<float>& right);
    std::pair<std::vector<float>, std::vector<float>> deinterleaveChannels(const std::vector<float>& stereoData);
    float linearToDB(float linear);
    float dbToLinear(float db);
    int getBitDepth(int format);
};
