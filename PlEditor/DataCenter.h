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
	bool bUpdateUI;
SEVENT_END()

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

class CDataCenter : IProgListener, CPLEditor::IQueryRate
{
public:
	CDataCenter(void);
	~CDataCenter(void);

	void LoadSysPL();
	BOOL LoadRateProvider(LPCTSTR pszName);
	DWORD  GetRateDbSize();
	void LoadPL(LPCTSTR pszName);
	void SavePL(LPCTSTR pszName);
	void Import2Group(LPCTSTR pszFile,BYTE byRateMin, BYTE byRateMax,BYTE byDefRate,BYTE iGroup=-1);
	std::vector<GROUPINFO> GetGroup() const;
	int AddGroup(const GROUPINFO &groupInfo);
	void SetGroup(BYTE iGroup,const GROUPINFO &groupInfo);
	void ValidGroup(BYTE iGroup,BOOL bValid);
	bool IsReady() const; 
	void ExportGroup(LPCTSTR pszFile,BYTE iGroup);
	void Clear();
	void EraseGroup(BYTE iGroup);
protected:

	void OnLoadSysPL();
	void OnLoadPL(const std::tstring &name);
	void OnImport2Group(const std::tstring &strFile,BYTE byRateMin, BYTE byRateMax,BYTE byDefRate,BYTE iGroup=-1);
	void OnExportGroup(const std::tstring & strFile,BYTE iGroup);
	void OnEraseGroup(BYTE iGroup);

	virtual void OnStart(DWORD dwMax);

	virtual void OnProg(DWORD dwValue,DWORD dwMax);

	virtual void OnEnd(bool bUpdateUI);

	virtual BYTE QueryPhraseRate(LPCWSTR pszPhrase,BYTE byLen) override;

protected:
	mutable SCriticalSection	m_cs;
	bool				m_bReady;
	CPLEditor			m_plEditor;
	SAutoRefPtr<ITaskLoop> m_taskLoop;
	CPLReader			m_plRateProvider;
};
