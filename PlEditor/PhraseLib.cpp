//	PhraseLib.cpp: implementation of the CPLEditor class.
//	词库分为2部分：	1－经过树排序的词库;
//					2－每一个词在库中的偏移量；
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhraseLib.h"
#include "TxtReader.h"
#include "WordRate.h"

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
	m_funQueryRate= NULL;
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
	return wcscmp(pSour->szWord,pDest->szWord);
}

//-------------------------------------------------
//	往词库中添加一个新词汇,-1:dest phrase has existed and failed
//	BOOL bCheckNeed:检查是否存在的开关,在数据载入时不置为FALSE
//	BYTE byGroup:所属的词库组
//---------------------------------------------------
BOOL CPLEditor::AddWord(LPCWSTR pszWord,BYTE cLen/*=-1*/,BYTE byRate/*=0*/,BOOL bCheckNeed/*=TRUE*/,BYTE byGroup/*=0*/)
{
	if(byGroup>=m_arrGroup.size())
		return FALSE;
	int nLen = cLen;
	if(nLen==0xff) nLen=wcslen(pszWord);
	if(nLen>MAX_PHRASE)
		return FALSE;
	if(!CWordID::IsHanzi(pszWord[0]) || !CWordID::IsValidWord(pszWord[0]))
		return 0;
	cLen = (BYTE)nLen;

	PHRASE2 p={byGroup,byRate,cLen};
	wcsncpy(p.szWord,pszWord,cLen);
	p.szWord[cLen]=0;

	SStringW key(p.szWord,cLen);
	if(bCheckNeed)
	{
		SMap<SStringW,PHRASE2>::CPair *p = m_mapPhrase.Lookup(key);
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
	WCHAR szWord[MAX_PHRASE];
	fseek(f,sizeof(DWORD),SEEK_CUR);//跨过dwDataSize
	BYTE byGroup=0;
	if(m_funProgCB) m_funProgCB->OnStart(100);
	int nSegLen = dwCount/100;

	for(int j=0;j<m_arrGroup.size();j++)
	{
		m_arrGroup[j].dwCount=0;
	}
	for(DWORD i=0;i<dwCount;i++)
	{
		fread(&byGroup,1,1,f);
		fread(&byRate,1,1,f);
		fread(&cLen,1,1,f);
		fread(szWord,sizeof(WCHAR),cLen,f);
		AddWord(szWord,cLen,byRate,FALSE,byGroup);
		if(m_funProgCB) 
		{
			if(i%nSegLen==0)
				m_funProgCB->OnProg(i/nSegLen,100);
		}
	}
	if(m_funProgCB) m_funProgCB->OnEnd(true);
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
	if(!ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR3))
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
	int nSegLen = nCount/100;
	if(m_funProgCB) m_funProgCB->OnStart(100);

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
	for(UINT i=0;i<nCount;i++)
	{
		pdwOffset[i]=ftell(f)-lBegin;
		const PHRASE2 &p=pArr[i];
		fwrite(&p.byGroup,1,1,f);
		fwrite(&p.byRate,1,1,f);
		fwrite(&p.cLen,1,1,f);
		fwrite(p.szWord,sizeof(WCHAR),p.cLen,f);
	 	if(m_funProgCB) 
		{
			if(i%nSegLen==0)
				m_funProgCB->OnProg(i/nSegLen,100);
		}
	}
	//计算数据长度
	dwDataSize=ftell(f)-lBegin;
	if(m_funProgCB) m_funProgCB->OnEnd(false);
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
	ISDOC_WriteHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR3,dwID);
	WriteData(f);
	fclose(f);
	return TRUE;
}

void CPLEditor::SetProgCallBack(IProgListener *funCb)
{
	m_funProgCB=funCb;
}

//解析文本行
//LPCWSTR pszLine:输入文本行
//int &nBegin:词频开始位置
//int &nEnd:词结束位置
BOOL CPLEditor::ParseLine(LPCWSTR pszLine,int &nBegin,int &nEnd)
{
	int i=0;
	while(pszLine[i])
	{
		if(pszLine[i]=='\t' || pszLine[i]==0x20)
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

int CPLEditor::Import2Group(LPCTSTR pszFile,BYTE byMin, BYTE byMax,BYTE byDefault,BYTE iGroup/*=-1*/)
{
	CTxtReader f('#');
	int nRet = 0;
	if(f.Open(pszFile))
	{
		if(iGroup == 0xFF)
		{
			if(m_arrGroup.size()>=250)
				return 0;
			GROUPINFO groupInfo={0};
			wcscpy(groupInfo.szName,L"none");
			m_arrGroup.push_back(groupInfo);
			iGroup = m_arrGroup.size()-1;
		}else if (iGroup>250)
		{
			return 0;
		}
		
		if(m_funProgCB)
			m_funProgCB->OnStart(100);
		int nSegLen=f.getLength()/100;

		WCHAR szLine[300];
		BOOL bRead=f.getline(szLine,300);
		int nEnd,nBegin;
		BYTE byGroup=(BYTE)iGroup;
		while(bRead)
		{
			if(szLine[0]!='@')
			{
				BYTE byRate=0;
				if(ParseLine(szLine,nBegin,nEnd))
				{
					byRate=_wtoi(szLine+nBegin);
					if(byRate>MAX_RATE) byRate=MAX_RATE;
				}else
				{
					nEnd=wcslen(szLine);
					if(m_funQueryRate && nEnd<=MAX_PHRASE)
					{
						byRate = m_funQueryRate->QueryPhraseRate(szLine,nEnd);
						if(byRate==0) byRate=byDefault;
					}
				}
				if(byMin<=byRate && byRate<=byMax ) 
					nRet+=AddWord(szLine,nEnd,byRate,TRUE,byGroup)?1:0;
			}
			if(m_funProgCB)
			{
				if(f.getReadPos()%nSegLen == 0)
				m_funProgCB->OnProg(f.getReadPos()/nSegLen,100);
			}
			bRead = f.getline(szLine,300);
		}
		if(m_funProgCB)
			m_funProgCB->OnEnd(true);
	}
	return 0;
}

BOOL CPLEditor::ExportGroup(LPCTSTR pszFile,BYTE iGroup)
{
	FILE *f = _tfopen(pszFile,_T("wb"));
	if(!f)
		return FALSE;
	fwrite("\xff\xfe",1,2,f);//bom
	DWORD dwCount = m_mapPhrase.GetCount();
	int   nSegLen = dwCount/100;
	if(nSegLen==0) nSegLen = dwCount;
	if(m_funProgCB)
		m_funProgCB->OnStart(100);
	DWORD i=0;
	SPOSITION pos = m_mapPhrase.GetStartPosition();
	while(pos)
	{
		i++;
		if(m_funProgCB)
		{
			if(i%nSegLen == 0)
			{
				m_funProgCB->OnProg(i/nSegLen,100);
			}
		}
		const PHRASE2 & value=m_mapPhrase.GetNextValue(pos);
		if(value.byGroup == iGroup)
		{
			fwrite(value.szWord,sizeof(WCHAR),value.cLen,f);
			fwprintf(f,L"\t%d\r\n",(int)value.byRate);
		}
	}
	if(m_funProgCB)
		m_funProgCB->OnEnd(false);
	fclose(f);
	return TRUE;
}

BOOL CPLEditor::EraseGroup(BYTE iGroup)
{
	if(iGroup>=m_arrGroup.size())
		return FALSE;

	DWORD dwCount = m_mapPhrase.GetCount();
	int   nSegLen = dwCount/100;
	if(m_funProgCB)
		m_funProgCB->OnStart(100);
	DWORD i=0;
	SPOSITION pos = m_mapPhrase.GetStartPosition();
	while(pos)
	{
		i++;
		if(m_funProgCB)
		{
			if(i%nSegLen == 0)
			{
				m_funProgCB->OnProg(i/nSegLen,100);
			}
		}
		SPOSITION posPrev = pos;
		PHRASE2 & value=m_mapPhrase.GetNextValue(pos);
		if(value.byGroup == iGroup)
		{
			m_mapPhrase.RemoveAtPos(posPrev);
		}else if(value.byGroup>iGroup)
		{
			value.byGroup--;
		}
	}
	m_arrGroup.erase(m_arrGroup.begin()+iGroup);
	if(m_funProgCB)
		m_funProgCB->OnEnd(true);
	return TRUE;
}

void CPLEditor::SetQueryRate(IQueryRate * pQuery)
{
	m_funQueryRate = pQuery;
}



/////////////////////////////////////////////////////////////////
//	CPLReader
/////////////////////////////////////////////////////////////////

void CPLReader::LoadData(FILE *f)
{
	//数据组数
	fread(&m_byGroups,1,1,f);
	ASSERT(m_byGroups);
	m_pGroupInfo=(GROUPINFO*)malloc(sizeof(GROUPINFO)*m_byGroups);
	//词库组信息
	fread(m_pGroupInfo,sizeof(GROUPINFO),m_byGroups,f);
	for(BYTE i=0;i<m_byGroups;i++) m_dwCount+=m_pGroupInfo[i].dwCount;
	//数据区
	fread(&m_dwBufSize,sizeof(DWORD),1,f);
	m_pbyBuf=(BYTE*)malloc(m_dwBufSize);
	fread(m_pbyBuf,	1,m_dwBufSize,f);
	//索引表
	DWORD *pdwIndex=(DWORD*)malloc(m_dwCount*4);
	int nReaded=fread(pdwIndex,sizeof(DWORD),m_dwCount,f);
	m_arrIndex.Attach(pdwIndex,m_dwCount);
}

void CPLReader::WriteData(FILE *f)
{
	fseek(f,1,SEEK_CUR);								//数据组数
	fwrite(m_pGroupInfo,sizeof(GROUPINFO),m_byGroups,f);//更新词库组信息
	fseek(f,sizeof(DWORD),SEEK_CUR);				//数据区长度
	fwrite(m_pbyBuf,1,m_dwBufSize,f);				//数据区:更新词频
	fseek(f,m_dwCount*sizeof(DWORD),SEEK_CUR);		//索引数据区
}

//保存词频信息到文件
BOOL CPLReader::Save(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("r+b"));
	if(!f) return FALSE;
	DWORD dwID=0;
	if(!ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR3,&dwID))
	{
		fclose(f);
		return FALSE;
	}
	if(dwID != m_dwID)
	{//phrase lib had been changed after loaded.
		fclose(f);
		return FALSE;
	}
	WriteData(f);

	fclose(f);
	return TRUE;
}

#pragma pack(push,1)
struct GROUPINFO_V3
{//组信息
	char szName[50];	//名称
	char szEditor[50];	//编辑
	char szRemark[200];	//备注
	DWORD dwCount;		//词数
	BOOL  bValid;		//有效标志
};

struct PHRASE2_V3
{
	BYTE	byGroup;	//所在组
	BYTE	byRate;		//词频，初始时为0
	BYTE	cLen;		//词长
	char 	szWord[250+1];	//缓冲区
};
#pragma pack(pop)

BOOL ConvertPhraseLib2V4(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f) return FALSE;
	SISHEAD head={0};
	if(!ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2,NULL,&head) || head.wVersionMinor != SVCL_MINOR2)
	{
		fclose(f);
		return FALSE;
	}

	BYTE byGroup=0;
	//数据组数
	fread(&byGroup,1,1,f);
	ASSERT(byGroup);
	GROUPINFO_V3 *pGroupInfo=(GROUPINFO_V3*)malloc(sizeof(GROUPINFO_V3)*byGroup);
	//词库组信息
	fread(pGroupInfo,sizeof(GROUPINFO_V3),byGroup,f);
	DWORD dwCount=0;
	for(BYTE i=0;i<byGroup;i++) dwCount+=pGroupInfo[i].dwCount;
	//数据区
	DWORD dwBufSize=0;
	fread(&dwBufSize,sizeof(DWORD),1,f);
	LPBYTE pbyBuf=(BYTE*)malloc(dwBufSize);
	fread(pbyBuf,1,dwBufSize,f);
	//索引表
	DWORD *pdwIndex=(DWORD*)malloc(dwCount*4);
	int nReaded=fread(pdwIndex,sizeof(DWORD),dwCount,f);
	fclose(f);

	CPLEditor editor;
	//add groups
	for(BYTE i=0;i<byGroup;i++)
	{
		GROUPINFO info={0};
		MultiByteToWideChar(936,0,pGroupInfo[i].szName,strlen(pGroupInfo[i].szName),info.szName,50);
		MultiByteToWideChar(936,0,pGroupInfo[i].szEditor,strlen(pGroupInfo[i].szEditor),info.szEditor,50);
		MultiByteToWideChar(936,0,pGroupInfo[i].szRemark,strlen(pGroupInfo[i].szRemark),info.szRemark,200);
		char iGroup = editor.AddGroup(info.szName,info.szEditor,info.szRemark);
		editor.ValidGroup(iGroup,pGroupInfo[i].bValid);
	}
	for(DWORD i=0;i<dwCount;i++)
	{
		PHRASE2_V3 *p=(PHRASE2_V3*)(pbyBuf+pdwIndex[i]);
		WCHAR szBuf[MAX_PHRASE+1]={0};
		int nLen =MultiByteToWideChar(936,0,p->szWord,p->cLen,szBuf,MAX_PHRASE+1);
		if(nLen<=MAX_PHRASE)
		{
			editor.AddWord(szBuf,nLen,p->byRate,FALSE,p->byGroup);
		}
	}
	editor.Save(pszFileName);
	free(pGroupInfo);
	free(pdwIndex);
	free(pbyBuf);
	return TRUE;
}


BOOL CPLReader::CheckPhraseLib(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f) return FALSE;
	BOOL bValid=ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2);
	fclose(f);
	return bValid;
}

BOOL CPLReader::Load(LPCTSTR pszFileName)
{
	Free();
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f) return FALSE;
	SISHEAD head={0};
	if(!ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR2,&m_dwID,&head))
	{
		fclose(f);
		return FALSE;
	}
	if(head.wVersionMinor == SVCL_MINOR2)
	{//auto convert to SVCL_MINOR3
		fclose(f);
		ConvertPhraseLib2V4(pszFileName);
		return Load(pszFileName);
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
	BYTE i=0;
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
int CPLReader::QueryAssociate(LPCWSTR pszHead, BYTE cSize,CSArray<PPHRASE>  *pArray)
{
	if(cSize>MAX_WORDLEN) return 0;
	PHRASE p;
	p.cLen=cSize;
	wcsncpy(p.szWord,pszHead,cSize);
	DWORD dwBegin=m_arrIndex.SortFind((DWORD*)&p,PhraseFindCmp,(LPARAM)m_pbyBuf,0,SFT_HEAD);
	if(dwBegin!=-1)
	{
		DWORD dwEnd=m_arrIndex.SortFind((DWORD*)&p,PhraseFindCmp,(LPARAM)m_pbyBuf,dwBegin,SFT_TAIL);
		ASSERT(dwEnd!=-1);
		dwEnd = min(dwBegin+MAX_ASSO_CANDS,dwEnd);
		for(DWORD i=dwBegin;i<=dwEnd;i++)
		{
			if(!IsPhraseValid(i)) continue;
			PPHRASE pp=GetPhrase(i);
			if(pp->cLen>cSize) pArray->SortInsert(pp,TRUE,PhraseCmp,SFT_HEAD);
		}
	}
	return pArray->GetSize();
}

DWORD CPLReader::IsPhraseExist(LPCWSTR pszText,BYTE cSize)
{
	if(cSize==0) return -1;
	PHRASE p;
	p.cLen=cSize;
	wcsncpy(p.szWord,pszText,cSize);
	p.szWord[cSize]=0;
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

BOOL CPLReader::GroupValid(LPCWSTR pszGroupName, BOOL bValid)
{
	for (BYTE i = 0; i < m_byGroups; i++)
	{
		if (wcscmp(m_pGroupInfo[i].szName, pszGroupName) == 0)
		{
			m_pGroupInfo[i].bValid = bValid;
			return TRUE;
		}
	}
	return FALSE;
}

//找到一个词组头在词库中的范围
MYRANGE CPLReader::GetPhraseHeadRange(PHRASE *p,int nMin,int nMax)
{
	MYRANGE r={-1,-1};
	if(nMin==-1) nMin=0;
	if(nMax==-1) nMax=m_arrIndex.GetSize();
	r.nMin=m_arrIndex.SortFindEx((DWORD*)p,nMin,nMax,PhraseFindCmp,(LPARAM)m_pbyBuf,SFT_HEAD);
	if(r.nMin!=-1) r.nMax=m_arrIndex.SortFindEx((DWORD*)p,r.nMin,nMax,PhraseFindCmp,(LPARAM)m_pbyBuf,SFT_TAIL)+1;
	return r;
}

PPHRASE CPLReader::IsPhraseExist2(LPCWSTR pszText,BYTE cSize)
{
	DWORD dwIndex=IsPhraseExist(pszText,cSize);
	if(dwIndex==-1) return NULL;
	if(!IsPhraseValid(dwIndex)) return NULL;
	PPHRASE pp=GetPhrase(dwIndex);
	if(pp->cLen!=cSize) return NULL;
	return pp;
}
