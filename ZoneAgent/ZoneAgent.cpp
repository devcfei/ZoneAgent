#include "framework.h"



ZoneAgent::ZoneAgent()
{
    InitializeDefaultConfig();
}

ZoneAgent::~ZoneAgent()
{
}

HRESULT ZoneAgent::Initialize(LPCTSTR lpszCfgFile)
{
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

    if (!lpszCfgFile)
    {
        LOGE(_T("%s failed"), __FUNCTION__);
        hr = E_INVALIDARG;
        return hr;
    }

    BOOL bFileExist = FALSE;
    DWORD dwAttrib = GetFileAttributes(lpszCfgFile);
    if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        bFileExist = TRUE;
        hr = S_OK;
    }

    if (!bFileExist)
    {
        LOGW(_T("ZoneAgent.ini doesn't exist, create it"));
        hr = S_OK;
        WriteConfig(lpszCfgFile);
    }
    else
    {
        ReadConfig(lpszCfgFile);
    }

    return hr;
}


HRESULT ZoneAgent::Start()
{
    HRESULT hr = S_OK;

    return hr;
}

HRESULT ZoneAgent::Stop()
{
    HRESULT hr = S_OK;

    return hr;
}


HRESULT ZoneAgent::InitializeDefaultConfig()
{
    HRESULT hr = S_OK;

    nServerID_ = 0;
    nAgentID_ = 0;
    wZAPort_ = 3550;
    StringCchCopy(szZAIP_, 16, _T("127.0.0.1"));
    return hr;
}

HRESULT ZoneAgent::ReadConfig(LPCTSTR lpszCfgFile)
{
    HRESULT hr = S_OK;

    // [STARTUP]
    nServerID_ = GetPrivateProfileInt(_T("STARTUP"), _T("SERVERID"), 0, lpszCfgFile);
    nAgentID_ = GetPrivateProfileInt(_T("STARTUP"), _T("AGENTID"), 0, lpszCfgFile);
    GetPrivateProfileString(_T("STARTUP"), _T("IP"), _T("127.0.0.1"), szZAIP_, 16, lpszCfgFile);
    wZAPort_ = GetPrivateProfileInt(_T("STARTUP"), _T("PORT"), 3550, lpszCfgFile);

    return hr;
}

HRESULT ZoneAgent::WriteConfig(LPCTSTR lpszCfgFile)
{
    HRESULT hr = S_OK;

    TCHAR value[32];

    // [STARTUP]
    StringCchPrintf(value, 32, _T("%d"), nServerID_);
    WritePrivateProfileString(_T("STARTUP"), _T("SERVERID"), value, lpszCfgFile);
    StringCchPrintf(value, 32, _T("%d"), nAgentID_);
    WritePrivateProfileString(_T("STARTUP"), _T("AGENTID"), value, lpszCfgFile);
    WritePrivateProfileString(_T("STARTUP"), _T("IP"), szZAIP_, lpszCfgFile);
    StringCchPrintf(value, 32, _T("%d"), wZAPort_);
    WritePrivateProfileString(_T("STARTUP"), _T("PORT"), value, lpszCfgFile);

    return hr;
}

