// ForeignLangMap.cpp: implementation of the CForeignLangMap class.
//	外语信息映射表
//	整体数据结构：分类信息+外文词条+外文词条索引+逆向中文词条+中文词条索引
//	逆向中文词条: 逆序保存的中文词条信息+对应的外文词条序号
//	外文词条:外文词条+中文词条序号+附加信息。
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ForeignLangMap.h"
#include "inputstardoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFLMEditor::CFLMEditor()
{
	memset(&m_flmInfo,sizeof(FLMINFO),0);
	m_funProgCB=NULL;
}

CFLMEditor::~CFLMEditor()
{
	Free();
}

//添加一条记录
//char cGroup:组号
//LPCSTR pszFLBuf:英文关键字
//LPCSTR pszAddition:附加信息
//CStrArray *pArrCnBuf:相对应的英文词条
BOOL CFLMEditor::AddItem(char cGroup,LPCWSTR pszFLBuf, LPCWSTR pszAddition, CStringArray *pArrCnBuf)
{
	if(cGroup>=m_arrGroup.GetSize()) return FALSE;
	int		nCmp=-1;
	SLISTPOS pos=m_lstFLItem.GetHeadPosition();
	
	while(pos)
	{
		FLITEM *pItem=m_lstFLItem.GetPtAt(pos);
		nCmp=wcscmp(pszFLBuf,pItem->pszFLBuf);
		if(nCmp<=0) break;
		m_lstFLItem.GetNext(pos);
	}
	SLISTPOS posInsert=NULL;
	if(nCmp==0)
	{//关键词已经存在
		return FALSE;
	}else
	{
		FLITEM newItem={0};
		newItem.cGroup=cGroup;
		newItem.nCnCount=0;
		newItem.pPos=NULL;
		newItem.pszFLBuf=new WCHAR[wcslen(pszFLBuf)+1];
		wcscpy(newItem.pszFLBuf,pszFLBuf);
		if(pszAddition)
		{
			newItem.pszAddition=new WCHAR[wcslen(pszAddition)+1];
			wcscpy(newItem.pszAddition,pszAddition);
		}
		if(pos)
			posInsert=m_lstFLItem.InsertBefore(pos,newItem);
		else
			posInsert=m_lstFLItem.AddTail(newItem);
		m_arrGroup.GetPtAt(cGroup)->dwCount++;
	}
	ASSERT(posInsert);
	if(pArrCnBuf)
	{//有中文词条
		FLITEM *pItem=m_lstFLItem.GetPtAt(posInsert);
		pItem->pPos=(SLISTPOS*)malloc(sizeof(SLISTPOS)*pArrCnBuf->GetSize());
		for(int i=0;i<pArrCnBuf->GetSize();i++)
		{
			int nLen=pArrCnBuf->GetAt(i).GetLength();
			if(nLen>50) continue;
			LPCWSTR pszCnBuf=pArrCnBuf->GetAt(i);
			nCmp=-1;
			SLISTPOS posCn=m_lstCnItem.GetHeadPosition();
			while(posCn)
			{
				CNITEM *pCnItem=m_lstCnItem.GetPtAt(posCn);
				nCmp=wcscmp(pszCnBuf,pCnItem->pszCnBuf);
				if(nCmp<=0) break;
				m_lstCnItem.GetNext(posCn);
			}
			if(nCmp!=0)
			{//加入中文词条
				CNITEM cnItem={0};
				cnItem.pszCnBuf=new WCHAR[nLen+1];
				wcscpy(cnItem.pszCnBuf,pszCnBuf);
				if(nCmp<0)
					posCn=m_lstCnItem.InsertBefore(posCn,cnItem);
				else//if(posCn==NULL)
					posCn=m_lstCnItem.AddTail(cnItem);
			}
			BOOL bExist=FALSE;
			for(int j=0;j<pItem->nCnCount;j++)
			{
				if(pItem->pPos[j]==posCn)
				{
					bExist=TRUE;
					break;
				}
			}
			if(!bExist)
			{
				pItem->pPos[pItem->nCnCount++]=posCn;
				CNITEM *pCnItem=m_lstCnItem.GetPtAt(posCn);
				pCnItem->pPos=(SLISTPOS*)realloc(pCnItem->pPos,sizeof(SLISTPOS)*(pCnItem->nFLCount+1));
				pCnItem->pPos[pCnItem->nFLCount++]=posInsert;
			}
		}
	}
	return TRUE;
}



void CFLMEditor::Free()
{
	CGroupManager::Free();
	SLISTPOS pos=m_lstCnItem.GetHeadPosition();
	while(pos)
	{
		CNITEM cnItem=m_lstCnItem.GetNext(pos);
		delete []cnItem.pszCnBuf;
		if(cnItem.pPos) free(cnItem.pPos);
	}
	m_lstCnItem.RemoveAll();
	pos=m_lstFLItem.GetHeadPosition();
	while(pos)
	{
		FLITEM flItem=m_lstFLItem.GetNext(pos);
		delete []flItem.pszFLBuf;
		if(flItem.pszAddition) delete []flItem.pszAddition;
		if(flItem.pPos) free(flItem.pPos);
	}	
	m_lstFLItem.RemoveAll();
}

BOOL CFLMEditor::Load(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,SSID_FL,SVFL_MAJOR1,SVFL_MINOR1))
	{
		fclose(f);
		return FALSE;
	}
	fread(&m_flmInfo,sizeof(FLMINFO),1,f);
	CGroupManager::Read(f);
	fseek(f,sizeof(FLMHEAD),SEEK_CUR);
	DWORD i,dwCount;
	fread(&dwCount,sizeof(DWORD),1,f);
	if(m_funProgCB) m_funProgCB(PT_MAX,dwCount,m_cbParam);
	for(i=0;i<dwCount;i++)
	{
		FLITEM flItem={0};
		char cLen=0;
		fread(&cLen,1,1,f);
		flItem.pszFLBuf=new WCHAR[cLen+1];
		fread(flItem.pszFLBuf,2,cLen+1,f);
		fread(&cLen,1,1,f);
		if(cLen)
		{
			flItem.pszAddition=new WCHAR[cLen+1];
			fread(flItem.pszAddition,2,cLen+1,f);
		}else
		{
			fseek(f,1,SEEK_CUR);
		}
		fread(&flItem.cGroup,1,1,f);
		fread(&flItem.nCnCount,sizeof(int),1,f);
		flItem.pIndex=(int*)malloc(sizeof(int)*flItem.nCnCount);
		fread(flItem.pIndex,sizeof(int),flItem.nCnCount,f);
		m_lstFLItem.AddTail(flItem);
		if(m_funProgCB) m_funProgCB(PT_PROG,i,m_cbParam);
	}
	if(m_funProgCB) m_funProgCB(PT_END,0,m_cbParam);
	fseek(f,dwCount*sizeof(DWORD),SEEK_CUR);
	fread(&dwCount,sizeof(DWORD),1,f);
	if(m_funProgCB) m_funProgCB(PT_MAX,dwCount,m_cbParam);
	for( i=0;i<dwCount;i++)
	{
		CNITEM cnItem={0};
		char cLen=0;
		fread(&cLen,1,1,f);
		cnItem.pszCnBuf=new WCHAR[cLen+1];
		fread(cnItem.pszCnBuf,2,cLen+1,f);
		fread(&cnItem.nFLCount,sizeof(int),1,f);
		cnItem.pIndex=(int*)malloc(sizeof(int)*cnItem.nFLCount);
		fread(cnItem.pIndex,sizeof(int),cnItem.nFLCount,f);
		m_lstCnItem.AddTail(cnItem);
		if(m_funProgCB) m_funProgCB(PT_PROG,i,m_cbParam);
	}	
	if(m_funProgCB) m_funProgCB(PT_END,0,m_cbParam);
	fclose(f);
	//update index data to position data
	SLISTPOS pos=m_lstFLItem.GetHeadPosition();
	i=0;
	if(m_funProgCB) m_funProgCB(PT_MAX,m_lstFLItem.GetCount(),m_cbParam);
	while(pos)
	{
		FLITEM *pflItem=m_lstFLItem.GetPtAt(pos);
		int *pIndex=pflItem->pIndex;
		pflItem->pPos=(SLISTPOS*)malloc(pflItem->nCnCount*sizeof(SLISTPOS));
		for(int j=0;j<pflItem->nCnCount;j++)
		{
			pflItem->pPos[j]=m_lstCnItem.Index2Pos(pIndex[j]);
		}
		free(pIndex);
		m_lstFLItem.GoNext(pos);
		i++;
		if(m_funProgCB) m_funProgCB(PT_PROG,i,m_cbParam);
	}
	if(m_funProgCB) m_funProgCB(PT_END,0,m_cbParam);
	pos=m_lstCnItem.GetHeadPosition();
	i=0;
	if(m_funProgCB) m_funProgCB(PT_MAX,m_lstCnItem.GetCount(),m_cbParam);
	while(pos)
	{
		CNITEM *pcnItem=m_lstCnItem.GetPtAt(pos);
		int *pIndex=pcnItem->pIndex;
		pcnItem->pPos=(SLISTPOS*)malloc(sizeof(SLISTPOS)*pcnItem->nFLCount);
		for(int j=0;j<pcnItem->nFLCount;j++)
		{
			pcnItem->pPos[j]=m_lstFLItem.Index2Pos(pIndex[j]);
		}
		free(pIndex);
		m_lstCnItem.GoNext(pos);
		i++;
		if(m_funProgCB) m_funProgCB(PT_PROG,i,m_cbParam);
	}
	if(m_funProgCB) m_funProgCB(PT_END,0,m_cbParam);
	return TRUE;
}

BOOL CFLMEditor::Save(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("wb"));
	if(!f) return FALSE;
	ISDOC_WriteHeader(f,SSID_FL,SVFL_MAJOR1,SVFL_MINOR1);

	fwrite(&m_flmInfo,sizeof(FLMINFO),1,f);
	CGroupManager::Write(f);
	FLMHEAD head={0};
	LONG lPos=ftell(f);
	fwrite(&head,sizeof(FLMHEAD),1,f);

	DWORD dwCount=m_lstFLItem.GetCount();
	fwrite(&dwCount,sizeof(DWORD),1,f);
	if(m_funProgCB) m_funProgCB(PT_MAX,dwCount,m_cbParam);

	DWORD *pdwOffset=(DWORD*)malloc(dwCount*sizeof(DWORD));
	DWORD iItem=0;
	LONG lBegin=ftell(f);
	SLISTPOS pos=m_lstFLItem.GetHeadPosition();
	while(pos)
	{
		pdwOffset[iItem++]=ftell(f)-lBegin;
		FLITEM flItem=m_lstFLItem.GetNext(pos);
		char cLen=wcslen(flItem.pszFLBuf);
		fwrite(&cLen,1,1,f);
		fwrite(flItem.pszFLBuf,2,cLen+1,f);
		cLen=0;
		if(flItem.pszAddition) cLen=wcslen(flItem.pszAddition);
		fwrite(&cLen,1,1,f);
		if(cLen)
			fwrite(flItem.pszAddition,2,cLen+1,f);
		else
			fwrite(L"\0",2,1,f);
		fwrite(&flItem.cGroup,1,1,f);
		fwrite(&flItem.nCnCount,sizeof(int),1,f);
		for(int i=0;i<flItem.nCnCount;i++)
		{
			int nIndex=m_lstCnItem.Pos2Index(flItem.pPos[i]);
			fwrite(&nIndex,sizeof(int),1,f);
		}
		if(m_funProgCB) m_funProgCB(PT_PROG,iItem,m_cbParam);
	}
	if(m_funProgCB) m_funProgCB(PT_END,0,m_cbParam);

	head.dwFlBodySize=ftell(f)-lBegin+sizeof(DWORD);
	head.dwFlIndexSize=dwCount*sizeof(DWORD);
	fwrite(pdwOffset,sizeof(DWORD),dwCount,f);

	free(pdwOffset);

	dwCount=m_lstCnItem.GetCount();
	fwrite(&dwCount,sizeof(DWORD),1,f);

	if(m_funProgCB) m_funProgCB(PT_MAX,dwCount,m_cbParam);
	lBegin=ftell(f);
	pdwOffset=(DWORD*)malloc(dwCount*sizeof(DWORD));
	iItem=0;
	pos=m_lstCnItem.GetHeadPosition();
	while(pos)
	{
		pdwOffset[iItem++]=ftell(f)-lBegin;
		CNITEM cnItem=m_lstCnItem.GetNext(pos);
		char cLen=wcslen(cnItem.pszCnBuf);
		fwrite(&cLen,1,1,f);
		fwrite(cnItem.pszCnBuf,2,cLen+1,f);
		fwrite(&cnItem.nFLCount,sizeof(int),1,f);
		for(int i=0;i<cnItem.nFLCount;i++)
		{
			int nIndex=m_lstFLItem.Pos2Index(cnItem.pPos[i]);
			fwrite(&nIndex,sizeof(int),1,f);
		}
		if(m_funProgCB) m_funProgCB(PT_PROG,iItem,m_cbParam);
	}
	if(m_funProgCB) m_funProgCB(PT_END,0,m_cbParam);
	head.dwCnBodySize=ftell(f)-lBegin+sizeof(DWORD);
	head.dwCnIndexSize=dwCount*sizeof(DWORD);
	fwrite(pdwOffset,sizeof(DWORD),dwCount,f);
	free(pdwOffset);
	fseek(f,lPos,SEEK_SET);
	fwrite(&head,sizeof(FLMHEAD),1,f);
	fclose(f);
	return TRUE;
}	

void CFLMEditor::SetInfo(LPCWSTR pszName,LPCWSTR pszAddition,LPCWSTR pszAddFont)
{
	if(wcslen(pszName)>49 || wcslen(pszAddition)>40 ||wcslen(pszAddFont)>LF_FACESIZE) return;
	wcscpy(m_flmInfo.szName,pszName);
	wcscpy(m_flmInfo.szAddition,pszAddition);
	wcscpy(m_flmInfo.szAddFont,pszAddFont);
}

