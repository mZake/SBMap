#include <imgui.h>
#include <SDL3/SDL.h>

#include "app.h"
#include "core.h"
#include "error_popup.h"
#include "error.h"
#include "texture.h"
#include "tile_palette.h"
#include "tilemap.h"

namespace SBMap
{
    constexpr int32 MINIMUM_TILE_WIDTH = 4;
    constexpr int32 MINIMUM_TILE_HEIGHT = 4;
    
    static void OpenFileDialogCallback(void* userdata, const char* const* filelist, int filter)
    {
        (void)filter;
        
        if (!filelist || !(*filelist))
            return;
        
        TilePalette* tile_palette = (TilePalette*)userdata;
        tile_palette->OpenAtlasFile(*filelist);
    }
    
    Result<TilePalette> TilePalette::Create(AppContext& context)
    {
        auto placeholder_result = LoadTexture("assets/images/placeholder.png", context.GetRenderer());
        if (IsResultError(placeholder_result))
        {
            const Error& error = GetResultError(placeholder_result);
            return MakeError("Placeholder texture loading failed: %s", error.message);
        }
        
        TilePalette instance;
        instance.m_Context = &context;
        instance.m_Placeholder = GetResultValue(placeholder_result);
        instance.m_Tileset.width = MINIMUM_TILE_WIDTH;
        instance.m_Tileset.height = MINIMUM_TILE_HEIGHT;
        instance.m_Scale = 1.0f;
        instance.m_InputTileWidth = MINIMUM_TILE_WIDTH;
        instance.m_InputTileHeight = MINIMUM_TILE_HEIGHT;
        
        return instance;
    }
    
    void TilePalette::ShowUI()
    {
        ImGui::Begin("Tile Palette");
        
        ShowSelectTileSectionUI();
        ShowPropertiesSectionUI();
        
        ImGui::End();
    }
    
    void TilePalette::OpenAtlas()
    {
        static SDL_DialogFileFilter filters[] = {
            { "PNG images",  "png" },
            { "JPEG images", "jpg;jpeg" },
            { "All images",  "png;jpg;jpeg" },
            { "All files",   "*" },
        };
        
        SDL_ShowOpenFileDialog(OpenFileDialogCallback,
            this, m_Context->GetWindow(), filters, SDL_arraysize(filters), nullptr, false);
    }
    
    void TilePalette::OpenAtlasFile(const char* filepath)
    {
        auto result = LoadTexture(filepath, m_Context->GetRenderer());
        if (IsResultValue(result))
        {
            Texture2D& atlas_texture = GetResultValue(result);
            m_Tileset = CreateTileset(atlas_texture, m_InputTileWidth, m_InputTileHeight);
        }
        else
        {
            const Error& error = GetResultError(result);
            OpenErrorPopup("Failed to Open Atlas", "%s", error.message);
        }
    }
    
    void TilePalette::RemoveAtlas()
    {
        m_Tileset = {};
    }
    
    void TilePalette::SetTileSize()
    {
        if (m_InputTileWidth > m_Tileset.atlas.width)
            m_InputTileWidth = m_Tileset.atlas.width;
        if (m_InputTileWidth < MINIMUM_TILE_WIDTH)
            m_InputTileWidth = MINIMUM_TILE_WIDTH;
        
        if (m_InputTileHeight > m_Tileset.atlas.height)
            m_InputTileHeight = m_Tileset.atlas.height;
        if (m_InputTileHeight < MINIMUM_TILE_HEIGHT)
            m_InputTileHeight = MINIMUM_TILE_HEIGHT;
        
        m_Tileset.tile_width = m_InputTileWidth;
        m_Tileset.tile_height = m_InputTileHeight;
        m_Tileset.width = m_Tileset.atlas.width / m_Tileset.tile_width;
        m_Tileset.height = m_Tileset.atlas.height / m_Tileset.tile_height;
        
        m_SelectedTileX = 0;
        m_SelectedTileY = 0;
    }
    
    void TilePalette::ResetTileSize()
    {
        m_InputTileWidth = MINIMUM_TILE_WIDTH;
        m_InputTileHeight = MINIMUM_TILE_HEIGHT;
        SetTileSize();
    }
    
    void TilePalette::ShowSelectTileSectionUI()
    {
        ImGui::SeparatorText("Select Tile");
        
        if (IsTilesetValid(m_Tileset))
        {
            ImVec2 content_size;
            content_size.x = (float32)m_Tileset.atlas.width * m_Scale;
            content_size.y = (float32)m_Tileset.atlas.height * m_Scale;
            
            float32 tile_width_scaled = (float32)m_Tileset.tile_width * m_Scale;
            float32 tile_height_scaled = (float32)m_Tileset.tile_height * m_Scale;
            
            ImGuiChildFlags child_flags =
                ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY;
            
            ImGuiWindowFlags window_flags =
                ImGuiWindowFlags_HorizontalScrollbar;
            
            ImGui::BeginChild("TilePalette-SelectTile", ImVec2(480, 270), child_flags, window_flags);
            
            ImVec2 window_begin = ImGui::GetCursorScreenPos();
            
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                ImVec2 mouse_position = ImGui::GetMousePos() - window_begin;
                int32 selected_tile_x = (int32)(mouse_position.x / tile_width_scaled);
                int32 selected_tile_y = (int32)(mouse_position.y / tile_height_scaled);
                
                if (IsInTilesetBounds(m_Tileset, selected_tile_x, selected_tile_y))
                {
                    m_SelectedTileX = selected_tile_x;
                    m_SelectedTileY = selected_tile_y;
                }
            }
            
            ImTextureRef atlas_image_ref = GetTextureImGuiID(m_Tileset.atlas);
            ImGui::Image(atlas_image_ref, content_size);
            
            ImVec2 marker_min;
            marker_min.x = window_begin.x + (float32)m_SelectedTileX * tile_width_scaled;
            marker_min.y = window_begin.y + (float32)m_SelectedTileY * tile_height_scaled;
            
            ImVec2 marker_max;
            marker_max.x = marker_min.x + tile_width_scaled;
            marker_max.y = marker_min.y + tile_height_scaled;
            
            ImColor marker_color = { 255, 255, 255, 255 };
            
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRect(marker_min, marker_max, marker_color);
            
            ImGui::EndChild();
        }
        else
        {
            ImVec2 placeholder_size;
            placeholder_size.x = (float32)m_Placeholder.width;
            placeholder_size.y = (float32)m_Placeholder.height;
            
            ImTextureRef placeholder_image_ref = GetTextureImGuiID(m_Placeholder);
            ImGui::Image(placeholder_image_ref, placeholder_size);
        }
    }
    
    void TilePalette::ShowPropertiesSectionUI()
    {
        ImGui::SeparatorText("Properties");
        
        ImGui::BeginChild("TilePalette-Properties");
        
        ImGui::SliderFloat("Scale", &m_Scale, 0.25f, 4.0f);
        ImGui::InputInt("Tile Width", &m_InputTileWidth);
        ImGui::InputInt("Tile Height", &m_InputTileHeight);
        
        if (ImGui::Button("Resize##Tile"))
            SetTileSize();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset##Tile"))
            ResetTileSize();
        
        ImGui::EndChild();
    }
}
