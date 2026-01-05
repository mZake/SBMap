#include "app.h"

int main()
{
    SBMap::AppContext app_context;
    if (!SBMap::InitAppContext(app_context))
        return -1;
    
    SBMap::RunApp(app_context);
    
    SBMap::CloseAppContext(app_context);
}
