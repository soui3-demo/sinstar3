#pragma once

#include "../include/sinstar-i.h"
#include "../include/unknown.h"
#include "../ipcobject/include/IpcObject.h"
#include "../include/protocol.h"
#include <unknown\obj-ref-impl.hpp>

class CClientConnection : public SOUI::TObjRefImpl<SIpcConnection>
{
public:
	CClientConnection(ITextService * pTxtService) 
		:m_pTxtService(pTxtService) 
	{}
protected:
	LRESULT OnInputStringW( Param_InputStringW &param);
	LRESULT OnIsCompositing( Param_IsCompositing &param);
	LRESULT OnStartComposition( Param_StartComposition &param);
	LRESULT OnReplaceSelCompositionW( Param_ReplaceSelCompositionW &param);
	LRESULT OnUpdateResultAndCompositionStringW( Param_UpdateResultAndCompositionStringW &param);
	LRESULT OnEndComposition( Param_EndComposition &param);
	LRESULT OnGetImeContext( Param_GetImeContext &param);
	LRESULT OnReleaseImeContext( Param_ReleaseImeContext &param);
	LRESULT OnSetConversionMode( Param_SetConversionMode &param);
	LRESULT OnGetConversionMode( Param_GetConversionMode &param);
	LRESULT OnSetOpenStatus( Param_SetOpenStatus &param);
	LRESULT OnGetOpenStatus( Param_GetOpenStatus &param);

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
	ITextService * m_pTxtService;
};

class CSinstarProxy : public ISinstar, public CUnknown
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

public:
	IUNKNOWN_BEGIN(IUnknown)
	IUNKNOWN_END()

private:
	ITextService * m_pTxtService;
	CClientConnection	m_conn;
};

