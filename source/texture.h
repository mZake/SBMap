#pragma once

#include <SDL3/SDL.h>

#include "core.h"
#include "error.h"

namespace SBMap
{
    constexpr int32 TEXTURE_MINIMUM_WIDTH = 4;
    constexpr int32 TEXTURE_MAXIMUM_WIDTH = 8192;
    constexpr int32 TEXTURE_MINIMUM_HEIGHT = 4;
    constexpr int32 TEXTURE_MAXIMUM_HEIGHT = 8192;
    
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
    
    Result<Texture2D> CreateTexture(SDL_Surface* surface, SDL_Renderer* renderer);
    Result<Texture2D> LoadTexture(const char* filepath, SDL_Renderer* renderer);
    
    bool IsTextureValid(const Texture2D& texture);
    uint64 GetTextureImGuiID(const Texture2D& texture);
}
