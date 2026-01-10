cmake_minimum_required(VERSION 3.10)
project(video_importer)

# Build a shared library (.so)
add_library(video_importer SHARED
    src/VideoImporter.cpp
)

# If you use FFmpeg headers, add include dirs
# include_directories(src)

# Link against Android system libs if needed
find_library(log-lib log)
target_link_libraries(video_importer ${log-lib})