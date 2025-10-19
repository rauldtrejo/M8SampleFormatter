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
    EXPECT_EQ(result, "mySamplePack.wav");
}

TEST_F(PathManagerTest, ShortenFilenameWithUnderscores) {
    std::string result = pathManager->shortenFilename("KSHMR_Zafrir_Drum_Fill_01.wav");
    EXPECT_EQ(result, "kSHMRZafrirDrumFill01.wav");
}

TEST_F(PathManagerTest, ShortenFilenameWithHyphens) {
    std::string result = pathManager->shortenFilename("Lo-Fi-Soul-Beat.wav");
    EXPECT_EQ(result, "loFiSoulBeat.wav");
}

TEST_F(PathManagerTest, ShortenFilenameMixedSeparators) {
    std::string result = pathManager->shortenFilename("Sample_Pack - Final Version.wav");
    EXPECT_EQ(result, "samplePackFinalVersion.wav");
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
    EXPECT_EQ(result, "kSHMRZafrirDrumFill01125Cm.wav");
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
    std::string result = pathManager->shortenFilename("Sample@#$%^&*().wav");
    EXPECT_EQ(result, "sample#.wav");
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
