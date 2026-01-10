cmake_minimum_required(VERSION 3.10)
project(video_importer)

# Build a shared library from your source file(s)
add_library(video_importer SHARED
    src/VideoImporter.cpp
)

# Android logging library (safe to include everywhere)
find_library(log-lib log)

# Link against system log (Android) or nothing extra (Windows/macOS)
if(log-lib)
    target_link_libraries(video_importer ${log-lib})
endif()
