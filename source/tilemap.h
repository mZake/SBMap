#pragma once

#include <vector>

#include "core.h"
#include "texture.h"

namespace SBMap
{
    enum TileFlags : uint32_t
    {
        TILE_FLAGS_NONE         = 0,
        TILE_FLAGS_WALL         = 1 << 0,
        TILE_FLAGS_LEFT_GOAL    = 1 << 1,
        TILE_FLAGS_RIGHT_GOAL   = 1 << 2,
    };
    
    struct Cell
    {
        int32 tile_x;
        int32 tile_y;
        uint32 flags;
    };
    
    struct Tileset
    {
        Texture2D atlas;
        int32 tile_width;
        int32 tile_height;
        int32 width;
        int32 height;
    };
    
    struct Tilemap
    {
        const Tileset* tileset;
        std::vector<Cell> cells;
        int32 width;
        int32 height;
    };
    
    Tileset CreateTileset(Texture2D& atlas, int32 tile_width, int32 tile_height);
    bool IsTilesetValid(const Tileset& tileset);
    bool IsTilemapValid(const Tilemap& tilemap);
}
