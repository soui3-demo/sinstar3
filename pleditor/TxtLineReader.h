// TxtLineReader.h: interface for the CTxtLineReader class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CTxtLineReader 
{
public:
	CTxtLineReader(char cRemarkFlag=0);
	virtual ~CTxtLineReader();

	LPTSTR ReadString( LPTSTR lpsz, UINT nMax );
	void Close();
	BOOL Open(LPCTSTR pszFileName);

	CFile m_file;
private:
	char  m_cRemarkFlag;
	CArchive *m_pArchive;
};

