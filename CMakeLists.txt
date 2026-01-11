cmake_minimum_required(VERSION 3.16)
project(xx0wq_video_importer VERSION 0.70.93 LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to Geode SDK headers (set by workflow)
if(NOT DEFINED GEODE_SDK_DIR)
    set(GEODE_SDK_DIR "${CMAKE_SOURCE_DIR}/setup")
endif()

include_directories("${GEODE_SDK_DIR}/include")

# Platform detection
if(WIN32)
    add_definitions(-DGEODE_PLATFORM_WINDOWS=1)
elseif(APPLE)
    if(IOS)
        add_definitions(-DGEODE_PLATFORM_IOS=1)
    else()
        add_definitions(-DGEODE_PLATFORM_MACOS=1)
    endif()
elseif(ANDROID)
    add_definitions(-DGEODE_PLATFORM_ANDROID=1)
else()
    message(FATAL_ERROR "Unsupported platform for Geode mod")
endif()

# Source files
set(SOURCES
    src/main.cpp
    # add other .cpp files here
)

# Build shared library (mod binary)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Output name matches workflow expectations
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "xx0wq.video.importer.win64"
    )
elseif(APPLE)
    if(IOS)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            OUTPUT_NAME "xx0wq.video.importer.ios"
        )
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES
            OUTPUT_NAME "xx0wq.video.importer.macos"
        )
    endif()
elseif(ANDROID)
    if(${ANDROID_ABI} STREQUAL "arm64-v8a")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            OUTPUT_NAME "xx0wq.video.importer.android64"
        )
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES
            OUTPUT_NAME "xx0wq.video.importer.android32"
        )
    endif()
endif()

# Link FFmpeg (for video importer functionality)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
)

target_link_libraries(${PROJECT_NAME}
    PkgConfig::FFMPEG
)

# Install step (optional, for packaging)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
    LIBRARY DESTINATION .
)

# Copy resources (mod.json, icons, etc.)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/mod.json
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/mod.json
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)
