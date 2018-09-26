#pragma once

#include "TextService-i.h"


interface ISinstar : public IUnknown
{
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
	virtual void OnOpenStatusChanged(BOOL bOpen)=NULL;
	virtual void OnConversionModeChanged(EInputMethod uMode)=NULL;
	virtual void ShowHelp()=NULL;
	virtual EInputMethod GetDefInputMode() = NULL;
};

namespace SOUI
{
	struct ILog4zManager;
}

interface ILogStateListener
{
	virtual void OnLogMgrReady(SOUI::ILog4zManager *)=0;
	virtual void OnLogMgrFree()=0;
};

struct HostInfo
{
	ILogStateListener *pLogStateListener;
};

typedef ISinstar *(*FUN_Sinstar3_Create)(ITextService *);
typedef void (*FUN_Sinstar3_Delete)(ISinstar*);
typedef BOOL (*FUN_Sinstar3_Config)(HWND);
typedef HRESULT (*FUN_Sinstar3_CanUnloadNow)();
typedef void (*FUN_Sinstar3_SetHostInfo)(HostInfo *pHostInfo);
