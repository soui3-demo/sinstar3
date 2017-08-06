#pragma once

class CAutoLock
{
public:
	CAutoLock(CRITICAL_SECTION * cs):m_cs(cs)
	{
		EnterCriticalSection(m_cs);
	}

	~CAutoLock()
	{
		LeaveCriticalSection(m_cs);
	}

private:
	CRITICAL_SECTION * m_cs;
};