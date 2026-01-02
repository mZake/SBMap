#include <vector>

#include <SDL3/SDL.h>
#include <stb_image.h>

#include "core.h"
#include "error.h"
#include "texture.h"

namespace SBMap
{
    struct TextureInfo
    {
        SDL_Texture* handle;
        int32 width;
        int32 height;
        int32 count;
        bool empty;
    };
    
    static std::vector<TextureInfo> s_TextureInfo;
    static std::vector<int32> s_EmptySlots;
    
    extern SDL_Renderer* GetRenderer();
    
    static Result<SDL_Texture*> LoadSDLTexture(const char* filepath)
    {
        int width, height;
        int channels = 4;
        stbi_uc* pixels = stbi_load(filepath, &width, &height, nullptr, channels);
        if (!pixels)
            return Error("file '{}' not found", filepath);
        
        SDL_PixelFormat pixel_format = SDL_PIXELFORMAT_RGBA32;
        int pitch = width * 4;
        SDL_Surface* surface = SDL_CreateSurfaceFrom(width, height, pixel_format, pixels, pitch);
        if (!surface)
            return Error("{}", SDL_GetError());
        
        SDL_Renderer* renderer = GetRenderer();
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
            return Error("{}", SDL_GetError());
        
        SDL_DestroySurface(surface);
        stbi_image_free(pixels);
        
        return texture;
    }
    
    static int32 GetNextID()
    {
        if (s_EmptySlots.empty())
        {
            s_TextureInfo.emplace_back();
            int32 id = s_TextureInfo.size() - 1;
            return id;
        }
        
        int32 id = s_EmptySlots.back();
        s_EmptySlots.pop_back();
        
        return id;
    }
    
    static Result<int32> CreateTexture(const char* filepath)
    {
        auto result = LoadSDLTexture(filepath);
        if (!result.is_value)
            return GetResultError(result);
        
        SDL_Texture* handle = GetResultValue(result);
        float32 width, height;
        SDL_GetTextureSize(handle, &width, &height);
        
        TextureInfo info;
        info.handle = handle;
        info.width = static_cast<int32>(width);
        info.height = static_cast<int32>(height);
        info.count = 1;
        info.empty = false;
        
        int32 id = GetNextID();
        s_TextureInfo[id] = info;
        
        return id;
    }
    
    static void DestroyTexture(int32 id)
    {        
        TextureInfo& info = s_TextureInfo[id];
        
        SDL_DestroyTexture(info.handle);
        
        info.handle = nullptr;
        info.empty = true;
        
        s_EmptySlots.push_back(id);
    }
    
    static void AddRef(int32 id)
    {
        TextureInfo& info = s_TextureInfo[id];
        info.count++;
    }
    
    static void RemoveRef(int32 id)
    {
        TextureInfo& info = s_TextureInfo[id];
        info.count--;
        if (info.count <= 0)
            DestroyTexture(id);
    }
    
    static bool IsIDValid(int32 id)
    {
        bool result = (id >= 0) && (id < s_TextureInfo.size());
        return result;
    }
    
    Texture2D::Texture2D(const Texture2D& other)
    {
        if (IsTextureValid(other))
            AddRef(other.id);
                
        this->id = other.id;
        this->width = other.width;
        this->height = other.height;
    }
    
    Texture2D::Texture2D(Texture2D&& other)
    {        
        this->id = other.id;
        this->width = other.width;
        this->height = other.height;
        
        other.id = -1;
        other.width = 0;
        other.height = 0;
    }
    
    Texture2D::~Texture2D()
    {        
        if (IsTextureValid(*this))
            RemoveRef(this->id);
    }
    
    Texture2D& Texture2D::operator=(const Texture2D& other)
    {
        if (this != &other)
        {            
            if (IsTextureValid(*this))
                RemoveRef(this->id);
            
            if (IsTextureValid(other))
                AddRef(other.id);
            
            this->id = other.id;
            this->width = other.width;
            this->height = other.height;
        }
        
        return *this;
    }
    
    Texture2D& Texture2D::operator=(Texture2D&& other)
    {
        if (this != &other)
        {            
            if (IsTextureValid(*this))
                RemoveRef(this->id);
            
            this->id = other.id;
            this->width = other.width;
            this->height = other.height;
            
            other.id = -1;
            other.width = 0;
            other.height = 0;
        }
        
        return *this;
    }
    
    Result<Texture2D> LoadTexture(const char* filepath)
    {
        auto result = CreateTexture(filepath);
        if (!result.is_value)
            return GetResultError(result);
        
        int32 id = GetResultValue(result);
        TextureInfo& info = s_TextureInfo[id];
        
        Texture2D texture;
        texture.id = id;
        texture.width = info.width;
        texture.height = info.height;
        
        return texture;
    }
    
    bool IsTextureValid(const Texture2D& texture)
    {
        if (!IsIDValid(texture.id))
            return false;
        
        TextureInfo& info = s_TextureInfo[texture.id];
        if (info.empty)
            return false;
        
        return true;
    }
    
    uint64_t GetTextureImGuiID(const Texture2D& texture)
    {
        TextureInfo& info = s_TextureInfo[texture.id];
        return reinterpret_cast<uint64_t>(info.handle);
    }
}
