#include "AppleSiliconProcessor.h"
#include "Logger.h"
#include <algorithm>
#include <cmath>

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>
#include <sys/sysctl.h>
#endif

AppleSiliconProcessor::AppleSiliconProcessor() {
    Logger::getInstance().debug("AppleSiliconProcessor constructor");
}

AppleSiliconProcessor::~AppleSiliconProcessor() {
    cleanup();
    Logger::getInstance().debug("AppleSiliconProcessor destructor");
}

bool AppleSiliconProcessor::initialize() {
    if (m_available) {
        Logger::getInstance().info("AppleSiliconProcessor already initialized.");
        return true;
    }

    #ifdef __APPLE__
    if (!setupAppleSilicon()) {
        setError("Failed to set up Apple Silicon optimizations.");
        return false;
    }
    m_available = true;
    Logger::getInstance().info("AppleSiliconProcessor initialized successfully.");
    return true;
    #else
    Logger::getInstance().warning("Apple Silicon optimizations are only available on Apple platforms.");
    m_available = false;
    return false;
    #endif
}

bool AppleSiliconProcessor::processAudio(const std::vector<float>& inputData, std::vector<float>& outputData, int numChannels, const std::string& operation) {
    if (!m_available) {
        setError("AppleSiliconProcessor not initialized or not available.");
        return false;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    bool success = false;
    if (operation == "mono") {
        success = convertToMono(inputData, outputData);
    } else if (operation == "normalize") {
        success = normalize(inputData, outputData);
    } else if (operation == "batch") {
        success = batchProcess(inputData, outputData, numChannels);
    } else {
        setError("Unsupported operation: " + operation);
        return false;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    m_lastProcessingTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    m_processedFrames = outputData.size();
    m_processedBytes = outputData.size() * sizeof(float);

    return success;
}

bool AppleSiliconProcessor::convertToMono(const std::vector<float>& stereoData, std::vector<float>& monoData) {
    if (stereoData.size() % 2 != 0) {
        setError("Stereo data must have even number of samples.");
        return false;
    }

    size_t monoSize = stereoData.size() / 2;
    monoData.resize(monoSize);

    #ifdef __APPLE__
    if (m_vectorizationEnabled) {
        return vectorizedMonoConversion(stereoData, monoData);
    }
    #endif

    // Fallback to scalar implementation
    for (size_t i = 0; i < monoSize; ++i) {
        monoData[i] = (stereoData[i * 2] + stereoData[i * 2 + 1]) * 0.5f;
    }

    return true;
}

bool AppleSiliconProcessor::convertTo16Bit(const std::vector<float>& floatData, std::vector<short>& int16Data) {
    int16Data.resize(floatData.size());

    #ifdef __APPLE__
    if (m_vectorizationEnabled) {
        // Use vDSP for vectorized conversion
        // First convert float to 16-bit range, then to short
        std::vector<float> tempFloat(floatData.size());
        float scale = 32767.0f;
        vDSP_vsmul(floatData.data(), 1, &scale, tempFloat.data(), 1, floatData.size());
        
        // Convert float to short
        vDSP_vfix16(tempFloat.data(), 1, int16Data.data(), 1, floatData.size());
        return true;
    }
    #endif

    // Fallback to scalar implementation
    for (size_t i = 0; i < floatData.size(); ++i) {
        int16Data[i] = static_cast<short>(floatData[i] * 32767.0f);
    }

    return true;
}

bool AppleSiliconProcessor::normalize(const std::vector<float>& inputData, std::vector<float>& outputData) {
    if (inputData.empty()) {
        setError("Input data is empty.");
        return false;
    }

    outputData.resize(inputData.size());

    #ifdef __APPLE__
    if (m_vectorizationEnabled) {
        return vectorizedNormalization(inputData, outputData);
    }
    #endif

    // Fallback to scalar implementation
    float maxVal = 0.0f;
    for (float sample : inputData) {
        maxVal = std::max(maxVal, std::abs(sample));
    }

    if (maxVal == 0.0f) {
        outputData = inputData;
        return true;
    }

    float scale = 1.0f / maxVal;
    for (size_t i = 0; i < inputData.size(); ++i) {
        outputData[i] = inputData[i] * scale;
    }

    return true;
}

bool AppleSiliconProcessor::batchProcess(const std::vector<float>& inputData, std::vector<float>& outputData, int numChannels) {
    if (!m_available) {
        setError("AppleSiliconProcessor not initialized or not available.");
        return false;
    }

    #ifdef __APPLE__
    if (m_vectorizationEnabled) {
        return vectorizedBatchProcess(inputData, outputData, numChannels);
    }
    #endif

    // Fallback to scalar implementation
    outputData = inputData;
    return true;
}

bool AppleSiliconProcessor::enableVectorization(bool enable) {
    m_vectorizationEnabled = enable;
    Logger::getInstance().info("Vectorization " + std::string(enable ? "enabled" : "disabled"));
    return true;
}

bool AppleSiliconProcessor::setThreadCount(int threadCount) {
    m_threadCount = threadCount;
    Logger::getInstance().info("Thread count set to " + std::to_string(threadCount));
    return true;
}

int AppleSiliconProcessor::getOptimalThreadCount() const {
    #ifdef __APPLE__
    int numCores = 0;
    size_t size = sizeof(numCores);
    if (sysctlbyname("hw.ncpu", &numCores, &size, nullptr, 0) == 0) {
        return numCores;
    }
    #endif
    return 4; // Fallback
}

void AppleSiliconProcessor::setError(const std::string& error) {
    m_lastError = error;
    Logger::getInstance().error("AppleSiliconProcessor: " + error);
}

void AppleSiliconProcessor::cleanup() {
    m_available = false;
    Logger::getInstance().debug("AppleSiliconProcessor cleaned up");
}

#ifdef __APPLE__
bool AppleSiliconProcessor::setupAppleSilicon() {
    // Detect optimal vector length for this Apple Silicon chip
    m_optimalVectorLength = 1024; // Default, will be optimized based on performance
    
    // Set optimal thread count
    if (m_threadCount == 0) {
        m_threadCount = getOptimalThreadCount();
    }
    
    Logger::getInstance().debug("Apple Silicon setup complete - Vector length: " + 
                               std::to_string(m_optimalVectorLength) + 
                               ", Threads: " + std::to_string(m_threadCount));
    return true;
}

bool AppleSiliconProcessor::vectorizedMonoConversion(const std::vector<float>& stereoData, std::vector<float>& monoData) {
    size_t monoSize = stereoData.size() / 2;
    monoData.resize(monoSize);

    // Use vDSP for vectorized stereo-to-mono conversion
    // This is much faster than scalar operations on Apple Silicon
    vDSP_vadd(stereoData.data(), 2, stereoData.data() + 1, 2, monoData.data(), 1, monoSize);
    
    // Scale by 0.5
    float scale = 0.5f;
    vDSP_vsmul(monoData.data(), 1, &scale, monoData.data(), 1, monoSize);

    return true;
}

bool AppleSiliconProcessor::vectorizedNormalization(const std::vector<float>& inputData, std::vector<float>& outputData) {
    outputData.resize(inputData.size());

    // Find maximum absolute value using vDSP
    float maxVal = 0.0f;
    vDSP_maxmgv(inputData.data(), 1, &maxVal, inputData.size());

    if (maxVal == 0.0f) {
        outputData = inputData;
        return true;
    }

    // Normalize using vDSP
    float scale = 1.0f / maxVal;
    vDSP_vsmul(inputData.data(), 1, &scale, outputData.data(), 1, inputData.size());

    return true;
}

bool AppleSiliconProcessor::vectorizedBatchProcess(const std::vector<float>& inputData, std::vector<float>& outputData, int numChannels) {
    outputData.resize(inputData.size());

    if (numChannels == 2) {
        // Convert stereo to mono using vectorized operations
        size_t monoSize = inputData.size() / 2;
        std::vector<float> monoData(monoSize);
        
        vDSP_vadd(inputData.data(), 2, inputData.data() + 1, 2, monoData.data(), 1, monoSize);
        
        float scale = 0.5f;
        vDSP_vsmul(monoData.data(), 1, &scale, monoData.data(), 1, monoSize);
        
        // Interleave mono data back to stereo output
        for (size_t i = 0; i < monoSize; ++i) {
            outputData[i * 2] = monoData[i];
            outputData[i * 2 + 1] = monoData[i];
        }
    } else {
        // For mono input, just copy using vDSP_vadd with zero
        float zero = 0.0f;
        vDSP_vsadd(inputData.data(), 1, &zero, outputData.data(), 1, inputData.size());
    }

    return true;
}
#endif
