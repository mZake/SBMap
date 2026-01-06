#pragma once

#include <string>

#include "core.h"
#include "tile_palette.h"
#include "tilemap.h"

namespace SBMap
{
    enum class MapLayer
    {
        Tiles,
        Walls,
        LeftGoals,
        RightGoals,
    };
    
    struct MapViewport
    {
        Tilemap tilemap;
        MapLayer selected_layer;
        std::string input_tilemap;
        int32 input_width;
        int32 input_height;
        bool show_grid;
        bool show_marker;
    };
    
    bool InitMapViewport(MapViewport& map_viewport, TilePalette& tile_palette);
    void CloseMapViewport(MapViewport& map_viewport);
    
    void ShowMapViewport(MapViewport& map_viewport, TilePalette& tile_palette);
}
