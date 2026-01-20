#pragma once

namespace SBMap
{
    void OpenErrorPopup(const char* title, const char* format, ...);
    void ShowErrorPopup();
    
    void OpenNativeErrorPopup(const char* title, const char* format, ...);
}
