// PhraseLib.h: interface for the CPLEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PhraseLIB_H__7857A461_471F_4036_BE8A_7261E48846DB__INCLUDED_)
#define AFX_PhraseLIB_H__7857A461_471F_4036_BE8A_7261E48846DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "inputstardoc.h"
#include "GroupManager.h"
#include <string>
#include <vector>
using namespace std;

#define FN_PLSYS	"default.spl"

#define SSID_CL		'LC'
#define SVCL_MAJOR1	2005
#define SVCL_MINOR2	2

#define FLAG_INDEX		0xAA	//索引表标志
#define MAX_WILDMATCH	100		//万能键时最大的重码数
#define MAX_RATE		250		//允许的最大词频，其它词频数据有特定意义

#define MAX_WORDLEN		30		//same as iscomm.h
typedef struct tagPHRASE2
{
	BYTE	byGroup;	//所在组
	BYTE	byRate;		//词频，初始时为0
	BYTE	cLen;		//词长
	char 	szWord[MAX_WORDLEN+1];	//缓冲区
}PHRASE2,*PPHRASE2;

typedef struct tagPHRASEINFO
{
	BYTE	byGroup;	//所在组
	BYTE	byRate;		//词频，初始时为0
}PHRASEINFO;

struct IProgListener{
	virtual void OnStart(DWORD dwMax) = 0;
	virtual void OnProg(DWORD dwProg,DWORD dwMax) = 0;
	virtual void OnEnd() = 0;
};

class CPLEditor  :public CGroupManager
{
public:
	CPLEditor();
	virtual ~CPLEditor();
	BOOL Save(LPCTSTR pszFileName);
	BOOL Load(LPCTSTR pszFileName);
	void Free();
	void SetProgCallBack(IProgListener *funCb);
	BOOL AddWord(LPCSTR pszWord,char cLen=-1,BYTE byRate=0,BOOL bCheckNeed=TRUE,BYTE byGroup=0);
	void LoadData(FILE *f);
	void WriteData(FILE *f);
	int Import2Group(LPCTSTR pszFile,BYTE byRateMin, BYTE byRateMax,BYTE iGroup=0xFF);
private:
	BOOL ParseLine(LPCSTR pszLine,int &nBegin,int &nEnd);

	SMap<SStringA,PHRASE2> m_mapPhrase;
	BOOL			m_bModify;
	IProgListener * m_funProgCB;	//进度回调
};

#endif // !defined(AFX_PhraseLIB_H__7857A461_471F_4036_BE8A_7261E48846DB__INCLUDED_)
