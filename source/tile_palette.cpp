#include <imgui.h>
#include <imgui_stdlib.h>
#include <SDL3/SDL.h>
#include <stb_image.h>

#include "core.h"
#include "texture.h"
#include "tilemap.h"
#include "tile_palette.h"

namespace SBMap
{
    void TilePalette::OnAttach()
    {
        auto result = LoadTexture("assets/placeholder.png");
        m_Placeholder = GetResultValue(result);
    }
    
    void TilePalette::OnDetach()
    {
        m_Tileset = {};
        m_Placeholder = {};
        m_SelectedX = 0;
        m_SelectedY = 0;
        m_InputTileWidth = 4;
        m_InputTileHeight = 4;
        m_InputAtlasImage.clear();
    }
    
    void TilePalette::OnUIRender()
    {
        ImGui::Begin("Tile Palette");
        
        RenderSelectTile();
        RenderProperties();
        
        ImGui::End();
    }
    
    void TilePalette::RenderSelectTile()
    {
        ImGui::SeparatorText("Select Tile");
        
        if (IsTilesetValid(m_Tileset))
        {
            ImVec2 content_size;
            content_size.x = m_Tileset.atlas.width;
            content_size.y = m_Tileset.atlas.height;
            
            ImGui::BeginChild("TilePalette-SelectTile", content_size);
            
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                ImVec2 mouse_position = ImGui::GetMousePos() - ImGui::GetWindowPos();
                m_SelectedX = mouse_position.x / m_Tileset.tile_width;
                m_SelectedY = mouse_position.y / m_Tileset.tile_height;
            }
            
            ImTextureRef atlas_image_ref = GetTextureImGuiID(m_Tileset.atlas);
            ImGui::Image(atlas_image_ref, content_size);
            
            ImVec2 window_position = ImGui::GetWindowPos();
            
            ImVec2 marker_min;
            marker_min.x = window_position.x + m_SelectedX * m_Tileset.tile_width;
            marker_min.y = window_position.y + m_SelectedY * m_Tileset.tile_height;
            
            ImVec2 marker_max;
            marker_max.x = marker_min.x + m_Tileset.tile_width;
            marker_max.y = marker_min.y + m_Tileset.tile_height;
            
            ImColor marker_color = { 255, 255, 255, 255 };
            
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRect(marker_min, marker_max, marker_color);
            
            ImGui::EndChild();
        }
        else
        {
            ImVec2 placeholder_size;
            placeholder_size.x = m_Placeholder.width;
            placeholder_size.y = m_Placeholder.height;
            
            ImTextureRef placeholder_image_ref = GetTextureImGuiID(m_Placeholder);
            ImGui::Image(placeholder_image_ref, placeholder_size);
        }
    }
    
    void TilePalette::RenderProperties()
    {
        ImGui::SeparatorText("Properties");
        
        ImGui::BeginChild("TilePalette-Properties");
        
        ImGui::InputInt("Tile Width", &m_InputTileWidth);
        ImGui::InputInt("Tile Height", &m_InputTileHeight);
        
        if (ImGui::Button("Resize##Tile"))
            ResizeTile();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset##Tile"))
            ResetTile();
        
        ImGui::Spacing();
        
        ImGui::InputText("Atlas Image", &m_InputAtlasImage);
        
        if (ImGui::Button("Open"))
            OpenAtlas();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset##Atlas"))
            ResetAtlas();
        
        ImGui::EndChild();
    }
    
    void TilePalette::ResizeTile()
    {
        if (m_InputTileWidth > m_Tileset.atlas.width)
            m_InputTileWidth = m_Tileset.atlas.width;
        if (m_InputTileWidth < 4)
            m_InputTileWidth = 4;
        
        if (m_InputTileHeight > m_Tileset.atlas.height)
            m_InputTileHeight = m_Tileset.atlas.height;
        if (m_InputTileHeight < 4)
            m_InputTileHeight = 4;
        
        m_Tileset.tile_width = m_InputTileWidth;
        m_Tileset.tile_height = m_InputTileHeight;
        
        m_SelectedX = 0;
        m_SelectedY = 0;
    }
    
    void TilePalette::ResetTile()
    {
        m_InputTileWidth = 4;
        m_InputTileHeight = 4;
        
        ResizeTile();
    }
    
    void TilePalette::OpenAtlas()
    {
        const char* filepath = m_InputAtlasImage.c_str();
        
        auto result = LoadTexture(filepath);
        if (!result.is_value)
            return;
        
        Texture2D& texture = GetResultValue(result);
        int32 tile_width = m_Tileset.tile_width;
        int32 tile_height = m_Tileset.tile_height;
        
        m_Tileset = CreateTileset(texture, tile_width, tile_height);
        ResizeTile();
    }
    
    void TilePalette::ResetAtlas()
    {
        m_Tileset.atlas = {};
        m_Tileset.width = 0;
        m_Tileset.height = 0;
        m_InputAtlasImage.clear();
    }
}
