#pragma once

#include <string>
#include <vector>

class FileOperations {
public:
    FileOperations();
    ~FileOperations();
    
    // File operations
    bool copyFile(const std::string& source, const std::string& destination);
    bool moveFile(const std::string& source, const std::string& destination);
    bool deleteFile(const std::string& filepath);
    
    // Directory operations
    bool createDirectory(const std::string& path);
    bool createDirectories(const std::string& path);
    bool deleteDirectory(const std::string& path);
    
    // Batch operations
    bool copyFiles(const std::vector<std::string>& sources, const std::string& destination);
    bool moveFiles(const std::vector<std::string>& sources, const std::string& destination);
    
    // Safety checks
    bool isSafeToCopy(const std::string& source, const std::string& destination);
    bool isSafeToMove(const std::string& source, const std::string& destination);
    
    // Status
    std::string getLastError() const { return m_lastError; }
    
private:
    std::string m_lastError;
    
    void setError(const std::string& error);
};
