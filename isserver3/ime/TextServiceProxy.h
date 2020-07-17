#pragma once

#include "../include/sinstar-i.h"
#include "../include/unknown.h"
#include "../include/protocol.h"

#include <unknown/obj-ref-impl.hpp>
#include <core/SNativeWnd.h>

class CSvrConnection;
struct IConntionFocusListener
{
	virtual void OnSetFocus(CSvrConnection * pConn) = 0;
	virtual void OnKillFocus(CSvrConnection * pConn) = 0;
};

class CSvrConnection : public ITextService, public TObjRefImpl<SOUI::IIpcConnection>, public SNativeWnd  {
	friend class CIsSvrProxy;
public:
	CSvrConnection(IIpcHandle *pIpcHandle,HWND hSvr,IConntionFocusListener * pFocusListener);

	~CSvrConnection(void);
public:
	// Í¨¹ý IIpcConnection ¼Ì³Ð
	virtual int GetBufSize() const override;
	virtual int GetStackSize() const override;

	virtual IIpcHandle * GetIpcHandle() override;
	virtual void BuildShareBufferName(ULONG_PTR idLocal, ULONG_PTR idRemote, TCHAR szBuf[MAX_PATH]) const override;

	bool CallFun(IFunParams *params) const;
	void OnSkinChanged();

protected:
	LRESULT OnReq(UINT msg,WPARAM wp,LPARAM lp);
	BEGIN_MSG_MAP_EX(CSvrConnection)
		MESSAGE_HANDLER_EX(UM_CALL_FUN,OnReq)
	END_MSG_MAP()
public:
	virtual BOOL InputStringW(LPCWSTR pszBuf, int nLen);

	virtual BOOL IsCompositing() const;

	virtual void StartComposition(UINT64 imeContext);

	virtual void ReplaceSelCompositionW(UINT64 imeContext, int nLeft, int nRight, const WCHAR *wszComp, int nLen);

	virtual void UpdateResultAndCompositionStringW(UINT64 imeContext, const WCHAR *wszResultStr, int nResStrLen, const WCHAR *wszCompStr, int nCompStrLen);

	virtual void EndComposition(UINT64 imeContext);

	virtual void SetConversionMode(EInputMethod mode);

	virtual EInputMethod GetConversionMode();

	virtual void SetOpenStatus(BOOL bOpen);

	virtual BOOL GetOpenStatus() const;

	virtual DWORD GetActiveWnd() const;
protected:
	void HandleCreate(Param_Create &param);
	void HandleScaleInfo(Param_ScaleInfo &param);
	void HandleDestroy(Param_Destroy &param);
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
		FUN_HANDLER(Param_Create,HandleCreate)
		FUN_HANDLER(Param_ScaleInfo,HandleScaleInfo)
		FUN_HANDLER(Param_Destroy, HandleDestroy)
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
	SComPtr<ISinstar>	m_pSinstar;
	HWND				m_hSvr;
	std::string			m_strHostPath;
	float				m_xScale;
	float				m_yScale;
	SAutoRefPtr<IIpcHandle> m_ipcHandle;
	IConntionFocusListener	* m_pFocusListener;
};
