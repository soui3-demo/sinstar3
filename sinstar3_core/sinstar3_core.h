#pragma once

#include "../include/module_ref.h"

#ifdef SINSTAR3_CORE_EXPORTS
#define SINSTAR3_API __declspec(dllexport)
#else
#define SINSTAR3_API __declspec(dllimport)
#endif


EXTERN_C SINSTAR3_API  ISinstar* Sinstar3_Create(ITextService* pTxtSvr,HINSTANCE hInst);

EXTERN_C SINSTAR3_API  void Sinstar3_Delete(ISinstar* pSinstar3);

EXTERN_C SINSTAR3_API HRESULT Sinstar3_CanUnloadNow();

EXTERN_C SINSTAR3_API BOOL Sinstar3_Config(HWND hWnd);

EXTERN_C SINSTAR3_API void Sinstar3_SetLogStateListener(ILogStateListener *pListener);

class CSinstar3Core :public CModuleRef
{
public:
	CSinstar3Core(HINSTANCE hInst);
	~CSinstar3Core();

	void SetLogStateListener(ILogStateListener *pListener);
protected:
	virtual void OnInit();

	virtual void OnFinalRelease();

	ILogStateListener * m_pLogStateListener;
};

extern CSinstar3Core *theModule;