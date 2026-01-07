cmake_minimum_required(VERSION 3.15)
project(SBMap LANGUAGES CXX)

find_package(SDL3 REQUIRED)

add_subdirectory(vendor/imgui)
add_subdirectory(vendor/stb_image)

set(SOURCE_FILES
    source/app.cpp
    source/app.h
    source/core.h
    source/error.cpp
    source/error.h
    source/main.cpp
    source/map_viewport.cpp
    source/map_viewport.h
    source/texture.cpp
    source/texture.h
    source/tile_palette.cpp
    source/tile_palette.h
    source/tilemap.cpp
    source/tilemap.h
    source/widget.h)

add_executable(SBMap ${SOURCE_FILES})

set_target_properties(SBMap PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

target_link_libraries(SBMap PRIVATE ImGui::ImGui SDL3::SDL3 stb_image::stb_image)
