#include <SDL3/SDL.h>
#include <stb_image.h>

#include "core.h"
#include "error.h"
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
    
    Texture2D CreateTexture(SDL_Surface* surface, SDL_Renderer* renderer)
    {
        SDL_assert(surface != nullptr);
        SDL_assert(renderer != nullptr);
        
        SDL_Texture* handle = SDL_CreateTextureFromSurface(renderer, surface);
        if (!handle)
            return Texture2D();
        
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
            return MakeError("The selected file was not found.");
        
        if (path_info.type != SDL_PATHTYPE_FILE)
            return MakeError("The selected item is not a file.");
        
        uint8* pixels = nullptr;
        SDL_Surface* surface = nullptr;
        
        auto cleanup_intermediate = [&](){
            if (surface)
                SDL_DestroySurface(surface);
            if (pixels)
                stbi_image_free(pixels);
        };
        
        int32 width, height, channels;
        pixels = stbi_load(filepath, &width, &height, &channels, 4);
        if (!pixels)
        {
            cleanup_intermediate();
            return MakeError("Could not load image from the selected file.");
        }
        
        SDL_PixelFormat pixel_format = SDL_PIXELFORMAT_RGBA32;
        surface = SDL_CreateSurfaceFrom(width, height, pixel_format, pixels, width * 4);
        if (!surface)
        {
            cleanup_intermediate();
            return MakeError("Could not create a surface from the image.");
        }
        
        Texture2D texture = CreateTexture(surface, renderer);
        if (!IsTextureValid(texture))
        {
            cleanup_intermediate();
            return MakeError("Could not create a texture from the surface.");
        }
        
        cleanup_intermediate();
        
        return texture;
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
