#pragma once

#include "core.h"
#include "error.h"
#include "texture.h"
#include "tilemap.h"

namespace SBMap
{
    struct AppContext;
    
    class TilePalette
    {
    public:
        static Result<TilePalette> Create(AppContext& context);
        
        void ShowUI();
        
        void SetInputAtlasImage(const char* value);
        
    private:
        void SetTileSize();
        void ResetTileSize();
        
        void OpenAtlasImage();
        void ResetAtlasImage();
        void ExploreAtlasImage();
        
        void ShowSelectTileSectionUI();
        void ShowPropertiesSectionUI();
        
    private:
        AppContext* m_Context = nullptr;
        Texture2D m_Placeholder;
        Tileset m_Tileset = {};
        int32 m_SelectedTileX = 0;
        int32 m_SelectedTileY = 0;
        float32 m_Scale = 0.0f;
        char m_InputAtlasImage[320] = {0};
        int32 m_InputTileWidth = 0;
        int32 m_InputTileHeight = 0;
    };
}
