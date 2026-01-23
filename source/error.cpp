#include <stdio.h>
#include <stdarg.h>

#include "error.h"

namespace SBMap
{
    Error MakeError(const char* format, ...)
    {
        Error error;
        
        va_list args;
        va_start(args, format);
        // vsnprintf(error.message, sizeof(error.message), format, args);
        va_end(args);
        
        return error;
    }
}
