#include "StdAfx.h"
#include "HotKeyMgr.h"

bool operator <(const PRESERVEDKEY &key1,const PRESERVEDKEY & key2)
{
	return memcmp(&key1,&key2,sizeof(PRESERVEDKEY))<0;
}

CHotKeyMgr::CHotKeyMgr(void)
{
}

CHotKeyMgr::~CHotKeyMgr(void)
{
	m_mapHotKeys.clear();
}



BOOL CHotKeyMgr::RegisterHotKey(REFGUID guid, const PRESERVEDKEY * pKey)
{
	if(m_mapHotKeys.find(*pKey)!=m_mapHotKeys.end()) return FALSE;
	m_mapHotKeys[*pKey]=guid;
	return TRUE;
}

BOOL CHotKeyMgr::UnregisterHotKey(REFGUID guid,const PRESERVEDKEY * pKey)
{
	std::map<PRESERVEDKEY,GUID>::iterator it=m_mapHotKeys.find(*pKey);
	if(it==m_mapHotKeys.end()) return FALSE;
	m_mapHotKeys.erase(it);
	return TRUE;
}

BOOL CHotKeyMgr::ProcessHotKey(UINT uVKey,const LPBYTE lpKeyState,GUID *pGuid)
{
	PRESERVEDKEY key={uVKey,TF_MOD_IGNORE_ALL_MODIFIER};
	std::map<PRESERVEDKEY,GUID>::iterator it=m_mapHotKeys.find(key);
	if(it!=m_mapHotKeys.end() 
		&& !(lpKeyState[VK_MENU]&0x80) 
		&& !(lpKeyState[VK_CONTROL]&0x80) 
		&&!(lpKeyState[VK_SHIFT]&0x80))
	{
		if(pGuid) *pGuid=it->second;
		return TRUE;
	}
	key.uModifiers=0;
	if(lpKeyState[VK_LMENU]&0x80) key.uModifiers|=TF_MOD_LALT;
	if(lpKeyState[VK_RMENU]&0x80) key.uModifiers|=TF_MOD_RALT;
	if(lpKeyState[VK_LCONTROL]&0x80) key.uModifiers|=TF_MOD_LCONTROL;
	if(lpKeyState[VK_RCONTROL]&0x80) key.uModifiers|=TF_MOD_RCONTROL;
	if(lpKeyState[VK_LSHIFT]&0x80) key.uModifiers|=TF_MOD_LSHIFT;
	if(lpKeyState[VK_RSHIFT]&0x80) key.uModifiers|=TF_MOD_RSHIFT;
	
	it=m_mapHotKeys.find(key);
	if(it!=m_mapHotKeys.end())
	{
		if(pGuid) *pGuid=it->second;
		return TRUE;
	}
	if((key.uModifiers|TF_MOD_LALT) || (key.uModifiers|TF_MOD_RALT))
	{
		key.uModifiers&=~(TF_MOD_LALT|TF_MOD_RALT);
		key.uModifiers|=TF_MOD_ALT;
		it=m_mapHotKeys.find(key);
		if(it!=m_mapHotKeys.end())
		{
			if(pGuid) *pGuid=it->second;
			return TRUE;
		}
	}
	if((key.uModifiers|TF_MOD_LCONTROL) || (key.uModifiers|TF_MOD_RCONTROL))
	{
		key.uModifiers&=~(TF_MOD_LCONTROL|TF_MOD_RCONTROL);
		key.uModifiers|=TF_MOD_CONTROL;
		it=m_mapHotKeys.find(key);
		if(it!=m_mapHotKeys.end())
		{
			if(pGuid) *pGuid=it->second;
			return TRUE;
		}
	}
	if((key.uModifiers|TF_MOD_LSHIFT) || (key.uModifiers|TF_MOD_RSHIFT))
	{
		key.uModifiers&=~(TF_MOD_LSHIFT|TF_MOD_RSHIFT);
		key.uModifiers|=TF_MOD_SHIFT;
		it=m_mapHotKeys.find(key);
		if(it!=m_mapHotKeys.end())
		{
			if(pGuid) *pGuid=it->second;
			return TRUE;
		}
	}
	return FALSE;
}