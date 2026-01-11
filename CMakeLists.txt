cmake_minimum_required(VERSION 3.20)
project(xx0wq_video_importer VERSION 0.70.93 LANGUAGES CXX)

# Use C++20 (Geode headers use concepts/requires)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Path to Geode SDK headers (provided by CI as -DGEODE_SDK_DIR="$PWD/setup")
if(NOT DEFINED GEODE_SDK_DIR)
    set(GEODE_SDK_DIR "${CMAKE_SOURCE_DIR}/setup")
endif()

# Core Geode includes and STL shims
include_directories(
    "${GEODE_SDK_DIR}/include"
    "${GEODE_SDK_DIR}/include/Geode"
    "${GEODE_SDK_DIR}/include/Geode/c++stl"
)

# Explicit sanity checks so mis-synced headers fail fast with a clear message
if(NOT EXISTS "${GEODE_SDK_DIR}/include/Geode/Bindings.hpp")
    message(FATAL_ERROR "Geode headers missing: ${GEODE_SDK_DIR}/include/Geode/Bindings.hpp not found")
endif()
if(NOT EXISTS "${GEODE_SDK_DIR}/include/Geode/c++stl/string.hpp")
    message(FATAL_ERROR "Geode STL shim missing: ${GEODE_SDK_DIR}/include/Geode/c++stl/string.hpp not found")
endif()

# Platform defines for Geode
if(WIN32)
    add_compile_definitions(GEODE_PLATFORM_WINDOWS=1)
elseif(APPLE)
    # CMake defines IOS when using an iOS toolchain
    if(IOS)
        add_compile_definitions(GEODE_PLATFORM_IOS=1)
    else()
        add_compile_definitions(GEODE_PLATFORM_MACOS=1)
    endif()
elseif(ANDROID)
    add_compile_definitions(GEODE_PLATFORM_ANDROID=1)
else()
    message(FATAL_ERROR "Unsupported platform for Geode mod")
endif()

# Sources
set(SOURCES
    src/main.cpp
    # add other source files as needed
)

# Shared library (mod binary)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Output names expected by your workflow
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xx0wq.video.importer.win64")
elseif(APPLE)
    if(IOS)
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xx0wq.video.importer.ios")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xx0wq.video.importer.macos")
    endif()
elseif(ANDROID)
    # ANDROID_ABI is defined by the Android toolchain; default to android32 if not present
    if(DEFINED ANDROID_ABI AND ANDROID_ABI STREQUAL "arm64-v8a")
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xx0wq.video.importer.android64")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xx0wq.video.importer.android32")
    endif()
endif()

# FFmpeg: link only on desktop (Android/iOS toolchains don't provide pkg-config for FFmpeg)
if(NOT ANDROID AND NOT IOS)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
        libavcodec
        libavformat
        libavutil
        libswscale
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::FFMPEG)
endif()

# Install (optional)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
    LIBRARY DESTINATION .
    ARCHIVE DESTINATION .
)

# Copy mod.json and resources next to the built binary (for packaging)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/mod.json"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/mod.json"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
)
