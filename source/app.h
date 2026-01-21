#pragma once

#include <SDL3/SDL.h>

#include "core.h"
#include "map_viewport.h"
#include "texture.h"
#include "tile_palette.h"

namespace SBMap
{
    class AppContext
    {
    public:
        ~AppContext();
        
        bool Init();
        void Run();
        
        SDL_Window* GetWindow() const { return m_Window; }
        SDL_Renderer* GetRenderer() const {return m_Renderer; }
        const TilePalette& GetTilePalette() const { return m_TilePalette; }
        const MapViewport& GetMapViewport() const { return m_MapViewport; }
        const Texture2D& GetCheckerboard() const { return m_Checkerboard; }
        
    private:
        void ProcessEvents();
        
    private:
        SDL_Window* m_Window = nullptr;
        SDL_Renderer* m_Renderer = nullptr;
        TilePalette m_TilePalette;
        MapViewport m_MapViewport;
        Texture2D m_Checkerboard;
        float32 m_DisplayScale = 0.0f;
        bool m_ImGuiInit = false;
        bool m_Fullscreen = false;
        bool m_Running = false;
    };
}
