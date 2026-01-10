cmake_minimum_required(VERSION 3.16)
project(video_importer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(video_importer SHARED
    src/VideoImporter.cpp
    src/EditorIntegration.cpp
    src/VideoDecoder.cpp
    src/Utils.cpp
)

target_include_directories(video_importer PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Android logging (optional)
if(ANDROID)
    find_library(log-lib log)
    if(log-lib)
        target_link_libraries(video_importer ${log-lib})
    endif()
endif()

# Optional FFmpeg linking (disabled by default)
option(VI_USE_FFMPEG "Enable FFmpeg for real decoding" OFF)
if(VI_USE_FFMPEG)
    # Example: you must set these paths or find packages
    # find_package(FFmpeg REQUIRED COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE)
    # target_link_libraries(video_importer FFmpeg::AVCODEC FFmpeg::AVFORMAT FFmpeg::AVUTIL FFmpeg::SWSCALE)
endif()

# Export symbols on Windows if needed
if(WIN32)
    target_compile_definitions(video_importer PRIVATE VIDEO_IMPORTER_EXPORTS)
endif()