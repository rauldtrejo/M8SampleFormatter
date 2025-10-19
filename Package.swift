// swift-tools-version: 5.9
import PackageDescription

let package = Package(
    name: "M8FormatterGUI",
    platforms: [
        .macOS(.v11)
    ],
    products: [
        .executable(name: "M8FormatterGUI", targets: ["M8FormatterGUI"])
    ],
    dependencies: [
        // No external dependencies for now
    ],
    targets: [
        .executableTarget(
            name: "M8FormatterGUI",
            dependencies: [],
            path: "src/swift"
        )
    ]
)