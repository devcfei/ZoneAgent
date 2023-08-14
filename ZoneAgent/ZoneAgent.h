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

    virtual HRESULT ProcessEvent(size_t len);
    virtual HRESULT OnClose();
private:
    BYTE* bufSend_;
    size_t bufSendLen_;
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