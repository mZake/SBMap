#include <SDL3/SDL.h>
#include <stb_image.h>

#include "core.h"
#include "error.h"
#include "scope.h"
#include "texture.h"

namespace SBMap
{
    static void IncrementCount(const Texture2D& texture)
    {
        if (!texture.handle || !texture.count)
            return;
        
        (*texture.count)++;
    }
    
    static void DecrementCount(const Texture2D& texture)
    {
        if (!texture.handle || !texture.count)
            return;
        
        (*texture.count)--;
        if (*texture.count == 0)
        {
            SDL_DestroyTexture(texture.handle);
            delete texture.count;
        }
    }
    
    Texture2D::Texture2D(const Texture2D& other)
    {
        IncrementCount(other);
        
        handle = other.handle;
        count = other.count;
        width = other.width;
        height = other.height;
    }
    
    Texture2D::Texture2D(Texture2D&& other)
    {        
        handle = other.handle;
        count = other.count;
        width = other.width;
        height = other.height;
        
        other.handle = nullptr;
        other.count = nullptr;
        other.width = 0;
        other.height = 0;
    }
    
    Texture2D::~Texture2D()
    {        
        DecrementCount(*this);
    }
    
    Texture2D& Texture2D::operator=(const Texture2D& other)
    {
        if (this != &other)
        {          
            if (handle != other.handle)
            {
                DecrementCount(*this);
                IncrementCount(other);
            }  
            
            handle = other.handle;
            count = other.count;
            width = other.width;
            height = other.height;
        }
        
        return *this;
    }
    
    Texture2D& Texture2D::operator=(Texture2D&& other)
    {
        if (this != &other)
        {            
            DecrementCount(*this);
            
            handle = other.handle;
            count = other.count;
            width = other.width;
            height = other.height;
            
            other.handle = nullptr;
            other.count = nullptr;
            other.width = 0;
            other.height = 0;
        }
        
        return *this;
    }
    
    Result<Texture2D> CreateTexture(SDL_Surface* surface, SDL_Renderer* renderer)
    {
        SDL_assert(surface != nullptr);
        SDL_assert(renderer != nullptr);
        
        if (surface->w < TEXTURE_MINIMUM_WIDTH || surface->h < TEXTURE_MINIMUM_HEIGHT)
            return Error{ "Image dimensions are smaller than the minimum allowed." };
        if (surface->w > TEXTURE_MAXIMUM_WIDTH || surface->h > TEXTURE_MAXIMUM_HEIGHT)
            return Error{ "Image dimensions are greater than the maximum allowed." };
        
        SDL_Texture* handle = SDL_CreateTextureFromSurface(renderer, surface);
        if (!handle)
            return Error{ "Could not process image.", SDL_GetError() };
        
        SDL_SetTextureScaleMode(handle, SDL_SCALEMODE_NEAREST);
        
        Texture2D texture;
        texture.handle = handle;
        texture.count = new size_t(1);
        texture.width = surface->w;
        texture.height = surface->h;
        
        return texture;
    }
    
    Result<Texture2D> LoadTexture(const char* filepath, SDL_Renderer* renderer)
    {
        SDL_assert(filepath != nullptr);
        SDL_assert(renderer != nullptr);
        
        SDL_PathInfo path_info;
        if (!SDL_GetPathInfo(filepath, &path_info))
            return Error{ "Path does not exist." };
        
        if (path_info.type != SDL_PATHTYPE_FILE)
            return Error{ "Path exists but is not a file." };
        
        int32 width, height, channels;
        auto pixels = MakeScope(stbi_load(filepath, &width, &height, &channels, 4), stbi_image_free);
        if (!pixels)
            return Error{ "Could not load image.", stbi_failure_reason() };
        
        SDL_PixelFormat pixel_format = SDL_PIXELFORMAT_RGBA32;
        auto surface = MakeScope(SDL_CreateSurfaceFrom(width, height, pixel_format, pixels.Get(), width * 4), SDL_DestroySurface);
        if (!surface)
            return Error{ "Could not process image.", SDL_GetError() };
        
        return CreateTexture(surface.Get(), renderer);
    }
    
    bool IsTextureValid(const Texture2D& texture)
    {
        if (!texture.handle)
            return false;
        
        SDL_assert(texture.count != nullptr);
        SDL_assert(texture.width > 0);
        SDL_assert(texture.height > 0);
        
        return true;
    }
    
    uint64 GetTextureImGuiID(const Texture2D& texture)
    {
        return reinterpret_cast<uint64>(texture.handle);
    }
}
