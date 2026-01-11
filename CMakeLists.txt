cmake_minimum_required(VERSION 3.18)
project(video_importer)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files
set(SOURCES
    src/main.cpp
)

# Create shared library (Geode mod binary)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Include Geode SDK headers
# Make sure GEODE_SDK_DIR points to your Geode SDK path
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GEODE_SDK_DIR}/include
)

# Link against Geode
target_link_libraries(${PROJECT_NAME} geode)

# --- Platform specific settings ---
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "xx0wq.video.importer.win64"
    )
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "xx0wq.video.importer.macos"
    )
elseif(ANDROID)
    # Build both 32 and 64 bit if needed
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME