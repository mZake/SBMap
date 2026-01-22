cmake_minimum_required(VERSION 3.15)
project(SBMap LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    SDL3
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL                 https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.2.28.zip
    URL_HASH            SHA256=a221b497324a2e6fe0543f55fde00e241f3e3f500e4a75c01d441a117911b89d
    FIND_PACKAGE_ARGS   3.2 QUIET
)

FetchContent_MakeAvailable(SDL3)

add_subdirectory(vendor/imgui)
add_subdirectory(vendor/stb_image)

set(SOURCE_FILES
    source/app.cpp
    source/app.h
    source/core.h
    source/embedded.cpp
    source/embedded.h
    source/error_popup.cpp
    source/error_popup.h
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
    source/tilemap.h)

add_executable(SBMap ${SOURCE_FILES})

set_target_properties(SBMap PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(SBMap PRIVATE /W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(SBMap PRIVATE -Wall -Wconversion -Wextra -Wpedantic)
endif()

if (MINGW)
    target_link_options(SBMap PRIVATE -static-libgcc -static-libstdc++)
endif()

target_link_libraries(SBMap PRIVATE ImGui::ImGui SDL3::SDL3 stb_image::stb_image)

get_target_property(SDL_TYPE SDL3::SDL3 TYPE)
if (WIN32 AND SDL_TYPE STREQUAL "SHARED_LIBRARY")
    add_custom_command(
        TARGET SBMap
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:SBMap>"
    )
endif()
