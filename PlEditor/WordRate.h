#pragma once
#include <map>


#define NAME_WORDLIB _T("wordrate.dat")
#define MAX_RATE		250		//允许的最大词频，其它词频数据有特定意义

class CWordRate
{
private:
	CWordRate(void);
	~CWordRate(void);
	
	typedef std::map<wchar_t,BYTE> RATEMAP;
	RATEMAP	m_mapRate;
	static CWordRate & GetInstance();
	
	BOOL _LoadFromFile(LPCTSTR pszRateFile);
	BOOL _SaveToFile(LPCTSTR pszRateFile);
	BYTE _GetWordRate(wchar_t wIndex) const;
	BOOL _Export(LPCTSTR pszTxtFile) const;
	int  _Import(LPCTSTR pszTxtFile);
public:
	static BOOL LoadFromFile(LPCTSTR pszRateFile);
	static BOOL SaveToFile(LPCTSTR pszRateFile);
	static BYTE GetWordRate(WCHAR wIndex);
	static BOOL SetWordRate(WCHAR wIndex,BYTE byRate);
	static void UpdateWordRate(WCHAR wIndex,int change);
	static BOOL Export(LPCTSTR pszTxtFile);
	static int  Import(LPCTSTR pszTxtFile);
};

enum {
	ASCII=0,GBK2,GBK3,GBK4,GBK1,GBK5,GBK_UNDEFINE
};

class CWordID
{
public:
	static int GetWordType(WCHAR wIndex);
	static bool IsGBK(WCHAR wIndex);
	static bool IsHanzi(WCHAR wWord);
	static bool IsAlphabit(WCHAR wWord);
	static bool IsValidWord(WCHAR wWord);
};