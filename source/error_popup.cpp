#include <imgui.h>
#include <SDL3/SDL.h>

#include <stdarg.h>

#include "error_popup.h"

namespace SBMap
{
    static char s_Title[128] = {};
    static char s_Message[512] = {};
    static bool s_ShouldOpen = false;
    
    static void ClearErrorPopup()
    {
        s_Title[0] = '\0';
        s_Message[0] = '\0';
        s_ShouldOpen = false;
    }
    
    static bool IsErrorPopupValid()
    {
        return (s_Title[0] != '\0') && (s_Message[0] != '\0');
    }
    
    void OpenErrorPopup(const char* title, const char* format, ...)
    {
        SDL_assert(title != nullptr);
        SDL_assert(format != nullptr);
        
        SDL_strlcpy(s_Title, title, sizeof(s_Title));
        
        va_list args;
        va_start(args, format);
        SDL_vsnprintf(s_Message, sizeof(s_Message), format, args);
        va_end(args);
        
        s_ShouldOpen = true;
    }
    
    void ShowErrorPopup()
    {
        if (s_ShouldOpen)
        {
            SDL_assert(IsErrorPopupValid());
            ImGui::OpenPopup("Error");
            s_ShouldOpen = false;
        }
        
        if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushFont(nullptr, 24.0f);
            ImGui::TextUnformatted(s_Title);
            ImGui::PopFont();
            
            ImGui::Spacing();
            
            ImGui::PushTextWrapPos(480.0f);
            ImGui::TextUnformatted(s_Message);
            ImGui::PopTextWrapPos();
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            if (ImGui::Button("Ok", ImVec2(120, 0)))
            {
                ClearErrorPopup();
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndPopup();
        }
    }
}
