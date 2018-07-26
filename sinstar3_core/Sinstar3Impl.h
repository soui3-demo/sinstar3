#pragma once
#include "../include/sinstar-i.h"

#include "ui/InputWnd.h"
#include "ui/StatusWnd.h"
#include "ui/STipWnd.h"

#include "InputState.h"
#include "CmdHandler.h"

class CSinstar3Impl:
	public ISinstar,
	public IInputListener,
	public SOUI::SObject,
	public SOUI::CSimpleWnd
{
	friend class CCmdHandler;// CCmdHandler need access this private members.
public:
	CSinstar3Impl(ITextService *pTxtSvr);
	virtual ~CSinstar3Impl(void);

	virtual void OnIMESelect(BOOL bSelect);
	virtual void OnCompositionStarted();
	virtual void OnCompositionChanged();
	virtual void OnCompositionTerminated();
	virtual void OnSetCaretPosition(POINT pt,int nHei);
	virtual void OnSetFocusSegmentPosition(POINT pt,int nHei);
	virtual void ProcessKeyStoke(LPVOID lpImeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown,BOOL *pbEaten);
	virtual void TranslateKey(LPVOID lpImeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown,BOOL *pbEaten);
	virtual void OnSetFocus(BOOL bFocus);
	virtual int  GetCompositionSegments();
	virtual int  GetCompositionSegmentEnd(int iSeg);
	virtual int	 GetCompositionSegmentAttr(int iSeg);
	virtual BOOL OnHotkey(LPVOID lpImeContext,REFGUID guidHotKey);
	virtual void OnOpenStatusChanged(BOOL bOpen);
	virtual void OnConversionModeChanged(EInputMethod uMode);
	virtual void ShowHelp();
	virtual EInputMethod GetDefInputMode();
	virtual BOOL ShowCompWnd();
	virtual LRESULT OnWildMessage(WPARAM wParam,LPARAM lParam);

public:
	virtual void OnFinalRelease();
	virtual HRESULT OnQueryInterface(REFIID riid, void **ppvObject);

	virtual int GetID() const {	return SENDER_SINSTSR3;}
protected://IInputListener
	virtual HWND GetHwnd() const;
	virtual void OnInputStart();
	virtual void OnInputResult(const SStringT & strResult,const SStringT & strComp=SStringT() );
	virtual void OnInputEnd();
	virtual void UpdateInputWnd();

	virtual BOOL GoNextCandidatePage();
	virtual BOOL GoPrevCandidatePage();
	virtual short SelectCandidate(short iCand);
	virtual void OpenInputWnd();
	virtual void CloseInputWnd(BOOL bDelay);
	virtual BOOL SetOpenStatus(BOOL bOpen);
	virtual BOOL GetOpenStatus() const;
	virtual void UpdateStatusbar();
protected://ICmdListener
	virtual void OnCommand(WORD cmd, LPARAM lp);
	virtual InputContext * GetInputContext();
public:
	LRESULT OnSvrNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
	BEGIN_MSG_MAP_EX(CSinstar3Impl)
		MESSAGE_HANDLER_EX(ISComm_GetCommMsgID(),OnSvrNotify)
		CHAIN_MSG_MAP_MEMBER(m_cmdHandler)
		CHAIN_MSG_MAP(SOUI::CSimpleWnd)
	END_MSG_MAP()


private:
	ITextService	*m_pTxtSvr;
	CInputWnd		*m_pInputWnd;
	CStatusWnd		*m_pStatusWnd;
	STipWnd			*m_pTipWnd;
	CInputState		m_inputState;

	void			*m_pCurImeContext;

	CCmdHandler	    m_cmdHandler;
};



