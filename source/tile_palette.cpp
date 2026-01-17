#include <imgui.h>
#include <SDL3/SDL.h>

#include "core.h"
#include "error_popup.h"
#include "texture.h"
#include "tilemap.h"
#include "tile_palette.h"

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
        auto& input_atlas_image = tile_palette->input_atlas_image;
        SDL_strlcpy(input_atlas_image, *filelist, sizeof(input_atlas_image));
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
        tileset.width = tileset.atlas.width / tileset.tile_width;
        tileset.height = tileset.atlas.height / tileset.tile_height;
        
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
            Texture2D& atlas = GetResultValue(atlas_result);
            int32 tile_width = tile_palette.input_tile_width;
            int32 tile_height = tile_palette.input_tile_height;
            
            tile_palette.tileset = CreateTileset(atlas, tile_width, tile_height);
        }
        else
        {
            Error error = GetResultError(atlas_result);
            OpenErrorPopup("Failed to Open Atlas", "%s", error.message);
        }
    }
    
    static void ExploreAtlas(TilePalette& tile_palette, SDL_Window* window)
    {
        static SDL_DialogFileFilter filters[] = {
            { "PNG images",  "png" },
            { "JPEG images", "jpg;jpeg" },
            { "All images",  "png;jpg;jpeg" },
            { "All files",   "*" },
        };
        
        SDL_ShowOpenFileDialog(OpenFileDialogCallback,
            &tile_palette, window, filters, SDL_arraysize(filters), nullptr, false);
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
            content_size.x = (float32)tileset.atlas.width * tile_palette.scale;
            content_size.y = (float32)tileset.atlas.height * tile_palette.scale;
            
            float32 tile_width_scaled = (float32)tileset.tile_width * tile_palette.scale;
            float32 tile_height_scaled = (float32)tileset.tile_height * tile_palette.scale;
            
            ImGuiChildFlags child_flags =
                ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY;
            
            ImGuiWindowFlags window_flags =
                ImGuiWindowFlags_HorizontalScrollbar;
            
            ImGui::BeginChild("TilePalette-SelectTile", ImVec2(480, 270), child_flags, window_flags);
            
            ImVec2 window_begin = ImGui::GetCursorScreenPos();
            
            if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                ImVec2 mouse_position = ImGui::GetMousePos() - window_begin;
                int32 selected_x = mouse_position.x / tile_width_scaled;
                int32 selected_y = mouse_position.y / tile_height_scaled;
                
                if (selected_x >= 0 && selected_x < tileset.width &&
                    selected_y >= 0 && selected_y < tileset.height)
                {
                    tile_palette.selected_x = selected_x;
                    tile_palette.selected_y = selected_y;
                }
            }
            
            ImTextureRef atlas_image_ref = GetTextureImGuiID(tileset.atlas);
            ImGui::Image(atlas_image_ref, content_size);
            
            ImVec2 marker_min;
            marker_min.x = window_begin.x + tile_palette.selected_x * tile_width_scaled;
            marker_min.y = window_begin.y + tile_palette.selected_y * tile_height_scaled;
            
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
            Texture2D& placeholder = tile_palette.placeholder;
            
            ImVec2 placeholder_size;
            placeholder_size.x = placeholder.width;
            placeholder_size.y = placeholder.height;
            
            ImTextureRef placeholder_image_ref = GetTextureImGuiID(placeholder);
            ImGui::Image(placeholder_image_ref, placeholder_size);
        }
    }
    
    static void ShowProperties(TilePalette& tile_palette, SDL_Window* window, SDL_Renderer* renderer)
    {
        ImGui::SeparatorText("Properties");
        
        ImGui::BeginChild("TilePalette-Properties");
        
        ImGui::SliderFloat("Scale", &tile_palette.scale, 0.25f, 4.0f);
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
        
        if (ImGui::Button("Explore"))
            ExploreAtlas(tile_palette, window);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset##Atlas"))
            ResetAtlas(tile_palette);
        
        ImGui::EndChild();
    }
    
    Result<TilePalette> CreateTilePalette(SDL_Renderer* renderer)
    {
        SDL_assert(renderer != nullptr);
        
        auto placeholder_result = LoadTexture("assets/images/placeholder.png", renderer);
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
        tile_palette.scale = 1.0f;
        tile_palette.input_atlas_image[0] = '\0';
        tile_palette.input_tile_width = MINIMUM_TILE_WIDTH;
        tile_palette.input_tile_height = MINIMUM_TILE_HEIGHT;
        
        return tile_palette;
    }
    
    void ShowTilePalette(TilePalette& tile_palette, SDL_Window* window, SDL_Renderer* renderer)
    {
        SDL_assert(renderer != nullptr);
        
        ImGui::Begin("Tile Palette");
        
        ShowSelectTile(tile_palette);
        ShowProperties(tile_palette, window, renderer);
        
        ImGui::End();
    }
}
