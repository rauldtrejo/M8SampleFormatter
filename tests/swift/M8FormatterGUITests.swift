import XCTest
import SwiftUI
@testable import M8FormatterGUI

class M8FormatterGUITests: XCTestCase {
    var viewModel: ProcessingViewModel!
    
    override func setUp() {
        super.setUp()
        viewModel = ProcessingViewModel()
    }
    
    override func tearDown() {
        viewModel = nil
        super.tearDown()
    }
    
    func testInitialState() {
        XCTAssertFalse(viewModel.isProcessing)
        XCTAssertFalse(viewModel.isCompleted)
        XCTAssertEqual(viewModel.progress, 0.0)
        XCTAssertEqual(viewModel.processedFiles, 0)
        XCTAssertEqual(viewModel.totalFiles, 0)
        XCTAssertEqual(viewModel.processedFolders, 0)
        XCTAssertEqual(viewModel.currentFolder, "")
        XCTAssertEqual(viewModel.processingTime, 0.0)
        XCTAssertEqual(viewModel.averageSpeed, 0.0)
        XCTAssertEqual(viewModel.errorMessage, "")
        XCTAssertFalse(viewModel.showingError)
    }
    
    func testAddFolders() {
        let testPaths = ["/path/to/folder1", "/path/to/folder2"]
        
        viewModel.addFolders(testPaths)
        
        XCTAssertEqual(viewModel.selectedFolders.count, 2)
        XCTAssertEqual(viewModel.selectedFolders[0].path, "/path/to/folder1")
        XCTAssertEqual(viewModel.selectedFolders[1].path, "/path/to/folder2")
    }
    
    func testAddDuplicateFolders() {
        let testPath = "/path/to/folder"
        
        viewModel.addFolders([testPath])
        viewModel.addFolders([testPath]) // Add same path again
        
        XCTAssertEqual(viewModel.selectedFolders.count, 1) // Should not add duplicate
    }
    
    func testRemoveFolder() {
        let testPaths = ["/path/to/folder1", "/path/to/folder2"]
        viewModel.addFolders(testPaths)
        
        let folderToRemove = viewModel.selectedFolders[0]
        viewModel.removeFolder(folderToRemove)
        
        XCTAssertEqual(viewModel.selectedFolders.count, 1)
        XCTAssertEqual(viewModel.selectedFolders[0].path, "/path/to/folder2")
    }
    
    func testClearAllFolders() {
        let testPaths = ["/path/to/folder1", "/path/to/folder2", "/path/to/folder3"]
        viewModel.addFolders(testPaths)
        
        viewModel.clearAllFolders()
        
        XCTAssertEqual(viewModel.selectedFolders.count, 0)
    }
    
    func testStartProcessingWithoutFolders() {
        viewModel.outputFolderPath = "/output"
        
        viewModel.startProcessing()
        
        XCTAssertFalse(viewModel.isProcessing)
    }
    
    func testStartProcessingWithoutOutputPath() {
        viewModel.addFolders(["/path/to/folder"])
        
        viewModel.startProcessing()
        
        XCTAssertFalse(viewModel.isProcessing)
    }
    
    func testStartProcessingWithValidInputs() {
        viewModel.addFolders(["/path/to/folder"])
        viewModel.outputFolderPath = "/output"
        
        viewModel.startProcessing()
        
        XCTAssertTrue(viewModel.isProcessing)
        XCTAssertFalse(viewModel.isCompleted)
        XCTAssertEqual(viewModel.progress, 0.0)
        XCTAssertEqual(viewModel.processedFiles, 0)
        XCTAssertEqual(viewModel.totalFiles, 0)
        XCTAssertEqual(viewModel.processedFolders, 0)
        XCTAssertEqual(viewModel.currentFolder, "")
        XCTAssertEqual(viewModel.processingTime, 0.0)
        XCTAssertEqual(viewModel.averageSpeed, 0.0)
        XCTAssertEqual(viewModel.errorMessage, "")
    }
    
    func testStopProcessing() {
        viewModel.addFolders(["/path/to/folder"])
        viewModel.outputFolderPath = "/output"
        viewModel.startProcessing()
        
        viewModel.stopProcessing()
        
        XCTAssertFalse(viewModel.isProcessing)
        XCTAssertEqual(viewModel.currentFolder, "")
    }
    
    func testReset() {
        viewModel.addFolders(["/path/to/folder"])
        viewModel.outputFolderPath = "/output"
        viewModel.startProcessing()
        
        viewModel.reset()
        
        XCTAssertFalse(viewModel.isProcessing)
        XCTAssertFalse(viewModel.isCompleted)
        XCTAssertEqual(viewModel.progress, 0.0)
        XCTAssertEqual(viewModel.processedFiles, 0)
        XCTAssertEqual(viewModel.totalFiles, 0)
        XCTAssertEqual(viewModel.processedFolders, 0)
        XCTAssertEqual(viewModel.currentFolder, "")
        XCTAssertEqual(viewModel.processingTime, 0.0)
        XCTAssertEqual(viewModel.averageSpeed, 0.0)
        XCTAssertEqual(viewModel.errorMessage, "")
    }
    
    func testConvertBitDepthToggle() {
        XCTAssertTrue(viewModel.convertBitDepth)
        
        viewModel.convertBitDepth = false
        XCTAssertFalse(viewModel.convertBitDepth)
        
        viewModel.convertBitDepth = true
        XCTAssertTrue(viewModel.convertBitDepth)
    }
    
    func testFlattenFoldersToggle() {
        XCTAssertFalse(viewModel.flattenFolders)
        
        viewModel.flattenFolders = true
        XCTAssertTrue(viewModel.flattenFolders)
        
        viewModel.flattenFolders = false
        XCTAssertFalse(viewModel.flattenFolders)
    }
    
    func testSelectedFolderProperties() {
        let testPath = "/path/to/test/folder"
        let expectedName = "folder"
        
        viewModel.addFolders([testPath])
        
        let folder = viewModel.selectedFolders[0]
        XCTAssertEqual(folder.path, testPath)
        XCTAssertEqual(folder.name, expectedName)
        XCTAssertNotNil(folder.id)
    }
    
    func testMultipleFolders() {
        let testPaths = [
            "/path/to/folder1",
            "/path/to/folder2",
            "/path/to/folder3"
        ]
        
        viewModel.addFolders(testPaths)
        
        XCTAssertEqual(viewModel.selectedFolders.count, 3)
        
        for (index, path) in testPaths.enumerated() {
            XCTAssertEqual(viewModel.selectedFolders[index].path, path)
        }
    }
    
    func testFolderNameExtraction() {
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
            XCTAssertEqual(viewModel.selectedFolders[index].name, expectedName)
        }
    }
}

// MARK: - Integration Tests
class M8FormatterIntegrationTests: XCTestCase {
    var viewModel: ProcessingViewModel!
    
    override func setUp() {
        super.setUp()
        viewModel = ProcessingViewModel()
    }
    
    override func tearDown() {
        viewModel = nil
        super.tearDown()
    }
    
    func testProcessingWorkflow() {
        // Setup
        viewModel.addFolders(["/path/to/folder1", "/path/to/folder2"])
        viewModel.outputFolderPath = "/output"
        viewModel.convertBitDepth = true
        viewModel.flattenFolders = false
        
        // Start processing
        viewModel.startProcessing()
        
        // Verify initial state
        XCTAssertTrue(viewModel.isProcessing)
        XCTAssertFalse(viewModel.isCompleted)
        XCTAssertEqual(viewModel.progress, 0.0)
        
        // Simulate processing completion
        viewModel.reset()
        
        // Verify final state
        XCTAssertFalse(viewModel.isProcessing)
        XCTAssertFalse(viewModel.isCompleted)
    }
    
    func testErrorHandling() {
        // This would test error scenarios in a real implementation
        // For now, we'll test the error state properties
        XCTAssertEqual(viewModel.errorMessage, "")
        XCTAssertFalse(viewModel.showingError)
    }
}

// MARK: - Performance Tests
class M8FormatterPerformanceTests: XCTestCase {
    var viewModel: ProcessingViewModel!
    
    override func setUp() {
        super.setUp()
        viewModel = ProcessingViewModel()
    }
    
    override func tearDown() {
        viewModel = nil
        super.tearDown()
    }
    
    func testLargeNumberOfFolders() {
        let numberOfFolders = 1000
        let folderPaths = (0..<numberOfFolders).map { "/path/to/folder\($0)" }
        
        measure {
            viewModel.addFolders(folderPaths)
        }
        
        XCTAssertEqual(viewModel.selectedFolders.count, numberOfFolders)
    }
    
    func testFolderOperationsPerformance() {
        let folderPaths = (0..<100).map { "/path/to/folder\($0)" }
        viewModel.addFolders(folderPaths)
        
        measure {
            for _ in 0..<1000 {
                let randomIndex = Int.random(in: 0..<viewModel.selectedFolders.count)
                let folder = viewModel.selectedFolders[randomIndex]
                viewModel.removeFolder(folder)
                viewModel.addFolders([folder.path])
            }
        }
    }
}
