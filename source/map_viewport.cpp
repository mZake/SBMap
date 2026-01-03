#include <fstream>

#include <string.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "core.h"
#include "map_viewport.h"
#include "tile_palette.h"
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
    
    static uint32_t GetMapLayerTileFlag(MapLayer layer)
    {
        switch (layer)
        {
            case MapLayer::Tiles:       return TILE_FLAGS_NONE;
            case MapLayer::Walls:       return TILE_FLAGS_WALL;
            case MapLayer::LeftGoals:   return TILE_FLAGS_LEFT_GOAL;
            case MapLayer::RightGoals:  return TILE_FLAGS_RIGHT_GOAL;
        }
    }
    
    static const char* GetMapLayerPreview(MapLayer layer)
    {
        switch (layer)
        {
            case MapLayer::Tiles:       return "Tiles";
            case MapLayer::Walls:       return "Walls";
            case MapLayer::LeftGoals:   return "Left Goals";
            case MapLayer::RightGoals:  return "Right Goals";
        }
    }
    
    static void SelectableMapLayer(MapLayer layer, MapLayer& selected)
    {
        const char* preview = GetMapLayerPreview(layer);
        if (ImGui::Selectable(preview, selected == layer))
            selected = layer;
    }
    
    void MapViewport::OnAttach()
    {
        m_Tilemap.tileset = &m_TilePalette.GetTileset();
        ResetTilemap();
    }
    
    void MapViewport::OnDetach()
    {
        m_Tilemap = {};
    }
    
    void MapViewport::OnUIRender()
    {
        ImGui::Begin("Map Viewport", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
        
        RenderMapUI();
        RenderPropertiesUI();
        
        ImGui::End();
    }
    
    void MapViewport::RenderMapUI()
    {
        ImGui::SeparatorText("Map");
        
        if (!IsTilemapValid(m_Tilemap))
            return;
        
        const Tileset& tileset = *m_Tilemap.tileset;
        
        ImVec2 content_size;
        content_size.x = m_Tilemap.width * tileset.tile_width + 1;
        content_size.y = m_Tilemap.height * tileset.tile_height + 1;
        
        ImGui::BeginChild("MapViewport-Map", content_size);
        
        RenderTilemap();
        RenderTilemapOverlay();
        RenderGrid();
        RenderMarker();
        
        ImGui::EndChild();
    }
    
    void MapViewport::RenderPropertiesUI()
    {
        ImGui::SeparatorText("Properties");
        
        ImGui::BeginChild("MapViewport-Properties");
        
        if (ImGui::BeginCombo("Layer", GetMapLayerPreview(m_SelectedLayer)))
        {
            SelectableMapLayer(MapLayer::Tiles, m_SelectedLayer);
            SelectableMapLayer(MapLayer::Walls, m_SelectedLayer);
            SelectableMapLayer(MapLayer::LeftGoals, m_SelectedLayer);
            SelectableMapLayer(MapLayer::RightGoals, m_SelectedLayer);
            
            ImGui::EndCombo();
        }
        
        ImGui::Spacing();
        
        ImGui::InputInt("Width", &m_InputWidth);
        ImGui::InputInt("Height", &m_InputHeight);
        
        if (ImGui::Button("Resize##Map"))
            ResizeTilemap();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset##Map"))
            ResetTilemap();
        
        ImGui::Spacing();
        
        ImGui::InputText("Tilemap", &m_InputTilemap);
        
        if (ImGui::Button("Save##Tilemap"))
            SaveTilemap();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Open##Tilemap"))
            OpenTilemap();
        
        ImGui::Spacing();
        
        ImGui::Checkbox("Show Grid", &m_ShowGrid);
        ImGui::SameLine();
        ImGui::Checkbox("Show Marker", &m_ShowMarker);
        
        ImGui::EndChild();
    }
    
    void MapViewport::RenderTilemap()
    {
        const Tileset& tileset = *m_Tilemap.tileset;
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        for (int32 y = 0; y < m_Tilemap.height; y++)
        {
            for (int32 x = 0; x < m_Tilemap.width; x++)
            {
                MapCell& cell = m_Tilemap.cells[x + y * m_Tilemap.width];
                if (cell.tile_x < 0 || cell.tile_y < 0)
                    continue;
                
                ImVec2 source_min;
                source_min.x = cell.tile_x * tileset.tile_width / (float)tileset.atlas.width;
                source_min.y = cell.tile_y * tileset.tile_height / (float)tileset.atlas.height;
                
                ImVec2 source_max;
                source_max.x = source_min.x + tileset.tile_width / (float)tileset.atlas.width;
                source_max.y = source_min.y + tileset.tile_height / (float)tileset.atlas.height;
                
                ImVec2 dest_min;
                dest_min.x = window_begin.x + x * tileset.tile_width;
                dest_min.y = window_begin.y + y * tileset.tile_height;
                
                ImVec2 dest_max;
                dest_max.x = dest_min.x + tileset.tile_width;
                dest_max.y = dest_min.y + tileset.tile_height;
                
                ImTextureRef atlas_image_ref = GetTextureImGuiID(tileset.atlas);
                draw_list->AddImage(atlas_image_ref, dest_min, dest_max, source_min, source_max);
            }
        }
    }
    
    void MapViewport::RenderTilemapOverlay()
    {
        if (m_SelectedLayer == MapLayer::Tiles)
            return;
        
        const Tileset& tileset = *m_Tilemap.tileset;
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        ImColor overlay_color = { 0, 0, 0, 80 };
        
        for (int32 y = 0; y < m_Tilemap.height; y++)
        {
            for (int32 x = 0; x < m_Tilemap.width; x++)
            {
                uint32_t tile_flag = GetMapLayerTileFlag(m_SelectedLayer);
                MapCell& cell = m_Tilemap.cells[x + y * m_Tilemap.width];
                if (!(cell.flags & tile_flag))
                    continue;
                
                ImVec2 dest_min;
                dest_min.x = window_begin.x + x * tileset.tile_width;
                dest_min.y = window_begin.y + y * tileset.tile_height;
                
                ImVec2 dest_max;
                dest_max.x = dest_min.x + tileset.tile_width;
                dest_max.y = dest_min.y + tileset.tile_height;
                
                draw_list->AddRectFilled(dest_min, dest_max, overlay_color);
            }
        }
    }
    
    void MapViewport::RenderGrid()
    {
        if (!m_ShowGrid)
            return;
        
        const Tileset& tileset = *m_Tilemap.tileset;
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        int32 line_width = m_Tilemap.width * tileset.tile_width;
        int32 column_height = m_Tilemap.height * tileset.tile_height;
        
        ImColor color = { 255, 255, 255, 255 };
        
        for (int32 x = 0; x <= m_Tilemap.width; x++)
        {
            ImVec2 point1;
            point1.x = window_begin.x + x * tileset.tile_width;
            point1.y = window_begin.y;
            
            ImVec2 point2;
            point2.x = window_begin.x + x * tileset.tile_width;
            point2.y = window_begin.y + column_height;
            
            draw_list->AddLine(point1, point2, color);
        }
        
        for (int32 y = 0; y <= m_Tilemap.height; y++)
        {
            ImVec2 point1;
            point1.x = window_begin.x;
            point1.y = window_begin.y + y * tileset.tile_height;
            
            ImVec2 point2;
            point2.x = window_begin.x + line_width;
            point2.y = window_begin.y + y * tileset.tile_height;
            
            draw_list->AddLine(point1, point2, color);
        }
    }
    
    void MapViewport::RenderMarker()
    {
        const Tileset& tileset = *m_Tilemap.tileset;
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        if (ImGui::IsWindowHovered())
        {
            ImVec2 mouse_position = ImGui::GetMousePos() - window_begin;
            int32 cell_x = mouse_position.x / tileset.tile_width;
            int32 cell_y = mouse_position.y / tileset.tile_height;
            
            if ((cell_x < 0) || (cell_x >= m_Tilemap.width) ||
                (cell_y < 0) || (cell_y >= m_Tilemap.height))
                return;
            
            MapCell& cell = m_Tilemap.cells[cell_x + cell_y * m_Tilemap.width];
            
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                if (m_SelectedLayer == MapLayer::Tiles)
                {
                    cell.tile_x = m_TilePalette.GetSelectedX();
                    cell.tile_y = m_TilePalette.GetSelectedY();
                }
                else
                {
                    uint32_t tile_flag = GetMapLayerTileFlag(m_SelectedLayer);
                    cell.flags |= tile_flag;
                }
            }
            else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                if (m_SelectedLayer == MapLayer::Tiles)
                {
                    cell.tile_x = -1;
                    cell.tile_y = -1;
                }
                else
                {
                    uint32_t tile_flag = GetMapLayerTileFlag(m_SelectedLayer);
                    cell.flags &= ~tile_flag;
                }
            }
            
            if (m_ShowMarker)
            {
                ImVec2 marker_min;
                marker_min.x = window_begin.x + cell_x * tileset.tile_width;
                marker_min.y = window_begin.y + cell_y * tileset.tile_height;
                
                ImVec2 marker_max;
                marker_max.x = marker_min.x + tileset.tile_width;
                marker_max.y = marker_min.y + tileset.tile_height;
                
                ImColor marker_color = { 255, 255, 255, 255 };
                
                draw_list->AddRect(marker_min, marker_max, marker_color, 0.0f, 0, 3.0f);
            }
        }
    }
    
    void MapViewport::ResizeTilemap()
    {
        if (m_InputWidth < 1)
            m_InputWidth = 1;
        
        if (m_InputHeight < 1)
            m_InputHeight = 1;
        
        m_Tilemap.width = m_InputWidth;
        m_Tilemap.height = m_InputHeight;
        m_Tilemap.cells.resize(m_InputWidth * m_InputHeight, { -1, -1 });
    }
    
    void MapViewport::ResetTilemap()
    {
        m_InputWidth = 1;
        m_InputHeight = 1;
        
        ResizeTilemap();
    }
    
    void MapViewport::SaveTilemap()
    {
        std::ofstream stream(m_InputTilemap, std::ios::binary);
        if (!stream)
            return;
        
        SBMHeader sbm_header;
        memcpy(sbm_header.magic, "SBMP", 4);
        sbm_header.width = m_Tilemap.width;
        sbm_header.height = m_Tilemap.height;
        
        size_t sbm_header_size = sizeof(SBMHeader);
        stream.write(reinterpret_cast<char*>(&sbm_header), sbm_header_size);
        
        size_t map_cell_count = m_Tilemap.cells.size();
        std::vector<SBMCell> sbm_cells(map_cell_count);
        
        for (size_t i = 0; i < map_cell_count; i++)
        {
            MapCell& map_cell = m_Tilemap.cells[i];
            SBMCell& sbm_cell = sbm_cells[i];
            
            sbm_cell.tile_x = map_cell.tile_x;
            sbm_cell.tile_y = map_cell.tile_y;
            sbm_cell.flags = map_cell.flags;
        }
        
        size_t sbm_cells_size = map_cell_count * sizeof(SBMCell);
        stream.write(reinterpret_cast<char*>(sbm_cells.data()), sbm_cells_size);
        
        stream.close();
    }
    
    void MapViewport::OpenTilemap()
    {
        std::ifstream stream(m_InputTilemap, std::ios::binary);
        if (!stream)
            return;
        
        stream.seekg(0, std::ios::end);
        size_t stream_size = stream.tellg();
        if (stream_size < SBM_MINIMUM_SIZE)
            return;
        
        stream.seekg(0, std::ios::beg);
        
        SBMHeader sbm_header;
        size_t sbm_header_size = sizeof(SBMHeader);
        stream.read(reinterpret_cast<char*>(&sbm_header), sbm_header_size);
        
        if (memcmp(sbm_header.magic, "SBMP", 4) != 0)
            return;
        
        size_t sbm_cells_begin = stream.tellg();
        
        size_t sbm_cell_count = sbm_header.width * sbm_header.height;
        size_t sbm_cells_size = sbm_cell_count * sizeof(SBMCell);
        size_t sbm_cells_real_size = stream_size - sbm_cells_begin;
        
        if (sbm_cells_real_size != sbm_cells_size)
            return;
        
        stream.seekg(sbm_cells_begin, std::ios::beg);
        
        std::vector<SBMCell> sbm_cells(sbm_cell_count);
        stream.read(reinterpret_cast<char*>(sbm_cells.data()), sbm_cells_size);
        
        std::vector<MapCell> new_map_cells(sbm_cell_count);
        
        for (size_t i = 0; i < sbm_cell_count; i++)
        {
            MapCell& map_cell = new_map_cells[i];
            SBMCell& sbm_cell = sbm_cells[i];
            
            map_cell.tile_x = sbm_cell.tile_x;
            map_cell.tile_y = sbm_cell.tile_y;
            map_cell.flags = sbm_cell.flags;
        }
        
        m_Tilemap.cells = std::move(new_map_cells);
        m_Tilemap.width = m_InputWidth = sbm_header.width;
        m_Tilemap.height = m_InputHeight = sbm_header.height;
        
        stream.close();
    }
}
