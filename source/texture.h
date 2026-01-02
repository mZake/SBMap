#pragma once

#include "core.h"
#include "error.h"

namespace SBMap
{
    struct Texture2D
    {
        int32 id = -1;
        int32 width = 0;
        int32 height = 0;
        
        Texture2D() = default;
        Texture2D(const Texture2D& other);
        Texture2D(Texture2D&& other);
        ~Texture2D();
        
        Texture2D& operator=(const Texture2D& other);
        Texture2D& operator=(Texture2D&& other);
    };
    
    Result<Texture2D> LoadTexture(const char* filepath);
    bool IsTextureValid(const Texture2D& other);
    uint64_t GetTextureImGuiID(const Texture2D& texture);
}
