#pragma once
#include "trayIcon.h"
#include "MyTTS.h"
#include "KeyMapDlg.h"
#include <iscore-i.h>

typedef IServerCore * (*funIscore_Create)();
typedef void(*funIscore_Destroy)(IServerCore* pCore);

class CIsSvrProxy : public CSimpleWnd, public IUiMsgHandler, public IKeyMapListener
{
public:
	CIsSvrProxy(const SStringT & strDataPath);
	~CIsSvrProxy();

protected:
	virtual void OnKeyMapFree(CKeyMapDlg *pWnd);

protected:
	virtual void OnBuildShapePhraseIndex(PROGTYPE uType, unsigned int dwData);
	virtual void OnBuildSpellPhraseIndex(PROGTYPE uType, unsigned int dwData) ;
	virtual void OnBuildSpellPhraseIndex2(PROGTYPE uType, unsigned int dwData) ;

	virtual void OnClientActive() ;
	virtual void OnClientLogin() ;
	virtual void OnClientLogout() ;

	virtual void OnShowTray(bool bTray) ;

	virtual void OnShowKeyMap(IKeyMapData * pCompData, LPCSTR pszName, LPCSTR pszUrl) ;

	virtual int TtsGetSpeed();
	virtual int TtsGetVoice(bool bCh);
	virtual void TtsSetSpeed(int nSpeed) ;
	virtual void TtsSpeakText(const wchar_t* pText, int nLen, bool bChinese) ;
	virtual void TtsSetVoice(bool bCh, int iToken) ;
	virtual int TtsGetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize);
protected:
	int OnCreate(LPCREATESTRUCT pCS);
	void OnDestroy();
	LRESULT OnTrayNotify(UINT uMsg, WPARAM wp, LPARAM lp);
	LRESULT OnTaskbarCreated(UINT uMsg, WPARAM wp, LPARAM lp);
	void OnTimer(UINT_PTR uID);

	void OnMenuExit(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuAbout(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuAutoExit(UINT uNotifyCode, int nID, HWND wndCtl);

	BEGIN_MSG_MAP_EX(CIsSvrProxy)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER_EX(m_uMsgTaskbarCreated,OnTaskbarCreated)
		COMMAND_ID_HANDLER_EX(R.id.menu_about, OnMenuAbout)
		COMMAND_ID_HANDLER_EX(R.id.menu_exit, OnMenuExit)
		if(m_pCore) CHAIN_MSG_MAP_MEMBER(*m_pCore)
		MESSAGE_HANDLER_EX(UM_TRAYNOTIFY, OnTrayNotify)
		CHAIN_MSG_MAP_MEMBER(m_trayIcon)
		CHAIN_MSG_MAP(CSimpleWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	IServerCore * m_pCore;
	HMODULE		  m_hCoreModule;
	funIscore_Create m_funIsCore_Create;
	funIscore_Destroy m_funIsCore_Destroy;

	SStringT	m_strDataPath;

	CTrayIcon	m_trayIcon;
	CMyTTS		m_tts;
	UINT	    m_uMsgTaskbarCreated;

	CKeyMapDlg *  m_pKeyMapDlg;
};

