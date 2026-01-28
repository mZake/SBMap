set(SOURCE_FILES
    backends/imgui_impl_sdl3.cpp
    backends/imgui_impl_sdl3.h
    backends/imgui_impl_sdlrenderer3.cpp
    backends/imgui_impl_sdlrenderer3.h
    misc/cpp/imgui_stdlib.cpp
    misc/cpp/imgui_stdlib.h
    imconfig.h
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    imgui.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h)

add_library(ImGui STATIC ${SOURCE_FILES})
add_library(ImGui::ImGui ALIAS ImGui)

target_include_directories(ImGui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends"
    "${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp")

target_link_libraries(ImGui PUBLIC SDL3::SDL3)

target_compile_definitions(ImGui PUBLIC IMGUI_DEFINE_MATH_OPERATORS)
