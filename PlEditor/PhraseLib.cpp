//	PhraseLib.cpp: implementation of the CPLEditor class.
//	词库分为2部分：	1－经过树排序的词库;
//					2－每一个词在库中的偏移量；
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhraseLib.h"
#include "TxtLineReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPLEditor::CPLEditor()
{
	m_bModify=FALSE;
	m_funProgCB=NULL;
}

CPLEditor::~CPLEditor()
{
	Free();
}

void CPLEditor::Free()
{
	CGroupManager::Free();
	m_mapPhrase.RemoveAll();
	m_bModify=FALSE;
}

int Phrase2Cmp(void *ctx,const void *p1,const void *p2)
{
	const PHRASE2 *pSour=(const PHRASE2*)p1;
	const PHRASE2 *pDest = (const PHRASE2 *)p2;
	return strcmp(pSour->szWord,pDest->szWord);
}

//-------------------------------------------------
//	往词库中添加一个新词汇,-1:dest phrase has existed and failed
//	BOOL bCheckNeed:检查是否存在的开关,在数据载入时不置为FALSE
//	BYTE byGroup:所属的词库组
//---------------------------------------------------
BOOL CPLEditor::AddWord(LPCSTR pszWord,char cLen/*=-1*/,BYTE byRate/*=0*/,BOOL bCheckNeed/*=TRUE*/,BYTE byGroup/*=0*/)
{
	if(byGroup>=m_arrGroup.size()) return FALSE;
	if(cLen==-1) cLen=strlen(pszWord);
	if(cLen>MAX_WORDLEN) return FALSE;

	PHRASE2 p={byGroup,byRate,cLen};
	strncpy(p.szWord,pszWord,cLen);
	p.szWord[cLen]=0;

	SStringA key(p.szWord,cLen);
	if(bCheckNeed)
	{
		SMap<SStringA,PHRASE2>::CPair *p = m_mapPhrase.Lookup(key);
		if(p)
		{
			if(byGroup!=p->m_value.byGroup && byGroup<m_arrGroup.size())
			{//update group info.
				m_arrGroup[p->m_value.byGroup].dwCount--;
				m_arrGroup[byGroup].dwCount++;
				p->m_value.byGroup = byGroup;
				if(byRate != 0) 
					p->m_value.byRate = byRate;
			}
			return FALSE;
		}
	}
	m_mapPhrase[key]=p;
	m_arrGroup[byGroup].dwCount++;
	m_bModify=TRUE;
	return TRUE;
}


void CPLEditor::LoadData(FILE *f)
{
	CGroupManager::Read(f);
	DWORD dwCount=CGroupManager::GetCount();
	BYTE byRate;
	char cLen;
	char szWord[MAX_WORDLEN];
	fseek(f,sizeof(DWORD),SEEK_CUR);//跨过dwDataSize
	BYTE byGroup=0;
	if(m_funProgCB) m_funProgCB->OnStart(dwCount);
	for(int j=0;j<m_arrGroup.size();j++)
	{
		m_arrGroup[j].dwCount=0;
	}
	for(DWORD i=0;i<dwCount;i++)
	{
		fread(&byGroup,1,1,f);
		fread(&byRate,1,1,f);
		fread(&cLen,1,1,f);
		fread(szWord,1,cLen,f);
		AddWord(szWord,cLen,byRate,FALSE,byGroup);
		if(m_funProgCB) m_funProgCB->OnProg(i,dwCount);
	}
	if(m_funProgCB) m_funProgCB->OnEnd();
	m_bModify=FALSE;
}


//---------------------------------------------------------
//	format:head+indexPos+data+index
//---------------------------------------------------------
BOOL CPLEditor::Load(LPCTSTR pszFileName)
{
	Free();
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2))
	{
		fclose(f);
		return FALSE;
	}
	
	LoadData(f);

	fclose(f);
	return TRUE;
}

void CPLEditor::WriteData(FILE *f)
{
	CGroupManager::Write(f);//写入词库组信息
	DWORD dwDataSize=0;	//数据长度
	fwrite(&dwDataSize,sizeof(DWORD),1,f);
	//写入经过树排序的词库数据
	UINT nCount = m_mapPhrase.GetCount();
	PHRASE2 *pArr = (PHRASE2*) malloc(sizeof(PHRASE2)*nCount);
	SPOSITION pos = m_mapPhrase.GetStartPosition();
	int i=0;
	while(pos)
	{
		pArr[i++]=m_mapPhrase.GetNextValue(pos);
	}
	qsort_s(pArr,m_mapPhrase.GetCount(),sizeof(PHRASE2),Phrase2Cmp,NULL);

	DWORD *pdwOffset=(DWORD*)malloc(sizeof(DWORD)*nCount);
	LONG lBegin=ftell(f);
	if(m_funProgCB) m_funProgCB->OnStart(nCount);
	for(UINT i=0;i<nCount;i++)
	{
		pdwOffset[i]=ftell(f)-lBegin;
		const PHRASE2 &p=pArr[i];
		fwrite(&p.byGroup,1,1,f);
		fwrite(&p.byRate,1,1,f);
		fwrite(&p.cLen,1,1,f);
		fwrite(p.szWord,1,p.cLen,f);
	 	if(m_funProgCB) m_funProgCB->OnProg(i,nCount);
	}
	//计算数据长度
	dwDataSize=ftell(f)-lBegin;
	if(m_funProgCB) m_funProgCB->OnEnd();
	//写入索引表
	fwrite(pdwOffset,sizeof(DWORD),nCount,f);
	free(pdwOffset);
	//回写数据长度
	fseek(f,lBegin-sizeof(DWORD),SEEK_SET);
	fwrite(&dwDataSize,sizeof(DWORD),1,f);
	fseek(f,0,SEEK_END);
	
	free(pArr);
}

BOOL CPLEditor::Save(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("wb"));
	if(!f) return FALSE;
	DWORD dwID=GetTickCount();
	ISDOC_WriteHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2,dwID);
	WriteData(f);
	fclose(f);
	return TRUE;
}

void CPLEditor::SetProgCallBack(IProgListener *funCb)
{
	m_funProgCB=funCb;
}

//解析文本行
//LPCTSTR pszLine:输入文本行
//int &nBegin:词频开始位置
//int &nEnd:词结束位置
BOOL CPLEditor::ParseLine(LPCSTR pszLine,int &nBegin,int &nEnd)
{
	int i=0;
	while(pszLine[i])
	{
		if(pszLine[i]&0x80)
			i+=2;
		else if(pszLine[i]=='\t' || pszLine[i]==0x20)
		{
			nEnd=i;
			break;
		}else
		{
			i++;
		}
	}
	if(pszLine[i])
	{
		while(pszLine[i] && (pszLine[i]=='\t' || pszLine[i]==0x20)) i++;
		nBegin=i;
		return TRUE;
	}
	return FALSE;
}

int CPLEditor::Import2Group(LPCTSTR pszFile,BYTE byMin, BYTE byMax,BYTE iGroup/*=-1*/)
{
	CTxtLineReader f('#');
	int nRet = 0;
	if(f.Open(pszFile))
	{
		if(iGroup == 0xFF)
		{
			if(m_arrGroup.size()>=250)
				return 0;
			GROUPINFO groupInfo={0};
			strcpy(groupInfo.szName,"none");
			m_arrGroup.push_back(groupInfo);
			iGroup = m_arrGroup.size()-1;
		}else if (iGroup>250)
		{
			return 0;
		}
		
		if(m_funProgCB)
			m_funProgCB->OnStart(f.GetLength());

		char szLine[100];
		LPSTR pszBuf=f.ReadString(szLine,100);
		int nEnd,nBegin;
		BYTE byGroup=(BYTE)iGroup;
		while(pszBuf)
		{
			if(szLine[0]!='@')
			{
				BYTE byRate=0;
				if(ParseLine(szLine,nBegin,nEnd))
				{
					byRate=atoi(szLine+nBegin);
					if(byRate>MAX_RATE) byRate=MAX_RATE;
				}else
				{
					nEnd=strlen(szLine);
				}
				if(byMin<=byRate && byRate<=byMax ) 
					nRet+=AddWord(szLine,nEnd,byRate,TRUE,byGroup)?1:0;
			}
			if(m_funProgCB)
				m_funProgCB->OnProg(f.GetPos(),f.GetLength());
			pszBuf=f.ReadString(szLine,100);
		}
		if(m_funProgCB)
			m_funProgCB->OnEnd();
	}
	return 0;
}
