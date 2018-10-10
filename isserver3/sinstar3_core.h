#pragma once

#include "../include/module_ref.h"
#include <Sddl.h>
#include <Aclapi.h>

#ifdef SINSTAR3_CORE_EXPORTS
#define SINSTAR3_API __declspec(dllexport)
#else
#define SINSTAR3_API __declspec(dllimport)
#endif


EXTERN_C SINSTAR3_API  ISinstar* Sinstar3_Create(ITextService* pTxtSvr,HWND hParentWnd);

EXTERN_C SINSTAR3_API  void Sinstar3_Delete(ISinstar* pSinstar3);

EXTERN_C SINSTAR3_API HRESULT Sinstar3_CanUnloadNow();

EXTERN_C SINSTAR3_API BOOL Sinstar3_Config(HWND hWnd);

EXTERN_C SINSTAR3_API void Sinstar3_SetHostInfo(HostInfo *pHostInfo);

class CSinstar3Core :public CModuleRef
{
public:
	CSinstar3Core(HINSTANCE hInst);
	~CSinstar3Core();

	void SetLogStateListener(ILogStateListener *pListener);

	SOUI::SStringT GetDataPath() const {return m_strDataPath;}
	SOUI::SStringT GetSettingPath() const;
	SOUI::SStringT GetCfgIni() const { return m_strConfigIni; }
protected:
	virtual void OnInit();

	virtual void OnFinalRelease();

	ILogStateListener * m_pLogStateListener;
	SOUI::SStringT m_strDataPath;
	SOUI::SStringT m_strConfigIni;
	HANDLE m_hMutex;
	HANDLE m_hSettingFileMap;
};

extern CSinstar3Core *theModule;
extern int				g_nRefCount;