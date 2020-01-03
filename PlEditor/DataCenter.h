#pragma once
#include "PhraseLib.h"
#include <helper/SCriticalSection.h>

SEVENT_BEGIN(EventProgStart,EVT_EXTERNAL_BEGIN+1)
	DWORD dwMax;
SEVENT_END()
SEVENT_BEGIN(EventProgRun,EVT_EXTERNAL_BEGIN+2)
	DWORD dwValue,dwMax;
SEVENT_END()
SEVENT_BEGIN(EventProgEnd,EVT_EXTERNAL_BEGIN+3)
SEVENT_END()

class CDataCenter : IProgListener
{
public:
	CDataCenter(void);
	~CDataCenter(void);

	void LoadSysPL();

	const std::vector<GROUPINFO> & GetGroup() const;
protected:

	void OnLoadSysPL();

	virtual void OnStart(DWORD dwMax);

	virtual void OnProg(DWORD dwValue,DWORD dwMax);

	virtual void OnEnd();

protected:
	SCriticalSection	m_cs;
	bool				m_bReady;
	CPLEditor			m_plEditor;
	SAutoRefPtr<ITaskLoop> m_taskLoop;
};
