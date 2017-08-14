#pragma once

#ifdef DLL_CORE
#   define SOUI_EXP __declspec(dllimport)
#else
#   define SOUI_EXP
#endif

#include <interface/slog-i.h>

#define GETLOGMGR() g_LogListener.GetLogMgr()
#include <helper/slog.h>


class CLogStateListener : public ILogStateListener
{
public:
	CLogStateListener():m_pLogMgr(NULL)
	{

	}

	virtual void OnLogMgrReady(SOUI::ILog4zManager * pLogMgr)
	{
		m_pLogMgr = pLogMgr;
	}

	virtual void OnLogMgrFree()
	{
		m_pLogMgr = NULL;
	}

	SOUI::ILog4zManager * GetLogMgr() {
		return m_pLogMgr;
	}
private:
	SOUI::ILog4zManager * m_pLogMgr;
};

extern CLogStateListener g_LogListener;
