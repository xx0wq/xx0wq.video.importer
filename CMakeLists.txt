# Headers fetched at build time
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/ffmpeg/include)

# Choose the AAR-extracted .so by ABI
if(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
    set(FFMPEG_LIB_DIR ${CMAKE_SOURCE_DIR}/thirdparty/ffmpeg/lib/arm64-v8a)
elseif(${CMAKE_ANDROID_ARCH_ABI} STREQUAL "armeabi-v7a")
    set(FFMPEG_LIB_DIR ${CMAKE_SOURCE_DIR}/thirdparty/ffmpeg/lib/armeabi-v7a)
endif()

# Import the merged libffmpeg.so from FFmpegKit
add_library(ffmpeg SHARED IMPORTED)
set_target_properties(ffmpeg PROPERTIES IMPORTED_LOCATION ${FFMPEG_LIB_DIR}/libffmpeg.so)

# Your library
add_library(video_importer SHARED
    src/VideoImporter.cpp
    # add your source files here
)

# Link
target_link_libraries(video_importer ffmpeg)