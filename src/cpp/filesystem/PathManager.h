#pragma once

#include <string>
#include <filesystem>

class PathManager {
public:
    PathManager() = default;
    
    // Core functionality: shorten filename by converting separators to camelCase
    std::string shortenFilename(const std::string& filename);
    
    // Generate output path preserving directory structure
    std::string generateOutputPath(const std::string& inputPath, 
                                   const std::string& sourceRoot,
                                   const std::string& outputRoot);
    
    // Generate flattened output path (inspired by M8 Sample Organizer)
    std::string generateFlattenedOutputPath(const std::string& inputPath,
                                           const std::string& sourceRoot,
                                           const std::string& outputRoot);
    
    // Folder flattening helper methods (inspired by M8 Sample Organizer)
    std::string flattenFolderStructure(const std::string& inputPath,
                                       const std::string& sourceRoot);
    std::string cleanFolderName(const std::string& folderName);
    std::string removeDuplicateWords(const std::string& text);
    std::string removeFillerWords(const std::string& text);
    
private:
    // Convert underscores/hyphens to camelCase
    std::string toCamelCase(const std::string& str);
    
    // Extract extension from filename
    std::string extractExtension(const std::string& filename);
    
    // Remove extension from filename
    std::string removeExtension(const std::string& filename);
};
