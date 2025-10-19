#include "AudioProcessor.h"
#include "AppleSiliconProcessor.h"
#include "Logger.h"
#include <sndfile.h>
#include <iostream>
#include <algorithm>
#include <cmath>

#ifdef __APPLE__
#include <AudioToolbox/AudioToolbox.h>
#include <Accelerate/Accelerate.h>
#endif

class AudioProcessor::Impl {
public:
    AppleSiliconProcessor appleSiliconProcessor;
    
    Impl() {
        // Initialize Apple Silicon processor
        if (!appleSiliconProcessor.initialize()) {
            Logger::getInstance().warning("Apple Silicon processor initialization failed, falling back to CPU");
        }
    }
    
    ~Impl() {
        // Cleanup handled by AppleSiliconProcessor destructor
    }
};

AudioProcessor::AudioProcessor() : m_impl(std::make_unique<Impl>()) {
    Logger::getInstance().debug("AudioProcessor initialized");
}

AudioProcessor::~AudioProcessor() = default;

bool AudioProcessor::loadAudioFile(const std::string& filepath, std::vector<float>& audioData, AudioInfo& info) {
    SF_INFO sfInfo;
    SNDFILE* file = sf_open(filepath.c_str(), SFM_READ, &sfInfo);
    
    if (!file) {
        Logger::getInstance().error("Failed to open audio file: " + filepath);
        return false;
    }
    
    // Set audio info
    info.sampleRate = sfInfo.samplerate;
    info.channels = sfInfo.channels;
    info.frameCount = sfInfo.frames;
    info.isStereo = (sfInfo.channels == 2);
    info.isPCM = (sfInfo.format & SF_FORMAT_PCM_16) || (sfInfo.format & SF_FORMAT_PCM_24) || (sfInfo.format & SF_FORMAT_PCM_32);
    info.bitDepth = getBitDepth(sfInfo.format);
    
    // Read audio data
    audioData.resize(sfInfo.frames * sfInfo.channels);
    sf_count_t framesRead = sf_readf_float(file, audioData.data(), sfInfo.frames);
    
    sf_close(file);
    
    if (framesRead != sfInfo.frames) {
        Logger::getInstance().warning("Did not read all frames from: " + filepath);
    }
    
    Logger::getInstance().debug("Loaded audio file: " + filepath + " (" + std::to_string(framesRead) + " frames)");
    return true;
}

bool AudioProcessor::saveAudioFile(const std::string& filepath, const std::vector<float>& audioData, const AudioInfo& info) {
    SF_INFO sfInfo;
    sfInfo.samplerate = info.sampleRate;
    sfInfo.channels = info.channels;
    sfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; // Always save as 16-bit WAV
    
    Logger::getInstance().debug("Converting to 16-bit WAV: " + filepath);
    
    SNDFILE* file = sf_open(filepath.c_str(), SFM_WRITE, &sfInfo);
    
    if (!file) {
        Logger::getInstance().error("Failed to create audio file: " + filepath);
        return false;
    }
    
    sf_count_t framesWritten = sf_writef_float(file, audioData.data(), audioData.size() / info.channels);
    sf_close(file);
    
    if (framesWritten != audioData.size() / info.channels) {
        Logger::getInstance().warning("Did not write all frames to: " + filepath);
        return false;
    }
    
    Logger::getInstance().debug("Saved audio file: " + filepath + " (" + std::to_string(framesWritten) + " frames)");
    return true;
}

bool AudioProcessor::convertToMono(const std::vector<float>& stereoData, std::vector<float>& monoData) {
    if (stereoData.size() % 2 != 0) {
        Logger::getInstance().error("Invalid stereo data size");
        return false;
    }
    
    size_t frameCount = stereoData.size() / 2;
    monoData.resize(frameCount);
    
    for (size_t i = 0; i < frameCount; i++) {
        float left = stereoData[i * 2];
        float right = stereoData[i * 2 + 1];
        monoData[i] = (left + right) * 0.5f;
    }
    
    Logger::getInstance().debug("Converted stereo to mono: " + std::to_string(frameCount) + " frames");
    return true;
}

bool AudioProcessor::convertBitDepth(const std::vector<float>& inputData, std::vector<float>& outputData, int targetBitDepth) {
    // The actual bit depth conversion happens during saveAudioFile
    // This function is called to indicate that conversion is needed
    outputData = inputData;
    
    Logger::getInstance().debug("Preparing bit depth conversion to " + std::to_string(targetBitDepth) + "-bit");
    return true;
}

bool AudioProcessor::convertToPCM(const std::string& inputFile, const std::string& outputFile) {
    std::vector<float> audioData;
    AudioInfo info;
    
    if (!loadAudioFile(inputFile, audioData, info)) {
        return false;
    }
    
    if (!saveAudioFile(outputFile, audioData, info)) {
        return false;
    }
    
    Logger::getInstance().debug("Converted to PCM: " + inputFile + " -> " + outputFile);
    return true;
}

bool AudioProcessor::isValidAudioFile(const std::string& filepath) {
    SF_INFO sfInfo;
    SNDFILE* file = sf_open(filepath.c_str(), SFM_READ, &sfInfo);
    
    if (file) {
        sf_close(file);
        return true;
    }
    
    return false;
}

bool AudioProcessor::isPCMFormat(const std::string& filepath) {
    SF_INFO sfInfo;
    SNDFILE* file = sf_open(filepath.c_str(), SFM_READ, &sfInfo);
    
    if (file) {
        bool isPCM = (sfInfo.format & SF_FORMAT_PCM_16) || (sfInfo.format & SF_FORMAT_PCM_24) || (sfInfo.format & SF_FORMAT_PCM_32);
        sf_close(file);
        return isPCM;
    }
    
    return false;
}

bool AudioProcessor::isSupportedFormat(const std::string& filepath) {
    std::string ext = filepath.substr(filepath.find_last_of('.'));
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return (ext == ".wav" || ext == ".aif" || ext == ".aiff" || 
            ext == ".flac" || ext == ".ogg" || ext == ".mp3");
}

float AudioProcessor::calculateRMS(const std::vector<float>& audioData) {
    if (audioData.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float sample : audioData) {
        sum += sample * sample;
    }
    
    return std::sqrt(sum / audioData.size());
}

bool AudioProcessor::isSilent(const std::vector<float>& audioData, float threshold) {
    return calculateRMS(audioData) < threshold;
}

bool AudioProcessor::initializeAppleSilicon() {
    #ifdef __APPLE__
    if (m_impl && m_impl->appleSiliconProcessor.initialize()) {
        m_appleSiliconInitialized = true;
        Logger::getInstance().info("Apple Silicon processor initialized");
        return true;
    }
    Logger::getInstance().warning("Apple Silicon processor initialization failed");
    return false;
    #else
    Logger::getInstance().warning("Apple Silicon optimizations not available on this platform");
    return false;
    #endif
}

bool AudioProcessor::processWithAppleSilicon(const std::vector<float>& inputData, std::vector<float>& outputData, const std::string& operation) {
    #ifdef __APPLE__
    if (!m_appleSiliconInitialized || !m_impl) {
        Logger::getInstance().warning("Apple Silicon processor not initialized");
        return false;
    }
    
    // Use Apple Silicon optimized processing
    bool success = m_impl->appleSiliconProcessor.processAudio(inputData, outputData, 2, operation);
    
    if (success) {
        Logger::getInstance().debug("Processed with Apple Silicon: " + operation);
    } else {
        Logger::getInstance().warning("Apple Silicon processing failed for operation: " + operation);
    }
    
    return success;
    #else
    Logger::getInstance().warning("Apple Silicon optimizations not available on this platform");
    return false;
    #endif
}

std::vector<float> AudioProcessor::interleaveChannels(const std::vector<float>& left, const std::vector<float>& right) {
    std::vector<float> stereo;
    stereo.reserve(left.size() + right.size());
    
    for (size_t i = 0; i < left.size(); i++) {
        stereo.push_back(left[i]);
        stereo.push_back(right[i]);
    }
    
    return stereo;
}

std::pair<std::vector<float>, std::vector<float>> AudioProcessor::deinterleaveChannels(const std::vector<float>& stereoData) {
    std::vector<float> left, right;
    left.reserve(stereoData.size() / 2);
    right.reserve(stereoData.size() / 2);
    
    for (size_t i = 0; i < stereoData.size(); i += 2) {
        left.push_back(stereoData[i]);
        right.push_back(stereoData[i + 1]);
    }
    
    return {left, right};
}

float AudioProcessor::linearToDB(float linear) {
    return 20.0f * std::log10(std::max(linear, 1e-6f));
}

float AudioProcessor::dbToLinear(float db) {
    return std::pow(10.0f, db / 20.0f);
}

int AudioProcessor::getBitDepth(int format) {
    if (format & SF_FORMAT_PCM_16) return 16;
    if (format & SF_FORMAT_PCM_24) return 24;
    if (format & SF_FORMAT_PCM_32) return 32;
    return 16; // Default
}
