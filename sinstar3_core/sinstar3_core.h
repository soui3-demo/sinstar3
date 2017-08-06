#pragma once

#ifdef SINSTAR3_CORE_EXPORTS
#define BAIDUJP3_API __declspec(dllexport)
#else
#define BAIDUJP3_API __declspec(dllimport)
#endif


EXTERN_C BAIDUJP3_API  ISinstar* BaiduJP3_Create(CTextService* pTxtSvr,HINSTANCE hInst);

EXTERN_C BAIDUJP3_API  void BaiduJP3_Delete(ISinstar* pBaiduJP3);

EXTERN_C BAIDUJP3_API HRESULT BaiduJP3_CanUnloadNow();

EXTERN_C BAIDUJP3_API BOOL BaiduJP3_Config(HWND hWnd);