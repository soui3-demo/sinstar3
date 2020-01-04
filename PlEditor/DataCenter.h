#pragma once
#include "PhraseLib.h"
#include <helper/SCriticalSection.h>
#include <string>

SEVENT_BEGIN(EventProgStart,EVT_EXTERNAL_BEGIN+1)
	DWORD dwMax;
SEVENT_END()
SEVENT_BEGIN(EventProgRun,EVT_EXTERNAL_BEGIN+2)
	DWORD dwValue,dwMax;
SEVENT_END()
SEVENT_BEGIN(EventProgEnd,EVT_EXTERNAL_BEGIN+3)
SEVENT_END()

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

class CDataCenter : IProgListener
{
public:
	CDataCenter(void);
	~CDataCenter(void);

	void LoadSysPL();
	void LoadPL(LPCTSTR pszName);
	void SavePL(LPCTSTR pszName);
	void Import2Group(LPCTSTR pszFile,BYTE byRateMin, BYTE byRateMax,BYTE iGroup=-1);
	std::vector<GROUPINFO> GetGroup() const;
	void AddGroup(GROUPINFO groupInfo);
protected:

	void OnLoadSysPL();
	void OnLoadPL(const std::tstring &name);
	void OnImport2Group(const std::tstring &strFile,BYTE byRateMin, BYTE byRateMax,BYTE iGroup=-1);

	virtual void OnStart(DWORD dwMax);

	virtual void OnProg(DWORD dwValue,DWORD dwMax);

	virtual void OnEnd();

protected:
	mutable SCriticalSection	m_cs;
	bool				m_bReady;
	CPLEditor			m_plEditor;
	SAutoRefPtr<ITaskLoop> m_taskLoop;
};
