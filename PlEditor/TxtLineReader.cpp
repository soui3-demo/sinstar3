// TxtLineReader.cpp: implementation of the CTxtLineReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TxtLineReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTxtLineReader::CTxtLineReader(char cRemarkFlag)
:m_cRemarkFlag(cRemarkFlag)
,m_dwLen(0)
{
}

CTxtLineReader::~CTxtLineReader()
{
	Close();
}

BOOL CTxtLineReader::Open(LPCTSTR pszFileName)
{
	m_file.open(pszFileName,std::ios_base::in);
	BOOL bOpen =  m_file.good();
	if(bOpen)
	{
		m_file.seekg(0,std::ios::end);
		m_dwLen = m_file.tellg();
		m_file.seekg(std::ios::beg);
	}
	return bOpen;
}

void CTxtLineReader::Close()
{
	m_file.close();
	m_dwLen=0;
}

//ÅÅ³ý¿ÕÐÐ¼°×¢ÊÍÐÐ
LPSTR CTxtLineReader::ReadString(LPSTR lpsz, UINT nMax)
{
	if(!m_file.good())
		return NULL;
	do{
		m_file.getline(lpsz,nMax);
		if(!m_file.good())
			return NULL;
	}while(lpsz[0]==m_cRemarkFlag || lpsz[0]==0);

	return lpsz;
}

DWORD CTxtLineReader::GetLength() const
{
	return m_dwLen;
}

DWORD CTxtLineReader::GetPos()
{
	if(!m_file.good())
		return 0;
	return m_file.tellg();
}
