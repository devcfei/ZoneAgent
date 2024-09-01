#include "framework.h"



LibEvent::LibEvent()
{

}

LibEvent::~LibEvent()
{

}

HRESULT LibEvent::Initialize(WORD wPort)
{
    HRESULT hr = S_OK;

    wPort_ = wPort;

    // WSAStartup
    WSADATA wsaData;
    if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return hr;
}

HRESULT LibEvent::Start()
{
    HRESULT hr = S_OK;

    hThreadWorker_ = CreateThread(NULL, 0, WorkerThreadProc, this, 0, &dwThreadID_);
    if (hThreadWorker_ == NULL) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        LOGF(_T("CreateThread failed! hr = %08X"), hr);
        return hr;

    }


    return hr;
}


HRESULT LibEvent::Stop()
{
    HRESULT hr = S_OK;

    return hr;
}




HRESULT LibEvent::OnConnect(Session** ppSession)
{
    HRESULT hr = S_OK;

    Session* pSession = new Session;
    if (!pSession)
    {
        hr = E_OUTOFMEMORY;
        return hr;
    }

    *ppSession = pSession;

    return hr;
}

HRESULT LibEvent::OnDisconnect(Session* pSession)
{
    HRESULT hr = S_OK;

    delete pSession;

    return hr;
}



DWORD WINAPI LibEvent::WorkerThreadProc(LPVOID lpParam)
{
    LibEvent* pThis = (LibEvent*)lpParam;
    return pThis->WorkerThread();
}

DWORD LibEvent::WorkerThread()
{
    base = event_base_new();
    if (!base) {
        fprintf(stderr, "Could not initialize libevent!\n");
        return 1;
    }

    struct sockaddr_in sin = { 0 };

    sin.sin_family = AF_INET;
    sin.sin_port = htons(wPort_);

    listener = evconnlistener_new_bind(base, listener_cb, (void*)this,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
        (struct sockaddr*)&sin,
        sizeof(sin));

    if (!listener) {
        fprintf(stderr, "Could not create a listener!\n");
        return 1;
    }

    signal_event = evsignal_new(base, SIGINT, signal_cb, (void*)this);

    if (!signal_event || event_add(signal_event, NULL) < 0) {
        fprintf(stderr, "Could not create/add a signal event!\n");
        return 1;
    }

    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_free(signal_event);
    event_base_free(base);

    return 0;
}



void LibEvent::listener_cb(struct evconnlistener* listener, evutil_socket_t fd,
    struct sockaddr* sa, int socklen, void* user_data)
{
    LibEvent* pThis = (LibEvent*)user_data;
    struct event_base* base = pThis->base;
    struct bufferevent* bev;

    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev) {
        fprintf(stderr, "Error constructing bufferevent!");
        event_base_loopbreak(base);
        return;
    }

    Session* p = nullptr;

    pThis->OnConnect(&p);

    bufferevent_setcb(bev, conn_readcb, NULL, conn_eventcb, p);
    bufferevent_enable(bev, EV_WRITE);
    bufferevent_enable(bev, EV_READ);

    //bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

void LibEvent::conn_readcb(struct bufferevent* bev, void* user_data)
{
    struct evbuffer* input = bufferevent_get_input(bev);


    Session* pSession = (Session*)user_data;
    pSession->setbufferevent(bev);

    size_t sz = evbuffer_get_length(input);
    if (sz == 0)
    {
        printf("flushed answer\n");
        bufferevent_free(bev);
    }
    size_t Length = 0;
    size_t count = 0;
    DWORD* lenPacket = 0;
    void* buff = pSession->GetBuffer();

    size_t last = sz;
    size_t plen = 0;
    size_t plast = 0;
    BYTE* pbuff = static_cast<BYTE*>(buff);

    while (last > 4)
    {
        plen = bufferevent_read(bev, static_cast<BYTE*>(buff) + count, 4);
        lenPacket = reinterpret_cast<DWORD*>(pbuff);
        Length = *lenPacket;
        last -= plen;
        count += plen;
        plast = Length - plen;
        if (plast <= last)
        {
            plen = bufferevent_read(bev, static_cast<BYTE*>(buff) + count, plast);
            pSession->ProcessEvent(pbuff, Length);
            last -= plen;
            count += plen;
            pbuff += Length;
        }
        else
        {
            break;
        }
    }


}

void LibEvent::conn_writecb(struct bufferevent* bev, void* user_data)
{
    struct evbuffer* output = bufferevent_get_output(bev);
    if (evbuffer_get_length(output) == 0) {
        printf("flushed answer\n");
        bufferevent_free(bev);
    }
}

void LibEvent::conn_eventcb(struct bufferevent* bev, short events, void* user_data)
{

    Session* pSession = (Session*)user_data;

    if (events & BEV_EVENT_EOF) {
        printf("Connection closed.\n");
    }
    else if (events & BEV_EVENT_ERROR) {
        //printf("Got an error on the connection: %s\n",
        //    strerror(errno));/*XXX win32*/

        printf("Got an error on the connection\n");/*XXX win32*/
    }
    /* None of the other events can happen here, since we haven't enabled
     * timeouts */
    bufferevent_free(bev);

    pSession->OnClose();
}

void LibEvent::signal_cb(evutil_socket_t sig, short events, void* user_data)
{
    LibEvent* pThis = (LibEvent*)user_data;

    struct event_base* base = pThis->base;
    struct timeval delay = { 2, 0 };

    printf("Caught an interrupt signal; exiting cleanly in two seconds.\n");

    event_base_loopexit(base, &delay);
}
