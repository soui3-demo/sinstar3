#include "stdafx.h"
#include "spellmap.h"

///////////////////////////////////////////////////////////////////////////////////////
//	CSpellMap
///////////////////////////////////////////////////////////////////////////////////////
void CSpellMap::SetSpellCands(short nIdxSpell,LPCWSTR pszCand,int nLen)
{
	ASSERT(m_SpellMap[nIdxSpell].wCount==0);
	int nWords=nLen;
	m_SpellMap[nIdxSpell].wCount=nWords;
	m_SpellMap[nIdxSpell].pCands=(SPELLCAND*)malloc(sizeof(SPELLCAND)*nWords);
	for(int i=0;i<nWords;i++)
	{
		ASSERT(CWordID::IsHanzi(pszCand[i]));
		m_SpellMap[nIdxSpell].pCands[i].wWord = pszCand[i];
		m_SpellMap[nIdxSpell].pCands[i].byRate=0;
		m_SpellMap[nIdxSpell].pCands[i].byLen=1;
	}
}

void CSpellMap::LoadData(FILE *f)
{
	for(int i=0;i<SIZE_SPELL;i++)
	{
		fread(&m_SpellMap[i].wCount,sizeof(WORD),1,f);
		m_SpellMap[i].pCands=(SPELLCAND*)malloc(sizeof(SPELLCAND)*m_SpellMap[i].wCount);
		fread(m_SpellMap[i].pCands,sizeof(SPELLCAND),m_SpellMap[i].wCount,f);
	}
}

void CSpellMap::WriteData(FILE *f)
{
	for(int i=0;i<SIZE_SPELL;i++)
	{
		fwrite(&m_SpellMap[i].wCount,sizeof(WORD),1,f);
		fwrite(m_SpellMap[i].pCands,sizeof(SPELLCAND),m_SpellMap[i].wCount,f);
	}
}

void CSpellMap::Free()
{
	for(int i=0;i<SIZE_SPELL;i++)
	{
		if(m_SpellMap[i].pCands) free(m_SpellMap[i].pCands);
		m_SpellMap[i].pCands=NULL;
	}
}

PSPELLCAND CSpellMap::GetFirstCand(short nIndex)
{
	ASSERT(nIndex>=0 && nIndex<SIZE_SPELL);
	ASSERT(m_SpellMap[nIndex].pCands);
	return m_SpellMap[nIndex].pCands;
}

int WordRateCmp(WORDRATE *pwr1,WORDRATE *pwr2,LPARAM lParam)
{
	return memcmp(&pwr1->wWord,&pwr2->wWord,2);
}

//-------------------------------------------------------------
//	通过词频表来初始化单字的词频
//	CSArray<WORDRATE> *pArrWordRate:词频表
//	remark:在初始化Map时，重码是按照词频排序的，所以在通过词频树来设置词频时也应该保持词频的顺序
//-------------------------------------------------------------
void CSpellMap::InitRate(CSArray<WORDRATE> *pArrWordRate)
{
	WORDRATE wr;
	for(int i=0;i<SIZE_SPELL;i++)
	{
		BYTE  byRatePrev=0xFF;	
		for(WORD j=0;j<m_SpellMap[i].wCount;j++)
		{
			wr.wWord = m_SpellMap[i].pCands[j].wWord;
			int nIdx=pArrWordRate->SortFind(&wr,WordRateCmp);
			if(nIdx!=-1)
			{
				WORDRATE *pwr=pArrWordRate->GetPtAt(nIdx);
				if(pwr->byRate<byRatePrev) byRatePrev=pwr->byRate;
			}
			m_SpellMap[i].pCands[j].byRate =byRatePrev;
		}
	}
}


