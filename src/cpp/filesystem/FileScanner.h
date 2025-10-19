#pragma once

#include <string>
#include <vector>
#include <functional>
#include <atomic>

struct AudioFile {
    std::string filepath;
    std::string filename;
    std::string extension;
    size_t fileSize;
    std::string packName;
    bool isProcessed = false;
    std::string error;
};

class FileScanner {
public:
    FileScanner();
    ~FileScanner();
    
    // Scanning
    std::vector<AudioFile> scanDirectory(const std::string& directory, const std::vector<std::string>& ignoreFolders = {});
    std::vector<AudioFile> scanFileList(const std::string& fileListPath);
    
    // Progress tracking
    void setProgressCallback(std::function<void(size_t, size_t)> callback);
    void setFileCallback(std::function<void(const AudioFile&)> callback);
    
    // Filtering
    void setAllowedExtensions(const std::vector<std::string>& extensions);
    void setMaxFileSize(size_t maxSize);
    void setMinFileSize(size_t minSize);
    
    // Validation
    bool isValidAudioFile(const std::string& filepath);
    bool isSupportedExtension(const std::string& extension);
    bool isFileSizeValid(size_t fileSize);
    
    // Path operations
    std::string extractPackName(const std::string& filepath, const std::string& rootDirectory);
    std::string sanitizePath(const std::string& path);
    
    // Statistics
    size_t getTotalFiles() const { return m_totalFiles; }
    size_t getValidFiles() const { return m_validFiles; }
    size_t getSkippedFiles() const { return m_skippedFiles; }
    
    // Control
    void cancel();
    bool isCancelled() const { return m_cancelled.load(); }
    
private:
    std::vector<std::string> m_allowedExtensions = {".wav", ".aif", ".aiff", ".flac", ".ogg", ".mp3"};
    size_t m_maxFileSize = 0; // 0 = no limit
    size_t m_minFileSize = 0;
    
    std::function<void(size_t, size_t)> m_progressCallback;
    std::function<void(const AudioFile&)> m_fileCallback;
    
    std::atomic<bool> m_cancelled{false};
    std::atomic<size_t> m_totalFiles{0};
    std::atomic<size_t> m_validFiles{0};
    std::atomic<size_t> m_skippedFiles{0};
    
    // Internal scanning
    void scanDirectoryRecursive(const std::string& directory, 
                               const std::vector<std::string>& ignoreFolders,
                               std::vector<AudioFile>& results);
    
    bool shouldIgnoreDirectory(const std::string& dirname, const std::vector<std::string>& ignoreFolders);
    AudioFile createAudioFile(const std::string& filepath, const std::string& rootDirectory);
    
    // File validation
    bool checkFileExists(const std::string& filepath);
    bool checkFilePermissions(const std::string& filepath);
    size_t getFileSize(const std::string& filepath);
};
