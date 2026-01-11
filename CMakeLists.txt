cmake_minimum_required(VERSION 3.18)

project(xx0wq_video_importer
    VERSION 0.70.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Sources ---
set(SOURCES
    src/main.cpp
)

# --- Target ---
add_library(xx0wq_video_importer SHARED ${SOURCES})

# --- Geode SDK include path ---
if(NOT DEFINED GEODE_SDK_DIR)
    set(GEODE_SDK_DIR "${CMAKE_SOURCE_DIR}/external/geode")
endif()

set(GEODE_INCLUDE_DIR "${GEODE_SDK_DIR}/include")

# Validate Geode headers early with a helpful error
if(NOT EXISTS "${GEODE_INCLUDE_DIR}/Geode/Geode.hpp")
    message(FATAL_ERROR
        "Geode headers not found at: ${GEODE_INCLUDE_DIR}\n"
        "Expected: ${GEODE_INCLUDE_DIR}/Geode/Geode.hpp\n"
        "Tip: Ensure workflow clones geode to external/geode and passes -DGEODE_SDK_DIR"
    )
endif()

target_include_directories(xx0wq_video_importer PRIVATE
    "${GEODE_INCLUDE_DIR}"
)

# --- FFmpeg linking ---
if(WIN32)
    if(NOT DEFINED ENV{FFMPEG_DIR})
        message(FATAL_ERROR "FFMPEG_DIR environment variable not set on Windows")
    endif()

    target_include_directories(xx0wq_video_importer PRIVATE
        "$ENV{FFMPEG_DIR}/include"
    )
    target_link_libraries(xx0wq_video_importer
        "$ENV{FFMPEG_DIR}/lib/avcodec.lib"
        "$ENV{FFMPEG_DIR}/lib/avformat.lib"
        "$ENV{FFMPEG_DIR}/lib/avutil.lib"
        "$ENV{FFMPEG_DIR}/lib/swscale.lib"
    )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
        libavcodec
        libavformat
        libavutil
        libswscale
    )
    target_link_libraries(xx0wq_video_importer PkgConfig::FFMPEG)
endif()

# --- Output names per platform ---
if(WIN32)
    set_target_properties(xx0wq_video_importer PROPERTIES OUTPUT_NAME "xx0wq.video.importer.win64")
elseif(APPLE)
    set_target_properties(xx0wq_video_importer PROPERTIES OUTPUT_NAME "xx0wq.video.importer.macos")
elseif(ANDROID)
    set_target_properties(xx0wq_video_importer PROPERTIES OUTPUT_NAME "xx0wq.video.importer.android${ANDROID_ABI}")
else()
    set_target_properties(xx0wq_video_importer PROPERTIES OUTPUT_NAME "xx0wq.video.importer.linux")
endif()

# --- Copy resources after build (optional) ---
add_custom_command(TARGET xx0wq_video_importer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:xx0wq_video_importer>/resources"
)
