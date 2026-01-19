#pragma once

#include "core.h"
#include "error.h"
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
    
    struct AppContext;
    
    class MapViewport
    {
    public:
        static Result<MapViewport> Create(AppContext& context);
        
        void ShowUI();
        
        void SetInputTilemap(const char* value);
        
    private:
        void RenderTilemap();
        void RenderTilemapOverlay();
        void RenderTileGrid();
        void RenderTileMarker();
        
        void SetTilemapSize();
        void ResetTilemapSize();
        void SaveTilemap();
        void OpenTilemap();
        void ExploreTilemap();
        
        void ShowMapSectionUI();
        void ShowPropertiesSectionUI();
        
    private:
        AppContext* m_Context = nullptr;
        Tilemap m_Tilemap = {};
        MapLayer m_SelectedLayer = MapLayer::Tiles;
        float32 m_Scale = 0.0f;
        bool m_ShowGrid = false;
        bool m_ShowMarker = false;
        char m_InputTilemap[320] = {0};
        int32 m_InputWidth = 0;
        int32 m_InputHeight = 0;
    };
}
