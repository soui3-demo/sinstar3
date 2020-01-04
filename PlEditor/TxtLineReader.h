// TxtLineReader.h: interface for the CTxtLineReader class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include <fstream>

class CTxtLineReader 
{
public:
	CTxtLineReader(char cRemarkFlag=0);
	virtual ~CTxtLineReader();

	LPSTR ReadString( LPSTR lpsz, UINT nMax );
	void Close();
	BOOL Open(LPCTSTR pszFileName);
	
	DWORD GetLength() const;
	DWORD GetPos();
private:
	char  m_cRemarkFlag;
	std::fstream	m_file;
	DWORD m_dwLen;
};

