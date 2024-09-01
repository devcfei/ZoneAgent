#pragma once



class Session
{
public:
    Session()
    {
        len_ = 4096;
        pdata_ = new BYTE[len_];
    }
    virtual ~Session()
    {
        delete[] pdata_;
    }


    void setbufferevent(struct bufferevent* ev)
    {
        bev = ev;
    }

    void Shutdown()
    {
        bufferevent_free(bev);
    }

    virtual HRESULT ProcessEvent(void* pbuff, size_t len)
    {
        return S_OK;
    }

    virtual HRESULT OnClose()
    {
        return S_OK;
    }

    void* GetBuffer() { return pdata_; }
    size_t GetBufferSize() { return len_; }

    HRESULT SendData(BYTE* pdata, size_t len)
    {
        HRESULT hr = S_OK;
        bufferevent_write(bev, pdata, len);
        return hr;
    }



protected:
    BYTE* pdata_;
    size_t len_;

private:
    struct bufferevent* bev;
};

class LibEvent
{
public:
    LibEvent();
    ~LibEvent();
    HRESULT Initialize(WORD wPort);
    HRESULT Start();
    HRESULT Stop();


    virtual HRESULT OnConnect(Session** ppSession);
    virtual HRESULT OnDisconnect(Session* pSession);

private:
    WORD wPort_;

    HANDLE hThreadWorker_;
    DWORD dwThreadID_;
    static DWORD WINAPI WorkerThreadProc(LPVOID lpParam);
    DWORD WorkerThread();

private:
    // libevent 

    struct event_base* base;
    struct evconnlistener* listener;
    struct event* signal_event;

    static void listener_cb(struct evconnlistener*, evutil_socket_t,
        struct sockaddr*, int socklen, void*);
    static void signal_cb(evutil_socket_t, short, void*);

    static void conn_readcb(struct bufferevent*, void*);
    static void conn_writecb(struct bufferevent*, void*);
    static void conn_eventcb(struct bufferevent*, short, void*);

};