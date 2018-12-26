#pragma once

#include "../include/sinstar-i.h"
#include "../include/unknown.h"
#include "../ipcobject/include/IpcObject.h"
#include "../include/protocol.h"

class CSinstarProxy : public ISinstar, public CUnknown, public SIpcObject
{
public:
	CSinstarProxy(HWND hSvr,HWND hClient,ITextService *pTxtService);
	~CSinstarProxy();

public:
	// Í¨¹ý ISinstar ¼Ì³Ð
	virtual void OnIMESelect(BOOL bSelect) override;
	virtual void OnCompositionStarted() override;
	virtual void OnCompositionChanged() override;
	virtual void OnCompositionTerminated(bool bClearCtx) override;
	virtual void OnSetCaretPosition(POINT pt, int nHei) override;
	virtual void OnSetFocusSegmentPosition(POINT pt, int nHei) override;
	virtual void ProcessKeyStoke(LPVOID lpImeContext, UINT vkCode, LPARAM lParam, BOOL bKeyDown, BOOL * pbEaten) override;
	virtual void TranslateKey(LPVOID lpImeContext, UINT vkCode, UINT uScanCode, BOOL bKeyDown, BOOL * pbEaten) override;
	virtual void OnSetFocus(BOOL bFocus) override;
	virtual int GetCompositionSegments() override;
	virtual int GetCompositionSegmentEnd(int iSeg) override;
	virtual int GetCompositionSegmentAttr(int iSeg) override;
	virtual void OnOpenStatusChanged(BOOL bOpen) override;
	virtual void OnConversionModeChanged(EInputMethod uMode) override;
	virtual void ShowHelp() override;
	virtual EInputMethod GetDefInputMode() override;

protected:
	virtual HWND GetIpcObjectID() override;
protected:
	LRESULT OnInputStringW(HWND hClient, Param_InputStringW &param);
	LRESULT OnIsCompositing(HWND hClient, Param_IsCompositing &param);
	LRESULT OnStartComposition(HWND hClient, Param_StartComposition &param);
	LRESULT OnReplaceSelCompositionW(HWND hClient, Param_ReplaceSelCompositionW &param);
	LRESULT OnUpdateResultAndCompositionStringW(HWND hClient, Param_UpdateResultAndCompositionStringW &param);
	LRESULT OnEndComposition(HWND hClient, Param_EndComposition &param);
	LRESULT OnGetImeContext(HWND hClient, Param_GetImeContext &param);
	LRESULT OnReleaseImeContext(HWND hClient, Param_ReleaseImeContext &param);
	LRESULT OnSetConversionMode(HWND hClient, Param_SetConversionMode &param);
	LRESULT OnGetConversionMode(HWND hClient, Param_GetConversionMode &param);
	LRESULT OnSetOpenStatus(HWND hClient, Param_SetOpenStatus &param);
	LRESULT OnGetOpenStatus(HWND hClient, Param_GetOpenStatus &param);
	FUN_BEGIN
		FUN_HANDLER(Param_InputStringW, OnInputStringW)
		FUN_HANDLER(Param_IsCompositing,OnIsCompositing)
		FUN_HANDLER(Param_StartComposition,OnStartComposition)
		FUN_HANDLER(Param_ReplaceSelCompositionW, OnReplaceSelCompositionW)
		FUN_HANDLER(Param_UpdateResultAndCompositionStringW,OnUpdateResultAndCompositionStringW)
		FUN_HANDLER(Param_EndComposition,OnEndComposition)
		FUN_HANDLER(Param_GetImeContext, OnGetImeContext)
		FUN_HANDLER(Param_ReleaseImeContext, OnReleaseImeContext)
		FUN_HANDLER(Param_SetConversionMode, OnSetConversionMode)
		FUN_HANDLER(Param_GetConversionMode, OnGetConversionMode)
		FUN_HANDLER(Param_SetOpenStatus, OnSetOpenStatus)
		FUN_HANDLER(Param_GetOpenStatus, OnGetOpenStatus)
		FUN_END
public:
	IUNKNOWN_BEGIN(IUnknown)
	IUNKNOWN_END()

private:
	void CallFun(FunParams_Base * param);
private:
	HWND m_hSvr;
	HWND m_hClient;
	ITextService * m_pTxtService;
};

