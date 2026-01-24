#pragma once

#include "error.h"

namespace SBMap
{
    void OpenErrorPopup(const char* title, const Error& error);
    void ShowErrorPopup();
    
    void OpenNativeErrorPopup(const char* title, const char* format, ...);
}
