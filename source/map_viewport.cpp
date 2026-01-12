#include <string.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "core.h"
#include "error_popup.h"
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
    constexpr int32 MINIMUM_MAP_CELL_COUNT = 1;
    
    static uint32 GetMapLayerTileFlag(MapLayer layer)
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
    
    static void RenderTilemap(MapViewport& map_viewport)
    {
        Tilemap& tilemap = map_viewport.tilemap;
        const Tileset& tileset = *tilemap.tileset;
        
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        for (int32 y = 0; y < tilemap.height; y++)
        {
            for (int32 x = 0; x < tilemap.width; x++)
            {
                MapCell& cell = tilemap.cells[x + y * tilemap.width];
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
    
    static void RenderTilemapOverlay(MapViewport& map_viewport)
    {
        if (map_viewport.selected_layer == MapLayer::Tiles)
            return;
        
        Tilemap& tilemap = map_viewport.tilemap;
        const Tileset& tileset = *tilemap.tileset;
        
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        ImColor overlay_color = { 0, 0, 0, 80 };
        
        for (int32 y = 0; y < tilemap.height; y++)
        {
            for (int32 x = 0; x < tilemap.width; x++)
            {
                uint32_t tile_flag = GetMapLayerTileFlag(map_viewport.selected_layer);
                MapCell& cell = tilemap.cells[x + y * tilemap.width];
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
    
    static void RenderGrid(MapViewport& map_viewport)
    {
        if (!map_viewport.show_grid)
            return;
        
        Tilemap& tilemap = map_viewport.tilemap;
        const Tileset& tileset = *tilemap.tileset;
        
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        int32 line_width = tilemap.width * tileset.tile_width;
        int32 column_height = tilemap.height * tileset.tile_height;
        
        ImColor color = { 255, 255, 255, 255 };
        
        for (int32 x = 0; x <= tilemap.width; x++)
        {
            ImVec2 point1;
            point1.x = window_begin.x + x * tileset.tile_width;
            point1.y = window_begin.y;
            
            ImVec2 point2;
            point2.x = window_begin.x + x * tileset.tile_width;
            point2.y = window_begin.y + column_height;
            
            draw_list->AddLine(point1, point2, color);
        }
        
        for (int32 y = 0; y <= tilemap.height; y++)
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
    
    static void RenderMarker(MapViewport& map_viewport, TilePalette& tile_palette)
    {
        Tilemap& tilemap = map_viewport.tilemap;
        const Tileset& tileset = *tilemap.tileset;
        
        ImVec2 window_begin = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        if (ImGui::IsWindowHovered())
        {
            ImVec2 mouse_position = ImGui::GetMousePos() - window_begin;
            int32 cell_x = mouse_position.x / tileset.tile_width;
            int32 cell_y = mouse_position.y / tileset.tile_height;
            
            if ((cell_x < 0) || (cell_x >= tilemap.width) ||
                (cell_y < 0) || (cell_y >= tilemap.height))
                return;
            
            MapCell& cell = tilemap.cells[cell_x + cell_y * tilemap.width];
            
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                if (map_viewport.selected_layer == MapLayer::Tiles)
                {
                    cell.tile_x = tile_palette.selected_x;
                    cell.tile_y = tile_palette.selected_y;
                }
                else
                {
                    uint32_t tile_flag = GetMapLayerTileFlag(map_viewport.selected_layer);
                    cell.flags |= tile_flag;
                }
            }
            else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                if (map_viewport.selected_layer == MapLayer::Tiles)
                {
                    cell.tile_x = -1;
                    cell.tile_y = -1;
                }
                else
                {
                    uint32_t tile_flag = GetMapLayerTileFlag(map_viewport.selected_layer);
                    cell.flags &= ~tile_flag;
                }
            }
            
            if (map_viewport.show_marker)
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
    
    static void ShowMap(MapViewport& map_viewport, TilePalette& tile_palette)
    {
        ImGui::SeparatorText("Map");
        
        Tilemap& tilemap = map_viewport.tilemap;
        const Tileset& tileset = *tilemap.tileset;
        
        if (!IsTilemapValid(tilemap))
            return;
        
        ImVec2 content_size;
        content_size.x = tilemap.width * tileset.tile_width + 1;
        content_size.y = tilemap.height * tileset.tile_height + 1;
        
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_HorizontalScrollbar;
        
        ImGuiChildFlags child_flags =
            ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY;
        
        ImGui::BeginChild("MapViewport-Map", ImVec2(480, 270), child_flags, window_flags);
        
        RenderTilemap(map_viewport);
        RenderTilemapOverlay(map_viewport);
        RenderGrid(map_viewport);
        RenderMarker(map_viewport, tile_palette);
        
        ImGui::Dummy(content_size);
        
        ImGui::EndChild();
    }
    
    static void ResizeTilemap(MapViewport& map_viewport)
    {
        if (map_viewport.input_width < MINIMUM_MAP_WIDTH)
            map_viewport.input_width = MINIMUM_MAP_WIDTH;
        
        if (map_viewport.input_height < MINIMUM_MAP_HEIGHT)
            map_viewport.input_height = MINIMUM_MAP_HEIGHT;
        
        Tilemap& tilemap = map_viewport.tilemap;
        tilemap.width = map_viewport.input_width;
        tilemap.height = map_viewport.input_height;
        
        int32 cell_count = tilemap.width * tilemap.height;
        tilemap.cells.resize(cell_count, { -1, -1 });
    }
    
    static void ResetTilemap(MapViewport& map_viewport)
    {
        map_viewport.input_width = MINIMUM_MAP_WIDTH;
        map_viewport.input_height = MINIMUM_MAP_HEIGHT;
        
        ResizeTilemap(map_viewport);
    }
    
    static void SaveTilemap(MapViewport& map_viewport)
    {
        Tilemap& tilemap = map_viewport.tilemap;
        if (!IsTilemapValid(tilemap))
        {
            OpenErrorPopup("Failed to Save Tilemap",
                "The current tilemap is incomplete and cannot be saved.");
            return;
        }
        
        size_t header_size = sizeof(SBMHeader);
        size_t cell_count = tilemap.cells.size();
        size_t cells_size = sizeof(SBMCell) * cell_count;
        size_t buffer_size = header_size + cells_size;
        
        std::vector<uint8> buffer(buffer_size);
        
        SBMHeader header;
        memcpy(header.magic, "SBMP", 4);
        header.width = tilemap.width;
        header.height = tilemap.height;
        
        std::vector<SBMCell> sbm_cells(tilemap.cells.size());
        for (size_t i = 0; i < tilemap.cells.size(); i++)
        {
            MapCell& map_cell = tilemap.cells[i];
            SBMCell& sbm_cell = sbm_cells[i];
            
            sbm_cell.tile_x = map_cell.tile_x;
            sbm_cell.tile_y = map_cell.tile_y;
            sbm_cell.flags = map_cell.flags;
        }
        
        memcpy(buffer.data(), &header, header_size);
        memcpy(buffer.data() + header_size, sbm_cells.data(), cells_size);
        
        if (!SDL_SaveFile(map_viewport.input_tilemap, buffer.data(), buffer_size))
        {
            OpenErrorPopup("Failed to Save Tilemap",
                "Could not write to the selected file.");
        }
    }
    
    static void OpenTilemap(MapViewport& map_viewport)
    {
        const char* filepath = map_viewport.input_tilemap;
        
        SDL_PathInfo path_info;
        if (!SDL_GetPathInfo(filepath, &path_info))
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
        char* file_data = (char*)SDL_LoadFile(filepath, &file_size);
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
        memcpy(&header, file_data, sizeof(SBMHeader));
        if (memcmp(header.magic, "SBMP", 4) != 0)
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
        
        map_viewport.tilemap.cells = std::move(map_cells);
        map_viewport.input_width = header.width;
        map_viewport.input_height = header.height;
        
        ResizeTilemap(map_viewport);
        
        SDL_free(file_data);
    }
    
    static void ShowProperties(MapViewport& map_viewport)
    {
        ImGui::SeparatorText("Properties");
        
        ImGui::BeginChild("MapViewport-Properties");
        
        if (ImGui::BeginCombo("Layer", GetMapLayerPreview(map_viewport.selected_layer)))
        {
            SelectableMapLayer(MapLayer::Tiles, map_viewport.selected_layer);
            SelectableMapLayer(MapLayer::Walls, map_viewport.selected_layer);
            SelectableMapLayer(MapLayer::LeftGoals, map_viewport.selected_layer);
            SelectableMapLayer(MapLayer::RightGoals, map_viewport.selected_layer);
            
            ImGui::EndCombo();
        }
        
        ImGui::Spacing();
        
        ImGui::InputInt("Width", &map_viewport.input_width);
        ImGui::InputInt("Height", &map_viewport.input_height);
        
        if (ImGui::Button("Resize##Map"))
            ResizeTilemap(map_viewport);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset##Map"))
            ResetTilemap(map_viewport);
        
        ImGui::Spacing();
        
        ImGui::InputText("Tilemap", map_viewport.input_tilemap, sizeof(map_viewport.input_tilemap));
        
        if (ImGui::Button("Save##Tilemap"))
            SaveTilemap(map_viewport);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Open##Tilemap"))
            OpenTilemap(map_viewport);
        
        ImGui::Spacing();
        
        ImGui::Checkbox("Show Grid", &map_viewport.show_grid);
        ImGui::SameLine();
        ImGui::Checkbox("Show Marker", &map_viewport.show_marker);
        
        ImGui::EndChild();
    }
    
    Result<MapViewport> CreateMapViewport(TilePalette& tile_palette)
    {
        MapViewport map_viewport;
        map_viewport.tilemap = {};
        map_viewport.tilemap.tileset = &tile_palette.tileset;
        map_viewport.tilemap.cells.resize(MINIMUM_MAP_CELL_COUNT, { -1, -1 });
        map_viewport.tilemap.width = MINIMUM_MAP_WIDTH;
        map_viewport.tilemap.height = MINIMUM_MAP_HEIGHT;
        map_viewport.selected_layer = MapLayer::Tiles;
        map_viewport.input_tilemap[0] = '\0';
        map_viewport.input_width = MINIMUM_MAP_WIDTH;
        map_viewport.input_height = MINIMUM_MAP_HEIGHT;
        map_viewport.show_grid = true;
        map_viewport.show_marker = true;
        
        return map_viewport;
    }
    
    void ShowMapViewport(MapViewport& map_viewport, TilePalette& tile_palette)
    {
        ImGui::Begin("Map Viewport");
        
        ShowMap(map_viewport, tile_palette);
        ShowProperties(map_viewport);
        
        ImGui::End();
    }
}
