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
        bool is_imgui_sdl3_init;
        bool is_imgui_sdlren3_init;
        bool running;
    };
    
    bool InitAppContext(AppContext& context);
    void CloseAppContext(AppContext& context);
    
    void RunApp(AppContext& context);
}
