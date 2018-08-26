//	PhraseLib.cpp: implementation of the CPLEditor class.
//	词库分为2部分：	1－经过树排序的词库;
//					2－每一个词在库中的偏移量；
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhraseLib.h"

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
	m_arrPhrase.SetGrowBy(500);
	m_bModify=FALSE;
	m_funProgCB=NULL;
	m_cbParam=0;
}

CPLEditor::~CPLEditor()
{
	Free();
}

void CPLEditor::Free()
{
	m_arrPhrase.RemoveAll();
	m_arrGroup.RemoveAll();
	m_bModify=FALSE;
}

int Phrase2Cmp(PHRASE2 *pSour,PHRASE2 *pDest,LPARAM lParam)
{
	return strcmp(pSour->szWord,pDest->szWord);
}

//-------------------------------------------------
//	往词库中添加一个新词汇,-1:dest phrase has existed and failed
//	BOOL bCheckNeed:检查是否存在的开关,在数据载入时不置为FALSE
//	BYTE byGroup:所属的词库组
//---------------------------------------------------
BOOL CPLEditor::AddWord(LPCTSTR pszWord,char cLen/*=-1*/,BYTE byRate/*=0*/,BOOL bCheckNeed/*=TRUE*/,BYTE byGroup/*=0*/)
{
	if(byGroup>=m_arrGroup.GetSize()) return FALSE;
	if(cLen==-1) cLen=strlen(pszWord);
	if(cLen>MAX_WORDLEN) return FALSE;
	PHRASE2 p={byGroup,byRate,cLen};
	strncpy(p.szWord,pszWord,cLen);
	p.szWord[cLen]=0;
	if(bCheckNeed)
	{
		int nIdx=m_arrPhrase.SortFind(&p,Phrase2Cmp);
		if(nIdx!=-1) return FALSE;
	}
	m_arrPhrase.SortInsert(p,FALSE,Phrase2Cmp);
	m_arrGroup.GetPtAt(byGroup)->dwCount++;
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
	if(m_funProgCB) m_funProgCB(PT_MAX,dwCount,m_cbParam);
	CSArray<GROUPINFO> *pGroup=GetGroup();
	for(int j=0;j<pGroup->GetSize();j++)
	{
		pGroup->GetPtAt(j)->dwCount=0;
	}
	for(DWORD i=0;i<dwCount;i++)
	{
		fread(&byGroup,1,1,f);
		fread(&byRate,1,1,f);
		fread(&cLen,1,1,f);
		fread(szWord,1,cLen,f);
		AddWord(szWord,cLen,byRate,FALSE,byGroup);
		if(m_funProgCB) m_funProgCB(PT_PROG,i,m_cbParam);
	}
	if(m_funProgCB) m_funProgCB(PT_END,0,m_cbParam);
	m_bModify=FALSE;
}


//---------------------------------------------------------
//	format:head+indexPos+data+index
//---------------------------------------------------------
BOOL CPLEditor::Load(LPCTSTR pszFileName)
{
	Free();
	FILE *f=fopen(pszFileName,"rb");
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
	DWORD *pdwOffset=(DWORD*)malloc(sizeof(DWORD)*m_arrPhrase.GetSize());
	LONG lBegin=ftell(f);
	if(m_funProgCB) m_funProgCB(PT_MAX,m_arrPhrase.GetSize(),m_cbParam);
	for(UINT i=0;i<(UINT)m_arrPhrase.GetSize();i++)
	{
		pdwOffset[i]=ftell(f)-lBegin;
		PHRASE2 *p=m_arrPhrase.GetPtAt(i);
		fwrite(&p->byGroup,1,1,f);
		fwrite(&p->byRate,1,1,f);
		fwrite(&p->cLen,1,1,f);
		fwrite(p->szWord,1,p->cLen,f);
	 	if(m_funProgCB) m_funProgCB(PT_PROG,i,m_cbParam);
	}
	//计算数据长度
	dwDataSize=ftell(f)-lBegin;
	if(m_funProgCB) m_funProgCB(PT_END,0,m_cbParam);
	//写入索引表
	fwrite(pdwOffset,sizeof(DWORD),m_arrPhrase.GetSize(),f);
	free(pdwOffset);
	//回写数据长度
	fseek(f,lBegin-sizeof(DWORD),SEEK_SET);
	fwrite(&dwDataSize,sizeof(DWORD),1,f);
	fseek(f,0,SEEK_END);
}

BOOL CPLEditor::Save(LPCTSTR pszFileName)
{
	FILE *f=fopen(pszFileName,"wb");
	if(!f) return FALSE;
	DWORD dwID=GetTickCount();
	ISDOC_WriteHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2,dwID);
	WriteData(f);
	fclose(f);
	return TRUE;
}

/////////////////////////////////////////////////////////////////
//	CPLReader
/////////////////////////////////////////////////////////////////

void CPLReader::LoadData(FILE *f)
{
	fread(&m_byGroups,1,1,f);
	ASSERT(m_byGroups);
	m_pGroupInfo=(GROUPINFO*)malloc(sizeof(GROUPINFO)*m_byGroups);
	fread(m_pGroupInfo,sizeof(GROUPINFO),m_byGroups,f);
	for(BYTE i=0;i<m_byGroups;i++) m_dwCount+=m_pGroupInfo[i].dwCount;
	fread(&m_dwBufSize,sizeof(DWORD),1,f);
	m_pbyBuf=(BYTE*)malloc(m_dwBufSize);
	fread(m_pbyBuf,	1,m_dwBufSize,f);
	DWORD *pdwIndex=(DWORD*)malloc(m_dwCount*4);
	int nReaded=fread(pdwIndex,sizeof(DWORD),m_dwCount,f);
	m_arrIndex.Attach(pdwIndex,m_dwCount);
}

void CPLReader::WriteData(FILE *f)
{
	fseek(f,1,SEEK_CUR);
	fwrite(m_pGroupInfo,sizeof(GROUPINFO),m_byGroups,f);//更新词库组信息
	fseek(f,sizeof(DWORD),SEEK_CUR);
	fwrite(m_pbyBuf,1,m_dwBufSize,f);				//更新词频
}

//保存词频信息到文件
BOOL CPLReader::Save(LPCTSTR pszFileName)
{
	FILE *f=fopen(pszFileName,"r+b");
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2,&m_dwID))
	{
		fclose(f);
		return FALSE;
	}
	WriteData(f);

	fclose(f);
	return TRUE;
}

BOOL CPLReader::Load(LPCTSTR pszFileName)
{
	Free();
	FILE *f=fopen(pszFileName,"rb");
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2,&m_dwID))
	{
		fclose(f);
		return FALSE;
	}
	LoadData(f);

	fclose(f);
	return TRUE;
}

PPHRASE CPLReader::GetPhrase(DWORD index)
{
	ASSERT(index>=0&&index<m_dwCount); 
	return (PPHRASE)(m_pbyBuf+m_arrIndex[index]+1);
}

void CPLReader::Free()
{
	if(m_pbyBuf)
	{
		free(m_pbyBuf);
		m_pbyBuf=NULL;
	}
	if(m_arrIndex.GetSize()) free(m_arrIndex.Detach(NULL));
	m_dwBufSize=0;
	m_dwID=0;
	m_dwCount=0;
	if(m_pGroupInfo)
	{
		free(m_pGroupInfo);
		m_pGroupInfo=NULL;
	}
	m_byGroups=0;
}

//词长由小到大，词频由大小到
int PhraseCmp(PPHRASE *ppPhrase1,PPHRASE *ppPhrase2,LPARAM lParam)
{
	int nRet=(*ppPhrase2)->byRate-(*ppPhrase1)->byRate;
	if(nRet==0) nRet=(*ppPhrase1)->cLen-(*ppPhrase2)->cLen;
	return nRet;
}

int PhraseFindCmp(DWORD *pIndex1,DWORD *pIndex2,LPARAM lParam)
{
	BYTE *pBuf=(BYTE *)lParam;
	PPHRASE p1=(PPHRASE)pIndex1;
	PPHRASE p2=(PPHRASE)(pBuf+*pIndex2+1);
	int nRet=0;
	char i=0;
	while(i<p1->cLen && i<p2->cLen)
	{
		nRet=p1->szWord[i]-p2->szWord[i];
		if(nRet!=0) break;
		i++;
	}
	if(nRet==0 && p1->cLen>p2->cLen) nRet=p1->cLen-p2->cLen;
	return nRet;
}

//查询联想词组
int CPLReader::QueryAssociate(LPCTSTR pszHead, char cSize,CSArray<PPHRASE>  *pArray)
{
	PHRASE p;
	p.cLen=cSize;
	strncpy(p.szWord,pszHead,cSize);
	DWORD dwBegin=m_arrIndex.SortFind((DWORD*)&p,PhraseFindCmp,(LPARAM)m_pbyBuf,0,SFT_HEAD);
	if(dwBegin!=-1)
	{
		DWORD dwEnd=m_arrIndex.SortFind((DWORD*)&p,PhraseFindCmp,(LPARAM)m_pbyBuf,dwBegin,SFT_TAIL);
		ASSERT(dwEnd!=-1);
		for(DWORD i=dwBegin;i<=dwEnd;i++)
		{
			if(!IsPhraseValid(i)) continue;
			PPHRASE pp=GetPhrase(i);
			if(pp->cLen>cSize) pArray->SortInsert(pp,TRUE,PhraseCmp,SFT_HEAD);
		}
	}
	return pArray->GetSize();
}

DWORD CPLReader::IsPhraseExist(LPCTSTR pszText,char cSize)
{
	if(cSize==0) return -1;
	PHRASE p;
	p.cLen=cSize;
	strncpy(p.szWord,pszText,cSize);
	return m_arrIndex.SortFind((DWORD*)&p,PhraseFindCmp,(LPARAM)m_pbyBuf,0,SFT_HEAD);
}


BOOL CPLReader::IsPhraseValid(DWORD dwIndex)
{
	ASSERT(m_pGroupInfo);
	return m_pGroupInfo[GetPhraseGroup(dwIndex)].bValid;
}

BYTE CPLReader::GetPhraseGroup(DWORD dwIndex)
{
	return m_pbyBuf[m_arrIndex[dwIndex]];
}

void CPLReader::GroupValid(BYTE byGroup, BOOL bValid)
{
	ASSERT(m_pGroupInfo);
	m_pGroupInfo[byGroup].bValid=bValid;
}	
