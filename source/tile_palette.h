#pragma once

#include "core.h"
#include "tilemap.h"

namespace SBMap
{
    class AppContext;
    
    class TilePalette
    {
    public:
        static TilePalette Create(AppContext& context);
        
        void ShowUI();
        
        void OpenAtlas();
        void OpenAtlasFile(const char* filepath);
        void RemoveAtlas();
        
        int32 GetSelectedTileX() const { return m_SelectedTileX; }
        int32 GetSelectedTileY() const { return m_SelectedTileY; }
        const Tileset& GetTileset() const { return m_Tileset; }
        
    private:
        void SetTileSize();
        void ResetTileSize();
        
        void ShowSelectTileSectionUI();
        void ShowPropertiesSectionUI();
        
    private:
        AppContext* m_Context = nullptr;
        Tileset m_Tileset = {};
        int32 m_SelectedTileX = 0;
        int32 m_SelectedTileY = 0;
        float32 m_Scale = 0.0f;
        int32 m_InputTileWidth = 0;
        int32 m_InputTileHeight = 0;
    };
}
