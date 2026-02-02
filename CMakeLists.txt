cmake_minimum_required(VERSION 3.15)
project(SBMap LANGUAGES CXX VERSION 0.1.0)

include(FetchContent)
include(GNUInstallDirs)

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)

find_package(SDL3 3.2 CONFIG QUIET)
if(NOT TARGET SDL3::SDL3)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    
    FetchContent_Declare(SDL3 
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.2.28.zip
        URL_HASH SHA256=a221b497324a2e6fe0543f55fde00e241f3e3f500e4a75c01d441a117911b89d)
    FetchContent_MakeAvailable(SDL3)
endif()

add_subdirectory(vendor/imgui)
add_subdirectory(vendor/stb_image)

configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/config.h.in"
    "${CMAKE_SOURCE_DIR}/source/config.h"
    @ONLY
)

set(SOURCE_FILES
    assets/sbmap.rc
    source/app.cpp
    source/app.h
    source/config.h
    source/core.h
    source/embedded.cpp
    source/embedded.h
    source/error_popup.cpp
    source/error_popup.h
    source/error.h
    source/main.cpp
    source/map_viewport.cpp
    source/map_viewport.h
    source/scope.h
    source/texture.cpp
    source/texture.h
    source/tile_palette.cpp
    source/tile_palette.h
    source/tilemap.cpp
    source/tilemap.h)

add_executable(SBMap WIN32 ${SOURCE_FILES})

set_target_properties(SBMap PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(SBMap PRIVATE /W4)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(SBMap PRIVATE -Wall -Wconversion -Wextra -Wpedantic)
endif()

if(MINGW)
    target_link_options(SBMap PRIVATE -static-libgcc -static-libstdc++)
endif()

target_link_libraries(SBMap PRIVATE ImGui::ImGui SDL3::SDL3 stb_image::stb_image)

install(TARGETS SBMap DESTINATION "${CMAKE_INSTALL_BINDIR}")
