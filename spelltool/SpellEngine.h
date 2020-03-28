// SpellEngine.h: interface for the CSpellEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPELLENGINE_H__792C3673_A29B_46E6_BB16_BFF5C19D5FE2__INCLUDED_)
#define AFX_SPELLENGINE_H__792C3673_A29B_46E6_BB16_BFF5C19D5FE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "inputstardoc.h"
#include "spelldata.h"
#include "spellmap.h"
#include "sarray.hpp"

#define FLAG_SPELLENGINE	'PS'
#define SPVER_MAJON			2007
#define SPVER_MINOR			1
#define SPVER_MINOR2		2 //unicode

#define NAME_SPELL			_T("spell.dat")

typedef struct tagPPCONTEXT
{
	DWORD dwIndex;	//在上下文列表中的索引
	char  cLocate;	//多音字的位置
}PPCONTEXT;

typedef struct tagPPCONTEXT2
{
	char cLocate;
	WCHAR szPhrase[50];
}PPCONTEXT2;

typedef struct tagPHONEINFO
{
	BYTE byPhone[2];	//读音
	union
	{
		CSArray<PPCONTEXT> *pArrCntxt;//上下文表
		CSArray<PPCONTEXT2> *pArrCntxt2;//第二种形式的上下文 
	};
}PHONEINFO;

#define MAX_WORDPHONE	6	//一个汉字最多的读音数
typedef struct tagWORDPHONE
{
	WCHAR wWord;		//汉字
	BYTE byPhone[2];	//汉字的默认读音
	PHONEINFO pi[MAX_WORDPHONE];	//该字的多音字信息
	char cpp;			//该字的多音字个数
}WORDPHONE;

typedef struct tagWString
{
	char cLen;
	WCHAR szBuf[50];
}WString;

typedef struct tagAString
{
	char cLen;
	char szBuf[50];
}AString;

#define MAX_SENTPP	10	//一句中最多包含多音字个数
#include "strarray.h"
class CSpellEngine  
{
	typedef struct tagSPELLPHRASE
	{
		WCHAR szBuf[50];		//词组
		char cLen;			//词组长度
		char cPPSize;		//多音字个数
		char cPPPos[MAX_SENTPP];	//每个多音字的位置
		char cPPSp[MAX_SENTPP][7];	//每个多音字的读音
	}SPELLPHRASE;



public:
	CSpellEngine();
	virtual ~CSpellEngine();

	BOOL SaveSpellMap(LPCTSTR pszFileName);
	BOOL GetWordSpell(WCHAR wChar,char *pszSpell);
	WORDPHONE * GetWordPhone(WCHAR wChar);

	
	BOOL Open(LPCTSTR pszFileName);
	BOOL Save(LPCTSTR pszFileName);

	int GetPhraseSpell(LPCWSTR pszPhrase,int nLen,char szSpell[][7]);
	int Context_Export(LPCTSTR pszFileName);
	int Context_Import(LPCTSTR pszFileName);
#ifdef SPELLENGINE_EDITOR
	int ImportWord2Spell(LPCTSTR pszFileName);
	int ImportSpell2Word(LPCTSTR pszFileName);
	BOOL ImportWordRate(LPCTSTR pszFileName);
#endif// SPELLENGINE_EDITOR

	int MakeSpell(LPCWSTR pszSent,int nLen,BYTE szSpellIDs[][2],int nSize,BOOL bPolyPhoneDef=TRUE);
	int MakeSpell(LPCWSTR pszSent, int nLen,char szSpells[][7],int nSize,BOOL bPolyPhoneDef=TRUE);


	CSpellMap		m_spellMap;		//拼音到汉字
	CSArray<WORDPHONE> m_arrWord;	//汉字到拼音信息
	CSArray<WString>	m_arrContext;	//上下文表
	CWStrArray	m_arrCntxtRemark;	//上下文备注，可以包括，修改时间，修改人等
private:
	BOOL CalcPolyPhone(LPCWSTR pszSent,int nLen,int iWord,BYTE byPhone[2],BOOL bPolyPhoneDef=TRUE);
	static void MakeSpellPhrase(LPCWSTR pszLine,SPELLPHRASE *pPhrase);
};
int WordPhoneCmp(WORDPHONE *spWord1,WORDPHONE *spWord2,LPARAM lParam);
int TextCmp(WString *pt1,WString *pt2,LPARAM lParam);
#endif // !defined(AFX_SPELLENGINE_H__792C3673_A29B_46E6_BB16_BFF5C19D5FE2__INCLUDED_)
