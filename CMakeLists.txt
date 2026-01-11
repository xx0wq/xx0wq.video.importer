cmake_minimum_required(VERSION 3.16)   # safe baseline for Android/iOS builds
project(video_importer LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all source files in src/
file(GLOB SOURCES src/*.cpp)

# Define the shared library
add_library(video_importer SHARED ${SOURCES})

# Include headers
target_include_directories(video_importer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Platform-specific settings
if(WIN32)
    message(STATUS "Configuring for Windows")
    # Add Windows-specific libraries if needed
    target_compile_definitions(video_importer PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
    message(STATUS "Configuring for macOS/iOS")
    target_compile_definitions(video_importer PRIVATE PLATFORM_APPLE)
elseif(ANDROID)
    message(STATUS "Configuring for Android")
    target_compile_definitions(video_importer PRIVATE PLATFORM_ANDROID)
else()
    message(STATUS "Configuring for other platform")
endif()

# Optional: warnings and stricter build flags
if(MSVC)
    target_compile_options(video_importer PRIVATE /W4 /permissive-)
else()
    target_compile_options(video_importer PRIVATE -Wall -Wextra -Wpedantic)
endif()