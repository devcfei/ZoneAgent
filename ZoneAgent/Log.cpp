#include "framework.h"

HRESULT LogPrintf(LOG_LEVEL level, const TCHAR* fmt, ...)
{
    HRESULT hr;
    TCHAR szMessageBuffer[1024] = { 0 };
    va_list valist;

    va_start(valist, fmt);
    hr = StringCchVPrintf(szMessageBuffer, 1024, fmt, valist);
    if (FAILED(hr))
    {
        return hr;
    }
    va_end(valist);

    OutputDebugString(szMessageBuffer);

    return hr;
}
