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
        decode(pdata_, hdr->Size);

        switch (hdr->Protocol)
        {
        case C2ZA_PROTO_1106_SELECT_ROLE:
        {
            PACKET_ZA2C_CONFIRM_ROLE* pkt = (PACKET_ZA2C_CONFIRM_ROLE*)bufSend_;

            ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_CONFIRM_ROLE));
            pkt->Header.Size = sizeof(PACKET_ZA2C_CONFIRM_ROLE);
            pkt->Header.Ctrl = 0x3;
            pkt->Header.Cmd = 0xFF;
            pkt->Header.Uid = 0;
            pkt->Header.Protocol = ZA2C_PROTO_1106_CONFIRM_ROLE;

            StringCchCopyA(pkt->name, 29, "test");

            encode(bufSend_, sizeof(PACKET_ZA2C_CONFIRM_ROLE));
            SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_CONFIRM_ROLE));
            break;
        }

        case C2ZA_PROTO_1107_WORLD_LOGIN:
        {
            PACKET_C2ZA_WORLD_LOGIN *pktIn = (PACKET_C2ZA_WORLD_LOGIN*)pdata_;

            USES_CONVERSION;
            LOGI(_T("ROLE: %s login to world!\n"), A2T(pktIn->name));


            PACKET_ZA2C_WORLD_LOGIN_RESP* pkt = (PACKET_ZA2C_WORLD_LOGIN_RESP*)bufSend_;

            ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));
            pkt->Header.Size = sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP);
            pkt->Header.Ctrl = 0x3;
            pkt->Header.Cmd = 0xFF;
            pkt->Header.Uid = 0;
            pkt->Header.Protocol = ZA2C_PROTO_1107_WORLD_LOGIN_RESP;


            // Role info
            pkt->unknown1 = 1;
            pkt->Level = 165;
            pkt->Exp = 4220806300;
            pkt->MapIndex = 8;
            pkt->MapCell = 0x5c5d;

            pkt->MP = 10;
            pkt->HP = 100;
            pkt->Str = 200;
            pkt->Magic = 60;
            pkt->Dex = 46;
            pkt->Vit = 50;
            pkt->Mana = 65;
            pkt->MaxHPBar = 1000;
            pkt->MaxMPBar = 500;
            pkt->MaxHPStore = 200000;
            pkt->MaxMPStore = 5000;

            pkt->Sinfo = 1025;
            pkt->LorePoint = 5000;


            StringCchCopyA(pkt->CharacterName, 21, "test");

            encode(bufSend_, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));

            SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));


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