#pragma once

#include <string>

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
        std::string input_atlas_image;
        int32 input_tile_width;
        int32 input_tile_height;
    };
    
    Result<TilePalette> CreateTilePalette(SDL_Renderer* renderer);
    void CloseTilePalette(TilePalette& tile_palette);
    
    void ShowTilePalette(TilePalette& tile_palette, SDL_Renderer* renderer);
}
