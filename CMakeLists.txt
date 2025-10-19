cmake_minimum_required(VERSION 3.20)
project(M8SampleFormatter VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=arm64 -mtune=native -mcpu=apple-m1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=arm64 -mtune=native -mcpu=apple-m1")
    
    message(STATUS "Apple Silicon optimizations enabled")
endif()

# Find required packages
find_package(Threads)
find_package(PkgConfig REQUIRED)
find_library(SNDFILE_LIBRARY NAMES sndfile libsndfile)
pkg_check_modules(LIBSNDFILE REQUIRED sndfile)

# Include directories
include_directories(${LIBSNDFILE_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/audio)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/cpp/filesystem)

# Source files
set(SOURCES
    src/cpp/main.cpp
    src/cpp/audio/AudioProcessor.cpp
    src/cpp/audio/AppleSiliconProcessor.cpp
    src/cpp/filesystem/FileScanner.cpp
    src/cpp/filesystem/PathManager.cpp
    src/cpp/filesystem/FileOperations.cpp
    src/cpp/utils/ThreadPool.cpp
    src/cpp/utils/Logger.cpp
)

# Headers
set(HEADERS
    src/cpp/audio/AudioProcessor.h
    src/cpp/audio/AppleSiliconProcessor.h
    src/cpp/filesystem/FileScanner.h
    src/cpp/filesystem/PathManager.h
    src/cpp/filesystem/FileOperations.h
    src/cpp/utils/ThreadPool.h
    src/cpp/utils/Logger.h
)

# Create executable
add_executable(M8SampleFormatter ${SOURCES} ${HEADERS})

# Link libraries
if(Threads_FOUND)
    target_link_libraries(M8SampleFormatter Threads::Threads)
endif()
target_link_libraries(M8SampleFormatter 
    ${LIBSNDFILE_LIBRARIES}
    ${SNDFILE_LIBRARY}
    ${FRAMEWORKS}
)

# Add library directories
target_link_directories(M8SampleFormatter PRIVATE ${LIBSNDFILE_LIBRARY_DIRS})

# Compiler flags for optimization
target_compile_options(M8SampleFormatter PRIVATE
    -O3
    -march=native
    -mtune=native
    -flto
    -ffast-math
)

# Link time optimization
set_target_properties(M8SampleFormatter PROPERTIES
    LINK_FLAGS "-flto"
)

# Install target
install(TARGETS M8SampleFormatter DESTINATION bin)

# Testing
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        enable_testing()
        add_subdirectory(tests/cpp)
        message(STATUS "Google Test found, building tests")
    else()
        message(WARNING "Google Test not found, skipping tests. Install with: brew install googletest")
    endif()
endif()
