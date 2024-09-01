#pragma once


enum LOG_LEVEL
{
    LOG_FATAL = 0,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_VERBOSE
};

HRESULT LogPrintf(LOG_LEVEL level, const TCHAR* fmt, ...);
