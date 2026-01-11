cmake_minimum_required(VERSION 3.16)
project(video_importer VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(video_importer SHARED
    src/VideoImporter.cpp
    src/EditorIntegration.cpp
    src/VideoDecoder.cpp
    src/Utils.cpp
    src/Export.hpp
)

target_include_directories(video_importer PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Remove default "lib" prefix on Unix so names match exactly
set_target_properties(video_importer PROPERTIES PREFIX "")

# --- Platform-specific output naming ---
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set_target_properties(video_importer PROPERTIES
        OUTPUT_NAME "xx0wq.video.importer.ios"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
elseif(ANDROID)
    if(ANDROID_ABI STREQUAL "arm64-v8a")
        set(_android_name "xx0wq.video.importer.android64")
    else()
        set(_android_name "xx0wq.video.importer.android32")
    endif()
    set_target_properties(video_importer PROPERTIES
        OUTPUT_NAME "${_android_name}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
elseif(WIN32)
    set_target_properties(video_importer PROPERTIES
        OUTPUT_NAME "xx0wq.video.importer.win64"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        # Multi-config (Debug/Release) placement
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
    )
else()
    # macOS or Linux fallback
    if(APPLE)
        set(_unix_name "xx0wq.video.importer.macos")
    else()
        set(_unix_name "xx0wq.video.importer.linux64")
    endif()
    set_target_properties(video_importer PROPERTIES
        OUTPUT_NAME "${_unix_name}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

# --- Windows export macro ---
if(WIN32)
    target_compile_definitions(video_importer PRIVATE VIDEO_IMPORTER_EXPORTS)
endif()

message(STATUS "Target will be written to: ${CMAKE_SOURCE_DIR}/$<TARGET_FILE_NAME:video_importer>")