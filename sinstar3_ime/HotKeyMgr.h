#pragma once

#include <map>

class CHotKeyMgr
{
	typedef struct tagHotKey
	{
		GUID id;
		PRESERVEDKEY key; 
	}HOTKEY;

public:
	CHotKeyMgr(void);
	~CHotKeyMgr(void);
	
	std::map<PRESERVEDKEY,GUID> m_mapHotKeys;
	BOOL RegisterHotKey(REFGUID guid, const PRESERVEDKEY * pKey);
	BOOL UnregisterHotKey(REFGUID guid,const PRESERVEDKEY * pKey);
	
	BOOL ProcessHotKey(UINT vKey,const LPBYTE lpKeyState,GUID *pGuid=NULL);
};
