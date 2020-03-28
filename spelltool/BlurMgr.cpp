// BlurMgr.cpp: implementation of the CBlurMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BlurMgr.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlurMgr::CBlurMgr()
{

}

CBlurMgr::~CBlurMgr()
{

}

void CBlurMgr::AddBlurTune(BYTE byTune1,BYTE byTune2)
{
	BLURSYLLABLE blur={byTune1,byTune2};
	m_arBlurTune.Add(blur);
}
void CBlurMgr::AddBlurRhyme(BYTE byRhyme1,BYTE byRhyme2)
{
	BLURSYLLABLE blur={byRhyme1,byRhyme2};
	m_arBlurRhyme.Add(blur);
}
void CBlurMgr::AddBlurSpell(BYTE bySpell1[2],BYTE bySpell2[2])
{
	BLURSPELL blur;
	memcpy(blur.bySpell1,bySpell1,2);
	memcpy(blur.bySpell2,bySpell2,2);
	m_arBlurSpell.Add(blur);
}

int CBlurMgr::GetTuneBLurs(BYTE byTune, BYTE byBlurTune[/*SIZE_TUNE*/])
{
	int nRet=0;
	for(int i=0;i<m_arBlurTune.GetSize();i++)
	{
		BLURSYLLABLE &syllableBlur=m_arBlurTune.ElementAt(i);
		if(syllableBlur.bySyllable1==byTune)
			byBlurTune[nRet++]=syllableBlur.bySyllable2;
		if(syllableBlur.bySyllable2==byTune)
			byBlurTune[nRet++]=syllableBlur.bySyllable1;
	}
	return nRet;
}
int CBlurMgr::GetRhymeBLurs(BYTE byRhyme, BYTE byBlurRhyme[/*SIZE_RHYME*/])
{
	int nRet=0;
	for(int i=0;i<m_arBlurRhyme.GetSize();i++)
	{
		BLURSYLLABLE &syllableBlur=m_arBlurRhyme.ElementAt(i);
		if(syllableBlur.bySyllable1==byRhyme)
			byBlurRhyme[nRet++]=syllableBlur.bySyllable2;
		if(syllableBlur.bySyllable2==byRhyme)
			byBlurRhyme[nRet++]=syllableBlur.bySyllable1;
	}
	return nRet;
}
int CBlurMgr::GetSpellBlurs(BYTE byID[2],BYTE byBlurSpells[/*SIZE_SPELL*/][2])
{
	int nRet=0;
	for(int i=0;i<m_arBlurSpell.GetSize();i++)
	{
		BLURSPELL &spellBlur=m_arBlurSpell.ElementAt(i);
		if(memcmp(byID,spellBlur.bySpell1,2)==0)
			memcpy(byBlurSpells[nRet++],spellBlur.bySpell2,2);
		if(memcmp(byID,spellBlur.bySpell2,2)==0)
			memcpy(byBlurSpells[nRet++],spellBlur.bySpell1,2);
	}
	return nRet;
}


