#include "texture.h"
#include "tilemap.h"

namespace SBMap
{
    Tileset CreateTileset(Texture2D& atlas, int32 tile_width, int32 tile_height)
    {
        Tileset tileset;
        tileset.atlas = atlas;
        tileset.tile_width = tile_width;
        tileset.tile_height = tile_height;
        tileset.width = atlas.width / tile_width;
        tileset.height = atlas.height / tile_height;
        
        return tileset;
    }
    
    bool IsTilesetValid(const Tileset& tileset)
    {
        bool cond1 = IsTextureValid(tileset.atlas);
        bool cond2 = (tileset.width >= 0) && (tileset.height >= 0);
        bool cond3 = (tileset.tile_width >= 0) && (tileset.tile_height >= 0);
        
        return cond1 && cond2 && cond3;
    }
    
    bool IsTilemapValid(const Tilemap &tilemap)
    {
        bool cond1 = (tilemap.tileset != nullptr) && IsTilesetValid(*tilemap.tileset);
        bool cond2 = (!tilemap.cells.empty());
        bool cond3 = (tilemap.width >= 0) && (tilemap.height >= 0);
        
        return cond1 && cond2 && cond3;
    }
}
