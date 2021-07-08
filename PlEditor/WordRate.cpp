#include "StdAfx.h"
#include "WordRate.h"
#include "TxtReader.h"

#pragma pack(push,1)
struct WordRate{
	wchar_t wIndex;
	BYTE    byRate;
};
#pragma pack(pop)

static const char KRateHeader[4]="SWR";
#define VER_MAJOR 3
#define VER_MINOR 0
static const WORD KRateVer=(VER_MAJOR<<8)|VER_MINOR;

CWordRate::CWordRate(void)
{
}

CWordRate::~CWordRate(void)
{
}

CWordRate & CWordRate::GetInstance()
{
	static CWordRate s_instance;
	return s_instance;
}


BOOL CWordRate::_LoadFromFile(LPCTSTR pszRateFile)
{
	FILE *f = _tfopen(pszRateFile,_T("rb"));
	if(!f)
		return FALSE;
	char szHeader[sizeof(KRateHeader)]={0};
	WORD wVer = 0;
	//read header and ver
	fread(szHeader,1,sizeof(KRateHeader),f);
	fread(&wVer,sizeof(WORD),1,f);
	if(strncmp(szHeader,KRateHeader,sizeof(KRateHeader))!=0
		|| wVer!=KRateVer)
	{
		fclose(f);
		return FALSE;
	}
	m_mapRate.clear();
	//read count
	DWORD dwCount=0;
	fread(&dwCount,sizeof(DWORD),1,f);
	WordRate *pWordRate = new WordRate[dwCount];
	BOOL bRet = FALSE;
	if(pWordRate)
	{
		fread(pWordRate,sizeof(WordRate),dwCount,f);
		for(DWORD i=0;i<dwCount;i++)
		{
			m_mapRate.insert(std::make_pair(pWordRate[i].wIndex,pWordRate[i].byRate));
		}
		delete []pWordRate;
		bRet = TRUE;
	}
	fclose(f);

	return bRet;
}

BOOL CWordRate::LoadFromFile(LPCTSTR pszRateFile)
{
	return GetInstance()._LoadFromFile(pszRateFile);
}

BYTE CWordRate::_GetWordRate(wchar_t wIndex) const
{
	RATEMAP::const_iterator it = m_mapRate.find(wIndex);
	if(it==m_mapRate.end())
		return 0;
	return it->second;
}

BYTE CWordRate::GetWordRate(wchar_t wIndex)
{
	return GetInstance()._GetWordRate(wIndex);
}

BOOL CWordRate::_SaveToFile(LPCTSTR pszRateFile)
{
	FILE *f = _tfopen(pszRateFile,_T("wb"));
	if(!f)
		return FALSE;
	fwrite(KRateHeader,1,sizeof(KRateHeader),f);
	fwrite(&KRateVer,sizeof(KRateVer),1,f);
	DWORD dwCount = m_mapRate.size();
	fwrite(&dwCount,sizeof(DWORD),1,f);
	RATEMAP::const_iterator it = m_mapRate.begin();
	while(it!=m_mapRate.end())
	{
		WordRate wordRate={it->first,it->second};
		fwrite(&wordRate,sizeof(wordRate),1,f);
		it++;
	}
	fclose(f);
	return TRUE;
}

BOOL CWordRate::SaveToFile(LPCTSTR pszRateFile)
{
	return GetInstance()._SaveToFile(pszRateFile);
}

BOOL CWordRate::Export(LPCTSTR pszTxtFile)
{
	return GetInstance()._Export(pszTxtFile);
}

int CWordRate::Import(LPCTSTR pszTxtFile)
{
	return GetInstance()._Import(pszTxtFile);
}

static int WordRateCmpByRate( const void *arg1, const void *arg2 )
{
	const WordRate *p1=(const WordRate*)arg1;
	const WordRate *p2=(const WordRate*)arg2;
	return (int)p2->byRate-(int)p1->byRate;
}

BOOL CWordRate::_Export(LPCTSTR pszTxtFile) const
{
	FILE *f = _tfopen(pszTxtFile,_T("wb"));
	if(!f) return FALSE;
	
	const BYTE BOM[2]={0xff,0xfe};
	WordRate *pWordRate = new WordRate[m_mapRate.size()];
	UINT i=0;
	for(RATEMAP::const_iterator it=m_mapRate.begin();it!=m_mapRate.end();it++,i++)
	{
		pWordRate[i].wIndex=it->first;
		pWordRate[i].byRate=it->second;
	}
	qsort(pWordRate,m_mapRate.size(),sizeof(WordRate),WordRateCmpByRate);

	fwrite(BOM,1,2,f);
	for(i=0;i<m_mapRate.size();i++)
	{
		fwprintf(f,L"%c %u\r\n",pWordRate[i].wIndex,pWordRate[i].byRate);
	}
	fclose(f);
	delete []pWordRate;
	return TRUE;
}

int CWordRate::_Import(LPCTSTR pszTxtFile)
{
	CTxtReader reader;
	if(!reader.Open(pszTxtFile))
		return 0;
	WCHAR szBuf[50];
	int nCount = 0;
	while(reader.getline(szBuf,50))
	{
		WCHAR wWord;
		int nRate;
		if(szBuf[1]==VK_SPACE)
		{
			wWord=szBuf[0];
			nRate = _wtoi(szBuf+2);
			if(nRate>MAX_RATE)
				nRate=MAX_RATE;
			m_mapRate[wWord]=(BYTE)nRate;
			nCount ++;
		}
	}
	return nCount;
}


BOOL CWordRate::SetWordRate(WCHAR wIndex,BYTE byRate)
{
	if(byRate>MAX_RATE)
		return FALSE;
	GetInstance().m_mapRate[wIndex]=byRate;
	return TRUE;
}

void CWordRate::UpdateWordRate(WCHAR wIndex,int change)
{
	BYTE byRate = GetWordRate(wIndex);
	int sRate = byRate;
	sRate+=change;
	if(sRate>MAX_RATE)
		sRate=MAX_RATE;
	if(sRate<0)
		sRate=0;
	SetWordRate(wIndex,(BYTE)sRate);
}

void CWordRate::Clear()
{
	GetInstance()._Clear();
}

void CWordRate::_Clear()
{
	m_mapRate.clear();
}

int CWordRate::GetSize()
{
	return GetInstance()._GetSize();
}

int CWordRate::_GetSize() const
{
	return m_mapRate.size();
}

/*
二、码位分配及顺序 
GBK 亦采用双字节表示，总体编码范围为 8140-FEFE，首字节在 81-FE 之间，尾字节在 40-FE 之间，剔除 xx7F 一条线。总计 23940 个码位，共收入 21886 个汉字和图形符号，其中汉字（包括部首和构件）21003 个，图形符号 883 个。 
全部编码分为三大部分： 

1. 汉字区。包括： 
a. GB 2312 汉字区。即 GBK/2: B0A1-F7FE。收录 GB 2312 汉字 6763 个，按原顺序排列。 
b. GB 13000.1 扩充汉字区。包括： 
(1) GBK/3: 8140-A0FE。收录 GB 13000.1 中的 CJK 汉字 6080 个。 
(2) GBK/4: AA40-FEA0。收录 CJK 汉字和增补的汉字 8160 个。CJK 汉字在前，按 UCS 代码大小排列；增补的汉字（包括部首和构件）在后，按《康熙字典》的页码／字位排列。 

2. 图形符号区。包括： 
a. GB 2312 非汉字符号区。即 GBK/1: A1A1-A9FE。其中除 GB 2312 的符号外，还有 10 个小写罗马数字和 GB 12345 增补的符号。计符号 717 个。 
b. GB 13000.1 扩充非汉字区。即 GBK/5: A840-A9A0。BIG-5 非汉字符号、结构符和“○”排列在此区。计符号 166 个。 
*/
#define SIZE_GBK1	846		//(A9-A1+1)*(FE-A1+1) 图形符号区
#define SIZE_GBK2	6768	//(F7-B0+1)*(FE-A1+1) GB 2312 汉字区
#define SIZE_GBK3	6112	//(A0-81+1)*(FE-40+1) CJK 汉字 
#define SIZE_GBK4	8245	//(FE-AA+1)*(A0-40+1) 收录 CJK 汉字和增补的汉字
#define SIZE_GBK5	194		//(A9-A8+1)*(A0-40+1)


int CWordID::GetWordType(WCHAR wIndex)
{
	BYTE szWord[2];
	if(IsAlphabit(wIndex))
		return ASCII;
	if(!WideCharToMultiByte(936,0,&wIndex,1,(char*)szWord,2,NULL,NULL))
		return GBK_UNDEFINE;
	BYTE byLow=szWord[0],byHei=szWord[1];

	int nRet= GBK_UNDEFINE;
	if(byHei>=0xA1 && byLow<=0xFE)
	{
		if(byLow>=0x81 && byLow<=0xA0)
		{
			nRet=GBK3;
		}
		else if(byLow>=0xA1 && byLow<=0xA9)
		{
			nRet=GBK1;
		}
		else if(byLow>=0xB0 && byLow<=0xF7)
		{
			nRet=GBK2;
		}
	}
	else if(byHei>=0x40 && byHei<=0xA0)
	{
		if(byLow>=0x81 && byLow<=0xA0)
		{
			nRet=GBK3;
		}
		else if(byLow>=0xA8 && byLow<=0xA9)
		{
			nRet=GBK5;
		}
		else if(byLow>=0xAA && byLow<=0xFE)
		{
			nRet=GBK4;
		}
	}

	return nRet;
}

bool CWordID::IsHanzi(WCHAR wWord)
{
	return wWord>=128;
}

bool CWordID::IsAlphabit(WCHAR wWord)
{
	return wWord<128;
}

bool CWordID::IsValidWord(WCHAR wWord)
{
	return wWord<0xD800;
}

bool CWordID::IsGBK(WCHAR wIndex)
{
	int type = GetWordType(wIndex);
	return type > GBK2;
}
