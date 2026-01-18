#include "app.h"
#include "core.h"
#include "error_popup.h"
#include "map_viewport.h"
#include "tile_palette.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>

namespace SBMap
{
    static void SetupImGuiStyle()
    {
        // AdobeInspired style by nexacopic from ImThemes
        ImGuiStyle& style = ImGui::GetStyle();
        
        style.Alpha = 1.0f;
        style.DisabledAlpha = 0.6f;
        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.WindowRounding = 4.0f;
        style.WindowBorderSize = 1.0f;
        style.WindowMinSize = ImVec2(32.0f, 32.0f);
        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.ChildRounding = 4.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupRounding = 4.0f;
        style.PopupBorderSize = 1.0f;
        style.FramePadding = ImVec2(4.0f, 3.0f);
        style.FrameRounding = 4.0f;
        style.FrameBorderSize = 1.0f;
        style.ItemSpacing = ImVec2(8.0f, 4.0f);
        style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        style.CellPadding = ImVec2(4.0f, 2.0f);
        style.IndentSpacing = 21.0f;
        style.ColumnsMinSpacing = 6.0f;
        style.ScrollbarSize = 14.0f;
        style.ScrollbarRounding = 4.0f;
        style.GrabMinSize = 10.0f;
        style.GrabRounding = 20.0f;
        style.TabRounding = 4.0f;
        style.TabBorderSize = 1.0f;
        //style.TabMinWidthForCloseButton = 0.0f;
        style.ColorButtonPosition = ImGuiDir_Right;
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
        
        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.49803922f, 0.49803922f, 0.49803922f, 1.0f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11372549f, 0.11372549f, 0.11372549f, 1.0f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.078431375f, 0.078431375f, 0.078431375f, 0.94f);
        style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.16309011f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.08627451f, 0.08627451f, 0.08627451f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15294118f, 0.15294118f, 0.15294118f, 1.0f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1882353f, 0.1882353f, 0.1882353f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.11372549f, 0.11372549f, 0.11372549f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.105882354f, 0.105882354f, 0.105882354f, 1.0f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.11372549f, 0.11372549f, 0.11372549f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.019607844f, 0.019607844f, 0.019607844f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30980393f, 0.30980393f, 0.30980393f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40784314f, 0.40784314f, 0.40784314f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50980395f, 0.50980395f, 0.50980395f, 1.0f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.8784314f, 0.8784314f, 0.8784314f, 1.0f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.98039216f, 0.98039216f, 0.98039216f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.14901961f, 0.14901961f, 0.14901961f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24705882f, 0.24705882f, 0.24705882f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.32941177f, 0.32941177f, 0.32941177f, 1.0f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.9764706f, 0.9764706f, 0.9764706f, 0.30980393f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.9764706f, 0.9764706f, 0.9764706f, 0.8f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.9764706f, 0.9764706f, 0.9764706f, 1.0f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.42745098f, 0.42745098f, 0.49803922f, 0.5f);
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7490196f, 0.7490196f, 0.7490196f, 0.78039217f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.7490196f, 0.7490196f, 0.7490196f, 1.0f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9764706f, 0.9764706f, 0.9764706f, 0.2f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.9372549f, 0.9372549f, 0.9372549f, 0.67058825f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.9764706f, 0.9764706f, 0.9764706f, 0.9490196f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.22352941f, 0.22352941f, 0.22352941f, 0.8627451f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.32156864f, 0.32156864f, 0.32156864f, 0.8f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.27450982f, 0.27450982f, 0.27450982f, 1.0f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.14509805f, 0.14509805f, 0.14509805f, 0.972549f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.42352942f, 0.42352942f, 0.42352942f, 1.0f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.60784316f, 0.60784316f, 0.60784316f, 1.0f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.42745098f, 0.34901962f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392f, 0.69803923f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
        style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30980393f, 0.30980393f, 0.34901962f, 1.0f);
        style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.22745098f, 0.22745098f, 0.24705882f, 1.0f);
        style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 0.35f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 1.0f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
    }
    
    static void ProcessEvents(AppContext& context)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            
            switch (event.type)
            {
                case SDL_EVENT_QUIT: {
                    context.running = false;
                } break;
                case SDL_EVENT_KEY_DOWN: {
                    if (event.key.key == SDLK_F11)
                    {
                        context.fullscreen = !context.fullscreen;
                        SDL_SetWindowFullscreen(context.window, context.fullscreen);
                    }
                } break;
            }
        }
    }
    
    static bool InitSDL(AppContext& context)
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("SDL initialization failed: %s", SDL_GetError());
            return false;
        }
        
        float32 display_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        context.display_scale = display_scale;
        
        int32 window_width = 1280.0f * display_scale;
        int32 window_height = 720.0f * display_scale;
        SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN |
            SDL_WINDOW_HIGH_PIXEL_DENSITY;
        
        context.window = SDL_CreateWindow("SBMap", window_width, window_height, window_flags);
        if (!context.window)
        {
            SDL_Log("Window creation failed: %s", SDL_GetError());
            return false;
        }
        
        context.renderer = SDL_CreateRenderer(context.window, nullptr);
        if (!context.renderer)
        {
            SDL_Log("Renderer creation failed: %s", SDL_GetError());
            return false;
        }
        
        SDL_SetWindowPosition(context.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(context.window);
        
        SDL_SetRenderVSync(context.renderer, 1);
        
        // TODO: Set SDL_PROP_APP_METADATA_VERSION_STRING
        SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "SBMap");
        SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "com.mzake.sbmap");
        SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, "Zake");
        SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "Copyright (c) 2026 Zake");
        SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/mZake/SBMap");
        
        return true;
    }
    
    static bool InitImGui(AppContext& context)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForSDLRenderer(context.window, context.renderer);
        ImGui_ImplSDLRenderer3_Init(context.renderer);
        
        context.imgui_init = true;
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/inter/Inter-Variable.ttf");
        
        SetupImGuiStyle();
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(context.display_scale);
        style.FontScaleDpi = context.display_scale;
        
        return true;
    }
    
    static bool InitWidgets(AppContext& context)
    {
        auto tile_palette_result = CreateTilePalette(context.renderer);
        if (IsResultError(tile_palette_result))
        {
            Error error = GetResultError(tile_palette_result);
            SDL_Log("Tile Palette initialization failed: %s", error.message);
            return false;
        }
        
        context.tile_palette = GetResultValue(tile_palette_result);
        
        auto map_viewport_result = CreateMapViewport(context.tile_palette);
        if (IsResultError(map_viewport_result))
        {
            Error error = GetResultError(map_viewport_result);
            SDL_Log("Map Viewport initialization failed: %s", error.message);
            return false;
        }
        
        context.map_viewport = GetResultValue(map_viewport_result);
        
        return true;
    }
    
    static void CloseSDL(AppContext& context)
    {
        if (context.renderer)
            SDL_DestroyRenderer(context.renderer);
        if (context.window)
            SDL_DestroyWindow(context.window);
        
        context.renderer = nullptr;
        context.window = nullptr;
        
        SDL_Quit();
    }
    
    static void CloseImGui(AppContext& context)
    {
        if (context.imgui_init)
        {
            ImGui_ImplSDLRenderer3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
        }
        
        context.imgui_init = false;
    }
    
    bool InitAppContext(AppContext& context)
    {
        context.window = nullptr;
        context.renderer = nullptr;
        context.imgui_init = false;
        context.fullscreen = false;
        context.running = false;
        
        if (!InitSDL(context))
        {
            CloseAppContext(context);
            return false;
        }
        
        if (!InitImGui(context))
        {
            CloseAppContext(context);
            return false;
        }
        
        if (!InitWidgets(context))
        {
            CloseAppContext(context);
            return false;
        }
        
        return true;
    }
    
    void CloseAppContext(AppContext& context)
    {
        CloseImGui(context);
        CloseSDL(context);
    }
    
    void RunApp(AppContext& context)
    {
        SDL_assert(context.running == false);
        
        context.running = true;
        while (context.running)
        {
            ProcessEvents(context);
            
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();
            
            ImGui::DockSpaceOverViewport();
            
            ShowTilePalette(context.tile_palette, context.window, context.renderer);
            ShowMapViewport(context.map_viewport, context.tile_palette, context.window);
            ShowErrorPopup();
            
            SDL_SetRenderDrawColor(context.renderer, 32, 32, 40, 255);
            SDL_RenderClear(context.renderer);
            
            ImGui::Render();
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), context.renderer);
            
            SDL_RenderPresent(context.renderer);
        }
    }
}
