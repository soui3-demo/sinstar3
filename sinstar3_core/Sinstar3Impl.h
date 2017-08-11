#pragma once
#include "../include/sinstar-i.h"

#include "ImeWnd.h"

class CSinstar3Impl:
	public ISinstar
{
public:
	CSinstar3Impl(ITextService *pTxtSvr,HINSTANCE hInst);
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

private:
	ITextService	*m_pTxtSvr;
	CImeWnd			*m_pImeWnd;
	HINSTANCE		 m_hInst;

	CPoint			 m_ptCaret;
	int				 m_nCaretHeight;

};



