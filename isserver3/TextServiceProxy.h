#pragma once

#include "../include/sinstar-i.h"
#include "../include/unknown.h"
#include "../ipcobject/include/IpcObject.h"
#include "../include/protocol.h"
#include <unknown\obj-ref-impl.hpp>

class CSvrConnection : public ITextService, public SOUI::TObjRefImpl<SIpcConnection> {
public:
	CSvrConnection::CSvrConnection(ISinstar	*pSinstar):m_pSinstar(pSinstar)
	{
	}

	CSvrConnection::~CSvrConnection(void)
	{
	}
public:
	virtual BOOL InputStringW(LPCWSTR pszBuf, int nLen);

	virtual BOOL IsCompositing() const;

	virtual void StartComposition(LPVOID lpImeContext);

	virtual void ReplaceSelCompositionW(LPVOID lpImeContext, int nLeft, int nRight, const WCHAR *wszComp, int nLen);

	virtual void UpdateResultAndCompositionStringW(LPVOID lpImeContext, const WCHAR *wszResultStr, int nResStrLen, const WCHAR *wszCompStr, int nCompStrLen);

	virtual void EndComposition(LPVOID lpImeContext);

	virtual LPVOID GetImeContext();

	virtual BOOL ReleaseImeContext(LPVOID lpImeContext);

	virtual void SetConversionMode(EInputMethod mode);

	virtual EInputMethod GetConversionMode();

	virtual BOOL SetOpenStatus(LPVOID lpImeContext, BOOL bOpen);

	virtual BOOL GetOpenStatus(LPVOID lpImeContext) const;


protected:
	void HandleOnImeSelect(Param_OnImeSelect & param);
	void HandleOnCompositionStarted(Param_OnCompositionStarted &param);
	void HandleOnCompositionChanged(Param_OnCompositionChanged &param);
	void HandleOnCompositionTerminated(Param_OnCompositionTerminated &param);
	void HandleOnSetCaretPosition(Param_OnSetCaretPosition &param);
	void HandleOnSetFocusSegmentPosition(Param_OnSetFocusSegmentPosition &param);
	void HandleProcessKeyStoke(Param_ProcessKeyStoke &param);
	void HandleTranslateKey(Param_TranslateKey &param);
	void HandleOnSetFocus(Param_OnSetFocus &param);
	void HandleGetCompositionSegments(Param_GetCompositionSegments &param);
	void HandleGetCompositionSegmentEnd(Param_GetCompositionSegmentEnd &param);
	void HandleGetCompositionSegmentAttr(Param_GetCompositionSegmentAttr &param);
	void HandleOnOpenStatusChanged(Param_OnOpenStatusChanged &param);
	void HandleOnConversionModeChanged(Param_OnConversionModeChanged &param);
	void HandleShowHelp(Param_ShowHelp &param);
	void HandleGetDefInputMode(Param_GetDefInputMode &param);

	FUN_BEGIN
		FUN_HANDLER(Param_OnImeSelect, HandleOnImeSelect)
		FUN_HANDLER(Param_OnCompositionStarted, HandleOnCompositionStarted)
		FUN_HANDLER(Param_OnCompositionChanged, HandleOnCompositionChanged)
		FUN_HANDLER(Param_OnCompositionTerminated, HandleOnCompositionTerminated)
		FUN_HANDLER(Param_OnSetCaretPosition, HandleOnSetCaretPosition)
		FUN_HANDLER(Param_OnSetFocusSegmentPosition, HandleOnSetFocusSegmentPosition)
		FUN_HANDLER(Param_ProcessKeyStoke, HandleProcessKeyStoke)
		FUN_HANDLER(Param_TranslateKey, HandleTranslateKey)
		FUN_HANDLER(Param_OnSetFocus, HandleOnSetFocus)
		FUN_HANDLER(Param_GetCompositionSegments, HandleGetCompositionSegments)
		FUN_HANDLER(Param_GetCompositionSegmentEnd, HandleGetCompositionSegmentEnd)
		FUN_HANDLER(Param_GetCompositionSegmentAttr, HandleGetCompositionSegmentAttr)
		FUN_HANDLER(Param_OnOpenStatusChanged, HandleOnOpenStatusChanged)
		FUN_HANDLER(Param_OnConversionModeChanged, HandleOnConversionModeChanged)
		FUN_HANDLER(Param_ShowHelp, HandleShowHelp)
		FUN_HANDLER(Param_GetDefInputMode, HandleGetDefInputMode)
	FUN_END
private:
	ISinstar	*m_pSinstar;
};

class CServerObject: public SIpcServer
{
public:
	CServerObject(HWND hSvr) :m_hSvr(hSvr) {}
	~CServerObject(void) {}

protected:
	// Í¨¹ý SIpcServer ¼Ì³Ð
	virtual HWND GetSvrId() override;
	virtual HRESULT CreateConnection(SIpcConnection ** ppConnection) const override;
private:
	HWND m_hSvr;
};
