#pragma once

#include <reg.h>
#include <string>

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> >
tstring;

class CShellTypeReg
{
public:
	CShellTypeReg(void);
	~CShellTypeReg(void);

	static BOOL UnregShellType(const tstring &strType);
	static BOOL RegShellType(const tstring &strType,const tstring & strFileTypeID, const tstring & strFileTypeName,const tstring & strOpenFmt, int iIcon);
};
