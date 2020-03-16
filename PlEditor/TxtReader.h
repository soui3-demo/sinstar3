#pragma once

#include <sstream>

class CTxtReader
{
public:
	CTxtReader(WCHAR cRemarkFlag=0);
	~CTxtReader(void);

	BOOL Open(LPCTSTR pszFileName);
	void Close();

	BOOL getline(WCHAR *pBuf,int nLineMax);
	long getLength() const;
	long getReadPos() const;
private:
	mutable std::wstringstream	m_data;
	long m_length;
	WCHAR m_remarkFlag;
};
