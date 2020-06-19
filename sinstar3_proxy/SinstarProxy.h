#pragma once

#include "../include/sinstar-i.h"
#include "../include/unknown.h"
#include "../include/protocol.h"
#include <unknown\obj-ref-impl.hpp>

class CClientConnection : public SOUI::TObjRefImpl<SOUI::IIpcConnection>
{
public:
	CClientConnection(ITextService * pTxtService);

public:
	// 通过 IIpcConnection 继承
	virtual int GetBufSize() const override;
	virtual int GetStackSize() const override;
	virtual SOUI::IIpcHandle * GetIpcHandle() override;
	virtual void BuildShareBufferName(ULONG_PTR idLocal, ULONG_PTR idRemote, TCHAR szName[MAX_PATH]) const override;
	bool CallFun(SOUI::IFunParams *params) const;
protected:
	void OnInputStringW( Param_InputStringW &param);
	void OnIsCompositing( Param_IsCompositing &param);
	void OnStartComposition( Param_StartComposition &param);
	void OnReplaceSelCompositionW( Param_ReplaceSelCompositionW &param);
	void OnUpdateResultAndCompositionStringW( Param_UpdateResultAndCompositionStringW &param);
	void OnEndComposition( Param_EndComposition &param);
	void OnSetConversionMode( Param_SetConversionMode &param);
	void OnGetConversionMode( Param_GetConversionMode &param);
	void OnSetOpenStatus( Param_SetOpenStatus &param);
	void OnGetOpenStatus( Param_GetOpenStatus &param);
	void OnGetActiveWnd( Param_GetActiveWnd &param);

	FUN_BEGIN
		FUN_HANDLER(Param_InputStringW, OnInputStringW)
		FUN_HANDLER(Param_IsCompositing, OnIsCompositing)
		FUN_HANDLER(Param_StartComposition, OnStartComposition)
		FUN_HANDLER(Param_ReplaceSelCompositionW, OnReplaceSelCompositionW)
		FUN_HANDLER(Param_UpdateResultAndCompositionStringW, OnUpdateResultAndCompositionStringW)
		FUN_HANDLER(Param_EndComposition, OnEndComposition)
		FUN_HANDLER(Param_SetConversionMode, OnSetConversionMode)
		FUN_HANDLER(Param_GetConversionMode, OnGetConversionMode)
		FUN_HANDLER(Param_SetOpenStatus, OnSetOpenStatus)
		FUN_HANDLER(Param_GetOpenStatus, OnGetOpenStatus)
		FUN_HANDLER(Param_GetActiveWnd, OnGetActiveWnd)
	FUN_END

private:
	ITextService * m_pTxtService;
	SOUI::CAutoRefPtr<SOUI::IIpcHandle> m_ipcHandle;
};

class CSinstarProxy : public ISinstar, public CUnknown
{
private:
	CClientConnection	m_conn;
	HWND				m_hSvr;
public:
	CSinstarProxy(ITextService *pTxtService);
	~CSinstarProxy();

	BOOL Init(HWND hClient,  LPCTSTR pszSvrPath);

	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp, LRESULT &result)
	{
		BOOL bHandled=FALSE;
		if(!m_conn.GetIpcHandle())
			return FALSE;
		result = m_conn.GetIpcHandle()->OnMessage((ULONG_PTR)hWnd,uMsg,wp,lp,bHandled);
		return bHandled;
	}

	static bool isInBlackList(LPCTSTR pszBlacklistFile);
public:
	// 通过 ISinstar 继承
	virtual void OnIMESelect(BOOL bSelect) override;
	virtual void OnCompositionStarted() override;
	virtual void OnCompositionChanged() override;
	virtual void OnCompositionTerminated(bool bClearCtx) override;
	virtual void OnSetCaretPosition(POINT pt, int nHei) override;
	virtual void OnSetFocusSegmentPosition(POINT pt, int nHei) override;
	virtual void ProcessKeyStoke(UINT64 imeContext, UINT vkCode, LPARAM lParam, BOOL bKeyDown, BYTE byKeyState[256], BOOL * pbEaten) override;
	virtual void TranslateKey(UINT64 imeContext, UINT vkCode, UINT uScanCode, BOOL bKeyDown, BYTE byKeyState[256], BOOL * pbEaten) override;
	virtual void OnSetFocus(BOOL bFocus,DWORD dwActiveWnd) override;
	virtual int GetCompositionSegments() override;
	virtual int GetCompositionSegmentEnd(int iSeg) override;
	virtual int GetCompositionSegmentAttr(int iSeg) override;
	virtual void OnOpenStatusChanged(BOOL bOpen) override;
	virtual void OnConversionModeChanged(EInputMethod uMode) override;
	virtual void ShowHelp() override;
	virtual EInputMethod GetDefInputMode() override;
	virtual void NotifyScaleInfo(HWND hRefWnd) override;

public:
	IUNKNOWN_BEGIN(IUnknown)
	IUNKNOWN_END()

};

