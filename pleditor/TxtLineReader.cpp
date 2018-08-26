// TxtLineReader.cpp: implementation of the CTxtLineReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TxtLineReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTxtLineReader::CTxtLineReader(char cRemarkFlag)
{
	m_pArchive=NULL;
	m_cRemarkFlag=cRemarkFlag;
}

CTxtLineReader::~CTxtLineReader()
{
	Close();
}

BOOL CTxtLineReader::Open(LPCTSTR pszFileName)
{
	if(!m_file.Open(pszFileName,CFile::modeRead)) return FALSE;
	m_pArchive=new CArchive(&m_file,CArchive::load);
	return TRUE;
}

void CTxtLineReader::Close()
{
	if(m_pArchive)
	{
		m_pArchive->Close();
		delete m_pArchive;
		m_pArchive=NULL;
		m_file.Close();
	}
}

//ÅÅ³ý¿ÕÐÐ¼°×¢ÊÍÐÐ
LPTSTR CTxtLineReader::ReadString(LPTSTR lpsz, UINT nMax)
{
	LPTSTR pszRet=m_pArchive->ReadString(lpsz,nMax);
	if(m_cRemarkFlag) 
	{
		while(pszRet && (lpsz[0]==m_cRemarkFlag||lpsz[0]==0)) pszRet=m_pArchive->ReadString(lpsz,nMax);
	}
	return pszRet;
}
