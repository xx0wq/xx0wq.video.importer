# --- FFmpeg linking ---
if(NOT WIN32)
    # Linux/macOS: use pkg-config to find system FFmpeg
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
        libavcodec
        libavformat
        libavutil
        libswscale
    )
    target_link_libraries(${PROJECT_NAME} PkgConfig::FFMPEG)
else()
    # Windows: link against FFmpeg libs directly (assuming they are installed)
    # Adjust paths if your FFmpeg dev build is in a different location
    target_include_directories(${PROJECT_NAME} PRIVATE
        $ENV{FFMPEG_DIR}/include
    )
    target_link_libraries(${PROJECT_NAME}
        $ENV{FFMPEG_DIR}/lib/avcodec.lib
        $ENV{FFMPEG_DIR}/lib/avformat.lib
        $ENV{FFMPEG_DIR}/lib/avutil.lib
        $ENV{FFMPEG_DIR}/lib/swscale.lib
    )
endif()