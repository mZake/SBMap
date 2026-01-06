#pragma once

#include <SDL3/SDL.h>

#include "map_viewport.h"
#include "tile_palette.h"

namespace SBMap
{
    struct AppContext
    {
        SDL_Window* window;
        SDL_Renderer* renderer;
        TilePalette tile_palette;
        MapViewport map_viewport;
        bool imgui_impl_sdl3_init;
        bool imgui_impl_sdlren3_init;
        bool running;
    };
    
    bool InitAppContext(AppContext& app_context);
    void CloseAppContext(AppContext& app_context);
    
    void RunApp(AppContext& app_context);
}
