#pragma once
#include "ThreadObject.h"

struct EDITFILEINFO
{
	int nType;
	SStringT strFileName;
};

#define UM_EDITFILEFINISH	(WM_USER+400)

class CEditFileFinishMonitor : public CThreadObject
{
public:
	CEditFileFinishMonitor(EDITFILEINFO *pData, HWND hMsgRecv);
	~CEditFileFinishMonitor();

	const EDITFILEINFO * GetEditFileInfo() const {
		return m_editFileInfo;
	}
protected:
	virtual UINT Run();

	EDITFILEINFO *m_editFileInfo;
	HWND		 m_hWndRecv;
};

