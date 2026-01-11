cmake_minimum_required(VERSION 3.18)
project(video_importer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources
set(SOURCES
    src/main.cpp
)

# Build shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Geode SDK
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GEODE_SDK_DIR}/include
)
target_link_libraries(${PROJECT_NAME} geode)

# FFmpeg
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
    libavcodec
    libavformat
    libavutil
    libswscale
)
target_link_libraries(${PROJECT_NAME} PkgConfig::FFMPEG)

# Platform-specific output names
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xx0wq.video.importer.win64")
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xx0wq.video.importer.macos")
elseif(ANDROID)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xx0wq.video.importer.android${ANDROID_ABI}")
elseif(IOS)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xx0wq.video.importer.ios")
endif()

# Copy resources after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)
