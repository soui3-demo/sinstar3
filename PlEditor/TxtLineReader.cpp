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
	m_cRemarkFlag=cRemarkFlag;
}

CTxtLineReader::~CTxtLineReader()
{
	Close();
}

BOOL CTxtLineReader::Open(LPCTSTR pszFileName)
{
	m_file.open(pszFileName,std::ios_base::in);
	return m_file.good();
}

void CTxtLineReader::Close()
{
	m_file.close();
}

//ÅÅ³ý¿ÕÐÐ¼°×¢ÊÍÐÐ
LPSTR CTxtLineReader::ReadString(LPSTR lpsz, UINT nMax)
{
	if(!m_file.good())
		return NULL;
	do{
		m_file.getline(lpsz,nMax);
	}while(lpsz[0]==m_cRemarkFlag || lpsz[0]==0);

	return lpsz;
}
