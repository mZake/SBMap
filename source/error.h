#pragma once

#include <format>
#include <string>
#include <variant>

namespace SBMap
{
    struct Error
    {
        std::string what;
        
        template<typename... TArgs>
        Error(std::format_string<TArgs...> format, TArgs&&... args)
            : what(std::format(format, std::forward<TArgs>(args)...))
        {}
    };
    
    template<typename TValue>
    struct Result
    {
        std::variant<TValue, Error> result;
        bool is_value = false;
        
        Result(const TValue& value)
            : result(value)
            , is_value(true)
        {}
        
        Result(const Error& error)
            : result(error)
            , is_value(false)
        {}
    };
    
    template<typename TValue>
    TValue& GetResultValue(Result<TValue>& result)
    {
        TValue& value = std::get<TValue>(result.result);
        return value;
    }
    
    template<typename TValue>
    const TValue& GetResultValue(const Result<TValue>& result)
    {
        const TValue& value = std::get<TValue>(result.result);
        return value;
    }
    
    template<typename TValue>
    Error& GetResultError(Result<TValue>& result)
    {
        Error& error = std::get<Error>(result.result);
        return error;
    }
    
    template<typename TValue>
    const Error& GetResultError(const Result<TValue>& result)
    {
        const Error& error = std::get<Error>(result.result);
        return error;
    }
}
