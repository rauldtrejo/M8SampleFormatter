import SwiftUI
import UniformTypeIdentifiers
import AppKit

struct ContentView: View {
    @StateObject private var viewModel = ProcessingViewModel()
    
    // Function to open multi-folder picker using NSOpenPanel
    private func selectFolders(completion: @escaping ([String]) -> Void) {
        let panel = NSOpenPanel()
        panel.allowsMultipleSelection = true
        panel.canChooseDirectories = true
        panel.canChooseFiles = false
        panel.canCreateDirectories = true
        panel.prompt = "Select Folders"
        panel.message = "Select one or more sample pack folders (use ⌘ to select multiple)"
        
        panel.begin { response in
            if response == .OK {
                let paths = panel.urls.map { $0.path }
                completion(paths)
            } else {
                completion([])
            }
        }
    }
    
    // Function to open single output folder picker
    private func selectOutputFolder(completion: @escaping (String?) -> Void) {
        let panel = NSOpenPanel()
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = true
        panel.canChooseFiles = false
        panel.canCreateDirectories = true
        panel.prompt = "Select Output Folder"
        
        panel.begin { response in
            if response == .OK, let url = panel.url {
                completion(url.path)
            } else {
                completion(nil)
            }
        }
    }
    
    var body: some View {
        VStack(spacing: 0) {
            // Header Section
            VStack(spacing: 16) {
                HStack {
                    Image(systemName: "waveform.circle.fill")
                        .font(.system(size: 40))
                        .foregroundColor(.blue)
                    
                    VStack(alignment: .leading, spacing: 4) {
                        Text("M8 Sample Formatter")
                            .font(.title)
                            .fontWeight(.bold)
                        
                        Text("Shorten filenames and convert to 16-bit WAV")
                            .font(.subheadline)
                            .foregroundColor(.secondary)
                    }
                    
                    Spacer()
                }
                .padding(.horizontal, 24)
                .padding(.top, 20)
            }
            
            Divider()
            
            // Main Content - Scrollable
            ScrollView {
                VStack(spacing: 0) {
                    Spacer()
                        .frame(minHeight: 20, maxHeight: 40)
                    
                    VStack(spacing: 24) {
                    // Source Folders
                    VStack(alignment: .leading, spacing: 16) {
                        HStack {
                            Text("Source Folders")
                                .font(.title2)
                                .fontWeight(.semibold)
                            
                            Spacer()
                            
                            if !viewModel.selectedFolders.isEmpty {
                                Text("\(viewModel.selectedFolders.count)")
                                    .font(.callout)
                                    .foregroundColor(.white)
                                    .padding(.horizontal, 10)
                                    .padding(.vertical, 4)
                                    .background(Color.blue)
                                    .cornerRadius(12)
                            }
                        }
                        
                        if viewModel.selectedFolders.isEmpty {
                            VStack(spacing: 16) {
                                Image(systemName: "folder.badge.plus")
                                    .font(.system(size: 48))
                                    .foregroundColor(.secondary)
                                
                                Text("No folders selected")
                                    .font(.headline)
                                    .foregroundColor(.secondary)
                                
                                Text("Click below to add sample pack folders")
                                    .font(.caption)
                                    .foregroundColor(.secondary)
                            }
                            .frame(maxWidth: .infinity)
                            .padding(.vertical, 40)
                        } else {
                            // Table of selected folders
                            ScrollView {
                                VStack(spacing: 6) {
                                    ForEach(viewModel.selectedFolders) { folder in
                                        HStack(spacing: 12) {
                                            Image(systemName: "folder.fill")
                                                .foregroundColor(.blue)
                                                .font(.body)
                                            
                                            VStack(alignment: .leading, spacing: 4) {
                                                Text(folder.name)
                                                    .font(.body)
                                                    .fontWeight(.medium)
                                                    .lineLimit(1)
                                                
                                                Text(folder.path)
                                                    .font(.caption)
                                                    .foregroundColor(.secondary)
                                                    .lineLimit(1)
                                                    .truncationMode(.middle)
                                            }
                                            
                                            Spacer()
                                            
                                            Button(action: {
                                                viewModel.removeFolder(folder)
                                            }) {
                                                Image(systemName: "xmark.circle.fill")
                                                    .foregroundColor(.secondary)
                                                    .font(.title3)
                                            }
                                            .buttonStyle(PlainButtonStyle())
                                        }
                                        .padding(12)
                                        .background(Color(NSColor.controlBackgroundColor))
                                        .cornerRadius(8)
                                    }
                                }
                            }
                            .frame(height: 240)
                            .frame(maxHeight: 240)
                        }
                        
                        HStack(spacing: 12) {
                            Button(viewModel.selectedFolders.isEmpty ? "Add Folders..." : "Add More...") {
                                selectFolders { paths in
                                    viewModel.addFolders(paths)
                                }
                            }
                            .buttonStyle(.bordered)
                            .controlSize(.large)
                            
                            if !viewModel.selectedFolders.isEmpty {
                                Button("Clear All") {
                                    viewModel.clearAllFolders()
                                }
                                .buttonStyle(.bordered)
                                .controlSize(.large)
                            }
                        }
                    }
                    .padding(24)
                    .background(Color(NSColor.controlBackgroundColor).opacity(0.5))
                    .cornerRadius(16)
                    .frame(maxWidth: 700)
                    
                    // Output Folder
                    VStack(alignment: .leading, spacing: 16) {
                        Text("Output Folder")
                            .font(.title2)
                            .fontWeight(.semibold)
                        
                        HStack(spacing: 12) {
                            if viewModel.outputFolderPath.isEmpty {
                                Text("No output folder selected")
                                    .font(.body)
                                    .foregroundColor(.secondary)
                            } else {
                                VStack(alignment: .leading, spacing: 4) {
                                    Text(URL(fileURLWithPath: viewModel.outputFolderPath).lastPathComponent)
                                        .font(.body)
                                        .fontWeight(.medium)
                                    
                                    Text(viewModel.outputFolderPath)
                                        .font(.caption)
                                        .foregroundColor(.secondary)
                                        .lineLimit(1)
                                        .truncationMode(.middle)
                                }
                            }
                            
                            Spacer()
                            
                            Button("Browse...") {
                                selectOutputFolder { path in
                                    if let path = path {
                                        viewModel.outputFolderPath = path
                                    }
                                }
                            }
                            .buttonStyle(.bordered)
                            .controlSize(.large)
                        }
                        .padding(16)
                        .background(Color(NSColor.controlBackgroundColor))
                        .cornerRadius(8)
                    }
                    .padding(24)
                    .background(Color(NSColor.controlBackgroundColor).opacity(0.5))
                    .cornerRadius(16)
                    .frame(maxWidth: 700)
                    
                    // Processing Options
                    VStack(alignment: .leading, spacing: 16) {
                        Text("Processing Options")
                            .font(.title2)
                            .fontWeight(.semibold)
                        
                        VStack(spacing: 12) {
                            HStack {
                                Toggle("Convert to 16-bit WAV", isOn: $viewModel.convertBitDepth)
                                    .toggleStyle(SwitchToggleStyle())
                                
                                Spacer()
                            }
                            
                            HStack {
                                Toggle("Flatten folder structure", isOn: $viewModel.flattenFolders)
                                    .toggleStyle(SwitchToggleStyle())
                                
                                Spacer()
                                
                                Text("Inspired by M8 Sample Organizer")
                                    .font(.caption)
                                    .foregroundColor(.secondary)
                            }
                        }
                        .padding(16)
                        .background(Color(NSColor.controlBackgroundColor))
                        .cornerRadius(8)
                    }
                    .padding(24)
                    .background(Color(NSColor.controlBackgroundColor).opacity(0.5))
                    .cornerRadius(16)
                    .frame(maxWidth: 700)
                    
                    // Processing Status
                    if viewModel.isProcessing {
                        VStack(spacing: 16) {
                            ProgressView(value: viewModel.progress, total: 1.0)
                                .progressViewStyle(LinearProgressViewStyle())
                                .scaleEffect(y: 2.0)
                            
                            VStack(spacing: 8) {
                                HStack {
                                    Text("Processing...")
                                        .font(.headline)
                                    
                                    Spacer()
                                    
                                    Text("\(viewModel.processedFiles) files")
                                        .font(.subheadline)
                                        .foregroundColor(.secondary)
                                }
                                
                                if !viewModel.currentFolder.isEmpty {
                                    HStack {
                                        Image(systemName: "folder")
                                            .font(.caption)
                                            .foregroundColor(.blue)
                                        Text("Folder \(viewModel.processedFolders + 1) of \(viewModel.selectedFolders.count): \(viewModel.currentFolder)")
                                            .font(.caption)
                                            .foregroundColor(.secondary)
                                            .lineLimit(1)
                                            .truncationMode(.middle)
                                    }
                                }
                            }
                        }
                        .padding(24)
                        .background(Color(NSColor.controlBackgroundColor).opacity(0.5))
                        .cornerRadius(16)
                        .frame(maxWidth: 700)
                    }
                    
                    // Completion Statistics
                    if viewModel.isCompleted && !viewModel.isProcessing {
                        VStack(spacing: 16) {
                            HStack {
                                Image(systemName: "checkmark.circle.fill")
                                    .font(.system(size: 48))
                                    .foregroundColor(.green)
                                
                                VStack(alignment: .leading, spacing: 4) {
                                    Text("Processing Complete!")
                                        .font(.title2)
                                        .fontWeight(.bold)
                                        .foregroundColor(.green)
                                    
                                    Text("All sample packs have been processed")
                                        .font(.subheadline)
                                        .foregroundColor(.secondary)
                                }
                                
                                Spacer()
                            }
                            
                            Divider()
                            
                            HStack(spacing: 40) {
                                StatView(
                                    label: "Folders",
                                    value: "\(viewModel.processedFolders)",
                                    icon: "folder.fill"
                                )
                                
                                StatView(
                                    label: "Files",
                                    value: "\(viewModel.processedFiles)",
                                    icon: "doc.fill"
                                )
                                
                                StatView(
                                    label: "Time",
                                    value: String(format: "%.1fs", viewModel.processingTime),
                                    icon: "clock.fill"
                                )
                                
                                StatView(
                                    label: "Speed",
                                    value: String(format: "%.0f files/s", viewModel.averageSpeed),
                                    icon: "bolt.fill"
                                )
                            }
                            .padding(.vertical, 8)
                        }
                        .padding(24)
                        .background(Color(NSColor.controlBackgroundColor).opacity(0.5))
                        .cornerRadius(16)
                        .frame(maxWidth: 700)
                    }
                    
                    // Action Buttons
                    HStack(spacing: 16) {
                        if viewModel.isProcessing {
                            Button("Stop Processing") {
                                viewModel.stopProcessing()
                            }
                            .buttonStyle(.bordered)
                            .controlSize(.large)
                            .foregroundColor(.red)
                        } else {
                            Button(action: {
                                viewModel.startProcessing()
                            }) {
                                HStack(spacing: 8) {
                                    Image(systemName: "play.fill")
                                    Text("Start Processing")
                                }
                            }
                            .buttonStyle(.bordered)
                            .controlSize(.large)
                            .disabled(viewModel.selectedFolders.isEmpty || viewModel.outputFolderPath.isEmpty)
                            
                            if viewModel.isCompleted {
                                Button("Clear") {
                                    viewModel.reset()
                                }
                                .buttonStyle(.bordered)
                                .controlSize(.large)
                            }
                        }
                    }
                    .padding(.top, 8)
                    }
                    .padding(.horizontal, 40)
                    
                    Spacer()
                        .frame(minHeight: 20, maxHeight: 40)
                }
            }
        }
        .frame(minWidth: 800, minHeight: 700)
    }
}

struct StatView: View {
    let label: String
    let value: String
    let icon: String
    
    var body: some View {
        VStack(spacing: 8) {
            Image(systemName: icon)
                .font(.title2)
                .foregroundColor(.blue)
            
            Text(value)
                .font(.title)
                .fontWeight(.bold)
            
            Text(label)
                .font(.caption)
                .foregroundColor(.secondary)
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
