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
#include "SArray.hpp"

using namespace std;

#define FN_PLSYS	"default.spl"

#define SSID_CL		'LC'
#define SVCL_MAJOR1	2005
#define SVCL_MINOR2	2
#define SVCL_MINOR3	3	//change to unicode

#define FLAG_INDEX		0xAA	//索引表标志
#define MAX_WILDMATCH	100		//万能键时最大的重码数
#define MAX_RATE		250		//允许的最大词频，其它词频数据有特定意义
#define MAX_PHRASE		250		//max phrase length
#define MAX_ASSO_CANDS	100		//联想词最大数
#define RATE_DELETE		255

//#define MAX_WORDLEN		30		//same as iscomm.h
typedef struct tagPHRASE2
{
	BYTE	byGroup;	//所在组
	BYTE	byRate;		//词频，初始时为0
	BYTE	cLen;		//词长
	WCHAR 	szWord[MAX_PHRASE+1];	//缓冲区
}PHRASE2,*PPHRASE2;

typedef struct tagPHRASEINFO
{
	BYTE	byGroup;	//所在组
	BYTE	byRate;		//词频，初始时为0
}PHRASEINFO;

struct IProgListener{
	virtual void OnStart(DWORD dwMax) = 0;
	virtual void OnProg(DWORD dwProg,DWORD dwMax) = 0;
	virtual void OnEnd(bool bUpdateUI) = 0;
};

class CPLEditor  :public CGroupManager
{
public:
	struct IQueryRate
	{
		virtual BYTE QueryPhraseRate(LPCWSTR pszPhrase,BYTE byLen) = 0;
	};

	CPLEditor();
	virtual ~CPLEditor();
	BOOL Save(LPCTSTR pszFileName);
	BOOL Load(LPCTSTR pszFileName);
	void Free();
	void SetProgCallBack(IProgListener *funCb);
	void SetQueryRate(IQueryRate * pQuery );
	BOOL AddWord(LPCWSTR pszWord,BYTE cLen=0xff,BYTE byRate=0,BOOL bCheckNeed=TRUE,BYTE byGroup=0);
	void LoadData(FILE *f);
	void WriteData(FILE *f);
	int Import2Group(LPCTSTR pszFile,BYTE byRateMin, BYTE byRateMax,BYTE byDefault,BYTE iGroup=0xFF);
	BOOL ExportGroup(LPCTSTR pszFile,BYTE iGroup);
	BOOL EraseGroup(BYTE iGroup);
private:
	BOOL ParseLine(LPCWSTR pszLine,int &nBegin,int &nEnd);

	SMap<SStringW,PHRASE2> m_mapPhrase;
	BOOL			m_bModify;
	IProgListener * m_funProgCB;	//进度回调
	IQueryRate	  * m_funQueryRate;
};

typedef struct tagMYRANGE
{
	int	nMin;
	int nMax;
}MYRANGE;

typedef struct tagPHRASE{
	BYTE	byRate;		//词频，初始时为0
	BYTE	cLen;		//词长
	WCHAR 	szWord[MAX_PHRASE+1];	//缓冲区
}PHRASE,*PPHRASE;

class CPLReader
{
public:

	CPLReader()
	{
		m_pbyBuf=NULL;
		m_dwBufSize=0;
		m_dwCount=0;
		m_byGroups=0;
		m_pGroupInfo=NULL;
	}

	~CPLReader()
	{
		Free();
	}
	static BOOL CheckPhraseLib(LPCTSTR pszFileName);

	BOOL Load(LPCTSTR pszFileName);
	void LoadData(FILE *f);
	BOOL Save(LPCTSTR pszFileName);
	void WriteData(FILE *f);
	void Free();
	void GroupValid(BYTE byGroup,BOOL bValid);
	BOOL GroupValid(LPCWSTR pszGroupName, BOOL bValid);
	MYRANGE GetPhraseHeadRange(PHRASE *p,int nMin=-1,int nMax=-1);
	BYTE GetPhraseGroup(DWORD dwIndex);
	BOOL IsPhraseValid(DWORD dwIndex);
	PPHRASE GetPhrase(DWORD index);
	LPBYTE GetBuf(){return m_pbyBuf;}
	DWORD GetWords(){return m_dwCount;}
	int QueryAssociate(LPCWSTR pszHead, BYTE cSize,CSArray<PPHRASE> *pArray);

	PPHRASE IsPhraseExist2(LPCWSTR pszText,BYTE cSize);

	DWORD IsPhraseExist(LPCWSTR pszText,BYTE cSize);

	DWORD			m_dwID;		//词库的ID，编辑词库时用GetTickCount生成
	GROUPINFO		*	m_pGroupInfo;//词库组
	BYTE			m_byGroups;	//词库组数量
private:
	DWORD			m_dwCount;
	BYTE		*	m_pbyBuf;	//数据区
	DWORD			m_dwBufSize;
	CSArray<DWORD>	m_arrIndex;	//数据区的索引表
};

#endif // !defined(AFX_PhraseLIB_H__7857A461_471F_4036_BE8A_7261E48846DB__INCLUDED_)
