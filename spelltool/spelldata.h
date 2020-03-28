#pragma once

#include "skeytree.h"
#include "sarray.hpp"

#define SIZE_TUNE	24		//声母数量
#define SIZE_RHYME	33		//韵母数量
#define SIZE_SPELL	407		//所有拼音数量

#define SPELLID	'PSIS'

#define FLAG_REVS	0xFE	//逆向索引树标志
#define FLAG_SPELL	0xFD	//完整拼音索引
#define FLAG_RHYME	0xFC	//韵母索引表
#define FLAG_PHRASE	0xFB	//词组索引表

#define MAX_SENTLEN		50		//语句最大长度


typedef struct tagSPELLINFO
{
	char szSpell[7];
	BYTE byID[2];
}SPELLINFO,*PSPELLINFO;

typedef struct tagSPELLINDEX
{
	BYTE byID[2];
	short sIndex;
}SPELLINDEX;

typedef struct tagMYSPTEXT
{
	char cLen;
	char szText[5];
}MYSPTEXT;

class CSpellData
{	
public:
	CSpellData();
	virtual ~CSpellData();

	BOOL DetectSpell(LPCSTR pszSpell,int nLen);
	BOOL IsSpellValid(BYTE byID[2]);
	BYTE MakeupSpell(BYTE byTune,BYTE byRhyme[SIZE_RHYME]);
	short GetIDIndex(const BYTE byID[2]);
	BOOL GetRhymeID(LPCSTR pszRhyme,int nLen,BYTE *pbyRhymeID);
	BOOL GetRhymeID(LPCWSTR pszRhyme,int nLen,BYTE *pbyRhymeID);
	BOOL GetTuneID(LPCSTR pszTune,int nLen,BYTE *pbyTuneID);
	BOOL GetTuneID(LPCWSTR pszTune,int nLen,BYTE *pbyTundID);

	BOOL ID2Spell(BYTE byID[2],char szSpell[7]);
	BOOL Spell2ID(LPCSTR pszSpell,int nLen,BYTE byID[2]);
	BOOL Spell2ID(LPCWSTR pszSpell,int nLen,BYTE byID[2]);
	int Spell2IDEx(LPCSTR pszSpell,int nLen,BYTE byID[2]);
	int SplitSpell2Rhymes(LPCSTR pszSpell, int nLen, int cSpells[MAX_SENTLEN/2],bool bTestJP);
	int GuessRhymeIDs(const BYTE byID[2],BYTE outIDs[][2],int nLen);
private:
	int ExtractSpell(LPCSTR pszSpell, int nLen,BYTE byID[2]);
private:
	CSArray<SPELLINFO> m_arrSpInfo;
	CSArray<SPELLINDEX> m_arrSpIndex;
	CSArray<MYSPTEXT>		m_arrSpTune;
	CSArray<MYSPTEXT>	m_arrSpRhyme;
};

extern MYSPTEXT g_szTune[SIZE_TUNE];
extern MYSPTEXT g_szRhyme[SIZE_RHYME];
extern SPELLINFO g_SpellInfo[SIZE_SPELL];
extern CSpellData g_SpellData;