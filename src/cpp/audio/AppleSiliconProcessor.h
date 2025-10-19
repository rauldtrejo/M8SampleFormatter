#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>
#endif

class AppleSiliconProcessor {
public:
    AppleSiliconProcessor();
    ~AppleSiliconProcessor();

    bool initialize();
    bool isAvailable() const { return m_available; }
    std::string getLastError() const { return m_lastError; }

    // Process audio data with Apple Silicon optimizations
    // inputData: interleaved float samples
    // outputData: will contain processed interleaved float samples
    // numChannels: number of channels in inputData (e.g., 1 for mono, 2 for stereo)
    // operation: "mono", "16bit", "normalize", "batch"
    bool processAudio(const std::vector<float>& inputData, std::vector<float>& outputData, int numChannels, const std::string& operation);

    // Specific operations optimized for Apple Silicon
    bool convertToMono(const std::vector<float>& stereoData, std::vector<float>& monoData);
    bool convertTo16Bit(const std::vector<float>& floatData, std::vector<short>& int16Data);
    bool normalize(const std::vector<float>& inputData, std::vector<float>& outputData);
    bool batchProcess(const std::vector<float>& inputData, std::vector<float>& outputData, int numChannels);

    // Performance metrics
    double getLastProcessingTime() const { return m_lastProcessingTime; }
    size_t getProcessedFrames() const { return m_processedFrames; }
    size_t getProcessedBytes() const { return m_processedBytes; }

    // Apple Silicon specific optimizations
    bool enableVectorization(bool enable = true);
    bool setThreadCount(int threadCount);
    int getOptimalThreadCount() const;

private:
    bool m_available = false;
    bool m_vectorizationEnabled = true;
    int m_threadCount = 0; // 0 = auto-detect
    std::string m_lastError;
    double m_lastProcessingTime = 0.0;
    size_t m_processedFrames = 0;
    size_t m_processedBytes = 0;
    
    #ifdef __APPLE__
    // Apple Silicon specific data structures
    vDSP_Length m_optimalVectorLength = 0;
    vDSP_Stride m_stride = 1;
    #endif
    
    // Internal methods
    bool setupAppleSilicon();
    void setError(const std::string& error);
    void cleanup();
    
    // Vectorized operations
    bool vectorizedMonoConversion(const std::vector<float>& stereoData, std::vector<float>& monoData);
    bool vectorizedNormalization(const std::vector<float>& inputData, std::vector<float>& outputData);
    bool vectorizedBatchProcess(const std::vector<float>& inputData, std::vector<float>& outputData, int numChannels);
};
