#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "app.h"

int main(int argc, char** argv)
{
    SBMap::AppContext app_context;
    if (!SBMap::InitAppContext(app_context))
        return -1;
    
    SBMap::RunApp(app_context);
    
    SBMap::CloseAppContext(app_context);
    
    return 0;
}
