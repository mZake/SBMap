#pragma once

#include <variant>

#include <SDL3/SDL.h>

namespace SBMap
{
    struct Error
    {
        char message[512];
    };
    
    template<typename TValue>
    struct Result
    {
        std::variant<TValue, Error> value_or_error;
        
        Result(const TValue& value) : value_or_error(value) {}
        Result(const Error& error) : value_or_error(error) {}
    };
    
    Error MakeError(const char* format, ...);
    
    template<typename TValue>
    bool IsResultValue(const Result<TValue>& result)
    {
        return std::holds_alternative<TValue>(result.value_or_error);
    }
    
    template<typename TValue>
    bool IsResultError(const Result<TValue>& result)
    {
        return std::holds_alternative<Error>(result.value_or_error);
    }
    
    template<typename TValue>
    TValue& GetResultValue(Result<TValue>& result)
    {
        SDL_assert(IsResultValue(result));
        TValue& value = std::get<TValue>(result.value_or_error);
        return value;
    }
    
    template<typename TValue>
    const TValue& GetResultValue(const Result<TValue>& result)
    {
        SDL_assert(IsResultValue(result));
        const TValue& value = std::get<TValue>(result.value_or_error);
        return value;
    }
    
    template<typename TValue>
    Error& GetResultError(Result<TValue>& result)
    {
        SDL_assert(IsResultError(result));
        Error& error = std::get<Error>(result.value_or_error);
        return error;
    }
    
    template<typename TValue>
    const Error& GetResultError(const Result<TValue>& result)
    {
        SDL_assert(IsResultError(result));
        const Error& error = std::get<Error>(result.value_or_error);
        return error;
    }
}
