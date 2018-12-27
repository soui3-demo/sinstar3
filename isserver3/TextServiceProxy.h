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
	LRESULT OnImeSelect(Param_OnImeSelect & param);
	FUN_BEGIN
		FUN_HANDLER(Param_OnImeSelect, OnImeSelect)
		//FUN_HANDLER(Param_IsCompositing, OnIsCompositing)
		//FUN_HANDLER(Param_StartComposition, OnStartComposition)
		//FUN_HANDLER(Param_ReplaceSelCompositionW, OnReplaceSelCompositionW)
		//FUN_HANDLER(Param_UpdateResultAndCompositionStringW, OnUpdateResultAndCompositionStringW)
		//FUN_HANDLER(Param_EndComposition, OnEndComposition)
		//FUN_HANDLER(Param_GetImeContext, OnGetImeContext)
		//FUN_HANDLER(Param_ReleaseImeContext, OnReleaseImeContext)
		//FUN_HANDLER(Param_SetConversionMode, OnSetConversionMode)
		//FUN_HANDLER(Param_GetConversionMode, OnGetConversionMode)
		//FUN_HANDLER(Param_SetOpenStatus, OnSetOpenStatus)
		//FUN_HANDLER(Param_GetOpenStatus, OnGetOpenStatus)
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
