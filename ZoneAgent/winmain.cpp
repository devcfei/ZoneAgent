#include "framework.h"


static App* pApp;
App* GetApp()
{
    return pApp;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    pApp = new App();

    pApp->Initialize(hInstance);
    pApp->Run();


    delete pApp;

    return 0;
}