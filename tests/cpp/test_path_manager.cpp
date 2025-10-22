#include <gtest/gtest.h>
#include "PathManager.h"
#include <filesystem>

class PathManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        pathManager = std::make_unique<PathManager>();
        
        // Create test directory structure
        testDir = std::filesystem::temp_directory_path() / "m8_path_test";
        outputDir = testDir / "output";
        std::filesystem::create_directories(testDir);
        std::filesystem::create_directories(outputDir);
    }
    
    void TearDown() override {
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
    
    std::unique_ptr<PathManager> pathManager;
    std::filesystem::path testDir;
    std::filesystem::path outputDir;
};

TEST_F(PathManagerTest, ShortenFilenameBasic) {
    std::string result = pathManager->shortenFilename("My Sample Pack.wav");
    // "Sample" and "Pack" are filler words and get removed
    EXPECT_EQ(result, "My.wav");
}

TEST_F(PathManagerTest, ShortenFilenameWithUnderscores) {
    std::string result = pathManager->shortenFilename("KSHMR_Zafrir_Drum_Fill_01.wav");
    EXPECT_EQ(result, "KSHMR-Zafrir-Drum-Fill-01.wav");
}

TEST_F(PathManagerTest, ShortenFilenameWithHyphens) {
    std::string result = pathManager->shortenFilename("Lo-Fi-Soul-Beat.wav");
    EXPECT_EQ(result, "Lo-Fi-Soul-Beat.wav");
}

TEST_F(PathManagerTest, ShortenFilenameMixedSeparators) {
    std::string result = pathManager->shortenFilename("Sample_Pack - Final Version.wav");
    // "Sample", "Pack", "Final", and "Version" are all filler words and get removed
    EXPECT_EQ(result, ".wav");
}

TEST_F(PathManagerTest, ShortenFilenamePreservesExtension) {
    std::string result = pathManager->shortenFilename("test.aiff");
    EXPECT_EQ(result, "test.wav");
    
    result = pathManager->shortenFilename("test.flac");
    EXPECT_EQ(result, "test.wav");
}

TEST_F(PathManagerTest, GenerateOutputPath) {
    std::string inputPath = (testDir / "subfolder" / "nested" / "sample.wav").string();
    std::string sourceRoot = testDir.string();
    std::string outputRoot = outputDir.string();
    
    std::string result = pathManager->generateOutputPath(inputPath, sourceRoot, outputRoot);
    
    EXPECT_TRUE(result.find("output") != std::string::npos);
    EXPECT_TRUE(result.find("subfolder") != std::string::npos);
    EXPECT_TRUE(result.find("nested") != std::string::npos);
    EXPECT_TRUE(result.length() >= 4 && result.substr(result.length() - 4) == ".wav");
}

TEST_F(PathManagerTest, GenerateFlattenedOutputPath) {
    std::string inputPath = (testDir / "subfolder" / "nested" / "sample.wav").string();
    std::string sourceRoot = testDir.string();
    std::string outputRoot = outputDir.string();
    
    std::string result = pathManager->generateFlattenedOutputPath(inputPath, sourceRoot, outputRoot);
    
    EXPECT_TRUE(result.find("output") != std::string::npos);
    EXPECT_TRUE(result.length() >= 4 && result.substr(result.length() - 4) == ".wav");
    // Flattened path should contain flattened directory structure
    EXPECT_TRUE(result.find("subfolder") != std::string::npos || result.find("nested") != std::string::npos);
}

TEST_F(PathManagerTest, FlattenFolderStructure) {
    std::string inputPath = "Sample Pack/Drums/Kicks/Heavy Kick.wav";
    std::string sourceRoot = "Sample Pack";
    
    std::string result = pathManager->flattenFolderStructure(inputPath, sourceRoot);
    
    EXPECT_EQ(result, "Drums_Kicks");
}

TEST_F(PathManagerTest, CleanFolderName) {
    std::string result = pathManager->cleanFolderName("Drum Fills");
    EXPECT_EQ(result, "Drum_Fills");
    
    result = pathManager->cleanFolderName("Lo-Fi Soul");
    EXPECT_EQ(result, "Lo_Fi_Soul");
    
    result = pathManager->cleanFolderName("KSHMR_Presents");
    EXPECT_EQ(result, "KSHMR_Presents");
}

TEST_F(PathManagerTest, RemoveDuplicateWords) {
    std::string result = pathManager->removeDuplicateWords("Sample Sample Pack Pack");
    EXPECT_EQ(result, "Sample_Pack");
    
    result = pathManager->removeDuplicateWords("Drum Drum Fill Fill");
    EXPECT_EQ(result, "Drum_Fill");
}

TEST_F(PathManagerTest, RemoveFillerWords) {
    std::string result = pathManager->removeFillerWords("Sample Sound Audio");
    EXPECT_EQ(result, "Sound_Audio");
    
    result = pathManager->removeFillerWords("Drum Sample Sound");
    EXPECT_EQ(result, "Drum_Sound");
    
    result = pathManager->removeFillerWords("Kick Drum Sample");
    EXPECT_EQ(result, "Kick_Drum");
}

TEST_F(PathManagerTest, ComplexFilenameShortening) {
    std::string result = pathManager->shortenFilename("KSHMR Zafrir - Drum Fill 01 (125, Cm).wav");
    EXPECT_EQ(result, "KSHMR-Zafrir-Drum-Fill-01-125-Cm.wav");
}

TEST_F(PathManagerTest, PreservesMusicalKeys) {
    std::string result = pathManager->shortenFilename("Vocal Loop 01 (130, F#m).wav");
    EXPECT_TRUE(result.find("F#m") != std::string::npos);
    
    result = pathManager->shortenFilename("Bass Line (C#).wav");
    EXPECT_TRUE(result.find("C#") != std::string::npos);
}

TEST_F(PathManagerTest, HandlesEmptyStrings) {
    std::string result = pathManager->shortenFilename("");
    EXPECT_EQ(result, ".wav");
    
    result = pathManager->shortenFilename("   ");
    EXPECT_EQ(result, ".wav");
}

TEST_F(PathManagerTest, HandlesSpecialCharacters) {
    std::string result = pathManager->shortenFilename("Sound@#$%^&*().wav");
    // Special characters become hyphens, "Sound" is kept
    EXPECT_EQ(result, "Sound-#.wav");
}

TEST_F(PathManagerTest, FlatteningWithComplexStructure) {
    std::string inputPath = "Capsun - Lo-Fi Soul & Future Beats/CPA_CAPSUN_Lo_Fi_Soul___Future_Beats/Drums___Percussion/Drum___Perc_One_Shots/Snare/CLF_Snare_Chunk.wav";
    std::string sourceRoot = "Capsun - Lo-Fi Soul & Future Beats";
    
    std::string result = pathManager->flattenFolderStructure(inputPath, sourceRoot);
    
    EXPECT_TRUE(result.find("CPA_CAPSUN_Lo_Fi_Soul___Future_Beats") != std::string::npos);
    EXPECT_TRUE(result.find("Drums___Percussion") != std::string::npos);
    EXPECT_TRUE(result.find("Drum___Perc_One_Shots") != std::string::npos);
    EXPECT_TRUE(result.find("Snare") != std::string::npos);
    // Result should not contain the filename since flattenFolderStructure only processes directory path
    EXPECT_FALSE(result.find("CLF_Snare_Chunk.wav") != std::string::npos);
}

// ============================================================================
// New tests for fuzzy matching and path length management
// ============================================================================

TEST_F(PathManagerTest, FuzzyMatchingRemovesDuplicates) {
    // Test that "KSHMR" in filename is removed when pack contains "KSHMR"
    std::string packName = "KSHMR Presents Zafrir World Sounds";
    std::filesystem::path sourcePath = testDir / packName;
    std::filesystem::create_directories(sourcePath / "Kicks");
    
    std::string inputPath = (sourcePath / "Kicks" / "KSHMR Zafrir - Kick Fill 01.wav").string();
    std::string outputPath = pathManager->generateOutputPath(
        inputPath, 
        sourcePath.string(), 
        outputDir.string()
    );
    
    // Check the filename only (not the full path which includes the pack name folder)
    std::string filename = std::filesystem::path(outputPath).filename().string();
    
    // Filename should not contain "KSHMR", "Zafrir", "Presents", "World", "Sounds"
    // "Kick" is in folder name and removed from filename, leaving just "Fill-01"
    EXPECT_EQ(filename, "Fill-01.wav");
}

TEST_F(PathManagerTest, FuzzyMatchingCaseInsensitive) {
    // Test that fuzzy matching is case-insensitive
    std::string packName = "KSHMR Sample Pack";
    std::filesystem::path sourcePath = testDir / packName;
    std::filesystem::create_directories(sourcePath);
    
    std::string inputPath = (sourcePath / "kshmr_sample_01.wav").string();
    std::string outputPath = pathManager->generateOutputPath(
        inputPath, 
        sourcePath.string(), 
        outputDir.string()
    );
    
    // "kshmr" should be removed (lowercase version of pack word "KSHMR")
    // "sample" should be removed (lowercase version of pack word "Sample")
    std::string filename = std::filesystem::path(outputPath).filename().string();
    EXPECT_EQ(filename, "01.wav");
}

TEST_F(PathManagerTest, FuzzyMatchingVariations) {
    // Test variations like "K-S-H-M-R" vs "KSHMR"
    std::string packName = "KSHMR Productions";
    std::filesystem::path sourcePath = testDir / packName;
    std::filesystem::create_directories(sourcePath);
    
    // This should match because normalized versions are the same
    std::string inputPath = (sourcePath / "K_S_H_M_R_Kick.wav").string();
    std::string outputPath = pathManager->generateOutputPath(
        inputPath, 
        sourcePath.string(), 
        outputDir.string()
    );
    
    // "KSHMR" normalized to "kshmr" should match "K_S_H_M_R" normalized to "kshmr"
    std::string filename = std::filesystem::path(outputPath).filename().string();
    EXPECT_EQ(filename, "Kick.wav");
}

TEST_F(PathManagerTest, PathLengthValidation) {
    // Test that very long paths get abbreviated
    std::string packName = "Sample Pack";
    std::filesystem::path sourcePath = testDir / packName;
    std::filesystem::create_directories(sourcePath / "Drums" / "Percussion");
    
    // Create a filename that would be > 128 chars
    std::string longFilename = "Very_Long_Drum_Sample_With_Many_Words_And_Descriptors_"
                              "Including_Percussion_Elements_Drums_And_Vocals_"
                              "Plus_Additional_Information_Here.wav";
    std::string inputPath = (sourcePath / "Drums" / "Percussion" / longFilename).string();
    
    std::string outputPath = pathManager->generateOutputPath(
        inputPath, 
        sourcePath.string(), 
        outputDir.string()
    );
    
    std::string filename = std::filesystem::path(outputPath).filename().string();
    
    // Filename should be <= 128 characters
    EXPECT_LE(filename.length(), 128);
}

TEST_F(PathManagerTest, AbbreviationApplication) {
    // Test that common words get abbreviated when needed
    std::string packName = "Test Pack";
    std::filesystem::path sourcePath = testDir / packName;
    std::filesystem::create_directories(sourcePath / "Drums");
    
    std::string longFilename = "Drums-Percussion-Vocal-Loop-Sample-One-Shot-"
                              "Melody-Chord-Guitar-Bass-String-Synthesizer-"
                              "Instrument-Atmosphere-Texture.wav";
    std::string inputPath = (sourcePath / "Drums" / longFilename).string();
    
    std::string outputPath = pathManager->generateOutputPath(
        inputPath, 
        sourcePath.string(), 
        outputDir.string()
    );
    
    std::string filename = std::filesystem::path(outputPath).filename().string();
    
    // Should contain abbreviations
    EXPECT_TRUE(filename.find("Drm") != std::string::npos || 
                filename.find("Vox") != std::string::npos ||
                filename.find("Perc") != std::string::npos);
}

TEST_F(PathManagerTest, PreservesOriginalCasing) {
    // Test that original casing is preserved in hyphenated format
    std::string result = pathManager->shortenFilename("MyMixedCaseFile_WithUNDERSCORES.wav");
    EXPECT_EQ(result, "MyMixedCaseFile-WithUNDERSCORES.wav");
    
    result = pathManager->shortenFilename("ALL_CAPS_FILE_NAME.wav");
    EXPECT_EQ(result, "ALL-CAPS-FILE-NAME.wav");
    
    result = pathManager->shortenFilename("lowercase_file_name.wav");
    EXPECT_EQ(result, "lowercase-file-name.wav");
}

TEST_F(PathManagerTest, HandlesMultipleHyphens) {
    // Test that multiple consecutive hyphens are collapsed to single hyphen
    std::string result = pathManager->shortenFilename("Sound---With---Hyphens.wav");
    // "Sample" is a filler word, using "Sound" instead
    EXPECT_EQ(result, "Sound-With-Hyphens.wav");
}

TEST_F(PathManagerTest, RemovesTrailingHyphens) {
    // Test that trailing hyphens are removed
    std::string result = pathManager->shortenFilename("Sound File__.wav");
    // Using "Sound" instead of "Sample" (which is a filler word)
    EXPECT_EQ(result, "Sound-File.wav");
}

TEST_F(PathManagerTest, RemovesSerumAndWavFromPaths) {
    // Test that "serum" and "wav" words are removed from filenames
    std::string result = pathManager->shortenFilename("Serum Bass Wav Sample.wav");
    // Should remove "Serum", "Wav", and "Sample" (all filler words) but keep "Bass"
    // Abbreviations happen later in the full path generation
    EXPECT_TRUE(result.find("Bass") != std::string::npos);
    EXPECT_TRUE(result.find("serum") == std::string::npos);
    EXPECT_TRUE(result.find("Serum") == std::string::npos);
    EXPECT_EQ(result, "Bass.wav");
}

TEST_F(PathManagerTest, AlwaysAppliesAbbreviations) {
    // Test that abbreviations are always applied, not just when over 128 chars
    std::string packName = "Test Pack";
    std::filesystem::path sourcePath = testDir / packName;
    std::filesystem::create_directories(sourcePath);
    
    std::string inputPath = (sourcePath / "Drum Vocal Sample.wav").string();
    std::string outputPath = pathManager->generateOutputPath(
        inputPath, 
        sourcePath.string(), 
        outputDir.string()
    );
    
    std::string filename = std::filesystem::path(outputPath).filename().string();
    
    // Should always have abbreviations applied (Drum->Drm, Vocal->Vox)
    EXPECT_TRUE(filename.find("Drm") != std::string::npos || 
                filename.find("Vox") != std::string::npos);
}

TEST_F(PathManagerTest, SubstringMatchingRemovesDuplicates) {
    // Test that substring matching works - if pack name contains "ghosthack",
    // then "ghosthack" in filename should be removed even if full pack name is different
    std::string packName = "ghosthackUltimateTechnoBundle2025";
    std::filesystem::path sourcePath = testDir / packName;
    std::filesystem::create_directories(sourcePath / "Kicks");
    
    std::string inputPath = (sourcePath / "Kicks" / "ghosthack Kick 01.wav").string();
    std::string outputPath = pathManager->generateOutputPath(
        inputPath, 
        sourcePath.string(), 
        outputDir.string()
    );
    
    std::string filename = std::filesystem::path(outputPath).filename().string();
    
    // "ghosthack" should be removed via substring matching
    // "Kick" becomes "Kick" (not in abbreviation list for this specific word)
    EXPECT_TRUE(filename.find("ghosthack") == std::string::npos);
    EXPECT_TRUE(filename.find("Ghosthack") == std::string::npos);
    EXPECT_TRUE(filename.find("Kick") != std::string::npos || filename.find("01") != std::string::npos);
}
