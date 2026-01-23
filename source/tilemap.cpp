#include <SDL3/SDL.h>

#include "core.h"
#include "error.h"
#include "texture.h"
#include "tilemap.h"

namespace SBMap
{
    #pragma pack(push, 1)
    
    struct SBMHeader
    {
        uint8 magic[4];
        int32 width;
        int32 height;
    };
    
    struct SBMCell
    {
        int32 tile_x;
        int32 tile_y;
        uint32 flags;
    };
    
    #pragma pack(pop)
    
    constexpr size_t SBM_MINIMUM_SIZE = sizeof(SBMHeader) + sizeof(SBMCell);
    
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
    
    Result<Tilemap> LoadTilemapFromDisk(const Tileset& tileset, const char* filepath)
    {
        SDL_assert(IsTilesetValid(tileset));
        SDL_assert(filepath != nullptr);
        
        SDL_PathInfo path_info;
        if (!SDL_GetPathInfo(filepath, &path_info))
            return Error{ "The selected file was not found." };
        
        if (path_info.type != SDL_PATHTYPE_FILE)
            return Error{ "The selected item is not a file." };
        
        size_t file_size;
        char* file_data = (char*)SDL_LoadFile(filepath, &file_size);
        if (!file_data)
            return Error{ "Could not load the selected file." };
        
        if (file_size < SBM_MINIMUM_SIZE)
        {
            SDL_free(file_data);
            return Error{ "The selected file is too small." };
        }
        
        SBMHeader header;
        SDL_memcpy(&header, file_data, sizeof(SBMHeader));
        if (SDL_memcmp(header.magic, "SBMP", 4) != 0)
        {
            SDL_free(file_data);
            return Error{ "The selected file format is not supported." };
        }
        
        // TODO: Check if header.width and header.height >= 0
        size_t cell_count = (size_t)(header.width * header.height);
        size_t expected_cells_size = cell_count * sizeof(SBMCell);
        size_t cells_size = file_size - sizeof(SBMHeader);
        
        if (expected_cells_size != cells_size)
        {
            SDL_free(file_data);
            return Error{ "The selected SBM file has inconsistent data.\nThe cell count does not match the map dimensions." };
        }
        
        SBMCell* sbm_cells = (SBMCell*)(file_data + sizeof(SBMHeader));
        std::vector<MapCell> map_cells(cell_count);
        
        for (size_t i = 0; i < cell_count; i++)
        {
            MapCell& map_cell = map_cells[i];
            SBMCell& sbm_cell = sbm_cells[i];
            
            map_cell.tile_x = sbm_cell.tile_x;
            map_cell.tile_y = sbm_cell.tile_y;
            map_cell.flags = sbm_cell.flags;
        }
        
        SDL_free(file_data);
        
        Tilemap tilemap;
        tilemap.tileset = &tileset;
        tilemap.cells = std::move(map_cells);
        tilemap.width = header.width;
        tilemap.height = header.height;
        
        return tilemap;
    }
    
    Result<bool> SaveTilemapToDisk(const Tilemap& tilemap, const char* filepath)
    {
        SDL_assert(IsTilemapValid(tilemap));
        SDL_assert(filepath != nullptr);
        
        size_t header_size = sizeof(SBMHeader);
        size_t cell_count = tilemap.cells.size();
        size_t cells_size = sizeof(SBMCell) * cell_count;
        size_t buffer_size = header_size + cells_size;
        
        std::vector<uint8> buffer(buffer_size);
        
        SBMHeader header;
        SDL_memcpy(header.magic, "SBMP", 4);
        header.width = tilemap.width;
        header.height = tilemap.height;
        
        std::vector<SBMCell> sbm_cells(cell_count);
        for (size_t i = 0; i < cell_count; i++)
        {
            const MapCell& map_cell = tilemap.cells[i];
            SBMCell& sbm_cell = sbm_cells[i];
            
            sbm_cell.tile_x = map_cell.tile_x;
            sbm_cell.tile_y = map_cell.tile_y;
            sbm_cell.flags = map_cell.flags;
        }
        
        SDL_memcpy(buffer.data(), &header, header_size);
        SDL_memcpy(buffer.data() + header_size, sbm_cells.data(), cells_size);
        
        if (!SDL_SaveFile(filepath, buffer.data(), buffer_size))
            return Error{ "Could not write to the selected file." };
        
        return true;
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
    
    MapCell& GetTilemapCell(Tilemap& tilemap, int32 cell_x, int32 cell_y)
    {
        SDL_assert(IsInTilemapBounds(tilemap, cell_x, cell_y));
        size_t cell_index = (size_t)(cell_x + cell_y * tilemap.width);
        return tilemap.cells[cell_index];
    }
}
