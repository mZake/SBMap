#include <imgui.h>
#include <SDL3/SDL.h>

#include "error_popup.h"

namespace SBMap
{
    static char s_Title[128] = {};
    static char s_Message[512] = {};
    static char s_Details[512] = {};
    static bool s_ShouldOpen = false;
    
    static void ClearErrorPopup()
    {
        s_Title[0] = '\0';
        s_Message[0] = '\0';
        s_Details[0] = '\0';
        s_ShouldOpen = false;
    }
    
    static bool IsErrorPopupValid()
    {
        return (s_Title[0] != '\0') && (s_Message[0] != '\0');
    }
    
    void OpenErrorPopup(const char* title, const Error& error)
    {
        SDL_assert(title != nullptr);
        SDL_assert(error.message != nullptr);
        
        SDL_strlcpy(s_Title, title, sizeof(s_Title));
        SDL_strlcpy(s_Message, error.message, sizeof(s_Message));
        
        if (error.details)
            SDL_strlcpy(s_Details, error.details, sizeof(s_Details));
        
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
        
        ImGui::SetNextWindowSize(ImVec2(480.0f, 0.0f));
        if (ImGui::BeginPopupModal("Error"))
        {
            ImGui::PushFont(nullptr, 24.0f);
            ImGui::TextUnformatted(s_Title);
            ImGui::PopFont();
            
            ImGui::PushTextWrapPos(480.0f);
            
            ImGui::Spacing();
            ImGui::TextUnformatted(s_Message);
            
            if (s_Details[0])
            {
                ImGui::Spacing();
                if (ImGui::TreeNode("Details"))
                {
                    ImGui::TextUnformatted(s_Details);
                    ImGui::TreePop();  
                }
            }
            
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
    
    void OpenNativeErrorPopup(const char* context, const Error& error)
    {
        SDL_assert(context != nullptr);
        SDL_assert(error.message != nullptr);
        
        const char* details = error.details ? error.details : "Not available";
        
        char message[1024];
        SDL_snprintf(message, sizeof(message),
            "%s\n\n%s\n\nDetails:\n%s", context, error.message, details);
        
        if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SBMap", message, nullptr))
            SDL_Log("%s", message);
    }
}
