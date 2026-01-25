#include <SDL3/SDL.h>

#include "core.h"
#include "error.h"
#include "scope.h"
#include "texture.h"
#include "tilemap.h"

namespace SBMap
{
    #pragma pack(push, 1)
    
    struct SBMHeader
    {
        uint8 magic[4] = {};
        int32 width = 0;
        int32 height = 0;
    };
    
    struct SBMCell
    {
        int32 tile_x = -1;
        int32 tile_y = -1;
        uint32 flags = 0;
    };
    
    #pragma pack(pop)
    
    constexpr size_t SBM_MINIMUM_SIZE = sizeof(SBMHeader) + sizeof(SBMCell);
    
    Tileset CreateTileset(const Texture2D& atlas_texture, int32 tile_width, int32 tile_height)
    {
        SDL_assert(IsTextureValid(atlas_texture));
        SDL_assert(tile_width >= TILE_MINIMUM_WIDTH);
        SDL_assert(tile_height >= TILE_MINIMUM_HEIGHT);
        
        Tileset tileset;
        tileset.atlas = atlas_texture;
        tileset.tile_width = tile_width;
        tileset.tile_height = tile_height;
        tileset.width = atlas_texture.width / tile_width;
        tileset.height = atlas_texture.height / tile_height;
        
        return tileset;
    }
    
    Result<Tilemap> LoadTilemapFromDisk(const Tileset& tileset, const char* filepath)
    {
        SDL_assert(filepath != nullptr);
        
        if (!IsTilesetValid(tileset))
            return Error{ "Tileset is invalid." };
        
        SDL_PathInfo path_info;
        if (!SDL_GetPathInfo(filepath, &path_info))
            return Error{ "Path does not exist." };
        if (path_info.type != SDL_PATHTYPE_FILE)
            return Error{ "Path exists but is not a file." };
        
        size_t file_size;
        auto file_data = MakeScope((char*)SDL_LoadFile(filepath, &file_size), SDL_free);
        if (!file_data)
            return Error{ "Could not load file.", SDL_GetError() };
        
        if (file_size < SBM_MINIMUM_SIZE)
            return Error{ "File is too small." };
        
        SBMHeader header;
        SDL_memcpy(&header, file_data.Get(), sizeof(SBMHeader));
        if (SDL_memcmp(header.magic, "SBMP", 4) != 0)
            return Error{ "File has unsupported format." };
        
        if (header.width < TILEMAP_MINIMUM_WIDTH || header.height < TILEMAP_MINIMUM_HEIGHT)
            return Error{ "File has invalid data and is likely corrupted." };
        
        size_t sbm_cell_array_count = (size_t)(header.width * header.height);
        size_t sbm_cell_array_size = sbm_cell_array_count * sizeof(SBMCell);
        size_t real_sbm_cell_array_size = file_size - sizeof(SBMHeader);
        
        if (sbm_cell_array_size != real_sbm_cell_array_size)
            return Error{ "File has invalid data and is likely corrupted." };
        
        SBMCell* sbm_cell_array = (SBMCell*)(file_data.Get() + sizeof(SBMHeader));
        std::vector<Tilemap::Cell> tilemap_cells(sbm_cell_array_count);
        
        for (size_t i = 0; i < sbm_cell_array_count; i++)
        {
            Tilemap::Cell& tilemap_cell = tilemap_cells[i];
            
            SBMCell& sbm_cell = sbm_cell_array[i];
            if(!IsInTilesetBounds(tileset, sbm_cell.tile_x, sbm_cell.tile_y))
                return Error{ "Tile out of tileset bounds." };
            
            tilemap_cell.tile_x = sbm_cell.tile_x;
            tilemap_cell.tile_y = sbm_cell.tile_y;
            tilemap_cell.flags = sbm_cell.flags;
        }
        
        Tilemap tilemap;
        tilemap.tileset = tileset;
        tilemap.cells = std::move(tilemap_cells);
        tilemap.width = header.width;
        tilemap.height = header.height;
        
        return tilemap;
    }
    
    Result<bool> SaveTilemapToDisk(const Tilemap& tilemap, const char* filepath)
    {
        SDL_assert(filepath != nullptr);
        
        if (!IsTilemapValid(tilemap))
            return Error{ "Tilemap is incomplete and cannot be saved." };
        
        size_t sbm_cell_array_count = tilemap.cells.size();
        size_t sbm_cell_array_size = sbm_cell_array_count * sizeof(SBMCell);
        
        size_t buffer_size = sizeof(SBMHeader) + sbm_cell_array_size;
        std::vector<char> buffer(buffer_size);
        
        SBMHeader header;
        SDL_memcpy(header.magic, "SBMP", 4);
        header.width = tilemap.width;
        header.height = tilemap.height;
        
        SDL_memcpy(buffer.data(), &header, sizeof(SBMHeader));
        
        SBMCell* sbm_cell_array = (SBMCell*)(buffer.data() + sizeof(SBMHeader));
        for (size_t i = 0; i < sbm_cell_array_count; i++)
        {
            const Tilemap::Cell& tilemap_cell = tilemap.cells[i];
            SBMCell& sbm_cell = sbm_cell_array[i];
            
            sbm_cell.tile_x = tilemap_cell.tile_x;
            sbm_cell.tile_y = tilemap_cell.tile_y;
            sbm_cell.flags = tilemap_cell.flags;
        }
        
        if (!SDL_SaveFile(filepath, buffer.data(), buffer_size))
            return Error{ "Could not write to file.", SDL_GetError() };
        
        return true;
    }
    
    bool IsTilesetValid(const Tileset& tileset)
    {
        if (!IsTextureValid(tileset.atlas))
            return false;
        
        SDL_assert(tileset.tile_width >= TILE_MINIMUM_WIDTH);
        SDL_assert(tileset.tile_height >= TILE_MINIMUM_HEIGHT);
        SDL_assert(tileset.width >= TILESET_MINIMUM_WIDTH);
        SDL_assert(tileset.height >= TILESET_MINIMUM_HEIGHT);
        
        return true;
    }
    
    bool IsTilemapValid(const Tilemap& tilemap)
    {
        if (!IsTilesetValid(tilemap.tileset))
            return false;
        
        SDL_assert(tilemap.width >= TILEMAP_MINIMUM_WIDTH);
        SDL_assert(tilemap.height >= TILEMAP_MINIMUM_HEIGHT);
        SDL_assert(tilemap.cells.size() == (size_t)(tilemap.width * tilemap.height));
        
        return true;
    }
    
    bool IsInTilesetBounds(const Tileset& tileset, int32 tile_x, int32 tile_y)
    {
        SDL_assert(IsTilesetValid(tileset));
        
        bool result =
            tile_x >= 0 && tile_x < tileset.width &&
            tile_y >= 0 && tile_y < tileset.height;
        
        return result;
    }
    
    bool IsInTilemapBounds(const Tilemap& tilemap, int32 cell_x, int32 cell_y)
    {
        SDL_assert(IsTilemapValid(tilemap));
        
        bool result =
            cell_x >= 0 && cell_x < tilemap.width &&
            cell_y >= 0 && cell_y < tilemap.height;
        
        return result;
    }
    
    Tilemap::Cell& GetTilemapCell(Tilemap& tilemap, int32 cell_x, int32 cell_y)
    {
        SDL_assert(IsInTilemapBounds(tilemap, cell_x, cell_y));
        size_t cell_index = (size_t)(cell_x + cell_y * tilemap.width);
        return tilemap.cells[cell_index];
    }
}
