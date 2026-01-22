#pragma once

#include <SDL3/SDL.h>

#include "core.h"
#include "error.h"

namespace SBMap
{
    struct Texture2D
    {
        SDL_Texture* handle = nullptr;
        size_t* count = nullptr;
        int32 width = 0;
        int32 height = 0;
        
        Texture2D() = default;
        Texture2D(const Texture2D& other);
        Texture2D(Texture2D&& other);
        ~Texture2D();
        
        Texture2D& operator=(const Texture2D& other);
        Texture2D& operator=(Texture2D&& other);
    };
    
    Texture2D CreateTexture(SDL_Surface* surface, SDL_Renderer* renderer);
    Result<Texture2D> LoadTexture(const char* filepath, SDL_Renderer* renderer);
    
    bool IsTextureValid(const Texture2D& texture);
    uint64 GetTextureImGuiID(const Texture2D& texture);
}
