#pragma once
#include "trayIcon.h"
#include "MyTTS.h"
#include "KeyMapDlg.h"
#include "BuildIndexProgWnd.h"
#include <iscore-i.h>
#include "AboutDlg.h"
#include "ThreadObject.h"

typedef IServerCore * (*funIscore_Create)();
typedef void(*funIscore_Destroy)(IServerCore* pCore);

#define UM_BUILD_INDEX_PROG0	(WM_USER+2000)
#define UM_BUILD_INDEX_PROG1	(WM_USER+2001)
#define UM_BUILD_INDEX_PROG2	(WM_USER+2002)

class CIsSvrProxy : public CSimpleWnd
	, public IUiMsgHandler
	, public IKeyMapListener
	, public IUpdateIntervalObserver
	, public CThreadObject
{
public:
	CIsSvrProxy(const SStringT & strDataPath);
	~CIsSvrProxy();

protected:
	virtual void OnKeyMapFree(CKeyMapDlg *pWnd);

protected:
	virtual int GetUpdateInterval() const;
	virtual void OnUpdateIntervalChanged(int nInterval);
protected:
	virtual void OnBuildShapePhraseIndex(PROGTYPE uType, unsigned int dwData);
	virtual void OnBuildSpellPhraseIndex(PROGTYPE uType, unsigned int dwData) ;
	virtual void OnBuildSpellPhraseIndex2(PROGTYPE uType, unsigned int dwData) ;

	virtual void OnClientActive() ;
	virtual void OnClientLogin() ;
	virtual void OnClientLogout() ;

	virtual void OnShowTray(bool bTray) ;

	virtual void OnShowKeyMap(IDataBlock * pCompData, LPCSTR pszName, LPCSTR pszUrl) ;

	virtual int TtsGetSpeed();
	virtual int TtsGetVoice(bool bCh);
	virtual void TtsSetSpeed(int nSpeed) ;
	virtual void TtsSpeakText(const wchar_t* pText, int nLen, bool bChinese) ;
	virtual void TtsSetVoice(bool bCh, int iToken) ;
	virtual int TtsGetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize);

	virtual DWORD OnQueryVersion() const;
protected:
	void _OnBuildIndexProg(int indexMode, PROGTYPE uType, unsigned int dwData);
	int OnCreate(LPCREATESTRUCT pCS);
	void OnDestroy();
	LRESULT OnTrayNotify(UINT uMsg, WPARAM wp, LPARAM lp);
	LRESULT OnTaskbarCreated(UINT uMsg, WPARAM wp, LPARAM lp);
	LRESULT OnBuildIndexProg(UINT uMsg, WPARAM wp, LPARAM lp);

	void OnTimer(UINT_PTR uID);

	void OnMenuExit(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuAbout(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnMenuAutoExit(UINT uNotifyCode, int nID, HWND wndCtl);

	void CheckUpdate(BOOL bManual);
	

	BEGIN_MSG_MAP_EX(CIsSvrProxy)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER_EX(m_uMsgTaskbarCreated,OnTaskbarCreated)
		MESSAGE_RANGE_HANDLER_EX(UM_BUILD_INDEX_PROG0, UM_BUILD_INDEX_PROG2,OnBuildIndexProg)
		COMMAND_ID_HANDLER_EX(R.id.menu_about, OnMenuAbout)
		COMMAND_ID_HANDLER_EX(R.id.menu_exit, OnMenuExit)
		if(m_pCore) CHAIN_MSG_MAP_MEMBER(*m_pCore)
		MESSAGE_HANDLER_EX(UM_TRAYNOTIFY, OnTrayNotify)
		CHAIN_MSG_MAP_MEMBER(m_trayIcon)
		CHAIN_MSG_MAP(CSimpleWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	virtual UINT Run(LPARAM lp);

	int			m_nUpdateInterval;

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
	CBuildIndexProgWnd * m_pBuildIndexProg;

};

