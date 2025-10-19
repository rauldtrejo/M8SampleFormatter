import Testing
import SwiftUI
@testable import M8FormatterGUI

@MainActor
struct M8FormatterGUITests {
    
    @Test("Initial state should have correct default values")
    func testInitialState() async {
        let viewModel = ProcessingViewModel()
        
        #expect(viewModel.isProcessing == false)
        #expect(viewModel.isCompleted == false)
        #expect(viewModel.progress == 0.0)
        #expect(viewModel.processedFiles == 0)
        #expect(viewModel.totalFiles == 0)
        #expect(viewModel.processedFolders == 0)
        #expect(viewModel.currentFolder == "")
        #expect(viewModel.processingTime == 0.0)
        #expect(viewModel.averageSpeed == 0.0)
        #expect(viewModel.errorMessage == "")
        #expect(viewModel.showingError == false)
    }
    
    @Test("Should add folders correctly")
    func testAddFolders() async {
        let viewModel = ProcessingViewModel()
        let testPaths = ["/path/to/folder1", "/path/to/folder2"]
        
        viewModel.addFolders(testPaths)
        
        #expect(viewModel.selectedFolders.count == 2)
        #expect(viewModel.selectedFolders[0].path == "/path/to/folder1")
        #expect(viewModel.selectedFolders[1].path == "/path/to/folder2")
    }
    
    @Test("Should not add duplicate folders")
    func testAddDuplicateFolders() async {
        let viewModel = ProcessingViewModel()
        let testPath = "/path/to/folder"
        
        viewModel.addFolders([testPath])
        viewModel.addFolders([testPath]) // Add same path again
        
        #expect(viewModel.selectedFolders.count == 1) // Should not add duplicate
    }
    
    @Test("Should remove folders correctly")
    func testRemoveFolder() async {
        let viewModel = ProcessingViewModel()
        let testPaths = ["/path/to/folder1", "/path/to/folder2"]
        viewModel.addFolders(testPaths)
        
        let folderToRemove = viewModel.selectedFolders[0]
        viewModel.removeFolder(folderToRemove)
        
        #expect(viewModel.selectedFolders.count == 1)
        #expect(viewModel.selectedFolders[0].path == "/path/to/folder2")
    }
    
    @Test("Should clear all folders")
    func testClearAllFolders() async {
        let viewModel = ProcessingViewModel()
        let testPaths = ["/path/to/folder1", "/path/to/folder2", "/path/to/folder3"]
        viewModel.addFolders(testPaths)
        
        viewModel.clearAllFolders()
        
        #expect(viewModel.selectedFolders.count == 0)
    }
    
    @Test("Should not start processing without folders")
    func testStartProcessingWithoutFolders() async {
        let viewModel = ProcessingViewModel()
        viewModel.outputFolderPath = "/output"
        
        viewModel.startProcessing()
        
        #expect(viewModel.isProcessing == false)
    }
    
    @Test("Should not start processing without output path")
    func testStartProcessingWithoutOutputPath() async {
        let viewModel = ProcessingViewModel()
        viewModel.addFolders(["/path/to/folder"])
        
        viewModel.startProcessing()
        
        #expect(viewModel.isProcessing == false)
    }
    
    @Test("Should start processing with valid inputs")
    func testStartProcessingWithValidInputs() async {
        let viewModel = ProcessingViewModel()
        viewModel.addFolders(["/path/to/folder"])
        viewModel.outputFolderPath = "/output"
        
        viewModel.startProcessing()
        
        #expect(viewModel.isProcessing == true)
        #expect(viewModel.isCompleted == false)
        #expect(viewModel.progress == 0.0)
        #expect(viewModel.processedFiles == 0)
        #expect(viewModel.totalFiles == 0)
        #expect(viewModel.processedFolders == 0)
        #expect(viewModel.currentFolder == "")
        #expect(viewModel.processingTime == 0.0)
        #expect(viewModel.averageSpeed == 0.0)
        #expect(viewModel.errorMessage == "")
    }
    
    @Test("Should stop processing correctly")
    func testStopProcessing() async {
        let viewModel = ProcessingViewModel()
        viewModel.addFolders(["/path/to/folder"])
        viewModel.outputFolderPath = "/output"
        viewModel.startProcessing()
        
        viewModel.stopProcessing()
        
        #expect(viewModel.isProcessing == false)
        #expect(viewModel.currentFolder == "")
    }
    
    @Test("Should reset correctly")
    func testReset() async {
        let viewModel = ProcessingViewModel()
        viewModel.addFolders(["/path/to/folder"])
        viewModel.outputFolderPath = "/output"
        viewModel.startProcessing()
        
        viewModel.reset()
        
        #expect(viewModel.isProcessing == false)
        #expect(viewModel.isCompleted == false)
        #expect(viewModel.progress == 0.0)
        #expect(viewModel.processedFiles == 0)
        #expect(viewModel.totalFiles == 0)
        #expect(viewModel.processedFolders == 0)
        #expect(viewModel.currentFolder == "")
        #expect(viewModel.processingTime == 0.0)
        #expect(viewModel.averageSpeed == 0.0)
        #expect(viewModel.errorMessage == "")
    }
    
    @Test("Should toggle convert bit depth")
    func testConvertBitDepthToggle() async {
        let viewModel = ProcessingViewModel()
        #expect(viewModel.convertBitDepth == true)
        
        viewModel.convertBitDepth = false
        #expect(viewModel.convertBitDepth == false)
        
        viewModel.convertBitDepth = true
        #expect(viewModel.convertBitDepth == true)
    }
    
    @Test("Should toggle flatten folders")
    func testFlattenFoldersToggle() async {
        let viewModel = ProcessingViewModel()
        #expect(viewModel.flattenFolders == false)
        
        viewModel.flattenFolders = true
        #expect(viewModel.flattenFolders == true)
        
        viewModel.flattenFolders = false
        #expect(viewModel.flattenFolders == false)
    }
    
    @Test("Should extract folder names correctly")
    func testFolderNameExtraction() async {
        let viewModel = ProcessingViewModel()
        let testPaths = [
            "/Users/test/Sample Pack 1",
            "/Users/test/Sample Pack 2",
            "/Users/test/Deep/Nested/Folder"
        ]
        
        let expectedNames = [
            "Sample Pack 1",
            "Sample Pack 2",
            "Folder"
        ]
        
        viewModel.addFolders(testPaths)
        
        for (index, expectedName) in expectedNames.enumerated() {
            #expect(viewModel.selectedFolders[index].name == expectedName)
        }
    }
    
    @Test("Should handle multiple folders")
    func testMultipleFolders() async {
        let viewModel = ProcessingViewModel()
        let testPaths = [
            "/path/to/folder1",
            "/path/to/folder2",
            "/path/to/folder3"
        ]
        
        viewModel.addFolders(testPaths)
        
        #expect(viewModel.selectedFolders.count == 3)
        
        for (index, path) in testPaths.enumerated() {
            #expect(viewModel.selectedFolders[index].path == path)
        }
    }
}