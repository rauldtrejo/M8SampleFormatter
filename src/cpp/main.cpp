#include "utils/Logger.h"
#include "utils/ThreadPool.h"
#include "filesystem/FileScanner.h"
#include "filesystem/PathManager.h"
#include "audio/AudioProcessor.h"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <atomic>

class M8SampleFormatter {
public:
    struct ProcessingOptions {
        bool convertBitDepth = true;
        int targetBitDepth = 16;
        bool flattenFolders = false;  // New option for folder flattening
    };
    
    struct ProcessingStats {
        size_t totalFiles = 0;
        size_t processedFiles = 0;
        size_t errorFiles = 0;
        size_t convertedBitDepth = 0;
        double processingTime = 0.0;
    };
    
    M8SampleFormatter() 
        : m_logger(Logger::getInstance()), 
          m_threadPool(std::thread::hardware_concurrency() * 2) {
    }
    
    bool processDirectory(const std::string& sourceDir, const std::string& outputDir, const ProcessingOptions& options) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        m_options = options;
        m_logger.info("=== M8 Sample Formatter ===");
        m_logger.info("Source directory: " + sourceDir);
        m_logger.info("Output directory: " + outputDir);
        
        // Scan source directory
        m_logger.info("Scanning directory...");
        std::vector<std::string> ignoreFolders = {".DS_Store", ".Trashes", ".Spotlight-V100", ".fseventsd"};
        auto audioFiles = m_fileScanner.scanDirectory(sourceDir, ignoreFolders);
        
        if (audioFiles.empty()) {
            m_logger.error("No audio files found in source directory");
            return false;
        }
        
        m_stats.totalFiles = audioFiles.size();
        m_logger.info("Found " + std::to_string(audioFiles.size()) + " audio files");
        
        // Process files in parallel with real-time progress tracking
        m_logger.info("Processing files...");
        std::vector<std::future<bool>> futures;
        std::atomic<size_t> completedTasks{0};
        std::atomic<size_t> processedFiles{0};
        std::atomic<size_t> errorFiles{0};
        
        for (const auto& audioFile : audioFiles) {
            futures.push_back(m_threadPool.enqueue([this, audioFile, sourceDir, outputDir, &completedTasks, &processedFiles, &errorFiles, &audioFiles]() {
                bool success = processFile(audioFile, sourceDir, outputDir);
                size_t currentCompleted = completedTasks.fetch_add(1) + 1;
                
                if (success) {
                    processedFiles.fetch_add(1);
                } else {
                    errorFiles.fetch_add(1);
                }
                
                // Send progress update
                double progress = static_cast<double>(currentCompleted) / static_cast<double>(audioFiles.size());
                m_logger.info("Progress: " + std::to_string(static_cast<int>(progress * 100)) + "% (" + 
                             std::to_string(currentCompleted) + "/" + std::to_string(audioFiles.size()) + " files)");
                
                return success;
            }));
        }
        
        // Wait for all tasks to complete
        for (auto& future : futures) {
            future.get();
        }
        
        // Update stats
        m_stats.processedFiles = processedFiles.load();
        m_stats.errorFiles = errorFiles.load();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        m_stats.processingTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Print summary
        printSummary();
        
        // Output final stats for GUI
        m_logger.info("FINAL_STATS: " + std::to_string(m_stats.totalFiles) + " " + 
                     std::to_string(m_stats.processedFiles) + " " + 
                     std::to_string(m_stats.errorFiles) + " " + 
                     std::to_string(m_stats.processingTime));
        
        return true;
    }
    
private:
    Logger& m_logger;
    FileScanner m_fileScanner;
    PathManager m_pathManager;
    AudioProcessor m_audioProcessor;
    ThreadPool m_threadPool;
    ProcessingOptions m_options;
    ProcessingStats m_stats;
    
    bool processFile(const AudioFile& audioFile, const std::string& sourceDir, const std::string& outputDir) {
        try {
            m_logger.info("Processing: " + audioFile.filename);
            
            // Generate output path (preserves or flattens directory structure)
            std::string outputPath;
            if (m_options.flattenFolders) {
                outputPath = m_pathManager.generateFlattenedOutputPath(
                    audioFile.filepath,
                    sourceDir,
                    outputDir
                );
            } else {
                outputPath = m_pathManager.generateOutputPath(
                    audioFile.filepath,
                    sourceDir,
                    outputDir
                );
            }
            
            // Create output directory if it doesn't exist
            std::filesystem::path outPath(outputPath);
            std::filesystem::create_directories(outPath.parent_path());
            
            // Load audio file
            std::vector<float> audioData;
            AudioInfo audioInfo;
            
            if (!m_audioProcessor.loadAudioFile(audioFile.filepath, audioData, audioInfo)) {
                m_logger.error("Failed to load audio file: " + audioFile.filepath);
                return false;
            }
            
            // Convert bit depth if needed
            if (m_options.convertBitDepth && audioInfo.bitDepth != m_options.targetBitDepth) {
                std::vector<float> convertedData;
                if (m_audioProcessor.convertBitDepth(audioData, convertedData, m_options.targetBitDepth)) {
                    audioData = std::move(convertedData);
                    audioInfo.bitDepth = m_options.targetBitDepth;
                    m_stats.convertedBitDepth++;
                    m_logger.debug("Converted to " + std::to_string(m_options.targetBitDepth) + "-bit: " + audioFile.filename);
                }
            }
            
            // Save processed audio file
            if (!m_audioProcessor.saveAudioFile(outputPath, audioData, audioInfo)) {
                m_logger.error("Failed to save audio file: " + outputPath);
                return false;
            }
            
            m_logger.debug("Saved: " + outputPath);
            return true;
            
        } catch (const std::exception& e) {
            m_logger.error("Error processing file " + audioFile.filename + ": " + std::string(e.what()));
            return false;
        }
    }
    
    void printSummary() {
        m_logger.info("\n=== Processing Complete ===");
        m_logger.info("Total files: " + std::to_string(m_stats.totalFiles));
        m_logger.info("Processed: " + std::to_string(m_stats.processedFiles));
        m_logger.info("Errors: " + std::to_string(m_stats.errorFiles));
        m_logger.info("Converted bit depth: " + std::to_string(m_stats.convertedBitDepth));
        m_logger.info("Processing time: " + std::to_string(m_stats.processingTime) + " seconds");
        
        if (m_stats.processingTime > 0) {
            double filesPerSecond = m_stats.totalFiles / m_stats.processingTime;
            m_logger.info("Average speed: " + std::to_string(filesPerSecond) + " files/second");
        }
    }
    
    void saveReport(const std::string& outputDir) {
        std::string reportPath = outputDir + "/processing_report.txt";
        std::ofstream report(reportPath);
        
        report << "M8 Sample Formatter - Processing Report\n";
        report << "========================================\n\n";
        report << "Total files: " << m_stats.totalFiles << "\n";
        report << "Processed: " << m_stats.processedFiles << "\n";
        report << "Errors: " << m_stats.errorFiles << "\n";
        report << "Converted bit depth: " << m_stats.convertedBitDepth << "\n";
        report << "Processing time: " << m_stats.processingTime << " seconds\n";
        
        if (m_stats.processingTime > 0) {
            report << "Average speed: " << (m_stats.totalFiles / m_stats.processingTime) << " files/second\n";
        }
        
        report.close();
        m_logger.info("Processing report saved to: " + reportPath);
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <source_directory> <output_directory> [--no-bitdepth] [--flatten-folders]" << std::endl;
        return 1;
    }
    
    std::string sourceDir = argv[1];
    std::string outputDir = argv[2];
    
    // Parse options
    M8SampleFormatter::ProcessingOptions options;
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--no-bitdepth") {
            options.convertBitDepth = false;
        } else if (arg == "--flatten-folders") {
            options.flattenFolders = true;
        }
    }
    
    // Create formatter and process
    M8SampleFormatter formatter;
    bool success = formatter.processDirectory(sourceDir, outputDir, options);
    
    return success ? 0 : 1;
}
