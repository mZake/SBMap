#pragma once

#include <string>

#include "core.h"
#include "tilemap.h"
#include "widget.h"

namespace SBMap
{
    class TilePalette;
    
    enum class MapLayer
    {
        Tiles,
        Walls,
        LeftGoals,
        RightGoals,
    };
    
    class MapViewport : public IWidget
    {
    public:
        MapViewport(TilePalette& tile_palette)
            : m_TilePalette(tile_palette)
        {}
        
        void OnAttach() override;
        void OnDetach() override;
        void OnUIRender() override;
        
    private:
        void RenderMapUI();
        void RenderPropertiesUI();
        
        void RenderTilemap();
        void RenderTilemapOverlay();
        void RenderGrid();
        void RenderMarker();
        
        void ResizeTilemap();
        void ResetTilemap();
        void SaveTilemap();
        void OpenTilemap();
        
    private:
        TilePalette& m_TilePalette;
        Tilemap m_Tilemap;
        MapLayer m_SelectedLayer = MapLayer::Tiles;
        std::string m_InputTilemap;
        int32 m_InputWidth = 1;
        int32 m_InputHeight = 1;
        bool m_ShowGrid = true;
        bool m_ShowMarker = true;
    };
}
