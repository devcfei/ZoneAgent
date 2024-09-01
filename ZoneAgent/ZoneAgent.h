#pragma once



class ZoneAgentSession : public Session
{
public:
    ZoneAgentSession()
        :Session()
    {
        bufSendLen_ = 4096;
        bufSend_ = new BYTE[bufSendLen_];
    }
    virtual ~ZoneAgentSession()
    {
        delete[] bufSend_;
    }

    virtual HRESULT ProcessEvent(void* pbuff, size_t len);
    virtual HRESULT OnClose();

    // ProcessPacket

private:
    BYTE* bufSend_;
    size_t bufSendLen_;


    HRESULT OnSelectRole(PACKET_C2ZA_SELECT_ROLE* pktIn);
    HRESULT OnWorldLogin(PACKET_C2ZA_WORLD_LOGIN* pktIn);
    HRESULT OnWorldLoginWarrior();
    HRESULT OnWorldLoginKnight();
    HRESULT OnWorldLoginMage();
    HRESULT OnWorldLoginArcher();
    HRESULT OnWorldLoginGM();
    HRESULT OnRoleMove(PACKET_C2ZA_ROLE_MOVE* pktIn);
    HRESULT OnAttackMonster(PACKET_C2ZA_ATTACK_MONSTER* pktIn);
    HRESULT OnExitGame();
    HRESULT OnMessage(PACKET_C2ZA_MESSAGE* pktIn);
    HRESULT ProcessCommand(char* cmd);


    HRESULT SendMessage(uint32_t tag0, uint16_t tag1, char* message);
    HRESULT MonsterNew(DWORD dwID, WORD wLevel, BYTE cx, BYTE cy);
    HRESULT WorldExit();


private:
    BYTE cx_ = 93; // Character postion X
    BYTE cy_ = 75; // Character postion Y

};


class ZoneAgent : public LibEvent
{
public:
    ZoneAgent();
    ~ZoneAgent();

    HRESULT Initialize(LPCTSTR lpszCfgFile);

    HRESULT Start();
    HRESULT Stop();


private:
    HRESULT InitializeDefaultConfig();
    HRESULT ReadConfig(LPCTSTR lpszCfgFile);
    HRESULT WriteConfig(LPCTSTR lpszCfgFile);

private:
    // config file
    // STARTUP
    UINT nServerID_;
    UINT nAgentID_;
    TCHAR szZAIP_[16];
    WORD wZAPort_;


private:
    DWORD dwUserCount_ = 0;
    virtual HRESULT OnConnect(Session** ppSession)
    {
        HRESULT hr = S_OK;

        Session* pSession = new ZoneAgentSession;
        if (!pSession)
        {
            hr = E_OUTOFMEMORY;
            return hr;
        }

        *ppSession = pSession;


        return hr;
    }

    virtual HRESULT OnDisconnect(Session* pSession)
    {
        HRESULT hr = S_OK;

        delete pSession;

        return hr;
    }

};