#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <map>

class PathManager {
public:
    PathManager() = default;
    
    // Core functionality: shorten filename by converting separators to hyphens
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
    
    // Path length management (M8's 128 character limit)
    static constexpr size_t MAX_PATH_LENGTH = 128;
    
private:
    // Convert underscores/spaces to hyphens while preserving casing
    std::string toHyphenated(const std::string& str);
    
    // Fuzzy duplicate word removal
    std::vector<std::string> extractPackNameWords(const std::string& packName);
    std::string normalizeForComparison(const std::string& text);
    std::string removeDuplicatesFromPath(const std::string& text, 
                                         const std::vector<std::string>& packWords);
    
    // Remove acronyms and redundant category words
    std::string removePackAcronyms(const std::string& text);
    std::string removeRedundantCategoryWords(const std::string& filename, 
                                             const std::string& folderPath);
    
    // Path length validation and abbreviation
    bool validatePathLength(const std::string& path);
    std::string abbreviateCommonWords(const std::string& text);
    std::string truncateIfNeeded(const std::string& path, size_t maxLength);
    
    // Abbreviation dictionary for common audio terms
    const std::map<std::string, std::string> m_abbreviations = {
        {"Drum", "Drm"},
        {"Drums", "Drms"},
        {"Vocal", "Vox"},
        {"Vocals", "Vox"},
        {"Percussion", "Perc"},
        {"Synthesizer", "Synth"},
        {"Bass", "Bs"},
        {"Guitar", "Gtr"},
        {"String", "Str"},
        {"Strings", "Strs"},
        {"Instrument", "Inst"},
        {"One-Shot", "OS"},
        {"One-Shots", "OS"},
        {"OneShot", "OS"},
        {"OneShots", "OS"},
        {"Loop", "Lp"},
        {"Loops", "Lps"},
        {"Sample", "Smp"},
        {"Samples", "Smps"},
        {"Texture", "Txt"},
        {"Textures", "Txts"},
        {"Atmosphere", "Atm"},
        {"Atmospheres", "Atms"},
        {"Melody", "Mel"},
        {"Melodies", "Mels"},
        {"Chord", "Chd"},
        {"Chords", "Chds"}
    };
    
    // Extract extension from filename
    std::string extractExtension(const std::string& filename);
    
    // Remove extension from filename
    std::string removeExtension(const std::string& filename);
};
