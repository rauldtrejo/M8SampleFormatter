#include "PathManager.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <cctype>
#include <vector>
#include <functional>

std::string PathManager::shortenFilename(const std::string& filename) {
    std::string baseName = removeExtension(filename);
    // Always use .wav extension for output files
    std::string extension = ".wav";
    
    // Convert to hyphenated format (preserves casing)
    std::string shortened = toHyphenated(baseName);
    
    // Remove filler words like "serum" and "wav"
    shortened = removeFillerWords(shortened);
    
    // Convert underscores back to hyphens after filler removal
    std::replace(shortened.begin(), shortened.end(), '_', '-');
    
    return shortened + extension;
}

std::string PathManager::generateOutputPath(const std::string& inputPath,
                                           const std::string& sourceRoot,
                                           const std::string& outputRoot) {
    // Get the relative path from source root
    std::filesystem::path input(inputPath);
    std::filesystem::path source(sourceRoot);
    std::filesystem::path output(outputRoot);
    
    // Get the source folder name and remove parentheses/brackets content
    std::string sourceFolderName = source.filename().string();
    
    // Remove content in parentheses () and brackets []
    std::string cleaned = "";
    bool inParens = false;
    bool inBrackets = false;
    for (char c : sourceFolderName) {
        if (c == '(' || c == ')') {
            inParens = (c == '(');
            continue;
        }
        if (c == '[' || c == ']') {
            inBrackets = (c == '[');
            continue;
        }
        if (!inParens && !inBrackets) {
            cleaned += c;
        }
    }
    
    std::string topLevelFolder = toHyphenated(cleaned);
    
    // Remove filler words (WAV, SERUM, years, etc.) from top-level folder name
    topLevelFolder = removeFillerWords(topLevelFolder);
    // Convert underscores back to hyphens after filler removal
    std::replace(topLevelFolder.begin(), topLevelFolder.end(), '_', '-');
    
    // Remove any trailing hyphens
    while (!topLevelFolder.empty() && topLevelFolder.back() == '-') {
        topLevelFolder.pop_back();
    }
    
    // Extract pack name words for duplicate removal (use cleaned folder name)
    std::vector<std::string> packWords = extractPackNameWords(topLevelFolder);
    
    // Start with output root + hyphenated source folder name
    std::filesystem::path processedPath = output / topLevelFolder;
    
    // Calculate relative path from source root
    std::filesystem::path relativePath = std::filesystem::relative(input.parent_path(), source);
    
    // Apply hyphenation to each directory component and remove duplicates
    for (const auto& component : relativePath) {
        std::string dirName = component.string();
        // Skip "." which represents the current directory
        if (dirName != ".") {
            std::string hyphenatedDir = toHyphenated(dirName);
            std::string dedupedDir = removeDuplicatesFromPath(hyphenatedDir, packWords);
            // Remove common filler words like "serum" and "wav"
            dedupedDir = removeFillerWords(dedupedDir);
            // Convert underscores back to hyphens after filler removal
            std::replace(dedupedDir.begin(), dedupedDir.end(), '_', '-');
            if (!dedupedDir.empty()) {
                processedPath /= dedupedDir;
            }
        }
    }
    
    // Shorten the filename and remove duplicates
    std::string baseFilename = removeExtension(input.filename().string());
    std::string hyphenatedFilename = toHyphenated(baseFilename);
    std::string dedupedFilename = removeDuplicatesFromPath(hyphenatedFilename, packWords);
    // Remove common filler words like "serum" and "wav"
    dedupedFilename = removeFillerWords(dedupedFilename);
    // Remove pack-specific acronyms (ESE-, NRE-, HL-, etc.)
    dedupedFilename = removePackAcronyms(dedupedFilename);
    // Remove redundant category words from parent folder
    dedupedFilename = removeRedundantCategoryWords(dedupedFilename, processedPath.string());
    // Convert underscores back to hyphens after filler removal
    std::replace(dedupedFilename.begin(), dedupedFilename.end(), '_', '-');
    std::string shortenedFilename = dedupedFilename + ".wav";
    
    // Construct full path
    std::string fullPath = (processedPath / shortenedFilename).string();
    
    // Always apply abbreviations (not just when over limit)
    fullPath = abbreviateCommonWords(fullPath);
    
    // Check path length and truncate if still too long
    if (!validatePathLength(fullPath)) {
        fullPath = truncateIfNeeded(fullPath, MAX_PATH_LENGTH);
    }
    
    return fullPath;
}

std::string PathManager::toHyphenated(const std::string& str) {
    std::string result;
    bool lastWasHyphen = false;
    
    for (char c : str) {
        // Convert separators (underscore, space, multiple hyphens) to single hyphen
        if (c == '_' || c == ' ' || c == '-') {
            if (!lastWasHyphen && !result.empty()) {
                result += '-';
                lastWasHyphen = true;
            }
            continue;
        }
        
        // Handle alphanumeric characters - preserve original casing
        if (std::isalnum(c)) {
            result += c;
            lastWasHyphen = false;
        }
        // Keep # (for musical sharps like C#, F#)
        else if (c == '#') {
            result += c;
            lastWasHyphen = false;
        }
        // Keep dots and other characters that might be part of version numbers
        else if (c == '.') {
            result += c;
            lastWasHyphen = false;
        }
        // Skip other special characters (parentheses, brackets, etc.)
        // They effectively act as separators
        else if (!lastWasHyphen && !result.empty()) {
            result += '-';
            lastWasHyphen = true;
        }
    }
    
    // Remove trailing hyphen if present
    if (!result.empty() && result.back() == '-') {
        result.pop_back();
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
    // Get the source folder name and remove parentheses/brackets content
    std::filesystem::path source(sourceRoot);
    std::string sourceFolderName = source.filename().string();
    
    // Remove content in parentheses () and brackets []
    std::string cleaned = "";
    bool inParens = false;
    bool inBrackets = false;
    for (char c : sourceFolderName) {
        if (c == '(' || c == ')') {
            inParens = (c == '(');
            continue;
        }
        if (c == '[' || c == ']') {
            inBrackets = (c == '[');
            continue;
        }
        if (!inParens && !inBrackets) {
            cleaned += c;
        }
    }
    
    std::string topLevelFolder = toHyphenated(cleaned);
    
    // Remove filler words (WAV, SERUM, years, etc.) from top-level folder name
    topLevelFolder = removeFillerWords(topLevelFolder);
    // Convert underscores back to hyphens after filler removal
    std::replace(topLevelFolder.begin(), topLevelFolder.end(), '_', '-');
    
    // Remove any trailing hyphens
    while (!topLevelFolder.empty() && topLevelFolder.back() == '-') {
        topLevelFolder.pop_back();
    }
    
    // Extract pack name words for duplicate removal (use cleaned folder name)
    std::vector<std::string> packWords = extractPackNameWords(topLevelFolder);
    
    // Start with output root + hyphenated source folder name
    std::filesystem::path output(outputRoot);
    std::filesystem::path processedPath = output / topLevelFolder;
    
    // Flatten the folder structure
    std::string flattenedPath = this->flattenFolderStructure(inputPath, sourceRoot);
    std::string cleanedPath = this->cleanFolderName(flattenedPath);
    std::string hyphenatedPath = toHyphenated(cleanedPath);
    std::string dedupedPath = removeDuplicatesFromPath(hyphenatedPath, packWords);
    
    // Add the flattened path as a single directory
    if (!dedupedPath.empty()) {
        processedPath /= dedupedPath;
    }
    
    // Shorten the filename and remove duplicates
    std::filesystem::path input(inputPath);
    std::string baseFilename = removeExtension(input.filename().string());
    std::string hyphenatedFilename = toHyphenated(baseFilename);
    std::string dedupedFilename = removeDuplicatesFromPath(hyphenatedFilename, packWords);
    // Remove common filler words like "serum" and "wav"
    dedupedFilename = removeFillerWords(dedupedFilename);
    // Remove pack-specific acronyms (ESE-, NRE-, HL-, etc.)
    dedupedFilename = removePackAcronyms(dedupedFilename);
    // Remove redundant category words from parent folder
    dedupedFilename = removeRedundantCategoryWords(dedupedFilename, processedPath.string());
    // Convert underscores back to hyphens after filler removal
    std::replace(dedupedFilename.begin(), dedupedFilename.end(), '_', '-');
    std::string shortenedFilename = dedupedFilename + ".wav";
    
    // Construct full path
    std::string fullPath = (processedPath / shortenedFilename).string();
    
    // Always apply abbreviations (not just when over limit)
    fullPath = abbreviateCommonWords(fullPath);
    
    // Check path length and truncate if still too long
    if (!validatePathLength(fullPath)) {
        fullPath = truncateIfNeeded(fullPath, MAX_PATH_LENGTH);
    }
    
    return fullPath;
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
        "copy", "original", "edited", "mix", "remix", "remastered", "mastered",
        "serum",  // Added per user request
        // Marketing/filler words
        "essentials", "essential", "legends", "legend", "hero", "edition", 
        "exclusive", "bundle", "ultimate", "collection", "series",
        // Year numbers
        "2020", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029"
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

std::string PathManager::removePackAcronyms(const std::string& text) {
    // Remove pack-specific acronyms (2-4 capital letters at the start)
    // Examples: ESE-, NRE-, HL-, UE-, DH4-
    if (text.empty()) {
        return text;
    }
    
    // Check if text starts with 2-4 capital letters followed by hyphen or underscore
    size_t acronymLength = 0;
    for (size_t i = 0; i < text.length() && i < 4; ++i) {
        if (std::isupper(text[i]) || std::isdigit(text[i])) {
            acronymLength++;
        } else {
            break;
        }
    }
    
    // If we found 2-4 capital letters/digits followed by hyphen or underscore, remove them
    if (acronymLength >= 2 && acronymLength <= 4) {
        if (text.length() > acronymLength && (text[acronymLength] == '-' || text[acronymLength] == '_')) {
            return text.substr(acronymLength + 1);  // Skip acronym and separator
        }
    }
    
    return text;
}

std::string PathManager::removeRedundantCategoryWords(const std::string& filename, 
                                                       const std::string& folderPath) {
    // Extract words from folder path
    std::vector<std::string> folderWords;
    std::string currentWord;
    
    for (char c : folderPath) {
        if (c == '/' || c == '\\' || c == '-' || c == '_' || c == ' ') {
            if (!currentWord.empty() && currentWord.length() > 2) {  // Skip very short words
                folderWords.push_back(currentWord);
                currentWord.clear();
            } else {
                currentWord.clear();
            }
        } else if (std::isalnum(c)) {
            currentWord += c;
        }
    }
    if (!currentWord.empty() && currentWord.length() > 2) {
        folderWords.push_back(currentWord);
    }
    
    // Split filename into words
    std::vector<std::string> filenameWords;
    currentWord.clear();
    
    for (char c : filename) {
        if (c == '-' || c == '_' || c == ' ') {
            if (!currentWord.empty()) {
                filenameWords.push_back(currentWord);
                currentWord.clear();
            }
        } else {
            currentWord += c;
        }
    }
    if (!currentWord.empty()) {
        filenameWords.push_back(currentWord);
    }
    
    // Remove filename words that appear in folder path (case-insensitive)
    std::vector<std::string> filteredWords;
    for (const auto& fileWord : filenameWords) {
        bool foundInFolder = false;
        std::string normalizedFileWord = normalizeForComparison(fileWord);
        
        for (const auto& folderWord : folderWords) {
            std::string normalizedFolderWord = normalizeForComparison(folderWord);
            
            // Check for exact match or substring match
            if (normalizedFileWord == normalizedFolderWord ||
                normalizedFileWord.find(normalizedFolderWord) != std::string::npos ||
                normalizedFolderWord.find(normalizedFileWord) != std::string::npos) {
                foundInFolder = true;
                break;
            }
        }
        
        if (!foundInFolder) {
            filteredWords.push_back(fileWord);
        }
    }
    
    // Reconstruct filename
    std::string result;
    for (size_t i = 0; i < filteredWords.size(); ++i) {
        if (i > 0) {
            result += "-";
        }
        result += filteredWords[i];
    }
    
    return result;
}

// ============================================================================
// New methods for fuzzy duplicate removal and path length management
// ============================================================================

std::vector<std::string> PathManager::extractPackNameWords(const std::string& packName) {
    std::vector<std::string> words;
    std::string currentWord;
    
    // Split on separators
    for (char c : packName) {
        if (c == '_' || c == '-' || c == ' ' || c == '.' || c == '\'') {
            if (!currentWord.empty()) {
                words.push_back(currentWord);
                currentWord.clear();
            }
        } else if (std::isalnum(c) || c == '#') {
            currentWord += c;
        }
    }
    if (!currentWord.empty()) {
        words.push_back(currentWord);
    }
    
    return words;
}

std::string PathManager::normalizeForComparison(const std::string& text) {
    std::string normalized;
    
    for (char c : text) {
        if (std::isalnum(c)) {
            normalized += std::tolower(c);
        }
        // Skip separators and special characters for matching
    }
    
    return normalized;
}

std::string PathManager::removeDuplicatesFromPath(const std::string& text, 
                                                   const std::vector<std::string>& packWords) {
    if (packWords.empty() || text.empty()) {
        return text;
    }
    
    std::vector<std::string> words;
    std::string currentWord;
    
    // Split text into words
    for (char c : text) {
        if (c == '-' || c == '_' || c == ' ') {
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
    
    // Create normalized versions of pack words for comparison
    std::vector<std::string> normalizedPackWords;
    for (const auto& packWord : packWords) {
        std::string normalized = normalizeForComparison(packWord);
        if (!normalized.empty()) {
            normalizedPackWords.push_back(normalized);
        }
    }
    
    // Remove words that match pack words (with substring matching)
    std::vector<std::string> filteredWords;
    for (size_t i = 0; i < words.size(); ++i) {
        std::string normalizedWord = normalizeForComparison(words[i]);
        bool isDuplicate = false;
        
        // Check for exact word matches
        for (const auto& normalizedPackWord : normalizedPackWords) {
            if (normalizedWord == normalizedPackWord) {
                isDuplicate = true;
                break;
            }
            
            // Also check if the file word contains the pack word as a substring
            // e.g., "ghosthack" in pack matches "ghosthack" in "ghosthackKick"
            if (normalizedWord.find(normalizedPackWord) != std::string::npos) {
                isDuplicate = true;
                break;
            }
            
            // Also check if pack word contains the file word as a substring
            // e.g., "ghosthack" from "ghosthackBundle" matches standalone "ghosthack"
            if (normalizedPackWord.find(normalizedWord) != std::string::npos) {
                isDuplicate = true;
                break;
            }
        }
        
        // Also check if concatenating consecutive single-letter words matches a pack word
        // This handles cases like "K-S-H-M-R" matching "KSHMR"
        if (!isDuplicate && words[i].length() == 1) {
            std::string concatenated = normalizedWord;
            size_t j = i + 1;
            while (j < words.size() && words[j].length() == 1) {
                concatenated += normalizeForComparison(words[j]);
                j++;
            }
            
            // Check if concatenated version matches any pack word
            for (const auto& normalizedPackWord : normalizedPackWords) {
                if (concatenated == normalizedPackWord) {
                    // Mark all these single-letter words as duplicates
                    isDuplicate = true;
                    // Skip the remaining single letters in the outer loop
                    i = j - 1;
                    break;
                }
            }
        }
        
        if (!isDuplicate) {
            filteredWords.push_back(words[i]);
        }
    }
    
    // Reconstruct string with hyphens
    std::string result;
    for (size_t i = 0; i < filteredWords.size(); ++i) {
        if (i > 0) {
            result += "-";
        }
        result += filteredWords[i];
    }
    
    return result;
}

bool PathManager::validatePathLength(const std::string& path) {
    // Get just the filename portion (from last slash to end)
    size_t lastSlash = path.find_last_of("/\\");
    std::string filename;
    if (lastSlash != std::string::npos) {
        filename = path.substr(lastSlash + 1);
    } else {
        filename = path;
    }
    
    return filename.length() <= MAX_PATH_LENGTH;
}

std::string PathManager::abbreviateCommonWords(const std::string& text) {
    std::string result = text;
    
    // Apply abbreviations from the dictionary
    // We need to be careful to match whole words with hyphens as separators
    for (const auto& [fullWord, abbrev] : m_abbreviations) {
        std::string searchWord = fullWord;
        
        // Try different casing variations
        std::vector<std::string> variations = {
            fullWord,  // Original
            fullWord,  // Keep as-is for mixed case
        };
        
        for (const auto& variant : variations) {
            // Replace with word boundaries (hyphens or path separators)
            size_t pos = 0;
            while ((pos = result.find(variant, pos)) != std::string::npos) {
                // Check if it's a whole word (preceded and followed by hyphen, slash, or boundary)
                bool validStart = (pos == 0 || result[pos-1] == '-' || 
                                  result[pos-1] == '/' || result[pos-1] == '\\');
                bool validEnd = (pos + variant.length() >= result.length() || 
                                result[pos + variant.length()] == '-' ||
                                result[pos + variant.length()] == '/' ||
                                result[pos + variant.length()] == '\\' ||
                                result[pos + variant.length()] == '.');
                
                if (validStart && validEnd) {
                    result.replace(pos, variant.length(), abbrev);
                    pos += abbrev.length();
                } else {
                    pos += variant.length();
                }
            }
        }
    }
    
    return result;
}

std::string PathManager::truncateIfNeeded(const std::string& path, size_t maxLength) {
    // Get the path components
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos) {
        // No path, just filename - truncate filename
        if (path.length() <= maxLength) {
            return path;
        }
        
        // Keep extension, truncate middle
        size_t dotPos = path.find_last_of('.');
        if (dotPos != std::string::npos) {
            std::string ext = path.substr(dotPos);
            size_t availableLength = maxLength - ext.length();
            std::string truncated = path.substr(0, availableLength);
            
            // Log warning
            Logger::getInstance().warning("Truncated filename to fit 128 char limit: " + path);
            
            return truncated + ext;
        } else {
            Logger::getInstance().warning("Truncated filename to fit 128 char limit: " + path);
            return path.substr(0, maxLength);
        }
    }
    
    std::string directory = path.substr(0, lastSlash);
    std::string filename = path.substr(lastSlash + 1);
    
    if (filename.length() <= maxLength) {
        return path;  // Already OK
    }
    
    // Truncate filename intelligently
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string ext = filename.substr(dotPos);
        size_t availableLength = maxLength - ext.length();
        
        // Try to keep beginning and end, truncate middle
        if (availableLength > 10) {
            size_t keepStart = availableLength * 2 / 3;
            size_t keepEnd = availableLength - keepStart - 3;  // 3 for "..."
            
            std::string truncated = filename.substr(0, keepStart) + "..." + 
                                   filename.substr(dotPos - keepEnd, keepEnd);
            
            Logger::getInstance().warning("Truncated filename to fit 128 char limit: " + filename);
            
            return directory + "/" + truncated + ext;
        } else {
            std::string truncated = filename.substr(0, availableLength);
            Logger::getInstance().warning("Truncated filename to fit 128 char limit: " + filename);
            return directory + "/" + truncated + ext;
        }
    } else {
        Logger::getInstance().warning("Truncated filename to fit 128 char limit: " + filename);
        return directory + "/" + filename.substr(0, maxLength);
    }
}
