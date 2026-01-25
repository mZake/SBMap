#pragma once

#include <vector>

#include "core.h"
#include "error.h"
#include "texture.h"

namespace SBMap
{
    constexpr int32 TILE_MINIMUM_WIDTH = 4;
    constexpr int32 TILE_MINIMUM_HEIGHT = 4;
    constexpr int32 TILESET_MINIMUM_WIDTH = 1;
    constexpr int32 TILESET_MINIMUM_HEIGHT = 1;
    constexpr int32 TILEMAP_MINIMUM_WIDTH = 1;
    constexpr int32 TILEMAP_MINIMUM_HEIGHT = 1;
    
    struct Tileset
    {
        Texture2D atlas;
        int32 tile_width = 0;
        int32 tile_height = 0;
        int32 width = 0;
        int32 height = 0;
    };
    
    struct Tilemap
    {
        enum TileFlags : uint32
        {
            TileFlagsNone       = 0,
            TileFlagsWall       = 1 << 0,
            TileFlagsLeftGoal   = 1 << 1,
            TileFlagsRightGoal  = 1 << 2,
        };
        
        struct Cell
        {
            int32 tile_x = -1;
            int32 tile_y = -1;
            uint32 flags = 0;
        };
        
        std::vector<Cell> cells;
        Tileset tileset;
        int32 width = 0;
        int32 height = 0;
    };
    
    Tileset CreateTileset(const Texture2D& atlas_texture, int32 tile_width, int32 tile_height);
    Result<Tilemap> LoadTilemapFromDisk(const Tileset& tileset, const char* filepath);
    Result<bool> SaveTilemapToDisk(const Tilemap& tilemap, const char* filepath);
    
    bool IsTilesetValid(const Tileset& tileset);
    bool IsTilemapValid(const Tilemap& tilemap);
    
    bool IsInTilesetBounds(const Tileset& tileset, int32 tile_x, int32 tile_y);
    bool IsInTilemapBounds(const Tilemap& tilemap, int32 cell_x, int32 cell_y);
    
    Tilemap::Cell& GetTilemapCell(Tilemap& tilemap, int32 cell_x, int32 cell_y);
}
