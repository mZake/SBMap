#pragma once

#include <SDL3/SDL.h>

#include "core.h"
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
        float32 display_scale;
        bool imgui_init;
        bool fullscreen;
        bool running;
    };
    
    bool InitAppContext(AppContext& context);
    void CloseAppContext(AppContext& context);
    
    void RunApp(AppContext& context);
}
