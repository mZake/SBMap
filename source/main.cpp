#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "app.h"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    
    SBMap::AppContext app;
    if (!app.Init())
        return -1;
    
    app.Run();
    
    return 0;
}
