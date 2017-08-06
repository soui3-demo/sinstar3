#pragma once
#include "../include/sinstar-i.h"


class CBaiduJP3Imp:
	public ISinstar
{
public:
	CBaiduJP3Imp(CTextService *pTxtSvr,HINSTANCE hInst);
	virtual ~CBaiduJP3Imp(void);

	virtual void Init(CTextService * pTxtSvc);

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
	virtual void CheckDefIME();
	virtual void ShowHelp();
	virtual EInputMethod GetDefInputMode();
	virtual BOOL ShowCompWnd();
	virtual LRESULT OnWildMessage(WPARAM wParam,LPARAM lParam);
	virtual HMODULE GetModule();

	virtual INT_PTR MessageBox(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType);

public:
	virtual void OnFinalRelease();
	virtual HRESULT OnQueryInterface(REFIID riid, void **ppvObject);
};



