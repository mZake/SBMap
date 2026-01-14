#include <SDL3/SDL.h>

#include "texture.h"
#include "tilemap.h"

namespace SBMap
{
    Tileset CreateTileset(Texture2D& atlas, int32 tile_width, int32 tile_height)
    {
        SDL_assert(IsTextureValid(atlas));
        SDL_assert(tile_width > 0);
        SDL_assert(tile_height > 0);
        
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
        if (!IsTextureValid(tileset.atlas))
            return false;
        
        SDL_assert(tileset.tile_width > 0);
        SDL_assert(tileset.tile_height > 0);
        SDL_assert(tileset.width > 0);
        SDL_assert(tileset.height > 0);
        
        return true;
    }
    
    bool IsTilemapValid(const Tilemap& tilemap)
    {
        if (!(tilemap.tileset && IsTilesetValid(*tilemap.tileset)))
            return false;
        
        SDL_assert(tilemap.width > 0);
        SDL_assert(tilemap.height > 0);
        SDL_assert(tilemap.cells.size() == size_t(tilemap.width * tilemap.height));
        
        return true;
    }
}
