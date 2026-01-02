#pragma once

#include <string>

#include "core.h"
#include "tilemap.h"
#include "widget.h"

namespace SBMap
{
    class TilePalette : public IWidget
    {
    public:
        void OnAttach() override;
        void OnDetach() override;
        void OnUIRender() override;
        
        Tileset& GetTileset() { return m_Tileset; }
        uint32_t GetSelectedX() const { return m_SelectedX; }
        uint32_t GetSelectedY() const { return m_SelectedY; }
        
    private:
        void RenderSelectTile();
        void RenderProperties();
        
        void ResizeTile();
        void ResetTile();
        void OpenAtlas();
        void ResetAtlas();
        
    private:
        Tileset m_Tileset;
        Texture2D m_Placeholder;
        int32 m_SelectedX = 0;
        int32 m_SelectedY = 0;
        int32 m_InputTileWidth = 4;
        int32 m_InputTileHeight = 4;
        std::string m_InputAtlasImage;
    };
}
