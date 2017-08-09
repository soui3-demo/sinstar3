#pragma once

#ifdef SINSTAR3_CORE_EXPORTS
#define BAIDUJP3_API __declspec(dllexport)
#else
#define BAIDUJP3_API __declspec(dllimport)
#endif


EXTERN_C BAIDUJP3_API  ISinstar* BaiduJP3_Create(ITextService* pTxtSvr,HINSTANCE hInst);

EXTERN_C BAIDUJP3_API  void BaiduJP3_Delete(ISinstar* pBaiduJP3);

EXTERN_C BAIDUJP3_API HRESULT BaiduJP3_CanUnloadNow();

EXTERN_C BAIDUJP3_API BOOL BaiduJP3_Config(HWND hWnd);

class CSinstar3Core
{
public:
	CSinstar3Core(HINSTANCE hInst);
	~CSinstar3Core();

	LONG AddRef();
	LONG Release();

	LONG GetRefCount();
private:
	LONG m_cRef;
	HINSTANCE m_hInst;
	CRITICAL_SECTION m_cs;
};

extern CSinstar3Core * theCore;