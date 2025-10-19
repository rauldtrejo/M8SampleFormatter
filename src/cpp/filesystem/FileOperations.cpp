#include "FileOperations.h"
#include "Logger.h"
#include <filesystem>
#include <fstream>

FileOperations::FileOperations() {
    Logger::getInstance().debug("FileOperations initialized");
}

FileOperations::~FileOperations() = default;

bool FileOperations::copyFile(const std::string& source, const std::string& destination) {
    try {
        std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
        Logger::getInstance().debug("Copied file: " + source + " -> " + destination);
        return true;
    } catch (const std::exception& e) {
        setError("Failed to copy file: " + std::string(e.what()));
        return false;
    }
}

bool FileOperations::moveFile(const std::string& source, const std::string& destination) {
    try {
        std::filesystem::rename(source, destination);
        Logger::getInstance().debug("Moved file: " + source + " -> " + destination);
        return true;
    } catch (const std::exception& e) {
        setError("Failed to move file: " + std::string(e.what()));
        return false;
    }
}

bool FileOperations::deleteFile(const std::string& filepath) {
    try {
        std::filesystem::remove(filepath);
        Logger::getInstance().debug("Deleted file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        setError("Failed to delete file: " + std::string(e.what()));
        return false;
    }
}

bool FileOperations::createDirectory(const std::string& path) {
    try {
        std::filesystem::create_directory(path);
        Logger::getInstance().debug("Created directory: " + path);
        return true;
    } catch (const std::exception& e) {
        setError("Failed to create directory: " + std::string(e.what()));
        return false;
    }
}

bool FileOperations::createDirectories(const std::string& path) {
    try {
        std::filesystem::create_directories(path);
        Logger::getInstance().debug("Created directories: " + path);
        return true;
    } catch (const std::exception& e) {
        setError("Failed to create directories: " + std::string(e.what()));
        return false;
    }
}

bool FileOperations::deleteDirectory(const std::string& path) {
    try {
        std::filesystem::remove_all(path);
        Logger::getInstance().debug("Deleted directory: " + path);
        return true;
    } catch (const std::exception& e) {
        setError("Failed to delete directory: " + std::string(e.what()));
        return false;
    }
}

bool FileOperations::copyFiles(const std::vector<std::string>& sources, const std::string& destination) {
    bool success = true;
    
    for (const auto& source : sources) {
        if (!copyFile(source, destination)) {
            success = false;
        }
    }
    
    return success;
}

bool FileOperations::moveFiles(const std::vector<std::string>& sources, const std::string& destination) {
    bool success = true;
    
    for (const auto& source : sources) {
        if (!moveFile(source, destination)) {
            success = false;
        }
    }
    
    return success;
}

bool FileOperations::isSafeToCopy(const std::string& source, const std::string& destination) {
    if (!std::filesystem::exists(source)) {
        setError("Source file does not exist: " + source);
        return false;
    }
    
    if (std::filesystem::exists(destination)) {
        setError("Destination file already exists: " + destination);
        return false;
    }
    
    return true;
}

bool FileOperations::isSafeToMove(const std::string& source, const std::string& destination) {
    if (!std::filesystem::exists(source)) {
        setError("Source file does not exist: " + source);
        return false;
    }
    
    if (std::filesystem::exists(destination)) {
        setError("Destination file already exists: " + destination);
        return false;
    }
    
    return true;
}

void FileOperations::setError(const std::string& error) {
    m_lastError = error;
    Logger::getInstance().error("FileOperations: " + error);
}
