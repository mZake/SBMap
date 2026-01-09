#pragma once

#include <SDL3/SDL.h>

#include "core.h"
#include "error.h"
#include "tilemap.h"

namespace SBMap
{
    struct TilePalette
    {
        Tileset tileset;
        Texture2D placeholder;
        int32 selected_x;
        int32 selected_y;
        char input_atlas_image[320];
        int32 input_tile_width;
        int32 input_tile_height;
    };
    
    Result<TilePalette> CreateTilePalette(SDL_Renderer* renderer);
    
    void ShowTilePalette(TilePalette& tile_palette, SDL_Renderer* renderer);
}
