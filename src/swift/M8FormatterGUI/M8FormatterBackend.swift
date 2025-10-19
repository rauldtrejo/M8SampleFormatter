import Foundation
import os.log

class M8FormatterBackend {
    // MARK: - Private Properties
    private var sourceDirectory: String = ""
    private var outputDirectory: String = ""
    private var convertBitDepth: Bool = true
    private var flattenFolders: Bool = false
    private var finalResults: ProcessingResults?
    
    private var isInitialized: Bool = false
    private var processingTask: Process?
    private let logger = Logger(subsystem: "com.m8formatter", category: "backend")
    
    // MARK: - Public Methods
    func initialize() async throws {
        guard !isInitialized else { return }
        
        // Check if the C++ executable exists
        let executablePath = findExecutablePath()
        guard FileManager.default.fileExists(atPath: executablePath) else {
            throw M8FormatterError.executableNotFound(executablePath)
        }
        
        isInitialized = true
        logger.info("M8FormatterBackend initialized successfully")
    }
    
    func setSourceDirectory(_ path: String) {
        sourceDirectory = path
    }
    
    func setOutputDirectory(_ path: String) {
        outputDirectory = path
    }
    
    func setConvertBitDepth(_ enabled: Bool) {
        convertBitDepth = enabled
    }
    
    func setFlattenFolders(_ enabled: Bool) {
        flattenFolders = enabled
    }
    
    func startProcessing(progressHandler: @escaping (ProcessingStatus) -> Void) async throws {
        guard isInitialized else {
            throw M8FormatterError.notInitialized
        }
        
        let executablePath = findExecutablePath()
        
        // Build command line arguments
        var arguments = [sourceDirectory, outputDirectory]
        
        if !convertBitDepth {
            arguments.append("--no-bitdepth")
        }
        
        if flattenFolders {
            arguments.append("--flatten-folders")
        }
        
        logger.info("Starting processing with arguments: \(arguments)")
        
        // Create and configure process
        let process = Process()
        process.executableURL = URL(fileURLWithPath: executablePath)
        process.arguments = arguments
        
        // Set up output handling
        let outputPipe = Pipe()
        let errorPipe = Pipe()
        process.standardOutput = outputPipe
        process.standardError = errorPipe
        
        // Start process
        try process.run()
        processingTask = process
        
        // Monitor output for progress updates
        Task {
            await monitorOutput(outputPipe: outputPipe, errorPipe: errorPipe, progressHandler: progressHandler)
        }
        
        // Wait for completion
        process.waitUntilExit()
        
        if process.terminationStatus != 0 {
            let errorData = errorPipe.fileHandleForReading.readDataToEndOfFile()
            let errorString = String(data: errorData, encoding: .utf8) ?? "Unknown error"
            throw M8FormatterError.processingFailed(errorString)
        }
    }
    
    func getResults() async throws -> ProcessingResults {
        // Return the final results parsed during processing
        guard let results = finalResults else {
            throw M8FormatterError.processingFailed("No results available")
        }
        return results
    }
    
    // MARK: - Private Methods
    private func findExecutablePath() -> String {
        // Look for the C++ executable in various locations
        let possiblePaths = [
            // In app bundle
            Bundle.main.bundlePath + "/Contents/MacOS/M8SampleFormatterBackend",
            // Development paths
            "../M8SampleFormatter/build/M8SampleFormatter",
            "./M8SampleFormatter",
            "/usr/local/bin/M8SampleFormatter"
        ]
        
        for path in possiblePaths {
            if FileManager.default.fileExists(atPath: path) {
                logger.info("Found executable at: \(path)")
                return path
            }
        }
        
        // Default fallback
        let fallbackPath = Bundle.main.bundlePath + "/Contents/MacOS/M8SampleFormatterBackend"
        logger.error("Executable not found, using fallback: \(fallbackPath)")
        return fallbackPath
    }
    
    private func monitorOutput(outputPipe: Pipe, errorPipe: Pipe, progressHandler: @escaping (ProcessingStatus) -> Void) async {
        let outputHandle = outputPipe.fileHandleForReading
        let _ = errorPipe.fileHandleForReading
        
        while let line = outputHandle.readLine() {
            let trimmedLine = line.trimmingCharacters(in: .whitespacesAndNewlines)
            
            // Parse progress information from log output
            if let status = parseProgressFromLine(trimmedLine) {
                await MainActor.run {
                    progressHandler(status)
                }
            }
            
            // Parse final stats
            if let results = parseFinalStats(trimmedLine) {
                finalResults = results
            }
        }
    }
    
    private func parseProgressFromLine(_ line: String) -> ProcessingStatus? {
        // Parse progress updates (handle Logger format: "timestamp [INFO] Progress: X% (Y/Z files)")
        if line.contains("Progress:") {
            // Extract the progress part after the log level
            if let progressStart = line.range(of: "Progress:") {
                let progressPart = String(line[progressStart.upperBound...])
                let components = progressPart.components(separatedBy: " ")
                
                if components.count >= 4,
                   let progressPercent = Int(components[1].replacingOccurrences(of: "%", with: "")), // Skip empty first component
                   let fileCountIndex = components.firstIndex(of: "files)"),
                   fileCountIndex - 1 >= 0 {
                    
                    let fileCountString = components[fileCountIndex - 1]
                    let cleanFileCountString = fileCountString.replacingOccurrences(of: "(", with: "")
                    let fileCountComponents = cleanFileCountString.components(separatedBy: "/")
                    if fileCountComponents.count == 2,
                       let processedFiles = Int(fileCountComponents[0]),
                       let totalFiles = Int(fileCountComponents[1]) {
                        
                        return ProcessingStatus(
                            progress: Double(progressPercent) / 100.0,
                            processedFiles: processedFiles,
                            totalFiles: totalFiles
                        )
                    }
                }
            }
        }
        
        // Parse total file count
        if line.contains("files found") {
            let components = line.components(separatedBy: " ")
            if let fileCount = components.first(where: { $0.allSatisfy { $0.isNumber } }) {
                return ProcessingStatus(
                    progress: 0.0,
                    processedFiles: 0,
                    totalFiles: Int(fileCount) ?? 0
                )
            }
        }
        
        return nil
    }
    
    private func parseFinalStats(_ line: String) -> ProcessingResults? {
        // Parse final stats from C++ output: "timestamp [INFO] FINAL_STATS: totalFiles processedFiles errorFiles processingTime"
        if line.contains("FINAL_STATS:") {
            // Extract the stats part after the log level
            if let statsStart = line.range(of: "FINAL_STATS:") {
                let statsPart = String(line[statsStart.upperBound...])
                let components = statsPart.components(separatedBy: " ")
                
                if components.count >= 5, // Need 5 components because of empty first one
                   let totalFiles = Int(components[1]), // Skip empty first component
                   let processedFiles = Int(components[2]),
                   let _ = Int(components[3]), // errorFiles (not used in ProcessingResults)
                   let processingTime = Double(components[4]) {
                    
                    let averageSpeed = processingTime > 0 ? Double(processedFiles) / processingTime : 0.0
                    
                    return ProcessingResults(
                        processedFiles: processedFiles,
                        totalFiles: totalFiles,
                        averageSpeed: averageSpeed
                    )
                }
            }
        }
        return nil
    }
    
    private func parseProcessingReport(_ content: String) throws -> ProcessingResults {
        let lines = content.components(separatedBy: .newlines)
        
        var processedFiles = 0
        var totalFiles = 0
        var averageSpeed = 0.0
        
        for line in lines {
            if line.contains("Total files:") {
                let components = line.components(separatedBy: ":")
                if components.count > 1 {
                    totalFiles = Int(components[1].trimmingCharacters(in: .whitespaces)) ?? 0
                }
            } else if line.contains("Processed:") {
                let components = line.components(separatedBy: ":")
                if components.count > 1 {
                    processedFiles = Int(components[1].trimmingCharacters(in: .whitespaces)) ?? 0
                }
            } else if line.contains("Average speed:") {
                let components = line.components(separatedBy: ":")
                if components.count > 1 {
                    let speedString = components[1].trimmingCharacters(in: .whitespaces)
                    let speedComponents = speedString.components(separatedBy: " ")
                    if let speed = Double(speedComponents.first ?? "0") {
                        averageSpeed = speed
                    }
                }
            }
        }
        
        return ProcessingResults(
            processedFiles: processedFiles,
            totalFiles: totalFiles,
            averageSpeed: averageSpeed
        )
    }
}

// MARK: - Supporting Types
enum M8FormatterError: LocalizedError {
    case executableNotFound(String)
    case notInitialized
    case processingFailed(String)
    case reportNotFound(String)
    
    var errorDescription: String? {
        switch self {
        case .executableNotFound(let path):
            return "M8SampleFormatter executable not found at: \(path)"
        case .notInitialized:
            return "Backend not initialized"
        case .processingFailed(let message):
            return "Processing failed: \(message)"
        case .reportNotFound(let path):
            return "Processing report not found at: \(path)"
        }
    }
}

// MARK: - Extensions
extension FileHandle {
    func readLine() -> String? {
        let data = self.readData(ofLength: 1)
        guard !data.isEmpty else { return nil }
        
        var line = Data()
        var currentData = data
        
        while !currentData.isEmpty {
            if let char = String(data: currentData, encoding: .utf8), char == "\n" {
                break
            }
            line.append(currentData)
            currentData = self.readData(ofLength: 1)
        }
        
        return String(data: line, encoding: .utf8)
    }
}