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


    hr = LibEvent::Initialize(wZAPort_);

    return hr;
}


HRESULT ZoneAgent::Start()
{
    HRESULT hr = S_OK;

    hr = LibEvent::Start();


    return hr;
}

HRESULT ZoneAgent::Stop()
{
    HRESULT hr = S_OK;

    hr = LibEvent::Stop();

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



HRESULT ZoneAgentSession::ProcessEvent(size_t len)
{
    PACKET_HEADER* hdr = (PACKET_HEADER*)pdata_;

    switch (hdr->Ctrl)
    {
    case 1:
        switch (hdr->Cmd)
        {
        case C2ZA_CMD_E2_LOGIN_REQUEST:
            PACKET_ZA2C_LOGIN_RESP* pkt = (PACKET_ZA2C_LOGIN_RESP*)bufSend_;
            ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_LOGIN_RESP));
            pkt->Header.Size = sizeof(PACKET_ZA2C_LOGIN_RESP);
            pkt->Header.Ctrl = 0x3;
            pkt->Header.Cmd = 0xFF;
            pkt->Header.Uid = 0;
            pkt->Header.Protocol = ZA2C_PROTO_1105_LOGIN_RESP;


            int i = 0;
            StringCchCopyA(pkt->rolebrief[i].name, 21, "test");
            pkt->rolebrief[i].used = 1;
            pkt->rolebrief[i].classtype = 1;
            pkt->rolebrief[i].town = 1;
            pkt->rolebrief[i].level = 165;


            encode(bufSend_, sizeof(PACKET_ZA2C_LOGIN_RESP));

            SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_LOGIN_RESP));

            break;
         
        }
        break;

    case 3:
        switch (hdr->Protocol)
        {
        case C2ZA_PROTO_1106_LOGIN_REQUEST:
        {


            break;
        }

        break;
    }



    

   


    }

    return S_OK;
}


HRESULT ZoneAgentSession::OnClose()
{
    //GetApp()->NotifyEvent(EVENT_ID_USER_DISCONNECTED, WPARAM(this), 0);
    return S_OK;
}