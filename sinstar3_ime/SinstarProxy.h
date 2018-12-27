#pragma once

#include "../include/sinstar-i.h"
#include "../include/unknown.h"
#include "../ipcobject/include/IpcObject.h"
#include "../include/protocol.h"
#include <unknown\obj-ref-impl.hpp>

class CTextServiceProxy : public SOUI::TObjRefImpl<IIpcClient>
{
public:
	CTextServiceProxy(HWND hClient, ITextService * pTxtService) 
		:m_hClient(hClient)
		,m_pTxtService(pTxtService) 
	{}
protected:
	LRESULT OnInputStringW(SIpcObject *pIpcObj, Param_InputStringW &param);
	LRESULT OnIsCompositing(SIpcObject *pIpcObj, Param_IsCompositing &param);
	LRESULT OnStartComposition(SIpcObject *pIpcObj, Param_StartComposition &param);
	LRESULT OnReplaceSelCompositionW(SIpcObject *pIpcObj, Param_ReplaceSelCompositionW &param);
	LRESULT OnUpdateResultAndCompositionStringW(SIpcObject *pIpcObj, Param_UpdateResultAndCompositionStringW &param);
	LRESULT OnEndComposition(SIpcObject *pIpcObj, Param_EndComposition &param);
	LRESULT OnGetImeContext(SIpcObject *pIpcObj, Param_GetImeContext &param);
	LRESULT OnReleaseImeContext(SIpcObject *pIpcObj, Param_ReleaseImeContext &param);
	LRESULT OnSetConversionMode(SIpcObject *pIpcObj, Param_SetConversionMode &param);
	LRESULT OnGetConversionMode(SIpcObject *pIpcObj, Param_GetConversionMode &param);
	LRESULT OnSetOpenStatus(SIpcObject *pIpcObj, Param_SetOpenStatus &param);
	LRESULT OnGetOpenStatus(SIpcObject *pIpcObj, Param_GetOpenStatus &param);

	FUN_BEGIN
		FUN_HANDLER(Param_InputStringW, OnInputStringW)
		FUN_HANDLER(Param_IsCompositing, OnIsCompositing)
		FUN_HANDLER(Param_StartComposition, OnStartComposition)
		FUN_HANDLER(Param_ReplaceSelCompositionW, OnReplaceSelCompositionW)
		FUN_HANDLER(Param_UpdateResultAndCompositionStringW, OnUpdateResultAndCompositionStringW)
		FUN_HANDLER(Param_EndComposition, OnEndComposition)
		FUN_HANDLER(Param_GetImeContext, OnGetImeContext)
		FUN_HANDLER(Param_ReleaseImeContext, OnReleaseImeContext)
		FUN_HANDLER(Param_SetConversionMode, OnSetConversionMode)
		FUN_HANDLER(Param_GetConversionMode, OnGetConversionMode)
		FUN_HANDLER(Param_SetOpenStatus, OnSetOpenStatus)
		FUN_HANDLER(Param_GetOpenStatus, OnGetOpenStatus)
	FUN_END

private:
	HWND	m_hClient;
	ITextService * m_pTxtService;

};

class CSinstarProxy : public ISinstar, public CUnknown, public SIpcObject
{
public:
	CSinstarProxy(HWND hClient,ITextService *pTxtService);
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
	virtual HRESULT CreateIpcClient(HWND hClient, IIpcClient **ppClient) const
	{
		*ppClient = new CTextServiceProxy(hClient, m_pTxtService);
		return S_OK;
	}
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

