#include "stdafx.h"
#include "TipDict.h"

#define NAME_TIPDICT _T("\\server\\tipdict.dll")


template<>
CTipDict* SSingleton<CTipDict>::ms_Singleton = NULL;

CTipDict::CTipDict(SStringT strPath):m_hPlugin(NULL),m_funTipDict(NULL)
{
	SStringT strDict = strPath + NAME_TIPDICT;
	HMODULE hMod = LoadLibrary(strDict);
	if(hMod)
	{
		m_funTipDict = (FunTipDict)GetProcAddress(hMod,"TipDict");
		if(!m_funTipDict)
		{
			FreeLibrary(hMod);
		}else
		{
			m_hPlugin = hMod;
		}
	}
}

CTipDict::~CTipDict(void)
{
	if(m_hPlugin)
	{
		FreeLibrary(m_hPlugin);
	}
}

int CTipDict::TipDict(LPCWSTR pszKey,int nKeyLen,LPWSTR pszBuf,int nBufLen)
{
	if(m_funTipDict)
	{
		return m_funTipDict(pszKey,nKeyLen,pszBuf,nBufLen);
	}else
	{
		return -1;
	}
}
