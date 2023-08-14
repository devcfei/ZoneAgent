#include "framework.h"

HRESULT App::Initialize(HINSTANCE hInstance)
{
    HRESULT hr = S_OK;

    // save hInstance;
    hInst_ = hInstance;

    // Get application path
    GetModuleFileName(NULL, szAppPath_, MAX_PATH);
    PathRemoveFileSpec(szAppPath_);

    // build configuration file path and initialize ZoneAgent
    TCHAR szCfgFile[MAX_PATH];
    StringCchCopy(szCfgFile, MAX_PATH, szAppPath_);
    StringCchCat(szCfgFile, MAX_PATH, _T("\\ZoneAgent.ini"));

    // initialize ZoneAgent
    hr = ZA_.Initialize(szCfgFile);
    // lock
    InitializeCriticalSection(&csLog_);

    //hr = LibEvent::Initialize(ZA_.GetPort());


    return hr;
}

HRESULT App::Run()
{
    HRESULT hr = S_OK;
    DialogBoxParam(hInst_, MAKEINTRESOURCE(IDD_MAIN), NULL, DialogProc, (LPARAM)this);
    return hr;
}

HRESULT App::Start()
{
    HRESULT hr = S_OK;   

    hr = ZA_.Start();

    return hr;
}

HRESULT App::Stop()
{
    HRESULT hr = S_OK;

    hr = ZA_.Stop();

    return hr;
}



INT_PTR CALLBACK App::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT_PTR r = (INT_PTR)FALSE;
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:

        r = GetApp()->OnInitDialog(hDlg, message, wParam, lParam);
        break;

    case WM_CTLCOLORSTATIC:
        r = GetApp()->OnColorStatic(hDlg, message, wParam, lParam);
        break;

    case WM_COMMAND:
        r = GetApp()->OnCommand(hDlg, message, wParam, lParam);
        break;
    }

    return r;
}


INT_PTR App::OnInitDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    // make window in the center
    RECT rect;
    GetWindowRect(hDlg, &rect);
    int nWidth = rect.right - rect.left;
    int nHeight = rect.bottom - rect.top;

    int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    int X = (iScreenWidth - nWidth) / 2;
    int Y = (iScreenHeight - nHeight) / 2;
    SetWindowPos(hDlg, NULL, X, Y, nWidth, nHeight, SWP_NOSIZE);

    // set dialog icon
    HICON hIcon = LoadIcon(hInst_, MAKEINTRESOURCE(IDI_FAVICON));
    SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);



    // create a global brush for log
    hbrBkgnd_ = CreateSolidBrush(RGB(0, 0, 0));

    TCHAR lpszLatin[] = L"Lorem ipsum dolor sit amet, consectetur "
        L"adipisicing elit, sed do eiusmod tempor "
        L"incididunt ut labore et dolore magna "
        L"aliqua. Ut enim ad minim veniam, quis "
        L"nostrud exercitation ullamco laboris nisi "
        L"ut aliquip ex ea commodo consequat. Duis "
        L"aute irure dolor in reprehenderit in "
        L"voluptate velit esse cillum dolore eu "
        L"fugiat nulla pariatur. Excepteur sint "
        L"occaecat cupidatat non proident, sunt "
        L"in culpa qui officia deserunt mollit "
        L"anim id est laborum.";

    HWND hWndEditLSLog = GetDlgItem(hDlg, IDC_EDIT_LSLOG);

    SetWindowText(hWndEditLSLog, lpszLatin);

    HWND hWndEditLog = GetDlgItem(hDlg, IDC_EDIT_LOG);

    SetWindowText(hWndEditLog, lpszLatin);


    HRESULT hr;
    hr = GetApp()->Start();
    if (FAILED(hr))
    {
        GetApp()->Stop();
        LOGF(_T("App startup failed!\n"));
    }

    //PrintMessage(_T("App initialize success!\n"));


    return (INT_PTR)TRUE;
}


INT_PTR App::OnColorStatic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT_PTR r = (INT_PTR)FALSE;
    HDC hdc = (HDC)wParam;
    HWND hwnd = (HWND)lParam;

    int iCtrlID = GetDlgCtrlID(hwnd);

    switch (iCtrlID)
    {
    case IDC_EDIT_LOG:
    case IDC_EDIT_LSLOG:
        SetTextColor(hdc, RGB(200, 200, 200));
        SetBkColor(hdc, RGB(0, 0, 0));
        r = (INT_PTR)hbrBkgnd_;
        break;

    }

    return r;
}


INT_PTR App::OnCommand(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT_PTR r = (INT_PTR)FALSE;

    WORD idCtrlID = LOWORD(wParam);

    switch (idCtrlID)
    {
    case IDCANCEL:
        EndDialog(hDlg, LOWORD(wParam));
        r = (INT_PTR)TRUE;
        break;

    case IDC_BTN_RELOAD:
        MessageBox(hDlg, _T("Reload settings"), _T("Reload"), MB_OKCANCEL);
        r = (INT_PTR)TRUE;
        break;
    }
    return r;
}