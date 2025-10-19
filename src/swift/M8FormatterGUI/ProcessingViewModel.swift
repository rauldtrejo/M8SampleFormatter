import Foundation
import SwiftUI

struct SelectedFolder: Identifiable {
    let id = UUID()
    let path: String
    let name: String
}

@MainActor
class ProcessingViewModel: ObservableObject {
    // MARK: - Published Properties
    @Published var selectedFolders: [SelectedFolder] = []
    @Published var outputFolderPath: String = ""
    @Published var convertBitDepth: Bool = true
    @Published var flattenFolders: Bool = false
    
    // Processing State
    @Published var isProcessing: Bool = false
    @Published var isCompleted: Bool = false
    @Published var progress: Double = 0.0
    @Published var processedFiles: Int = 0
    @Published var totalFiles: Int = 0
    @Published var processedFolders: Int = 0
    @Published var currentFolder: String = ""
    @Published var processingTime: Double = 0.0
    @Published var averageSpeed: Double = 0.0
    
    // Error Handling
    @Published var errorMessage: String = ""
    @Published var showingError: Bool = false
    
    // MARK: - Private Properties
    private var processingTask: Task<Void, Never>?
    private let backend = M8FormatterBackend()
    
    // MARK: - Public Methods
    func addFolders(_ paths: [String]) {
        for path in paths {
            let folderName = URL(fileURLWithPath: path).lastPathComponent
            let folder = SelectedFolder(path: path, name: folderName)
            
            // Only add if not already in list
            if !selectedFolders.contains(where: { $0.path == path }) {
                selectedFolders.append(folder)
            }
        }
    }
    
    func removeFolder(_ folder: SelectedFolder) {
        selectedFolders.removeAll { $0.id == folder.id }
    }
    
    func clearAllFolders() {
        selectedFolders.removeAll()
    }
    
    func startProcessing() {
        guard !selectedFolders.isEmpty && !outputFolderPath.isEmpty else {
            return
        }
        
        guard !isProcessing else { return }
        
        isProcessing = true
        isCompleted = false
        progress = 0.0
        processedFiles = 0
        processedFolders = 0
        totalFiles = 0
        currentFolder = ""
        processingTime = 0.0
        averageSpeed = 0.0
        errorMessage = ""
        
        processingTask = Task {
            await processAllFolders()
        }
    }
    
    func stopProcessing() {
        processingTask?.cancel()
        isProcessing = false
        currentFolder = ""
    }
    
    func reset() {
        stopProcessing()
        isCompleted = false
        progress = 0.0
        processedFiles = 0
        processedFolders = 0
        totalFiles = 0
        currentFolder = ""
        processingTime = 0.0
        averageSpeed = 0.0
        errorMessage = ""
    }
    
    // MARK: - Private Methods
    private func processAllFolders() async {
        let startTime = Date()
        var totalFilesProcessed = 0
        var totalFilesInAllFolders = 0
        
        do {
            try await backend.initialize()
            
            // Process each folder
            for (index, folder) in selectedFolders.enumerated() {
                guard !Task.isCancelled else {
                    break
                }
                
                await MainActor.run {
                    self.currentFolder = folder.name
                    self.processedFolders = index
                }
                
                do {
                    // Set processing options for this folder
                    backend.setSourceDirectory(folder.path)
                    backend.setOutputDirectory(outputFolderPath)
                    backend.setConvertBitDepth(convertBitDepth)
                    backend.setFlattenFolders(flattenFolders)
                    
                    // Start processing with callback
                    try await backend.startProcessing { [weak self] status in
                        guard let self = self else { return }
                        Task { @MainActor in
                            self.updateProgress(status, folderIndex: index, totalFolders: self.selectedFolders.count, cumulativeProcessedFiles: totalFilesProcessed, cumulativeTotalFiles: totalFilesInAllFolders)
                        }
                    }
                    
                    // Get results for this folder
                    let results = try await backend.getResults()
                    
                    await MainActor.run {
                        totalFilesProcessed += results.processedFiles
                        totalFilesInAllFolders += results.totalFiles
                        self.processedFolders = index + 1
                    }
                    
                } catch {
                    // Continue processing other folders even if one fails
                    continue
                }
            }
            
            await MainActor.run {
                self.isProcessing = false
                self.isCompleted = true
                self.processingTime = Date().timeIntervalSince(startTime)
                self.processedFiles = totalFilesProcessed
                self.totalFiles = totalFilesInAllFolders
                self.averageSpeed = self.processingTime > 0 ? Double(totalFilesProcessed) / self.processingTime : 0
                self.progress = 1.0
                self.currentFolder = ""
            }
            
        } catch {
            await MainActor.run {
                self.isProcessing = false
                self.showError("Processing failed: \(error.localizedDescription)")
            }
        }
    }
    
    private func updateProgress(_ status: ProcessingStatus, folderIndex: Int, totalFolders: Int, cumulativeProcessedFiles: Int, cumulativeTotalFiles: Int) {
        // Calculate cumulative progress across all folders
        let folderProgress = status.progress
        let cumulativeProgress = (Double(folderIndex) + folderProgress) / Double(totalFolders)
        
        progress = cumulativeProgress
        
        // Show cumulative processed files (files from previous folders + current folder progress)
        processedFiles = cumulativeProcessedFiles + status.processedFiles
        
        // Show cumulative total files (files from previous folders + current folder total)
        totalFiles = cumulativeTotalFiles + status.totalFiles
    }
    
    private func showError(_ message: String) {
        errorMessage = message
        showingError = true
    }
}

// MARK: - Supporting Types
struct ProcessingStatus {
    let progress: Double
    let processedFiles: Int
    let totalFiles: Int
}

struct ProcessingResults {
    let processedFiles: Int
    let totalFiles: Int
    let averageSpeed: Double
}
