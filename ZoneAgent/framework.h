// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <strsafe.h>
#include <shlobj.h>
#include <shlwapi.h>

// ATL
#include <atlconv.h>

// Socket
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdarg.h>

// STL
#include <vector>
#include <list>
#include <string>
namespace std
{

#if defined(_UNICODE)
    typedef wstring tstring;
#else
    typedef string tstring;
#endif
};


// libevent
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>





// Resources
#include "resource.h"



// Private modules
#include "Event.h"
#include "Utils.h"
#include "Log.h"
#include "Conn.h"
#include "Packet.h"
#include "ZoneAgent.h"
#include "App.h"


// Macro
#define LOGF(...) LogPrintf(LOG_FATAL,__VA_ARGS__)
#define LOGE(...) LogPrintf(LOG_ERROR,__VA_ARGS__)
#define LOGW(...) LogPrintf(LOG_WARNING,__VA_ARGS__)
#define LOGI(...) LogPrintf(LOG_INFO,__VA_ARGS__)
#define LOGV(...) LogPrintf(LOG_VERBOSE,__VA_ARGS__)


// global function
extern App* GetApp();

// manifest
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


// Library
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Ws2_32.lib")

// libevent
#pragma comment(lib, "event.lib")
#pragma comment(lib, "event_core.lib")
#pragma comment(lib, "event_extra.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "bcrypt.lib")