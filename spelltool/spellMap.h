#pragma once

#include "spelldata.h"
#include "skeytreeEx.h"
#include "sarray.hpp"

//<!一个拼音重码的信息
typedef struct tagSPELLCAND{
	BYTE byRate;	//重码使用频率
	BYTE byLen;		//==1，为了与词组保持一致
	WCHAR wWord;	//重码
}SPELLCAND,*PSPELLCAND;

typedef struct tagWORDRATE
{
	WCHAR wWord;
	BYTE byRate;
}WORDRATE;

typedef struct tagSPELLCANDINFO
{
	WORD wCount;
	SPELLCAND *pCands;
}SPELLCANDINFO,*PSPELLCANDINFO;

int WordRateCmp(WORDRATE *pwr1,WORDRATE *pwr2,LPARAM lParam);

//!拼音与汉字表的映射表，只包含单字，不含词组
class CSpellMap
{
public:
	CSpellMap(){memset(m_SpellMap,0,sizeof(m_SpellMap));}
	~CSpellMap(){Free();}

	void SetSpellCands(short nIdxSpell,LPCWSTR pszCand,int nLen);

	void LoadData(FILE *f);

	void WriteData(FILE *f);

	void Free();


	PSPELLCAND GetFirstCand(short nIndex);

	SPELLCANDINFO * GetSpellMap(short sIdx)
	{
		ASSERT(sIdx>=0 && sIdx<SIZE_SPELL);
		return m_SpellMap+sIdx;
	}
	//-------------------------------------------------------------
	//	通过词频表来初始化单字的字频
	//	CSArray<WORDRATE> *pArrWordRate:词频表
	//-------------------------------------------------------------
	void InitRate(CSArray<WORDRATE> *pArrWordRate);
private:
	SPELLCANDINFO m_SpellMap[SIZE_SPELL];	//拼音重码表,每个拼音一个列表
};
