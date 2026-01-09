#include <imgui.h>
#include <imgui_stdlib.h>

#include "core.h"
#include "texture.h"
#include "tilemap.h"
#include "tile_palette.h"

namespace SBMap
{
    constexpr int32 MINIMUM_TILE_WIDTH = 4;
    constexpr int32 MINIMUM_TILE_HEIGHT = 4;
    
    static Error s_Error;
    static bool s_OpenErrorWindow = false;
    
    static void SetError(Error error)
    {
        s_Error = error;
        s_OpenErrorWindow = true;
    }
    
    static void ClearError()
    {
        s_Error.message[0] = '\0';
        s_OpenErrorWindow = false;
    }
    
    static void ResizeTile(TilePalette& tile_palette)
    {
        Tileset& tileset = tile_palette.tileset;
        
        if (tile_palette.input_tile_width > tileset.atlas.width)
            tile_palette.input_tile_width = tileset.atlas.width;
        if (tile_palette.input_tile_width < MINIMUM_TILE_WIDTH)
            tile_palette.input_tile_width = MINIMUM_TILE_WIDTH;
        
        if (tile_palette.input_tile_height > tileset.atlas.height)
            tile_palette.input_tile_height = tileset.atlas.height;
        if (tile_palette.input_tile_height < MINIMUM_TILE_HEIGHT)
            tile_palette.input_tile_height = MINIMUM_TILE_HEIGHT;
        
        tileset.tile_width = tile_palette.input_tile_width;
        tileset.tile_height = tile_palette.input_tile_height;
        
        tile_palette.selected_x = 0;
        tile_palette.selected_y = 0;
    }
    
    static void ResetTile(TilePalette& tile_palette)
    {
        tile_palette.input_tile_width = MINIMUM_TILE_WIDTH;
        tile_palette.input_tile_height = MINIMUM_TILE_HEIGHT;
        
        ResizeTile(tile_palette);
    }
    
    static void OpenAtlas(TilePalette& tile_palette, SDL_Renderer* renderer)
    {
        auto atlas_result = LoadTexture(tile_palette.input_atlas_image, renderer);
        if (IsResultValue(atlas_result))
        {
            Tileset& tileset = tile_palette.tileset;
            
            Texture2D& atlas = GetResultValue(atlas_result);
            int32 tile_width = tileset.tile_width;
            int32 tile_height = tileset.tile_height;
            
            tileset = CreateTileset(atlas, tile_width, tile_height);
            
            ResizeTile(tile_palette);
        }
        else
        {
            Error error = GetResultError(atlas_result);
            SetError(error);
        }
    }
    
    static void ResetAtlas(TilePalette& tile_palette)
    {
        tile_palette.tileset.atlas = {};
        tile_palette.tileset.width = 0;
        tile_palette.tileset.height = 0;
        tile_palette.input_atlas_image[0] = '\0';
    }
    
    static void ShowSelectTile(TilePalette& tile_palette)
    {
        ImGui::SeparatorText("Select Tile");
        
        Tileset& tileset = tile_palette.tileset;
        if (IsTilesetValid(tileset))
        {
            ImVec2 content_size;
            content_size.x = tileset.atlas.width;
            content_size.y = tileset.atlas.height;
            
            ImGui::BeginChild("TilePalette-SelectTile", content_size);
            
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                ImVec2 mouse_position = ImGui::GetMousePos() - ImGui::GetWindowPos();
                tile_palette.selected_x = mouse_position.x / tileset.tile_width;
                tile_palette.selected_y = mouse_position.y / tileset.tile_height;
            }
            
            ImTextureRef atlas_image_ref = GetTextureImGuiID(tileset.atlas);
            ImGui::Image(atlas_image_ref, content_size);
            
            ImVec2 window_position = ImGui::GetWindowPos();
            
            ImVec2 marker_min;
            marker_min.x = window_position.x + tile_palette.selected_x * tileset.tile_width;
            marker_min.y = window_position.y + tile_palette.selected_y * tileset.tile_height;
            
            ImVec2 marker_max;
            marker_max.x = marker_min.x + tileset.tile_width;
            marker_max.y = marker_min.y + tileset.tile_height;
            
            ImColor marker_color = { 255, 255, 255, 255 };
            
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRect(marker_min, marker_max, marker_color);
            
            ImGui::EndChild();
        }
        else
        {
            Texture2D& placeholder = tile_palette.placeholder;
            
            ImVec2 placeholder_size;
            placeholder_size.x = placeholder.width;
            placeholder_size.y = placeholder.height;
            
            ImTextureRef placeholder_image_ref = GetTextureImGuiID(placeholder);
            ImGui::Image(placeholder_image_ref, placeholder_size);
        }
    }
    
    static void ShowProperties(TilePalette& tile_palette, SDL_Renderer* renderer)
    {
        ImGui::SeparatorText("Properties");
        
        ImGui::BeginChild("TilePalette-Properties");
        
        ImGui::InputInt("Tile Width", &tile_palette.input_tile_width);
        ImGui::InputInt("Tile Height", &tile_palette.input_tile_height);
        
        if (ImGui::Button("Resize##Tile"))
            ResizeTile(tile_palette);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset##Tile"))
            ResetTile(tile_palette);
        
        ImGui::Spacing();
        
        ImGui::InputText("Atlas Image", tile_palette.input_atlas_image, sizeof(tile_palette.input_atlas_image));
        
        if (ImGui::Button("Open"))
            OpenAtlas(tile_palette, renderer);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset##Atlas"))
            ResetAtlas(tile_palette);
        
        ImGui::EndChild();
    }
    
    Result<TilePalette> CreateTilePalette(SDL_Renderer* renderer)
    {
        SDL_assert(renderer != nullptr);
        
        auto placeholder_result = LoadTexture("assets/placeholder.png", renderer);
        if (IsResultError(placeholder_result))
        {
            Error error = GetResultError(placeholder_result);
            return MakeError("Placeholder texture loading failed: %s", error.message);
        }
        
        TilePalette tile_palette;
        tile_palette.tileset = {};
        tile_palette.tileset.tile_width = MINIMUM_TILE_WIDTH;
        tile_palette.tileset.tile_height = MINIMUM_TILE_HEIGHT;
        tile_palette.placeholder = GetResultValue(placeholder_result);
        tile_palette.selected_x = 0;
        tile_palette.selected_y = 0;
        tile_palette.input_atlas_image[0] = '\0';
        tile_palette.input_tile_width = MINIMUM_TILE_WIDTH;
        tile_palette.input_tile_height = MINIMUM_TILE_HEIGHT;
        
        return tile_palette;
    }
    
    void ShowTilePalette(TilePalette& tile_palette, SDL_Renderer* renderer)
    {
        SDL_assert(renderer != nullptr);
        
        ImGui::Begin("Tile Palette");
        
        if (s_OpenErrorWindow)
            ImGui::OpenPopup("Error");
        
        if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextUnformatted(s_Error.message);
            s_OpenErrorWindow = false;
            
            if (ImGui::Button("Ok##Error", ImVec2(120, 0)))
            {
                ClearError();
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }
        
        ShowSelectTile(tile_palette);
        ShowProperties(tile_palette, renderer);
        
        ImGui::End();
    }
}
