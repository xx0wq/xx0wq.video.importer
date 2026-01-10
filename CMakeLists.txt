cmake_minimum_required(VERSION 3.20)
project(denys_video_importer)

set(CMAKE_CXX_STANDARD 17)

# Geode
find_package(Geode REQUIRED)
add_library(denys_video_importer SHARED
    src/VideoImporter.cpp
)

target_include_directories(denys_video_importer PRIVATE
    ${GEODE_INCLUDE_DIR}
    thirdparty/ffmpeg/include
)

# Link FFmpeg (adjust paths or use find_package if available)
target_link_directories(denys_video_importer PRIVATE
    thirdparty/ffmpeg/lib
)

target_link_libraries(denys_video_importer
    Geode
    avformat
    avcodec
    swscale
    avutil
)

# Android exports
geode_link_mod(denys_video_importer)