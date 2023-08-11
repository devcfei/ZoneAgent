#pragma once

enum EVENT_ID
{
    EVENT_ID_NULL
};

class IEvent
{
public:
    virtual HRESULT NotifyEvent(EVENT_ID eid, WPARAM wParam, LPARAM lParam) = 0;
};