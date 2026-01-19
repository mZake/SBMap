#include <imgui.h>

#include "app.h"
#include "core.h"
#include "error_popup.h"
#include "error.h"
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
    
    constexpr int32 MINIMUM_MAP_WIDTH = 1;
    constexpr int32 MINIMUM_MAP_HEIGHT = 1;
        
    static void OpenFileDialogCallback(void* userdata, const char* const* filelist, int filter)
    {
        (void)filter;
        
        if (!filelist || !(*filelist))
            return;
        
        MapViewport* map_viewport = (MapViewport*)userdata;
        map_viewport->SetInputTilemap(*filelist);
    }
    
    static uint32 GetMapLayerTileFlag(MapLayer layer)
    {
        switch (layer)
        {
            case MapLayer::Tiles:       return TILE_FLAGS_NONE;
            case MapLayer::Walls:       return TILE_FLAGS_WALL;
            case MapLayer::LeftGoals:   return TILE_FLAGS_LEFT_GOAL;
            case MapLayer::RightGoals:  return TILE_FLAGS_RIGHT_GOAL;
        }
        
        SDL_assert(false);
        return 0;
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
        
        SDL_assert(false);
        return nullptr;
    }
    
    static void SelectableMapLayer(MapLayer layer, MapLayer& selected)
    {
        const char* preview = GetMapLayerPreview(layer);
        if (ImGui::Selectable(preview, selected == layer))
            selected = layer;
    }
    
    Result<MapViewport> MapViewport::Create(AppContext& context)
    {
        MapViewport instance;
        instance.m_Context = &context;
        instance.m_Tilemap.tileset = &context.tile_palette.GetTileset();
        instance.m_Scale = 1.0f;
        instance.m_ShowGrid = true;
        instance.m_ShowMarker = true;
        instance.ResetTilemapSize();
        
        return instance;
    }
    
    void MapViewport::ShowUI()
    {
        ImGui::Begin("Map Viewport");
        
        ShowMapSectionUI();
        ShowPropertiesSectionUI();
        
        ImGui::End();
    }
    
    void MapViewport::SetInputTilemap(const char* value)
    {
        SDL_assert(value != nullptr);
        SDL_strlcpy(m_InputTilemap, value, sizeof(m_InputTilemap));
    }
    
    void MapViewport::RenderTilemap()
    {
        const Tileset& tileset = *m_Tilemap.tileset;
        
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        float32 tile_width_scaled = tileset.tile_width * m_Scale;
        float32 tile_height_scaled = tileset.tile_height * m_Scale;
        
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
                dest_min.x = window_begin.x + x * tile_width_scaled;
                dest_min.y = window_begin.y + y * tile_height_scaled;
                
                ImVec2 dest_max;
                dest_max.x = dest_min.x + tile_width_scaled;
                dest_max.y = dest_min.y + tile_height_scaled;
                
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
        
        float32 tile_width_scaled = tileset.tile_width * m_Scale;
        float32 tile_height_scaled = tileset.tile_height * m_Scale;
        
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
                dest_min.x = window_begin.x + x * tile_width_scaled;
                dest_min.y = window_begin.y + y * tile_height_scaled;
                
                ImVec2 dest_max;
                dest_max.x = dest_min.x + tile_width_scaled;
                dest_max.y = dest_min.y + tile_height_scaled;
                
                draw_list->AddRectFilled(dest_min, dest_max, overlay_color);
            }
        }
    }
    
    void MapViewport::RenderTileGrid()
    {
        if (!m_ShowGrid)
            return;
        
        const Tileset& tileset = *m_Tilemap.tileset;
        
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        float32 tile_width_scaled = tileset.tile_width * m_Scale;
        float32 tile_height_scaled = tileset.tile_height * m_Scale;
        
        int32 line_width = m_Tilemap.width * tile_width_scaled;
        int32 column_height = m_Tilemap.height * tile_height_scaled;
        
        ImColor color = { 255, 255, 255, 255 };
        
        for (int32 x = 0; x <= m_Tilemap.width; x++)
        {
            ImVec2 point1;
            point1.x = window_begin.x + x * tile_width_scaled;
            point1.y = window_begin.y;
            
            ImVec2 point2;
            point2.x = window_begin.x + x * tile_width_scaled;
            point2.y = window_begin.y + column_height;
            
            draw_list->AddLine(point1, point2, color);
        }
        
        for (int32 y = 0; y <= m_Tilemap.height; y++)
        {
            ImVec2 point1;
            point1.x = window_begin.x;
            point1.y = window_begin.y + y * tile_height_scaled;
            
            ImVec2 point2;
            point2.x = window_begin.x + line_width;
            point2.y = window_begin.y + y * tile_height_scaled;
            
            draw_list->AddLine(point1, point2, color);
        }
    }
    
    void MapViewport::RenderTileMarker()
    {
        const Tileset& tileset = *m_Tilemap.tileset;
        TilePalette& tile_palette = m_Context->tile_palette;
        
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        if (ImGui::IsWindowHovered())
        {
            float32 tile_width_scaled = tileset.tile_width * m_Scale;
            float32 tile_height_scaled = tileset.tile_height * m_Scale;
            
            ImVec2 mouse_position = ImGui::GetMousePos() - window_begin;
            int32 cell_x = mouse_position.x / tile_width_scaled;
            int32 cell_y = mouse_position.y / tile_height_scaled;
            
            if ((cell_x < 0) || (cell_x >= m_Tilemap.width) ||
                (cell_y < 0) || (cell_y >= m_Tilemap.height))
                return;
            
            MapCell& cell = m_Tilemap.cells[cell_x + cell_y * m_Tilemap.width];
            
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                if (m_SelectedLayer == MapLayer::Tiles)
                {
                    cell.tile_x = tile_palette.GetSelectedTileX();
                    cell.tile_y = tile_palette.GetSelectedTileY();
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
                marker_min.x = window_begin.x + cell_x * tile_width_scaled;
                marker_min.y = window_begin.y + cell_y * tile_height_scaled;
                
                ImVec2 marker_max;
                marker_max.x = marker_min.x + tile_width_scaled;
                marker_max.y = marker_min.y + tile_height_scaled;
                
                ImColor marker_color = { 255, 255, 255, 255 };
                
                draw_list->AddRect(marker_min, marker_max, marker_color, 0.0f, 0, 3.0f);
            }
        }
    }
    
    void MapViewport::SetTilemapSize()
    {
        if (m_InputWidth < MINIMUM_MAP_WIDTH)
            m_InputWidth = MINIMUM_MAP_WIDTH;
        
        if (m_InputHeight < MINIMUM_MAP_HEIGHT)
            m_InputHeight = MINIMUM_MAP_HEIGHT;
        
        m_Tilemap.width = m_InputWidth;
        m_Tilemap.height = m_InputHeight;
        
        int32 cell_count = m_Tilemap.width * m_Tilemap.height;
        m_Tilemap.cells.resize(cell_count, { -1, -1, 0 });
    }
    
    void MapViewport::ResetTilemapSize()
    {
        m_InputWidth = MINIMUM_MAP_WIDTH;
        m_InputHeight = MINIMUM_MAP_HEIGHT;
        SetTilemapSize();
    }
    
    void MapViewport::SaveTilemap()
    {
        if (!IsTilemapValid(m_Tilemap))
        {
            OpenErrorPopup("Failed to Save Tilemap",
                "The current tilemap is incomplete and cannot be saved.");
            return;
        }
        
        size_t header_size = sizeof(SBMHeader);
        size_t cell_count = m_Tilemap.cells.size();
        size_t cells_size = sizeof(SBMCell) * cell_count;
        size_t buffer_size = header_size + cells_size;
        
        std::vector<uint8> buffer(buffer_size);
        
        SBMHeader header;
        SDL_memcpy(header.magic, "SBMP", 4);
        header.width = m_Tilemap.width;
        header.height = m_Tilemap.height;
        
        std::vector<SBMCell> sbm_cells(cell_count);
        for (size_t i = 0; i < cell_count; i++)
        {
            MapCell& map_cell = m_Tilemap.cells[i];
            SBMCell& sbm_cell = sbm_cells[i];
            
            sbm_cell.tile_x = map_cell.tile_x;
            sbm_cell.tile_y = map_cell.tile_y;
            sbm_cell.flags = map_cell.flags;
        }
        
        SDL_memcpy(buffer.data(), &header, header_size);
        SDL_memcpy(buffer.data() + header_size, sbm_cells.data(), cells_size);
        
        if (!SDL_SaveFile(m_InputTilemap, buffer.data(), buffer_size))
        {
            OpenErrorPopup("Failed to Save Tilemap",
                "Could not write to the selected file.");
        }
    }
    
    void MapViewport::OpenTilemap()
    {
        SDL_PathInfo path_info;
        if (!SDL_GetPathInfo(m_InputTilemap, &path_info))
        {
            OpenErrorPopup("Failed to Open Tilemap",
                "The selected file was not found.");
            return;
        }
        
        if (path_info.type != SDL_PATHTYPE_FILE)
        {
            OpenErrorPopup("Failed to Open Tilemap",
                "The selected item is not a file.");
            return;
        }
        
        size_t file_size;
        char* file_data = (char*)SDL_LoadFile(m_InputTilemap, &file_size);
        if (!file_data)
        {
            OpenErrorPopup("Failed to Open Tilemap",
                "Could not load the selected file.");
            return;
        }
        
        if (file_size < SBM_MINIMUM_SIZE)
        {
            SDL_free(file_data);
            OpenErrorPopup("Failed to Open Tilemap",
                "The selected file is too small. Minimum size is %d bytes.", SBM_MINIMUM_SIZE);
            return;
        }
        
        SBMHeader header;
        SDL_memcpy(&header, file_data, sizeof(SBMHeader));
        if (SDL_memcmp(header.magic, "SBMP", 4) != 0)
        {
            SDL_free(file_data);
            OpenErrorPopup("Failed to Open Tilemap",
                "The selected file format is not supported.");
            return;
        }
        
        size_t cell_count = header.width * header.height;
        size_t expected_cells_size = cell_count * sizeof(SBMCell);
        size_t cells_size = file_size - sizeof(SBMHeader);
        
        if (expected_cells_size != cells_size)
        {
            SDL_free(file_data);
            OpenErrorPopup("Failed to Open Tilemap",
                "The selected SBM file has inconsistent data.\nThe cell count does not match the map dimensions.");
            return;
        }
        
        SBMCell* sbm_cells = (SBMCell*)(file_data + sizeof(SBMHeader));
        std::vector<MapCell> map_cells(cell_count);
        
        for (size_t i = 0; i < cell_count; i++)
        {
            MapCell& map_cell = map_cells[i];
            SBMCell& sbm_cell = sbm_cells[i];
            
            map_cell.tile_x = sbm_cell.tile_x;
            map_cell.tile_y = sbm_cell.tile_y;
            map_cell.flags = sbm_cell.flags;
        }
        
        m_Tilemap.cells = std::move(map_cells);
        m_InputWidth = header.width;
        m_InputHeight = header.height;
        
        SetTilemapSize();
        
        SDL_free(file_data);
    }
    
    void MapViewport::ExploreTilemap()
    {
        static SDL_DialogFileFilter filters[] = {
            { "SBM files", "sbm" },
            { "All files", "*" },
        };
        
        // TODO: Use the appropriate dialog type for Open/Save
        SDL_ShowSaveFileDialog(OpenFileDialogCallback,
            this, m_Context->window, filters, SDL_arraysize(filters), nullptr);
    }
    
    void MapViewport::ShowMapSectionUI()
    {
        ImGui::SeparatorText("Map");
        
        if (!IsTilemapValid(m_Tilemap))
            return;
        
        const Tileset& tileset = *m_Tilemap.tileset;
        
        ImVec2 content_size;
        content_size.x = (float32)m_Tilemap.width * tileset.tile_width * m_Scale + 1;
        content_size.y = (float32)m_Tilemap.height * tileset.tile_height * m_Scale + 1;
        
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_HorizontalScrollbar;
        
        ImGuiChildFlags child_flags =
            ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY;
        
        ImGui::BeginChild("MapViewport-Map", ImVec2(480, 270), child_flags, window_flags);
        
        RenderTilemap();
        RenderTilemapOverlay();
        RenderTileGrid();
        RenderTileMarker();
        
        ImGui::Dummy(content_size);
        
        ImGui::EndChild();
    }
    
    void MapViewport::ShowPropertiesSectionUI()
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
        
        ImGui::SliderFloat("Scale", &m_Scale, 0.25f, 4.0f);
        ImGui::InputInt("Width", &m_InputWidth);
        ImGui::InputInt("Height", &m_InputHeight);
        
        if (ImGui::Button("Resize##Map"))
            SetTilemapSize();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset##Map"))
            ResetTilemapSize();
        
        ImGui::Spacing();
        
        ImGui::InputText("Tilemap", m_InputTilemap, sizeof(m_InputTilemap));
        
        if (ImGui::Button("Save##Tilemap"))
            SaveTilemap();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Open##Tilemap"))
            OpenTilemap();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Explore"))
            ExploreTilemap();
        
        ImGui::Spacing();
        
        ImGui::Checkbox("Show Grid", &m_ShowGrid);
        ImGui::SameLine();
        ImGui::Checkbox("Show Marker", &m_ShowMarker);
        
        ImGui::EndChild();
    }
}
