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
    
    class AppContext;
    
    class MapViewport
    {
    public:
        static Result<MapViewport> Create(AppContext& context);
        
        void ShowUI();
        
        void OpenTilemap();
        void OpenTilemapFile(const char* filepath);
        void SaveTilemap();
        void SaveTilemapFile(const char* filepath);
        
    private:
        void RenderTilemap();
        void RenderTilemapOverlay();
        void RenderTileGrid();
        void RenderTileMarker();
        
        void SetTilemapSize();
        void ResetTilemapSize();
        
        void ShowMapSectionUI();
        void ShowPropertiesSectionUI();
        
    private:
        AppContext* m_Context = nullptr;
        Tilemap m_Tilemap = {};
        MapLayer m_SelectedLayer = MapLayer::Tiles;
        float32 m_Scale = 0.0f;
        bool m_ShowGrid = false;
        bool m_ShowMarker = false;
        int32 m_InputWidth = 0;
        int32 m_InputHeight = 0;
    };
}
