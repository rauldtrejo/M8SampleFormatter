#include "PathManager.h"
#include <algorithm>
#include <cctype>
#include <vector>
#include <functional>

std::string PathManager::shortenFilename(const std::string& filename) {
    std::string baseName = removeExtension(filename);
    // Always use .wav extension for output files
    std::string extension = ".wav";
    
    // Convert to camelCase (removes underscores, hyphens, and spaces)
    std::string shortened = toCamelCase(baseName);
    
    return shortened + extension;
}

std::string PathManager::generateOutputPath(const std::string& inputPath,
                                           const std::string& sourceRoot,
                                           const std::string& outputRoot) {
    // Get the relative path from source root
    std::filesystem::path input(inputPath);
    std::filesystem::path source(sourceRoot);
    std::filesystem::path output(outputRoot);
    
    // Get the source folder name and apply camelCase
    std::string sourceFolderName = source.filename().string();
    std::string topLevelFolder = toCamelCase(sourceFolderName);
    
    // Start with output root + camelCased source folder name
    std::filesystem::path processedPath = output / topLevelFolder;
    
    // Calculate relative path from source root
    std::filesystem::path relativePath = std::filesystem::relative(input.parent_path(), source);
    
    // Apply camelCase to each directory component in the relative path
    for (const auto& component : relativePath) {
        std::string dirName = component.string();
        // Skip "." which represents the current directory
        if (dirName != ".") {
            std::string shortenedDirName = toCamelCase(dirName);
            processedPath /= shortenedDirName;
        }
    }
    
    // Shorten the filename
    std::string shortenedFilename = shortenFilename(input.filename().string());
    
    // Return full output path
    return (processedPath / shortenedFilename).string();
}

std::string PathManager::toCamelCase(const std::string& str) {
    std::string result;
    bool capitalizeNext = false;
    bool firstChar = true;
    
    for (char c : str) {
        // Skip separators (underscore, hyphen, space)
        if (c == '_' || c == '-' || c == ' ') {
            capitalizeNext = true;
            continue;
        }
        
        // Handle alphanumeric characters
        if (std::isalnum(c)) {
            if (firstChar) {
                // Keep first character lowercase
                result += std::tolower(c);
                firstChar = false;
            } else if (capitalizeNext) {
                result += std::toupper(c);
                capitalizeNext = false;
            } else {
                result += c;
            }
        }
        // Keep # (for musical sharps like C#, F#)
        else if (c == '#') {
            result += c;
        }
        // Keep dots and other characters that might be part of version numbers, etc.
        else if (c == '.') {
            result += c;
        }
    }
    
    return result;
}

std::string PathManager::extractExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filename.length() - 1) {
        return filename.substr(dotPos);
    }
    return "";
}

std::string PathManager::removeExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filename.substr(0, dotPos);
    }
    return filename;
}

std::string PathManager::generateFlattenedOutputPath(const std::string& inputPath,
                                                     const std::string& sourceRoot,
                                                     const std::string& outputRoot) {
    // Get the source folder name and apply camelCase
    std::filesystem::path source(sourceRoot);
    std::string sourceFolderName = source.filename().string();
    std::string topLevelFolder = toCamelCase(sourceFolderName);
    
    // Start with output root + camelCased source folder name
    std::filesystem::path output(outputRoot);
    std::filesystem::path processedPath = output / topLevelFolder;
    
    // Flatten the folder structure
    std::string flattenedPath = this->flattenFolderStructure(inputPath, sourceRoot);
    std::string cleanedPath = this->cleanFolderName(flattenedPath);
    std::string camelCasedPath = toCamelCase(cleanedPath);
    
    // Add the flattened path as a single directory
    if (!camelCasedPath.empty()) {
        processedPath /= camelCasedPath;
    }
    
    // Shorten the filename
    std::filesystem::path input(inputPath);
    std::string shortenedFilename = shortenFilename(input.filename().string());
    
    // Return full output path
    return (processedPath / shortenedFilename).string();
}

std::string PathManager::flattenFolderStructure(const std::string& inputPath,
                                                const std::string& sourceRoot) {
    std::filesystem::path input(inputPath);
    std::filesystem::path source(sourceRoot);
    
    // Get relative path from source root
    std::filesystem::path relativePath = std::filesystem::relative(input.parent_path(), source);
    
    std::string result;
    bool first = true;
    
    // Concatenate all directory components into a single flattened path
    for (const auto& component : relativePath) {
        std::string dirName = component.string();
        // Skip "." which represents the current directory
        if (dirName != ".") {
            if (!first) {
                result += "_";
            }
            result += dirName;
            first = false;
        }
    }
    
    return result;
}

std::string PathManager::cleanFolderName(const std::string& folderName) {
    std::string cleaned = folderName;
    
    // Remove duplicate words
    cleaned = removeDuplicateWords(cleaned);
    
    // Remove filler words
    cleaned = removeFillerWords(cleaned);
    
    return cleaned;
}

std::string PathManager::removeDuplicateWords(const std::string& text) {
    std::string result;
    std::string currentWord;
    std::vector<std::string> words;
    
    // Split into words
    for (char c : text) {
        if (c == '_' || c == '-' || c == ' ') {
            if (!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord.clear();
            }
        } else {
            currentWord += c;
        }
    }
    if (!currentWord.empty()) {
        words.push_back(currentWord);
    }
    
    // Remove duplicates while preserving order
    std::vector<std::string> uniqueWords;
    for (const auto& word : words) {
        bool found = false;
        for (const auto& uniqueWord : uniqueWords) {
            if (word == uniqueWord) {
                found = true;
                break;
            }
        }
        if (!found) {
            uniqueWords.push_back(word);
        }
    }
    
    // Reconstruct the string
    for (size_t i = 0; i < uniqueWords.size(); ++i) {
        if (i > 0) {
            result += "_";
        }
        result += uniqueWords[i];
    }
    
    return result;
}

std::string PathManager::removeFillerWords(const std::string& text) {
    // Strike words inspired by M8 Sample Organizer Python version
    // Only remove truly redundant words, not meaningful musical terms
    std::vector<std::string> strikeWords = {
        "final", "sample", "label", "process", "edit", "pack", "wav", 
        "construct", "cpa", "splice", "export", "processed", "master", 
        "version", "v1", "v2", "v3", "v4", "v5", "new", "old", "backup", 
        "copy", "original", "edited", "mix", "remix", "remastered", "mastered"
    };
    
    std::string result;
    std::string currentWord;
    std::vector<std::string> words;
    
    // Split into words
    for (char c : text) {
        if (c == '_' || c == '-' || c == ' ') {
            if (!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord.clear();
            }
        } else {
            currentWord += c;
        }
    }
    if (!currentWord.empty()) {
        words.push_back(currentWord);
    }
    
    // Remove strike words (using startswith matching like Python version)
    for (const auto& word : words) {
        bool isStrikeWord = false;
        std::string lowerWord = word;
        std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
        
        for (const auto& strike : strikeWords) {
            if (lowerWord.find(strike) == 0) {  // startswith matching
                isStrikeWord = true;
                break;
            }
        }
        if (!isStrikeWord) {
            if (!result.empty()) {
                result += "_";
            }
            result += word;
        }
    }
    
    return result;
}
