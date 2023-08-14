#pragma once

class ZoneAgent
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

};