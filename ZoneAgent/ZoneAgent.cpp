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



HRESULT ZoneAgentSession::ProcessEvent(void* pbuff, size_t len)
{
    PACKET_HEADER* hdr = (PACKET_HEADER*)pbuff;

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


            struct CharaInvItem
            {
                uint32_t pid;
                uint32_t code;
                uint32_t prop;
                uint32_t sn;
            };

            int i = 0;
            StringCchCopyA(pkt->rolebrief[i].name, 21, "Warrior");
            pkt->rolebrief[i].used = 1;
            pkt->rolebrief[i].classtype = 0;
            pkt->rolebrief[i].town = 1;
            pkt->rolebrief[i].level = 165;

            i = 1;
            StringCchCopyA(pkt->rolebrief[i].name, 21, "Knight");
            pkt->rolebrief[i].used = 1;
            pkt->rolebrief[i].classtype = 1;
            pkt->rolebrief[i].town = 1;
            pkt->rolebrief[i].level = 165;

            i = 2;
            StringCchCopyA(pkt->rolebrief[i].name, 21, "Mage");
            pkt->rolebrief[i].used = 1;
            pkt->rolebrief[i].classtype = 2;
            pkt->rolebrief[i].town = 1;
            pkt->rolebrief[i].level = 165;


            i = 3;
            StringCchCopyA(pkt->rolebrief[i].name, 21, "Archer");
            pkt->rolebrief[i].used = 1;
            pkt->rolebrief[i].classtype = 3;
            pkt->rolebrief[i].town = 1;
            pkt->rolebrief[i].level = 165;


            i = 4;
            StringCchCopyA(pkt->rolebrief[i].name, 21, "GM");
            pkt->rolebrief[i].used = 0xFF;
            pkt->rolebrief[i].classtype = 2;
            pkt->rolebrief[i].town = 0;
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
            OnSelectRole((PACKET_C2ZA_SELECT_ROLE*)pdata_);
            break;
        }

        case C2ZA_PROTO_1107_WORLD_LOGIN:
        {
            OnWorldLogin((PACKET_C2ZA_WORLD_LOGIN*)pdata_);
            break;
        }

        case C2ZA_PROTO_1461_EXIT_GAME:
        {
            OnExitGame();
            break;
        }
        case C2ZA_PROTO_1200_ROLE_MOVE:
        {
            OnRoleMove((PACKET_C2ZA_ROLE_MOVE*)pdata_);
            break;
        }

        case C2ZA_PROTO_1202_ROLE_MOVE_DONE:
        {
            LOGI(_T("========C2ZA_PROTO_1202_ROLE_MOVE_DONE======\n"));
            break;
        }

        case C2ZA_PROTO_1400_ATTACK_MONSTER:
        {
            OnAttackMonster((PACKET_C2ZA_ATTACK_MONSTER*)pdata_);
            break;
        }

        case C2ZA_PROTO_1800_MESSAGE:
        {
            OnMessage((PACKET_C2ZA_MESSAGE*)pdata_);
            break;
        }

        default:        
            LOGI(_T("========UNSUPPORTED PACKET======\n"));
            LOGI(_T("Size %d \n"), hdr->Size);
            LOGI(_T("Protocol 0x%04X\n"), hdr->Protocol);
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


HRESULT ZoneAgentSession::OnSelectRole(PACKET_C2ZA_SELECT_ROLE* pktIn)
{

    USES_CONVERSION;
    LOGI(_T("========C2ZA_PROTO_1106_SELECT_ROLE======\n"));
    LOGI(_T("Name: %s !\n"), A2T(pktIn->name));


    PACKET_ZA2C_CONFIRM_ROLE* pkt = (PACKET_ZA2C_CONFIRM_ROLE*)bufSend_;

    ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_CONFIRM_ROLE));
    pkt->Header.Size = sizeof(PACKET_ZA2C_CONFIRM_ROLE);
    pkt->Header.Ctrl = 0x3;
    pkt->Header.Cmd = 0xFF;
    pkt->Header.Uid = 0;
    pkt->Header.Protocol = ZA2C_PROTO_1106_CONFIRM_ROLE;

    StringCchCopyA(pkt->name, 29, pktIn->name);

    encode(bufSend_, sizeof(PACKET_ZA2C_CONFIRM_ROLE));
    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_CONFIRM_ROLE));

    return S_OK;

}


HRESULT ZoneAgentSession::OnWorldLogin(PACKET_C2ZA_WORLD_LOGIN* pktIn)
{

    USES_CONVERSION;
    LOGI(_T("========C2ZA_PROTO_1107_WORLD_LOGIN======\n"));
    LOGI(_T("Name: %s !\n"), A2T(pktIn->name));
    LOGI(_T("MapIndex: %d\n"), pktIn->mapindex);
    LOGI(_T("Class type: %d\n"), pktIn->classtype);

    PACKET_ZA2C_WORLD_LOGIN_RESP* pkt = (PACKET_ZA2C_WORLD_LOGIN_RESP*)bufSend_;

    ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));
    pkt->Header.Size = sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP);
    pkt->Header.Ctrl = 0x3;
    pkt->Header.Cmd = 0xFF;
    pkt->Header.Uid = 0;
    pkt->Header.Protocol = ZA2C_PROTO_1107_WORLD_LOGIN_RESP;

    // Role info
    pkt->unknown1 = 1;
    pkt->unknown2 = 1;
    pkt->unknown3 = 1;
    pkt->MP = 11;
    pkt->Str = 200;
    pkt->Magic = 61;
    pkt->Dex = 46;
    pkt->Vit = 50;
    pkt->Mana = 65;
    pkt->MaxHPStore = 200000;
    pkt->MaxMPStore = 20000;
    pkt->HP = 900;
    pkt->Point = 800;
    pkt->MinAttack = 100;
    pkt->MinMagicAttack = 500;
    pkt->Defense = 200;
    pkt->FireAttack = 100;
    pkt->FireDefense= 200;
    pkt->IceAttack = 200;
    pkt->IceDefense = 300;
    pkt->LightningAttack = 300;
    pkt->LightningDefense = 400;
    pkt->MaxHPBar = 1000;
    pkt->MaxMPBar = 500;
    pkt->MaxAttack = 200;
    pkt->MaxMagicAttack = 1000;

    pkt->Sinfo = 1025;
    pkt->Woonz = 1000000000;
    pkt->MPStore = 5000;
    pkt->HPStore = 180000;
    pkt->LorePoint = 5000;
    StringCchCopyA(pkt->CharacterName, 21, pktIn->name);
    pkt->Level = 165;
    pkt->Exp = 4220806300;
    pkt->MapIndex = 37;         // TODO: why this value doesn't work?
    pkt->MapCell = MAKEWORD(cx_, cy_);

    for (int i = 0; i < 24; ++i)
    {
        pkt->Skill[i] = 0xFF;
    }


    if (0 == lstrcmpA(pktIn->name, "Warrior"))
    {
        pkt->Class = 0;
        OnWorldLoginWarrior();
    }
    else if (0 == lstrcmpA(pktIn->name, "Knight"))
    {
        pkt->Class = 1;
        OnWorldLoginKnight();
    }
    else if (0 == lstrcmpA(pktIn->name, "Mage"))
    {
        pkt->Class = 2;
        OnWorldLoginMage();
    }
    else if (0 == lstrcmpA(pktIn->name, "Archer"))
    {
        pkt->Class = 3;
        OnWorldLoginArcher();
    }
    else if (0 == lstrcmpA(pktIn->name, "GM"))
    {
        pkt->Class = 2;
        OnWorldLoginGM();
    }





    return S_OK;

}


HRESULT ZoneAgentSession::OnWorldLoginWarrior()
{
    PACKET_ZA2C_WORLD_LOGIN_RESP* pkt = (PACKET_ZA2C_WORLD_LOGIN_RESP*)bufSend_;

    struct CharaInvItem
    {
        uint32_t pid;
        uint32_t code;
        uint32_t prop;
        uint32_t sn;
        uint32_t index;
    };

    uint32_t code[] =
    {
        233133, // kni hog armor
        233134, // kni hog pant
        233135, // Kni hog boot
        233136, // Kni hog Gloves
        233137, // Kni Hog Helmet
        230447, // 10J
        230442,
        230437,
        230522, // 9T
        230523,
        230524,
        230513, // 8T
        230515,
        230514,
    };

    CharaInvItem* cii = (CharaInvItem*)pkt->CharacterInventory;
    for (int i = 0; i < ARRAYSIZE(code); ++i)
    {
        cii[i].pid = 100 + i;
        cii[i].code = code[i];
        cii[i].prop = 0xFFFFCC3A;
        cii[i].sn = i;
        cii[i].index = i;
    }

    CharaInvItem* ciw = (CharaInvItem*)pkt->WearList;

    int i = 2; // index 0 = wappen         
    ciw[i].pid = 100 + i;
    ciw[i].code = 230524;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    //i = 1; // index 1 = wappen            
    //ciw[i].pid = 100 + i;
    //ciw[i].code = 230522; 
    //ciw[i].prop = 0xFFFFCC3A;
    //ciw[i].sn = 200 + i;
    //ciw[i].index = i;

    //i = 2; 
    //ciw[i].pid = 100 + i;
    //ciw[i].code = 233137;
    //ciw[i].prop = 0xFFFFFFFF;
    //ciw[i].sn = 200 + i;
    //ciw[i].index = i;

    i = 3;  // Helmet 
    ciw[i].pid = 100 + i;
    ciw[i].code = 233137;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 4;  // armor
    ciw[i].pid = 100 + i;
    ciw[i].code = 233133;
    ciw[i].prop = 0xFFFFFFFF;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 5; // pant
    ciw[i].pid = 100 + i;
    ciw[i].code = 233134;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 6; // Gloves
    ciw[i].pid = 100 + i;
    ciw[i].code = 233136;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 7; // boot
    ciw[i].pid = 100 + i;
    ciw[i].code = 233135;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 8; // neck
    ciw[i].pid = 100 + i;
    ciw[i].code = 4151;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 9; //ring
    ciw[i].pid = 100 + i;
    ciw[i].code = 5176;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    encode(bufSend_, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));

    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));


    return S_OK;
}
HRESULT ZoneAgentSession::OnWorldLoginKnight()
{

    PACKET_ZA2C_WORLD_LOGIN_RESP* pkt = (PACKET_ZA2C_WORLD_LOGIN_RESP*)bufSend_;


    struct CharaInvItem
    {
        uint32_t pid;
        uint32_t code;
        uint32_t prop;
        uint32_t sn;
        uint32_t index;
    };

    uint32_t code[] =
    {
        233157,// Blue Dragon Armor
        233158,// Blue Dragon Pant
        233159,// Blue Dragon Boot
        233160,// Blue Dragon Gloves
        233161,// Blue Dragon Helmet
    };

    CharaInvItem* cii = (CharaInvItem*)pkt->CharacterInventory;
    for (int i = 0; i < ARRAYSIZE(code); ++i)
    {
        cii[i].pid = 100 + i;
        cii[i].code = code[i];
        cii[i].prop = 0xFFFFCC3A;
        cii[i].sn = i;
        cii[i].index = i;
    }

    CharaInvItem* ciw = (CharaInvItem*)pkt->WearList;

    int i = 0; // index 0 = sheild, arrow            
    //ciw[i].pid = 100;
    //ciw[i].code = 233161;
    //ciw[i].prop = 0xFFFFCC3A;
    //ciw[i].sn = 0;
    //ciw[i].index = 0;

    i = 1; // index 1 = wappen            
    ciw[i].pid = 100 + i;
    ciw[i].code = 230527;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    //i = 2; 
    //ciw[i].pid = 100 + i;
    //ciw[i].code = 233157;
    //ciw[i].prop = 0xFFFFFFFF;
    //ciw[i].sn = 200 + i;
    //ciw[i].index = i;

    i = 3;
    ciw[i].pid = 100 + i;
    ciw[i].code = 233161;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 4;
    ciw[i].pid = 100 + i;
    ciw[i].code = 233157;
    ciw[i].prop = 0xFFFFFFFF;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 5;
    ciw[i].pid = 100 + i;
    ciw[i].code = 233158;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 6;
    ciw[i].pid = 100 + i;
    ciw[i].code = 233160;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 7;
    ciw[i].pid = 100 + i;
    ciw[i].code = 233159;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 8;
    ciw[i].pid = 100 + i;
    ciw[i].code = 4151;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 9;
    ciw[i].pid = 100 + i;
    ciw[i].code = 5176;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    encode(bufSend_, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));

    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));


    return S_OK;
}
HRESULT ZoneAgentSession::OnWorldLoginMage()
{

    PACKET_ZA2C_WORLD_LOGIN_RESP* pkt = (PACKET_ZA2C_WORLD_LOGIN_RESP*)bufSend_;

    struct CharaInvItem
    {
        uint32_t pid;
        uint32_t code;
        uint32_t prop;
        uint32_t sn;
        uint32_t index;
    };

    uint32_t code[] =
    {
        233169, // Sapphire Armor
        233170, // Sapphire Pant
        233171, // Sapphire Boot
        233172, // Sapphire Gloves
        233173, // Sapphire Helmet
    };

    CharaInvItem* cii = (CharaInvItem*)pkt->CharacterInventory;
    for (int i = 0; i < ARRAYSIZE(code); ++i)
    {
        cii[i].pid = 100 + i;
        cii[i].code = code[i];
        cii[i].prop = 0xFFFFCC3A;
        cii[i].sn = i;
        cii[i].index = i;
    }

    CharaInvItem* ciw = (CharaInvItem*)pkt->WearList;

    int i = 2; // index 2 = wappen         
    ciw[i].pid = 100 + i;
    ciw[i].code = 231550;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    //i = 1; // index 1 = wappen            
    //ciw[i].pid = 100 + i;
    //ciw[i].code = 230522; 
    //ciw[i].prop = 0xFFFFCC3A;
    //ciw[i].sn = 200 + i;
    //ciw[i].index = i;

    //i = 2; 
    //ciw[i].pid = 100 + i;
    //ciw[i].code = 233137;
    //ciw[i].prop = 0xFFFFFFFF;
    //ciw[i].sn = 200 + i;
    //ciw[i].index = i;

    i = 3;  // Helmet
    ciw[i].pid = 100 + i;
    ciw[i].code = 233173;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 4;  // armor
    ciw[i].pid = 100 + i;
    ciw[i].code = 233169 ;
    ciw[i].prop = 0xFFFFFFFF;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 5; // pant
    ciw[i].pid = 100 + i;
    ciw[i].code = 233170;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 6; // Gloves
    ciw[i].pid = 100 + i;
    ciw[i].code = 233172;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 7; // boot
    ciw[i].pid = 100 + i;
    ciw[i].code = 233171;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 8; // neck
    ciw[i].pid = 100 + i;
    ciw[i].code = 4151;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 9; //ring
    ciw[i].pid = 100 + i;
    ciw[i].code = 5176;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    encode(bufSend_, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));

    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));

    return S_OK;
}
HRESULT ZoneAgentSession::OnWorldLoginArcher()
{
 

    PACKET_ZA2C_WORLD_LOGIN_RESP* pkt = (PACKET_ZA2C_WORLD_LOGIN_RESP*)bufSend_;

    struct CharaInvItem
    {
        uint32_t pid;
        uint32_t code;
        uint32_t prop;
        uint32_t sn;
        uint32_t index;
    };

    uint32_t code[] =
    {
        233145, // Lu Place Armor
        233146, // Lu Place Pant
        233147, // Lu Place Boot
        233148, // Lu Place Gloves
        233149, // Lu Place Helmet
        // 9T
        230530,
        230529,
        // 8T
        230520,
        230521
    };

    CharaInvItem* cii = (CharaInvItem*)pkt->CharacterInventory;
    for (int i = 0; i < ARRAYSIZE(code); ++i)
    {
        cii[i].pid = 100 + i;
        cii[i].code = code[i];
        cii[i].prop = 0xFFFFCC3A;
        cii[i].sn = i;
        cii[i].index = i;
    }

    CharaInvItem* ciw = (CharaInvItem*)pkt->WearList;

    int i = 1; // index 1 = wappen         
    ciw[i].pid = 100 + i;
    ciw[i].code = 230530;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    //i = 1; // index 1 = wappen            
    //ciw[i].pid = 100 + i;
    //ciw[i].code = 230522; 
    //ciw[i].prop = 0xFFFFCC3A;
    //ciw[i].sn = 200 + i;
    //ciw[i].index = i;

    //i = 2; 
    //ciw[i].pid = 100 + i;
    //ciw[i].code = 233137;
    //ciw[i].prop = 0xFFFFFFFF;
    //ciw[i].sn = 200 + i;
    //ciw[i].index = i;

    i = 3;  // Helmet
    ciw[i].pid = 100 + i;
    ciw[i].code = 233149;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 4;  // armor
    ciw[i].pid = 100 + i;
    ciw[i].code = 233145;
    ciw[i].prop = 0xFFFFFFFF;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 5; // pant
    ciw[i].pid = 100 + i;
    ciw[i].code = 233146;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 6; // Gloves
    ciw[i].pid = 100 + i;
    ciw[i].code = 233148;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 7; // boot
    ciw[i].pid = 100 + i;
    ciw[i].code = 233147;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 8; // neck
    ciw[i].pid = 100 + i;
    ciw[i].code = 4151;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 9; //ring
    ciw[i].pid = 100 + i;
    ciw[i].code = 5176;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    encode(bufSend_, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));

    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));

    return S_OK;
}


HRESULT ZoneAgentSession::OnWorldLoginGM()
{
    PACKET_ZA2C_WORLD_LOGIN_RESP* pkt = (PACKET_ZA2C_WORLD_LOGIN_RESP*)bufSend_;

    struct CharaInvItem
    {
        uint32_t pid;
        uint32_t code;
        uint32_t prop;
        uint32_t sn;
        uint32_t index;
    };

    uint32_t code[] =
    {
        233133, // kni hog armor
        233134, // kni hog pant
        233135, // Kni hog boot
        233136, // Kni hog Gloves
        233137, // Kni Hog Helmet
    };

    CharaInvItem* cii = (CharaInvItem*)pkt->CharacterInventory;
    for (int i = 0; i < ARRAYSIZE(code); ++i)
    {
        cii[i].pid = 100 + i;
        cii[i].code = code[i];
        cii[i].prop = 0xFFFFCC3A;
        cii[i].sn = i;
        cii[i].index = i;
    }

    CharaInvItem* ciw = (CharaInvItem*)pkt->WearList;

    int i = 2; // index 0 = wappen         
    ciw[i].pid = 100 + i;
    ciw[i].code = 231550;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    //i = 1; // index 1 = wappen            
    //ciw[i].pid = 100 + i;
    //ciw[i].code = 230522; 
    //ciw[i].prop = 0xFFFFCC3A;
    //ciw[i].sn = 200 + i;
    //ciw[i].index = i;

    //i = 2; 
    //ciw[i].pid = 100 + i;
    //ciw[i].code = 233137;
    //ciw[i].prop = 0xFFFFFFFF;
    //ciw[i].sn = 200 + i;
    //ciw[i].index = i;

    i = 3;  // armor
    ciw[i].pid = 100 + i;
    ciw[i].code = 3650;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 4;  // Helmet
    ciw[i].pid = 100 + i;
    ciw[i].code = 3649;
    ciw[i].prop = 0xFFFFFFFF;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    i = 5; // pant
    ciw[i].pid = 100 + i;
    ciw[i].code = 3652;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 6; // Gloves
    ciw[i].pid = 100 + i;
    ciw[i].code = 3653;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 7; // boot
    ciw[i].pid = 100 + i;
    ciw[i].code = 3651;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 8; // neck
    ciw[i].pid = 100 + i;
    ciw[i].code = 4151;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;

    i = 9; //ring
    ciw[i].pid = 100 + i;
    ciw[i].code = 5176;
    ciw[i].prop = 0xFFFFCC3A;
    ciw[i].sn = 200 + i;
    ciw[i].index = i;


    encode(bufSend_, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));

    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_WORLD_LOGIN_RESP));

    return S_OK;
}



HRESULT ZoneAgentSession::OnRoleMove(PACKET_C2ZA_ROLE_MOVE* pktIn)
{

    USES_CONVERSION;
    LOGI(_T("========C2ZA_PROTO_1200_ROLE_MOVE======\n"));
    LOGI(_T("TO: %d %d \n"), pktIn->destX, pktIn->destY);


    PACKET_ZA2C_ROLE_MOVE_RESP* pkt = (PACKET_ZA2C_ROLE_MOVE_RESP*)bufSend_;

    ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_ROLE_MOVE_RESP));
    pkt->Header.Size = sizeof(PACKET_ZA2C_ROLE_MOVE_RESP);
    pkt->Header.Ctrl = 0x3;
    pkt->Header.Cmd = 0xFF;
    pkt->Header.Uid = 0;
    pkt->Header.Protocol = ZA2C_PROTO_1200_ROLE_MOVE_RESP;

    pkt->destX = pktIn->destX;
    pkt->destY = pktIn->destY;

    pkt->unknown1 = 1;
    pkt->unknown2 = 1;



    encode(bufSend_, sizeof(PACKET_ZA2C_ROLE_MOVE_RESP));
    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_ROLE_MOVE_RESP));

    return S_OK;

}


HRESULT ZoneAgentSession::OnAttackMonster(PACKET_C2ZA_ATTACK_MONSTER* pktIn)
{
    USES_CONVERSION;
    LOGI(_T("========C2ZA_PROTO_1400_ATTACK_MONSTER======\n"));
    LOGI(_T("SN: %d ID: %d \n"), pktIn->sn, pktIn->id);


    PACKET_ZA2C_ATTACK_MONSTER_RESP* pkt = (PACKET_ZA2C_ATTACK_MONSTER_RESP*)bufSend_;

    ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_ATTACK_MONSTER_RESP));
    pkt->Header.Size = sizeof(PACKET_ZA2C_ATTACK_MONSTER_RESP);
    pkt->Header.Ctrl = 0x3;
    pkt->Header.Cmd = 0xFF;
    pkt->Header.Uid = 0;
    pkt->Header.Protocol = ZA2C_PROTO_1400_ATTACK_MONSTER_RESP;

    pkt->cx = cx_;
    pkt->cy = cy_ ;

    pkt->mx = 95;  // not used ?
    pkt->my = 77;
    static uint8_t cnt = 0;
    cnt++;


    pkt->fix2 = 2; 
    pkt->count = cnt;
    if (cnt == 255)
    {
        pkt->status = MOSNTER_STATUS_DEAD;
        cnt = 0;
    }
    else
        pkt->status = MOSNTER_STATUS_ALIVE;


    pkt->unused1[0] = cnt;
    pkt->unused1[1] = cnt;


    pkt->unused2[0] = cnt;
    pkt->unused2[1] = cnt;


    pkt->cmp = 400;

    pkt->sn = pktIn->sn;
    pkt->id = pktIn->id;

    pkt->damage = cnt;
    pkt->chp = 800;

    pkt->mhpp = 255 - cnt;
    pkt->unk0[0] = cnt;
    pkt->unk0[1] = cnt;
    pkt->unk0[2] = cnt;
    pkt->unk0[3] = cnt;
    pkt->unk0[4] = cnt;
    pkt->unk1 = cnt;
    pkt->unk2 = cnt;



    encode(bufSend_, sizeof(PACKET_ZA2C_ATTACK_MONSTER_RESP));
    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_ATTACK_MONSTER_RESP));



    return S_OK;
}

HRESULT ZoneAgentSession::OnExitGame()
{
    LOGI(_T("========C2ZA_PROTO_1461_EXIT_GAME======\n"));
    WorldExit();
    return S_OK;
}



HRESULT ZoneAgentSession::OnMessage(PACKET_C2ZA_MESSAGE* pktIn)
{
    LOGI(_T("========C2ZA_PROTO_1800_MESSAGE======\n"));
    LOGI(_T("Type: %d\n"), pktIn->Type);

    if (pktIn->Type == 0x800) // chat window message with ! prefix
    {
        USES_CONVERSION;
        LOGI(_T("MSG: %s\n"), A2T(pktIn->message));
        ProcessCommand(pktIn->message);


    }

    return S_OK;
}


HRESULT ZoneAgentSession::ProcessCommand(char* cmd)
{
    HRESULT hr = S_OK;
    char args[10][16] = {0};

    // parse the command
    int c = 0;
    int i = 0;
    int j = 0;
    char* p = cmd;
    if (lstrlenA(p))
        c = 1;
    while (*p)
    {
        if (*p == ' ')
        {
            while (*p == ' ')
                p++;

            i++;
            j = 0;
            c++;
        }

        args[i][j++] = *p;
        p++;
    }


    // dispatch command
    if (!lstrcmpA(args[0], "echo"))
    {
        if (lstrlenA(args[1]))
            SendMessage(0,0,args[1]);
    }
    else if (!lstrcmpA(args[0], "msg"))
    {
        uint32_t tag0 = StrToIntA(args[1]);
        uint16_t tag1 = StrToIntA(args[2]);
        if (lstrlenA(args[3]))
            SendMessage(tag0, tag1, args[3]);
    }
    else if (!lstrcmpA(args[0], "monster"))
    {
        if (!lstrcmpA(args[1], "new") )
        {
            DWORD id = StrToIntA(args[2]);
            WORD level = StrToIntA(args[3]);
            BYTE cx = StrToIntA(args[4]);
            BYTE cy = StrToIntA(args[5]);
            MonsterNew(id, level, cx, cy);       
        }
    }

    return hr;
}


HRESULT ZoneAgentSession::SendMessage(uint32_t tag0, uint16_t tag1, char* message)
{
    HRESULT hr = S_OK;
    PACKET_ZA2C_MESSAGE* pkt = (PACKET_ZA2C_MESSAGE*)bufSend_;
    ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_MESSAGE));
    pkt->Header.Size = sizeof(PACKET_ZA2C_MESSAGE);
    pkt->Header.Ctrl = 0x3;
    pkt->Header.Cmd = 0xFF;
    pkt->Header.Uid = 0;
    pkt->Header.Protocol = ZA2C_PROTO_1800_MESSAGE;


    pkt->tag0 = tag0;
    pkt->tag1 = tag1;
    StrCpyA(pkt->message, message);

    
    encode(bufSend_, sizeof(PACKET_ZA2C_MESSAGE));
    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_MESSAGE));


    return hr;

}



HRESULT ZoneAgentSession::MonsterNew(DWORD dwID, WORD wLevel, BYTE cx, BYTE cy)
{

    PACKET_ZA2C_MOSNTER_NEW* pkt = (PACKET_ZA2C_MOSNTER_NEW*)bufSend_;


    ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_MOSNTER_NEW));
    pkt->Header.Size = sizeof(PACKET_ZA2C_MOSNTER_NEW);
    pkt->Header.Ctrl = 0x3;
    pkt->Header.Cmd = 0xFF;
    pkt->Header.Uid = 0;
    pkt->Header.Protocol = ZA2C_PROTO_1300_MOSNTER_NEW;

    pkt->Fix1[0] = 0x11111111; // seems not used
    pkt->Fix1[1] = 0x11111111;  // seems not used
    pkt->FixF[0] = 0xFF; // seems not used
    pkt->FixF[1] = 0xFF; // seems not used
    pkt->FixF[2] = 0xFF; // seems not used
    pkt->Fix0[0] = 0x00; // seems not used
    pkt->Fix0[1] = 0x11; // seems not used
    pkt->Fix0[2] = 0x22; // seems not used
    pkt->Fix0[3] = 0x33; // seems not used
    pkt->Fix0[4] = 0x44; // seems not used

    pkt->ID = dwID;
    for (int i = 0; i < 10; ++i)
    {
        pkt->Unk[3 * i] = 0xFF;
        pkt->Unk[3 * i+1] = 0xCD;
    }
    pkt->Unk1 = 0xFF3F;
    pkt->Level = wLevel;
    pkt->sn = 1024;
    static uint16_t id = 1;
    pkt->id = id++;
    pkt->X = cx;
    pkt->Y = cy;

    //pkt->Unk2[0] = 0xab;
    //pkt->Unk2[1] = 0xcd;
    //pkt->Unk2[2] = 0xef;

    //pkt->Fix0000[0] = 0x1234abcd;
    //pkt->Fix0000[1] = 0x5678cdef;
    //pkt->Fix00 = 0xbaef;


    encode(bufSend_, sizeof(PACKET_ZA2C_MOSNTER_NEW));
    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_MOSNTER_NEW));

    return S_OK;
}

HRESULT ZoneAgentSession::WorldExit()
{
    PACKET_ZA2C_WORLD_EXIT* pkt = (PACKET_ZA2C_WORLD_EXIT*)bufSend_;


    ZeroMemory(bufSend_, sizeof(PACKET_ZA2C_WORLD_EXIT));
    pkt->Header.Size = sizeof(PACKET_ZA2C_WORLD_EXIT);
    pkt->Header.Ctrl = 0x3;
    pkt->Header.Cmd = 0xFF;
    pkt->Header.Uid = 0;
    pkt->Header.Protocol = ZA2C_PROTO_1108_WORLD_EXIT;

    encode(bufSend_, sizeof(PACKET_ZA2C_WORLD_EXIT));
    SendData((BYTE*)pkt, sizeof(PACKET_ZA2C_WORLD_EXIT));

    return S_OK;
}

