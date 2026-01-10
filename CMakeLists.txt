cmake_minimum_required(VERSION 3.10)
project(video_importer)

add_library(video_importer SHARED
    src/VideoImporter.cpp
)

find_library(log-lib log)
target_link_libraries(video_importer ${log-lib})
