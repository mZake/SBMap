#include "app.h"
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
            
            if (event.type == SDL_EVENT_QUIT)
                context.running = false;
        }
    }
    
    static bool InitSDL(AppContext& context)
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("SDL initialization failed: %s", SDL_GetError());
            return false;
        }
        
        SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
        context.window = SDL_CreateWindow("SBMap", 1280, 720, window_flags);
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
        SDL_SetRenderVSync(context.renderer, 1);
        SDL_ShowWindow(context.window);
        
        return true;
    }
    
    static bool InitImGui(AppContext& context)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        context.is_imgui_sdl3_init = ImGui_ImplSDL3_InitForSDLRenderer(context.window, context.renderer);
        if (!context.is_imgui_sdl3_init)
        {
            SDL_Log("ImGui_ImplSDL3 initialization failed");
            return false;
        }
        
        context.is_imgui_sdlren3_init = ImGui_ImplSDLRenderer3_Init(context.renderer);
        if (!context.is_imgui_sdlren3_init)
        {
            SDL_Log("ImGui_ImplSDLRenderer3 initialization failed");
            return false;
        }
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/paper-mono/PaperMono-Regular.ttf");
        
        SetupImGuiStyle();
        
        return true;
    }
    
    static bool InitWidgets(AppContext& context)
    {
        if (!InitTilePalette(context.tile_palette, context.renderer))
        {
            SDL_Log("Tile Palette widget initialization failed");
            return false;
        }
        
        if (!InitMapViewport(context.map_viewport, context.tile_palette))
        {
            SDL_Log("Map Viewport widget initialization failed");
            return false;
        }
        
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
        if (context.is_imgui_sdlren3_init)
            ImGui_ImplSDLRenderer3_Shutdown();
        
        if (context.is_imgui_sdl3_init)
            ImGui_ImplSDL3_Shutdown();
        
        if (ImGui::GetCurrentContext())
            ImGui::DestroyContext();
        
        context.is_imgui_sdlren3_init = false;
        context.is_imgui_sdl3_init = false;
    }
    
    static void CloseWidgets(AppContext& context)
    {
        CloseMapViewport(context.map_viewport);
        CloseTilePalette(context.tile_palette);
        
        context.map_viewport = {};
        context.tile_palette = {};
    }
    
    bool InitAppContext(AppContext& context)
    {
        context = {};
        
        if (!InitSDL(context))      return false;
        if (!InitImGui(context))    return false;
        if (!InitWidgets(context))  return false;
        
        return true;
    }
    
    void CloseAppContext(AppContext& context)
    {
        CloseWidgets(context);
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
            
            ShowTilePalette(context.tile_palette, context.renderer);
            ShowMapViewport(context.map_viewport, context.tile_palette);
            
            SDL_SetRenderDrawColor(context.renderer, 32, 32, 40, 255);
            SDL_RenderClear(context.renderer);
            
            ImGui::Render();
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), context.renderer);
            
            SDL_RenderPresent(context.renderer);
        }
    }
    
    SDL_Renderer* GetRenderer()
    {
        return nullptr;
    }
}
