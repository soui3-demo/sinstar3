#pragma once

#include "TextService-i.h"


interface ISinstar : public IUnknown
{
	virtual void Init(CTextService * pTxtSvc) = NULL;
	virtual void OnIMESelect(BOOL bSelect)=NULL;
	virtual void OnCompositionStarted()=NULL;
	virtual void OnCompositionChanged()=NULL;
	virtual void OnCompositionTerminated()=NULL;
	virtual void OnSetCaretPosition(POINT pt,int nHei)=NULL;
	virtual void OnSetFocusSegmentPosition(POINT pt,int nHei)=NULL;
	virtual void ProcessKeyStoke(LPVOID lpImeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown,BOOL *pbEaten)=NULL;
	virtual void TranslateKey(LPVOID lpImeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown,BOOL *pbEaten)=NULL;
	virtual void OnSetFocus(BOOL bFocus)=NULL;
	virtual int  GetCompositionSegments()=NULL;
	virtual int  GetCompositionSegmentEnd(int iSeg)=NULL;
	virtual int	 GetCompositionSegmentAttr(int iSeg)=NULL;
	virtual BOOL OnHotkey(LPVOID lpImeContext,REFGUID guidHotKey)=NULL;
	virtual void OnOpenStatusChanged(BOOL bOpen)=NULL;
	virtual void OnConversionModeChanged(EInputMethod uMode)=NULL;
	virtual void CheckDefIME()=NULL;
	virtual void ShowHelp()=NULL;
	virtual EInputMethod GetDefInputMode() = NULL;
	virtual BOOL ShowCompWnd() = NULL;
	virtual LRESULT OnWildMessage(WPARAM wParam,LPARAM lParam)=NULL;
	virtual INT_PTR MessageBox(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType);
	virtual HMODULE GetModule()=NULL;
};

typedef ISinstar *(*FUN_BaiduJP3_Create)(CTextService *,HINSTANCE);
typedef void (*FUN_BaiduJP3_Delete)(ISinstar*);
typedef BOOL (*FUN_BaiduJP3_Config)(HWND);
typedef HRESULT (*FUN_BaiduJP3_CanUnloadNow)();
