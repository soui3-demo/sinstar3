#include "stdafx.h"
#include "skeytreeEx.h"

//////////////////////////////////////////////////////////////
//	CSBufKeyTreeEx Implement
//////////////////////////////////////////////////////////////

CSBufKeyTreeEx::CSBufKeyTreeEx()
{
	m_funReadData=NULL;
	m_funIsLeaf=NULL;
	m_pBuffer=NULL;
	m_dwBufSize=0;
}

CSBufKeyTreeEx::~CSBufKeyTreeEx()
{
	Free();
}

//--------------------------------------------------------------------
//	搜索树中的关键字，不管是叶子节点还是分叉节点
//	要判断是不是叶子节点可以调用IsLeafNode
//--------------------------------------------------------------------
BKT_NODEEX * CSBufKeyTreeEx::IsKeyExist(BYTE *byKey, int nLen)
{
	ASSERT(m_pBuffer);
	BKT_NODEEX root=GetRoot();
	BKT_NODEEX *pRetNode=NULL;
	BKT_NODEEX *pParentNode=&root;
	BKT_NODEEX *pChildNode=(BKT_NODEEX*)(m_pBuffer+2);
	for(int i=0;i<nLen;i+=m_cKeySize)
	{
		if(!(pRetNode=FindKeyNode(pChildNode,pParentNode->wChilds,byKey+i)))
			return NULL;
		else
		{
			pParentNode=pRetNode;
			pChildNode=(BKT_NODEEX*)(m_pBuffer+pRetNode->dwPosChild);
		}
	}
	return pRetNode;
}

BOOL CSBufKeyTreeEx::EnumTreeNode(PBKT_NODEEX pNode,CBENUMTREEEX funEnumTree,LPARAM lParam,int iLevel,BYTE *byKeyHead)
{
	BOOL bContinue=TRUE;
	PBKT_NODEEX pNodeChild=GetChildHead(pNode);
	for(WORD i=0;i<pNode->wChilds;i++)
	{
		if(byKeyHead) memcpy(byKeyHead+iLevel*m_cKeySize,pNode->byKey,m_cKeySize);
		if(IsLeafNode(pNodeChild)) bContinue=funEnumTree(this,pNodeChild,iLevel,byKeyHead,lParam);
		if(bContinue) bContinue=EnumTreeNode(pNodeChild,funEnumTree,lParam,iLevel+1,byKeyHead);
		if(!bContinue) break;
	}
	return bContinue;
}

void CSBufKeyTreeEx::EnumKeyNode(PKEYEQUALEX pKeyEqual,int nSize,CBENUMKEYEX funEnumKey,LPARAM lParam,BOOL bDeepFirst)
{
	ASSERT(nSize>=1);
	BKT_NODEEX root=GetRoot();
	BYTE *pbyKeyArray=new BYTE[m_cKeySize*nSize];
	_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,&root,0,pbyKeyArray,bDeepFirst);
	delete []pbyKeyArray;
}

BOOL CSBufKeyTreeEx::_EnumKeyNode(PKEYEQUALEX pKeyEqual,int nSize,CBENUMKEYEX funEnumKey,LPARAM lParam,PBKT_NODEEX pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst)
{
	ASSERT(nSize>=1 && iLevel<nSize);
	int nLeftSiblings=(int)pParent->wChilds;
	int nCount=pKeyEqual[iLevel].wCount;
	if(nCount==0) nCount=nLeftSiblings;

	BOOL bContinue=FALSE;
	PBKT_NODEEX pNodeChild=GetChildHead(pParent);
	PBKT_NODEEX pNodeDest=NULL;
	for(int i=0;i<nCount && nLeftSiblings>0 ;i++)
	{
		if(pKeyEqual[iLevel].wCount!=0)
			pNodeDest=FindKeyNode(pNodeChild,nLeftSiblings,pKeyEqual[iLevel].pbyKeys+i*m_cKeySize);
		else
			pNodeDest=pNodeChild;
		if(pNodeDest)
		{
			memcpy(pbyKeyArray+iLevel*m_cKeySize,pNodeDest->byKey,m_cKeySize);
			if(bDeepFirst)
			{
				if(iLevel+1<nSize) bContinue=_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
				if(bContinue && IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
			}else
			{
				if(IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
				if(bContinue && iLevel+1<nSize) bContinue=_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
			}
			nLeftSiblings-=NodeSub(pNodeDest,pNodeChild)+1;
			pNodeChild=NextNode(pNodeDest);
		}
		if(!bContinue) break;
	}
	return bContinue;
}


//--------------------------------------------------------------------
//	从排好序的兄弟结点中用两分法查找关键字所在节点
//--------------------------------------------------------------------
PBKT_NODEEX  CSBufKeyTreeEx::FindKeyNode(BKT_NODEEX *pSibling, WORD wSiblings, BYTE *pbyKey)
{
	if(wSiblings==0)
		return NULL;
	if(wSiblings==1)
	{
		if(memcmp(pSibling->byKey,pbyKey,m_cKeySize)==0)
			return pSibling;
		else
			return NULL;
	}else if(wSiblings==2)
	{
		BKT_NODEEX *pNext=NextNode(pSibling);
		if(memcmp(pSibling->byKey,pbyKey,m_cKeySize)==0)
			return pSibling;
		else if(memcmp(pNext->byKey,pbyKey,m_cKeySize)==0)
			return pNext;
		else
			return NULL;
	}else
	{
		WORD wHalf=wSiblings/2;
		BKT_NODEEX *pMiddle=NextNode(pSibling,wHalf);
		int nCmp=memcmp(pMiddle->byKey,pbyKey,m_cKeySize);
		if(nCmp==0)
			return pMiddle;
		else if(nCmp>0)
			return FindKeyNode(pSibling,wHalf,pbyKey);
		else//if(nCmp<0)
			return FindKeyNode(NextNode(pMiddle,1),wSiblings-wHalf-1,pbyKey);
	}
}

//-----------------------------------------------------------------
//	匹配探测
//	BYTE *pbyKeys:要匹配的关键字串
//	int nSize:关键字串长度
//	HSTREEITEM *pMatchItem:最后匹配的节点
//	return:完成匹配的深度
//-----------------------------------------------------------------
int CSBufKeyTreeEx::MatchTestMax(BYTE *pbyKeys, int nSize ,PBKT_NODEEX *pMatchItem/*=NULL*/)
{
	BKT_NODEEX root=GetRoot();
	BKT_NODEEX * hsParent=&root;
	BKT_NODEEX * hsNode=NULL,*hsGet=NULL;
	int nDeep=0;
	for(int i=0;i<nSize;i+=m_cKeySize)
	{
		hsNode=FindKeyNode(hsNode,hsParent->wChilds,pbyKeys+i);
		if(!hsNode) break;
		hsParent=hsNode;
		hsGet=hsNode;
		nDeep++;
	}
	if(pMatchItem) *pMatchItem=hsGet;
	return nDeep;
}

//-------------------------------------------------------------------
//	搜索第一个实现匹配的叶子节点
//-------------------------------------------------------------------
PBKT_NODEEX CSBufKeyTreeEx::FindNextLeaf(BKT_NODEEX * hsParent,BYTE *pbyKeys, int nSize,int *pDeep)
{
	ASSERT(m_funIsLeaf);
	BKT_NODEEX * hsNode=hsParent;
	int i=0;
	while(i<nSize)
	{
		hsNode=FindKeyNode(hsNode,hsNode->wChilds,pbyKeys+i);
		if(!hsNode) return NULL;
		if(IsLeafNode(hsNode))
		{
			if(pDeep) *pDeep=i/m_cKeySize;
			return hsNode;
		}
		i+=m_cKeySize;
	}
	return NULL;
}


//---------------------------------------------------------------
//	载入数据
//---------------------------------------------------------------
BOOL CSBufKeyTreeEx::LoadData(FILE *f, BYTE byFlag)
{
	DWORD dwSize=0;
	fread(&dwSize,sizeof(DWORD),1,f);
	BYTE byFlagGet=0;
	fread(&byFlagGet,1,1,f);
	if(byFlag!=byFlagGet) return FALSE;
	fread(&m_cKeySize,1,1,f);
	m_pBuffer=(BYTE *)HeapAlloc(GetProcessHeap(),0,dwSize);
	if(!m_pBuffer) return FALSE;
	DWORD dwReaded=fread(m_pBuffer,1,dwSize,f);
	if(dwReaded!=dwSize)
	{
		Free();
		return FALSE;
	}
	m_dwBufSize=dwSize;
	return TRUE;
}


//---------------------------------------------------------------
//	载入数据
//---------------------------------------------------------------
BOOL CSBufKeyTreeEx::WriteData(FILE *f, BYTE byFlag)
{
	fwrite(&m_dwBufSize,sizeof(DWORD),1,f);
	fwrite(&byFlag,1,1,f);
	fwrite(&m_cKeySize,1,1,f);
	fwrite(m_pBuffer,1,m_dwBufSize,f);
	return TRUE;
}

//--------------------------------------------------------
//	释放内存
//--------------------------------------------------------
void CSBufKeyTreeEx::Free()
{
	if(m_pBuffer)
	{
		HeapFree(GetProcessHeap(),0,m_pBuffer);
		m_pBuffer=NULL;
		m_dwBufSize=0;
	}
}

//////////////////////////////////////////////////////////////
//	CSKeyTreeEx Implement
//////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CSKeyTreeEx::Dump(HSTREEITEM hsNode,int level)
{
	char tab[6]="     ";
	if(hsNode!=STVI_ROOT)
	{
		KEYDATAEX *pKEYDATAEX=GetItem(hsNode);
		for(int i=0;i<level;i++)	TRACE(tab);
		for(int j=0;j<m_cKeySize;j++)
			TRACE("%02x",pKEYDATAEX->byKey[j]);
		TRACE(":%08x\r\n",pKEYDATAEX->data);
	}
	HSTREEITEM hsChild=GetChildItem(hsNode);
	if(hsChild){
		Dump(hsChild,++level);
		--level;
	}
	if(hsNode==STVI_ROOT) return;
	HSTREEITEM hsSibling=GetNextSiblingItem(hsNode);
	if(hsSibling) Dump(hsSibling,level);
}
#endif

//--------------------------------------------------------------------
//	添加一个关键字
//	data:关键字对应的数据
//	key:关键字
//	len:关键字长度
//	return:最后一个加入的节点
//--------------------------------------------------------------------
HSTREEITEM CSKeyTreeEx::AddKey(DWORD data,const BYTE *key,int len)
{
	if(len==-1) len=strlen((char *)key);
	HSTREEITEM hParent=STVI_ROOT;
	int nCmp=0;
	for(int i=0;i<len;i+=m_cKeySize)
	{
		HSTREEITEM hDest=NULL;
		HSTREEITEM hsNode=GetChildItem(hParent);
		KEYDATAEX *pKEYDATAEX=NULL;
		//find a node which value must large key[i]
		while(hsNode)
		{
			pKEYDATAEX=GetItem(hsNode);
			nCmp=memcmp(pKEYDATAEX->byKey,key+i,m_cKeySize);
			if(nCmp>=0)
			{
				hDest=hsNode;
				break;
			}
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(hDest!=NULL)
		{
			if(nCmp==0)
			{
				if(i==len-m_cKeySize)
				{
					if(pKEYDATAEX->data==m_dwInvalidKey)
					{
						pKEYDATAEX->data=data;
						return hDest;
					}else
					{
						return NULL;
					}
				}else//i!=len-m_cKeySize
				{
					hParent=hDest;
				}
			}else//key[i]<pKEYDATAEX->c,insert the right key to tree
			{
				KEYDATAEX *pKEYDATAEX=NewItem(key+i,m_dwInvalidKey);
				if(i==len-m_cKeySize) pKEYDATAEX->data=data;
				HSTREEITEM   hInsertAfter=GetPrevSiblingItem(hDest);
				if(hInsertAfter==NULL) hInsertAfter=STVI_FIRST;
				hParent=InsertItem(pKEYDATAEX,hParent,hInsertAfter);
			}
		}else{//hDest==NULL,find no item 's value large key[i],inert to last
			KEYDATAEX *pKEYDATAEX=NewItem(key+i,m_dwInvalidKey);
			if(i==len-m_cKeySize) pKEYDATAEX->data=data;
			hParent=InsertItem(pKEYDATAEX,hParent,STVI_LAST);
		}
	}
	return hParent;
}

//--------------------------------------------------------------------
//	判断关键字是否存在
//	key:关键字
//	len:关键字长度
//	pdata:用来返回关键字对应数据的指针
//	return:查到的关键字的节点。
//--------------------------------------------------------------------
HSTREEITEM CSKeyTreeEx::IsKeyExist(const BYTE *key,int len,DWORD *pdata)
{
	HSTREEITEM hParent=STVI_ROOT;
	HSTREEITEM hsNode;
	if(m_cKeySize!=1 && len%m_cKeySize ) return NULL;

	for(int i=0;i<len;i+=m_cKeySize)
	{
		hsNode=GetChildItem(hParent);
		BOOL bFind=FALSE;
		KEYDATAEX  *pKEYDATAEX;
		while(hsNode)
		{
			pKEYDATAEX=GetItem(hsNode);
			int nCmp=memcmp(pKEYDATAEX->byKey,key+i,m_cKeySize);
			if(nCmp==0)
			{
				hParent=hsNode;
				bFind=TRUE;
				break;
			}else if(nCmp>0)
				break;
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(!bFind) return NULL;
		if(i==len-m_cKeySize && pdata) *pdata=pKEYDATAEX->data;
	}
	return hsNode;
}

//-------------------------------------------------------------------
//	枚举所有节点
//-------------------------------------------------------------------
BOOL CSKeyTreeEx::EnumTreeNode(HSTREEITEM hParent,CBENUMCHILD cbEnumChild,LPARAM lParam,int iLevel,BYTE *byKeyHead,BOOL bOnlyLeaf)
{
	BOOL bContinue=TRUE;
	HSTREEITEM hSibling=GetChildItem(hParent);
	while(hSibling)
	{
		if(byKeyHead) memcpy(byKeyHead+iLevel*m_cKeySize,GetItemChar(hSibling),m_cKeySize);
		if(!bOnlyLeaf || GetItemData(hSibling)!=m_dwInvalidKey)
		{
			bContinue=cbEnumChild(this,hSibling,byKeyHead,iLevel,lParam);
		}
		if(bContinue && GetChildItem(hSibling))
		{
			bContinue=EnumTreeNode(hSibling,cbEnumChild,lParam,iLevel+1,byKeyHead);
		}
		if(!bContinue) break;
		hSibling=GetNextSiblingItem(hSibling);
	}
	return bContinue;
}

//--------------------------------------------------------------------
//	将关键字树信息写到文件树中
//--------------------------------------------------------------------
void CSKeyTreeEx::WriteData(FILE *f)
{
	CSTree<FILETREEDATA> treeTool;
	MakeFileTree(STVI_ROOT,&treeTool,STVI_ROOT);
	//保存虚拟根节点信息(第一层节点数量
	WORD	wChilds=0;
	HSTREEITEM hChild=(HSTREEITEM)m_hRootFirst;
	while(hChild)
	{
		wChilds++;
		hChild=GetNextSiblingItem(hChild);
	}

	DWORD dwCurOffset=ftell(f);
	DWORD dwSize=0;
	fwrite(&dwSize,sizeof(DWORD),1,f);
	fwrite(&m_byFlag,1,1,f);			//保存索引类型标志
	fwrite(&m_cKeySize,1,1,f);			//保存关键字长度

	//保存子节点信息
	fwrite(&wChilds,sizeof(WORD),1,f);	//第一层节点数
	WriteItem(&treeTool,treeTool.GetRootItem(),dwCurOffset+sizeof(DWORD)+2,f);
	//写入数据长度
	fseek(f,0,SEEK_END);
	dwSize=ftell(f)-dwCurOffset-sizeof(DWORD)-2;
	fseek(f,dwCurOffset,SEEK_SET);
	fwrite(&dwSize,sizeof(DWORD),1,f);
	//重新定位到文件尾
	fseek(f,0,SEEK_END);
}

//--------------------------------------------------------------------
//	CSTree<FILETREEDATA> &toolTree:临时工具树
//	HSTREEITEM hsNode:待保存的节点
//	DWORD	dwRoot:根节点偏移
//	FILE *f:文件指针
//---------------------------------------------------------------------
void CSKeyTreeEx::WriteItem(CSTree<FILETREEDATA> *pTreeTool,
						  HSTREEITEM hsItem,
						  DWORD  dwRoot,
						  FILE *f)
{
	PBKT_NODEEX pbktNode=(PBKT_NODEEX)malloc(sizeof(BKT_NODEEX)-1+m_cKeySize);//在写空节点时需要关键字区
	//保存节点头
	HSTREEITEM hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		ftd->dwPosSelf=ftell(f);
		fwrite(pbktNode,sizeof(BKT_NODEEX)-1+m_cKeySize,1,f);//写空结点，没有真实数据
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
	//保存节点数据
	hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		ftd->dwPosData=ftell(f);
		//保存数据
		if(m_funWriteData) 
			m_funWriteData(ftd->pKEYDATAEX->data,f);
		else
			fwrite(&ftd->pKEYDATAEX->data,sizeof(DWORD),1,f);
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
	//保存每一个节点的真实数据及子节点数据
	hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		HSTREEITEM hChild=pTreeTool->GetChildItem(hSibling);
		DWORD dwEnd=ftell(f);

		pbktNode->dwPosData=ftd->dwPosData-dwRoot;
		pbktNode->dwPosChild=hChild?(dwEnd-dwRoot):-1;
		pbktNode->wChilds=ftd->wChilds;
		fseek(f,ftd->dwPosSelf,SEEK_SET);
		fwrite(pbktNode,sizeof(BKT_NODEEX)-1,1,f);
		fwrite(ftd->pKEYDATAEX->byKey,1,m_cKeySize,f);
		fseek(f,dwEnd,SEEK_SET);
		
		if(hChild) WriteItem(pTreeTool,hChild,dwRoot,f);
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
	free(pbktNode);
}

//----------------------------------------------------------------------
// 采用递归方式构造一棵中间树
//	HSTREEITEM hItemSour:源节点
//	CSTree<FILETREEDATA> *pTreeTool:中间树
//	HSTREEITEM hItemDest:目标节点
//----------------------------------------------------------------------
void CSKeyTreeEx::MakeFileTree(HSTREEITEM hItemSour, CSTree<FILETREEDATA> *pTreeTool, HSTREEITEM hItemDest)
{
	HSTREEITEM hsSibSrc=GetChildItem(hItemSour);
	HSTREEITEM hsSibDest=STVI_FIRST;
	while(hsSibSrc)
	{
		KEYDATAEX *pkd=GetItem(hsSibSrc);
		FILETREEDATA ftd={-1,-1,0,pkd};
		//累计子节点个数
		HSTREEITEM hChild=GetChildItem(hsSibSrc);
		while(hChild)
		{
			ftd.wChilds++;
			hChild=GetNextSiblingItem(hChild);
		}
		hsSibDest=pTreeTool->InsertItem(ftd,hItemDest,hsSibDest);
		if(pTreeTool->GetChildItem(hsSibSrc))
			MakeFileTree(hsSibSrc,pTreeTool,hsSibDest);
		hsSibSrc=GetNextSiblingItem(hsSibSrc);
	}
}

//------------------------------------------------
//	载入一个分枝
//	HSTREEITEM hParent:分枝Parent
//	PBKT_NODEEX pNode:缓冲区树的父结点
//	FILE *f:文件指针
//	DWORD dwRoot:根节点位置，因为文件树节点保存的位置数据都是相对位置
//------------------------------------------------
void CSKeyTreeEx::LoadItem(HSTREEITEM hParent, PBKT_NODEEX pNode, FILE *f,DWORD dwRoot)
{
	HSTREEITEM hInsertAfter=STVI_LAST;
	BYTE byKey[20];
	for(WORD i=0;i<pNode->wChilds;i++)
	{
		fseek(f,dwRoot+pNode->dwPosChild+i*(sizeof(BKT_NODEEX)-1+m_cKeySize),SEEK_SET);
		BKT_NODEEX node;
		fread(&node,sizeof(BKT_NODEEX)-1,1,f);
		fread(byKey,1,m_cKeySize,f);
		KEYDATAEX *pkd=NewItem(byKey,0);
		//load data
		fseek(f,dwRoot+node.dwPosData,SEEK_SET);
		if(m_funLoadData)
			m_funLoadData(pkd->data,f);
		else
			fread(&pkd->data,sizeof(DWORD),1,f);
		hInsertAfter=InsertItem(pkd,hParent,hInsertAfter);
		if(node.wChilds!=0) LoadItem(hInsertAfter,&node,f,dwRoot);
	}
}

//--------------------------------------------------
//	从文件缓冲树中载入数据
//--------------------------------------------------
BOOL CSKeyTreeEx::LoadData(FILE *f)
{
	DeleteAllItems();

	DWORD dwOffset=ftell(f);
	DWORD dwSize=0;
	fread(&dwSize,sizeof(DWORD),1,f);
	BYTE byFlag=0;
	fread(&byFlag,1,1,f);
	if(byFlag!=m_byFlag) return FALSE;
	fread(&m_cKeySize,1,1,f);

	BKT_NODEEX bktRoot={0};
	bktRoot.dwPosChild=2;
	fread(&bktRoot.wChilds,sizeof(WORD),1,f);
	LoadItem(STVI_ROOT,&bktRoot,f,dwOffset+sizeof(DWORD)+2);

	fseek(f,dwOffset+sizeof(DWORD)+2+dwSize,SEEK_SET);//将文件指针定位到数据尾，便于外部调用接口继续读取后续数据
	return TRUE;
}

//-----------------------------------------------------------------
//	匹配探测
//	BYTE *pbyKeys:要匹配的关键字串
//	int nSize:关键字串长度
//	HSTREEITEM *pMatchItem:最后匹配的节点
//	return:完成匹配的深度
//-----------------------------------------------------------------
int CSKeyTreeEx::MatchTest(BYTE *pbyKeys, int nSize ,HSTREEITEM *pMatchItem/*=NULL*/)
{
	HSTREEITEM hParent=STVI_ROOT;
	HSTREEITEM hsNode=NULL,hsGet=NULL;
	int nDeep=0;
	for(int i=0;i<nSize;i+=m_cKeySize)
	{
		hsNode=GetChildItem(hParent);
		BOOL bFind=FALSE;
		KEYDATAEX  *pKEYDATAEX;
		while(hsNode)
		{
			pKEYDATAEX=GetItem(hsNode);
			int nCmp=memcmp(pKEYDATAEX->byKey,pbyKeys+i*m_cKeySize,m_cKeySize);
			if(nCmp==0)
			{
				hParent=hsNode;
				bFind=TRUE;
				hsGet=hsNode;
				break;
			}else if(nCmp>0)
				break;
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(!bFind) break;
		nDeep++;
	}
	if(pMatchItem) *pMatchItem=hsGet;
	return nDeep;
}
