#include "FileScanner.h"
#include "Logger.h"
#include <filesystem>
#include <algorithm>

FileScanner::FileScanner() {
    Logger::getInstance().debug("FileScanner initialized");
}

FileScanner::~FileScanner() = default;

std::vector<AudioFile> FileScanner::scanDirectory(const std::string& directory, const std::vector<std::string>& ignoreFolders) {
    std::vector<AudioFile> results;
    m_cancelled = false;
    m_totalFiles = 0;
    m_validFiles = 0;
    m_skippedFiles = 0;
    
    Logger::getInstance().info("Scanning directory: " + directory);
    scanDirectoryRecursive(directory, ignoreFolders, results);
    
    Logger::getInstance().info("Scan complete: " + std::to_string(results.size()) + " files found");
    return results;
}

std::vector<AudioFile> FileScanner::scanFileList(const std::string& fileListPath) {
    std::vector<AudioFile> results;
    // Implementation for reading from file list
    return results;
}

void FileScanner::setProgressCallback(std::function<void(size_t, size_t)> callback) {
    m_progressCallback = callback;
}

void FileScanner::setFileCallback(std::function<void(const AudioFile&)> callback) {
    m_fileCallback = callback;
}

void FileScanner::setAllowedExtensions(const std::vector<std::string>& extensions) {
    m_allowedExtensions = extensions;
}

void FileScanner::setMaxFileSize(size_t maxSize) {
    m_maxFileSize = maxSize;
}

void FileScanner::setMinFileSize(size_t minSize) {
    m_minFileSize = minSize;
}

bool FileScanner::isValidAudioFile(const std::string& filepath) {
    if (!checkFileExists(filepath)) return false;
    if (!checkFilePermissions(filepath)) return false;
    
    std::string ext = filepath.substr(filepath.find_last_of('.'));
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return isSupportedExtension(ext);
}

bool FileScanner::isSupportedExtension(const std::string& extension) {
    return std::find(m_allowedExtensions.begin(), m_allowedExtensions.end(), extension) != m_allowedExtensions.end();
}

bool FileScanner::isFileSizeValid(size_t fileSize) {
    if (m_minFileSize > 0 && fileSize < m_minFileSize) return false;
    if (m_maxFileSize > 0 && fileSize > m_maxFileSize) return false;
    return true;
}

std::string FileScanner::extractPackName(const std::string& filepath, const std::string& rootDirectory) {
    std::string relativePath = filepath.substr(rootDirectory.length());
    size_t firstSlash = relativePath.find('/');
    if (firstSlash != std::string::npos) {
        return relativePath.substr(1, firstSlash - 1);
    }
    return "Unknown";
}

std::string FileScanner::sanitizePath(const std::string& path) {
    std::string sanitized = path;
    std::replace(sanitized.begin(), sanitized.end(), '\\', '/');
    return sanitized;
}

void FileScanner::cancel() {
    m_cancelled = true;
}

void FileScanner::scanDirectoryRecursive(const std::string& directory, 
                                        const std::vector<std::string>& ignoreFolders,
                                        std::vector<AudioFile>& results) {
    try {
        std::filesystem::recursive_directory_iterator iter(directory, 
            std::filesystem::directory_options::skip_permission_denied);
        
        for (const auto& entry : iter) {
            if (m_cancelled) break;
            
            try {
                if (entry.is_directory()) {
                    // Check if this directory should be ignored
                    std::string dirname = entry.path().filename().string();
                    if (shouldIgnoreDirectory(dirname, ignoreFolders)) {
                        iter.disable_recursion_pending();
                        Logger::getInstance().debug("Skipping ignored directory: " + dirname);
                        continue;
                    }
                }
                
                if (entry.is_regular_file()) {
                    std::string filepath = entry.path().string();
                    std::string filename = entry.path().filename().string();
                    
                    m_totalFiles++;
                    
                    if (isValidAudioFile(filepath)) {
                        AudioFile audioFile = createAudioFile(filepath, directory);
                        if (isFileSizeValid(audioFile.fileSize)) {
                            results.push_back(audioFile);
                            m_validFiles++;
                            
                            if (m_fileCallback) {
                                m_fileCallback(audioFile);
                            }
                        } else {
                            m_skippedFiles++;
                        }
                    } else {
                        m_skippedFiles++;
                    }
                    
                    if (m_progressCallback) {
                        m_progressCallback(m_validFiles, m_totalFiles);
                    }
                }
            } catch (const std::filesystem::filesystem_error& e) {
                // Skip individual files that cause errors (permission denied, symlinks, etc.)
                Logger::getInstance().warning("Skipping file/dir due to error: " + std::string(e.what()));
                continue;
            }
        }
    } catch (const std::exception& e) {
        Logger::getInstance().error("Error scanning directory: " + std::string(e.what()));
    }
}

bool FileScanner::shouldIgnoreDirectory(const std::string& dirname, const std::vector<std::string>& ignoreFolders) {
    std::string lowerDirname = dirname;
    std::transform(lowerDirname.begin(), lowerDirname.end(), lowerDirname.begin(), ::tolower);
    
    for (const auto& ignoreFolder : ignoreFolders) {
        std::string lowerIgnore = ignoreFolder;
        std::transform(lowerIgnore.begin(), lowerIgnore.end(), lowerIgnore.begin(), ::tolower);
        
        if (lowerDirname == lowerIgnore) {
            return true;
        }
    }
    
    return false;
}

AudioFile FileScanner::createAudioFile(const std::string& filepath, const std::string& rootDirectory) {
    AudioFile file;
    file.filepath = filepath;
    file.filename = filepath.substr(filepath.find_last_of('/') + 1);
    file.extension = filepath.substr(filepath.find_last_of('.'));
    file.fileSize = getFileSize(filepath);
    file.packName = extractPackName(filepath, rootDirectory);
    file.isProcessed = false;
    
    return file;
}

bool FileScanner::checkFileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

bool FileScanner::checkFilePermissions(const std::string& filepath) {
    return std::filesystem::is_regular_file(filepath);
}

size_t FileScanner::getFileSize(const std::string& filepath) {
    try {
        return std::filesystem::file_size(filepath);
    } catch (const std::exception&) {
        return 0;
    }
}
