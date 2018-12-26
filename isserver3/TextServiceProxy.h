#pragma once

#include "../include/sinstar-i.h"
#include "../include/unknown.h"
#include "../ipcobject/include/IpcObject.h"
#include "../include/protocol.h"

class CTextServiceProxy: public ITextService, public CUnknown, public SIpcObject
{
public:
	CTextServiceProxy(void);
	~CTextServiceProxy(void);

public:
	IUNKNOWN_BEGIN(IUnknown)
	IUNKNOWN_END()

public:
	virtual BOOL InputStringW(LPCWSTR pszBuf, int nLen);

	virtual BOOL IsCompositing() const;

	virtual void StartComposition(LPVOID lpImeContext);

	virtual void ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen);

	virtual void UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen);

	virtual void EndComposition(LPVOID lpImeContext);

	virtual LPVOID GetImeContext();

	virtual BOOL ReleaseImeContext(LPVOID lpImeContext);

	virtual void SetConversionMode(EInputMethod mode);

	virtual EInputMethod GetConversionMode();

	virtual BOOL SetOpenStatus(LPVOID lpImeContext,BOOL bOpen);

	virtual BOOL GetOpenStatus(LPVOID lpImeContext) const;


};
