#pragma once

#include <utility>

#include <SDL3/SDL.h>

namespace SBMap
{
    struct Error
    {
        char message[512];
    };
    
    template<typename TValue, typename TError = Error>
    class Result
    {
    public:
        Result(TValue value)
        {
            ::new(&m_Storage) TValue(std::move(value));
            m_IsValue = true;
        }
        
        Result(TError error)
        {
            ::new (&m_Storage) TError(std::move(error));
            m_IsValue = false;
        }
        
        ~Result()
        {
            if (m_IsValue)
                m_Storage.value.~TValue();
            else
                m_Storage.error.~TError();
        }
        
        Result(const Result&) = delete;
        Result(Result&&) = delete;
        
        Result& operator=(const Result&) = delete;
        Result& operator=(Result&&) = delete;
        
        bool IsValue() const { return m_IsValue; }
        bool IsError() const { return !m_IsValue; }
        operator bool() const { return IsValue(); }
        
        TValue& GetValue() { SDL_assert(IsValue()); return m_Storage.value; }
        TError& GetError() { SDL_assert(IsError()); return m_Storage.error; }
        const TValue& GetValue() const { SDL_assert(IsValue()); return m_Storage.value; }
        const TError& GetError() const { SDL_assert(IsError()); return m_Storage.error; }
        
    private:
        union Storage
        {
            TValue value;
            TError error;
            
            Storage() {}
            ~Storage() {}
        };
        
        Storage m_Storage;
        bool m_IsValue = false;
    };
}
