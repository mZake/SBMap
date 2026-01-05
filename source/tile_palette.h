#pragma once

#include <string>

#include "core.h"
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
    
    bool InitTilePalette(TilePalette& tile_palette);
    void CloseTilePalette(TilePalette& tile_palette);
    
    void ShowTilePalette(TilePalette& tile_palette);
}
