#include <gtest/gtest.h>
#include "filesystem/FileScanner.h"
#include <filesystem>
#include <vector>

class FileScannerTest : public ::testing::Test {
protected:
    void SetUp() override {
        scanner = std::make_unique<FileScanner>();
        
        // Create test directory structure
        testDir = std::filesystem::temp_directory_path() / "m8_scanner_test";
        std::filesystem::create_directories(testDir);
        
        // Create test files
        createTestFiles();
    }
    
    void TearDown() override {
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
    
    void createTestFiles() {
        // Create audio files
        createTestWavFile(testDir / "test1.wav");
        createTestWavFile(testDir / "test2.wav");
        createTestAiffFile(testDir / "test3.aiff");
        createTestMp3File(testDir / "test4.mp3");
        
        // Create non-audio files
        createTestTextFile(testDir / "readme.txt");
        createTestTextFile(testDir / "config.json");
        
        // Create subdirectories
        std::filesystem::create_directories(testDir / "subfolder");
        createTestWavFile(testDir / "subfolder" / "nested.wav");
        createTestTextFile(testDir / "subfolder" / "readme.txt");
        
        // Create ignored directories
        std::filesystem::create_directories(testDir / ".DS_Store");
        std::filesystem::create_directories(testDir / ".Trashes");
    }
    
    std::unique_ptr<FileScanner> scanner;
    std::filesystem::path testDir;
    
private:
    void createTestWavFile(const std::filesystem::path& path) {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) return;
        
        // Minimal WAV header
        file.write("RIFF", 4);
        int fileSize = 36;
        file.write(reinterpret_cast<const char*>(&fileSize), 4);
        file.write("WAVE", 4);
        file.write("fmt ", 4);
        int fmtSize = 16;
        file.write(reinterpret_cast<const char*>(&fmtSize), 4);
        short audioFormat = 1;
        file.write(reinterpret_cast<const char*>(&audioFormat), 2);
        short channels = 1;
        file.write(reinterpret_cast<const char*>(&channels), 2);
        int sampleRate = 44100;
        file.write(reinterpret_cast<const char*>(&sampleRate), 4);
        int byteRate = 88200;
        file.write(reinterpret_cast<const char*>(&byteRate), 4);
        short blockAlign = 2;
        file.write(reinterpret_cast<const char*>(&blockAlign), 2);
        short bitDepth = 16;
        file.write(reinterpret_cast<const char*>(&bitDepth), 2);
        file.write("data", 4);
        int dataSize = 0;
        file.write(reinterpret_cast<const char*>(&dataSize), 4);
        
        file.close();
    }
    
    void createTestAiffFile(const std::filesystem::path& path) {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) return;
        
        // Minimal AIFF header
        file.write("FORM", 4);
        int fileSize = 46;
        file.write(reinterpret_cast<const char*>(&fileSize), 4);
        file.write("AIFF", 4);
        file.write("COMM", 4);
        int commSize = 18;
        file.write(reinterpret_cast<const char*>(&commSize), 4);
        short channels = 1;
        file.write(reinterpret_cast<const char*>(&channels), 2);
        int numSampleFrames = 0;
        file.write(reinterpret_cast<const char*>(&numSampleFrames), 4);
        short bitDepth = 16;
        file.write(reinterpret_cast<const char*>(&bitDepth), 2);
        file.write("SSND", 4);
        int ssndSize = 8;
        file.write(reinterpret_cast<const char*>(&ssndSize), 4);
        int offset = 0;
        int blockSize = 0;
        file.write(reinterpret_cast<const char*>(&offset), 4);
        file.write(reinterpret_cast<const char*>(&blockSize), 4);
        
        file.close();
    }
    
    void createTestMp3File(const std::filesystem::path& path) {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) return;
        
        // Minimal MP3 header (simplified)
        file.write("ID3", 3);
        char version = 3;
        file.write(&version, 1);
        char flags = 0;
        file.write(&flags, 1);
        int size = 0;
        file.write(reinterpret_cast<const char*>(&size), 4);
        
        file.close();
    }
    
    void createTestTextFile(const std::filesystem::path& path) {
        std::ofstream file(path);
        file << "This is a text file";
        file.close();
    }
};

TEST_F(FileScannerTest, ScanDirectoryFindsAudioFiles) {
    std::vector<std::string> ignoreFolders = {".DS_Store", ".Trashes"};
    auto audioFiles = scanner->scanDirectory(testDir.string(), ignoreFolders);
    
    EXPECT_GE(audioFiles.size(), 4); // Should find at least 4 audio files
    
    // Check that we found the expected files
    std::vector<std::string> foundFiles;
    for (const auto& file : audioFiles) {
        foundFiles.push_back(file.filename);
    }
    
    EXPECT_TRUE(std::find(foundFiles.begin(), foundFiles.end(), "test1.wav") != foundFiles.end());
    EXPECT_TRUE(std::find(foundFiles.begin(), foundFiles.end(), "test2.wav") != foundFiles.end());
    EXPECT_TRUE(std::find(foundFiles.begin(), foundFiles.end(), "test3.aiff") != foundFiles.end());
    EXPECT_TRUE(std::find(foundFiles.begin(), foundFiles.end(), "test4.mp3") != foundFiles.end());
    EXPECT_TRUE(std::find(foundFiles.begin(), foundFiles.end(), "nested.wav") != foundFiles.end());
}

TEST_F(FileScannerTest, IgnoresNonAudioFiles) {
    auto audioFiles = scanner->scanDirectory(testDir.string());
    
    for (const auto& file : audioFiles) {
        EXPECT_NE(file.extension, ".txt");
        EXPECT_NE(file.extension, ".json");
    }
}

TEST_F(FileScannerTest, IgnoresSpecifiedFolders) {
    std::vector<std::string> ignoreFolders = {".DS_Store", ".Trashes"};
    auto audioFiles = scanner->scanDirectory(testDir.string(), ignoreFolders);
    
    // Should not find files in ignored directories
    for (const auto& file : audioFiles) {
        EXPECT_FALSE(file.filepath.find(".DS_Store") != std::string::npos);
        EXPECT_FALSE(file.filepath.find(".Trashes") != std::string::npos);
    }
}

TEST_F(FileScannerTest, ExtractsPackName) {
    std::string filepath = testDir / "subfolder" / "nested.wav";
    std::string packName = scanner->extractPackName(filepath, testDir.string());
    
    EXPECT_EQ(packName, "m8_scanner_test");
}

TEST_F(FileScannerTest, ValidatesFileExtensions) {
    EXPECT_TRUE(scanner->isSupportedExtension(".wav"));
    EXPECT_TRUE(scanner->isSupportedExtension(".aiff"));
    EXPECT_TRUE(scanner->isSupportedExtension(".flac"));
    EXPECT_TRUE(scanner->isSupportedExtension(".ogg"));
    EXPECT_TRUE(scanner->isSupportedExtension(".mp3"));
    EXPECT_FALSE(scanner->isSupportedExtension(".txt"));
    EXPECT_FALSE(scanner->isSupportedExtension(".json"));
}

TEST_F(FileScannerTest, ValidatesAudioFiles) {
    std::string wavFile = (testDir / "test1.wav").string();
    std::string txtFile = (testDir / "readme.txt").string();
    
    EXPECT_TRUE(scanner->isValidAudioFile(wavFile));
    EXPECT_FALSE(scanner->isValidAudioFile(txtFile));
}

TEST_F(FileScannerTest, FileSizeValidation) {
    scanner->setMinFileSize(100);
    scanner->setMaxFileSize(1000);
    
    auto audioFiles = scanner->scanDirectory(testDir.string());
    
    for (const auto& file : audioFiles) {
        EXPECT_GE(file.fileSize, 100);
        EXPECT_LE(file.fileSize, 1000);
    }
}

TEST_F(FileScannerTest, ProgressCallback) {
    std::vector<std::pair<size_t, size_t>> progressUpdates;
    
    scanner->setProgressCallback([&progressUpdates](size_t current, size_t total) {
        progressUpdates.push_back({current, total});
    });
    
    auto audioFiles = scanner->scanDirectory(testDir.string());
    
    EXPECT_FALSE(progressUpdates.empty());
}

TEST_F(FileScannerTest, FileCallback) {
    std::vector<AudioFile> callbackFiles;
    
    scanner->setFileCallback([&callbackFiles](const AudioFile& file) {
        callbackFiles.push_back(file);
    });
    
    auto audioFiles = scanner->scanDirectory(testDir.string());
    
    EXPECT_EQ(callbackFiles.size(), audioFiles.size());
}

TEST_F(FileScannerTest, CancelScanning) {
    scanner->cancel();
    
    auto audioFiles = scanner->scanDirectory(testDir.string());
    
    EXPECT_TRUE(scanner->isCancelled());
    EXPECT_TRUE(audioFiles.empty());
}

TEST_F(FileScannerTest, Statistics) {
    auto audioFiles = scanner->scanDirectory(testDir.string());
    
    EXPECT_EQ(scanner->getTotalFiles(), audioFiles.size());
    EXPECT_EQ(scanner->getValidFiles(), audioFiles.size());
    EXPECT_EQ(scanner->getSkippedFiles(), 0);
}
