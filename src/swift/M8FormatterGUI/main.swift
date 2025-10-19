import SwiftUI

struct M8FormatterGUIApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
                .frame(minWidth: 900, minHeight: 700)
        }
        .windowStyle(.automatic)
    }
}

// Entry point for the app
M8FormatterGUIApp.main()