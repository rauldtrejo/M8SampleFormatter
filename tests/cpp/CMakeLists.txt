cmake_minimum_required(VERSION 3.20)

# Find required packages
find_package(Threads QUIET)
find_package(GTest QUIET)
find_package(PkgConfig REQUIRED)
find_library(SNDFILE_LIBRARY NAMES sndfile libsndfile)
pkg_check_modules(LIBSNDFILE REQUIRED sndfile)

# Only proceed if we have the required dependencies
if(NOT GTest_FOUND)
    message(FATAL_ERROR "Google Test not found. Install with: brew install googletest")
endif()

# Threads might not be found on macOS, but pthread is always available
if(NOT Threads_FOUND)
    message(STATUS "Threads package not found, but pthread should be available on macOS")
endif()

# Include directories
include_directories(${LIBSNDFILE_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/cpp/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/cpp/audio)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/cpp/filesystem)

# Apple Silicon optimizations
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
    
    # Enable Apple Silicon optimized frameworks
    find_library(ACCELERATE_FRAMEWORK Accelerate)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
    find_library(AUDIOUNIT_FRAMEWORK AudioUnit)
    find_library(COREAUDIO_FRAMEWORK CoreAudio)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    
    # Link frameworks optimized for Apple Silicon
    set(FRAMEWORKS ${ACCELERATE_FRAMEWORK} ${AUDIOTOOLBOX_FRAMEWORK} ${AUDIOUNIT_FRAMEWORK} 
                   ${COREAUDIO_FRAMEWORK} ${FOUNDATION_FRAMEWORK} ${COREFOUNDATION_FRAMEWORK})
    
    # Enable Apple Silicon specific optimizations
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=native")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=native")
endif()

# Source files for tests
set(TEST_SOURCES
    test_main.cpp
    test_audio_processor.cpp
    test_file_scanner.cpp
    test_path_manager.cpp
)

# Source files from main project
set(PROJECT_SOURCES
    ../../src/cpp/audio/AudioProcessor.cpp
    ../../src/cpp/audio/AppleSiliconProcessor.cpp
    ../../src/cpp/filesystem/FileScanner.cpp
    ../../src/cpp/filesystem/PathManager.cpp
    ../../src/cpp/filesystem/FileOperations.cpp
    ../../src/cpp/utils/ThreadPool.cpp
    ../../src/cpp/utils/Logger.cpp
)

# Create test executable
add_executable(m8_formatter_tests ${TEST_SOURCES} ${PROJECT_SOURCES})

# Link libraries
if(Threads_FOUND)
    target_link_libraries(m8_formatter_tests Threads::Threads)
else()
    # On macOS, pthread is always available, so we can link it directly
    target_link_libraries(m8_formatter_tests pthread)
endif()
target_link_libraries(m8_formatter_tests 
    GTest::gtest
    GTest::gtest_main
    ${LIBSNDFILE_LIBRARIES}
    ${SNDFILE_LIBRARY}
    ${FRAMEWORKS}
)

# Add library directories
target_link_directories(m8_formatter_tests PRIVATE ${LIBSNDFILE_LIBRARY_DIRS})

# Compiler flags
target_compile_options(m8_formatter_tests PRIVATE
    -Wall
    -Wextra
    -O2
)

# Enable testing
enable_testing()

# Add test
add_test(NAME m8_formatter_tests COMMAND m8_formatter_tests)

# Set test properties
set_tests_properties(m8_formatter_tests PROPERTIES
    TIMEOUT 300
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
