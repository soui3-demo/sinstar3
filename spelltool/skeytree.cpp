#include "stdafx.h"
#include "skeytree.h"

//////////////////////////////////////////////////////////////
//	CSBufKeyTree Implement
//////////////////////////////////////////////////////////////

CSBufKeyTree::CSBufKeyTree()
{
	m_funReadData=NULL;
	m_funIsLeaf=NULL;
	m_pBuffer=NULL;
	m_dwBufSize=0;
}

CSBufKeyTree::~CSBufKeyTree()
{
	Free();
}

//--------------------------------------------------------------------
//	搜索树中的关键字，不管是叶子节点还是分叉节点
//	要判断是不是叶子节点可以调用IsLeafNode
//--------------------------------------------------------------------
BKT_NODE * CSBufKeyTree::IsKeyExist(BYTE *byKey, int nLen)
{
	ASSERT(m_pBuffer);
	BKT_NODE root=GetRoot();
	BKT_NODE *pRetNode=NULL;
	BKT_NODE *pParentNode=&root;
	BKT_NODE *pChildNode=(BKT_NODE*)(m_pBuffer+2);
	for(int i=0;i<nLen;i++)
	{
		if(!(pRetNode=FindKeyNode(pChildNode,pParentNode->wChilds,byKey[i])))
			return NULL;
		else
		{
			pParentNode=pRetNode;
			pChildNode=(BKT_NODE*)(m_pBuffer+pRetNode->dwPosChild);
		}
	}
	return pRetNode;
}

BOOL CSBufKeyTree::EnumTreeNode(PBKT_NODE pNode,CBENUMTREE funEnumTree,LPARAM lParam,int iLevel,BYTE *byKeyHead)
{
	BOOL bContinue=TRUE;
	PBKT_NODE pNodeChild=GetChildHead(pNode);
	for(WORD i=0;i<pNode->wChilds;i++)
	{
		if(byKeyHead) byKeyHead[iLevel]=pNode->byKey;
		if(IsLeafNode(pNodeChild)) bContinue=funEnumTree(this,pNodeChild,iLevel,byKeyHead,lParam);
		if(bContinue) bContinue=EnumTreeNode(pNodeChild,funEnumTree,lParam,iLevel+1,byKeyHead);
		if(!bContinue) break;
	}
	return bContinue;
}

void CSBufKeyTree::EnumKeyNode(BYTE *pbyKeys,int nSize,BYTE byWild,CBENUMKEY funEnumKey,LPARAM lParam,BOOL bDeepFirst)
{
	ASSERT(nSize>=1);
	BKT_NODE root=GetRoot();
	BYTE *pbyKeyArray=new BYTE[nSize];
	_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,&root,0,pbyKeyArray,bDeepFirst);
	delete []pbyKeyArray;
}

BOOL CSBufKeyTree::_EnumKeyNode(BYTE *pbyKeys,int nSize,BYTE byWild,CBENUMKEY funEnumKey,LPARAM lParam,PBKT_NODE pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst)
{
	ASSERT(nSize>=1 && iLevel<nSize);
	BOOL bContinue=TRUE;
	PBKT_NODE pNodeChild=GetChildHead(pParent);
	PBKT_NODE pNodeDest=NULL;
	if(pbyKeys[iLevel]==byWild)
	{
		pNodeDest=pNodeChild;
		for(WORD i=0;i<pParent->wChilds;i++)
		{
			pbyKeyArray[iLevel]=pNodeDest->byKey;
			if(bDeepFirst)
			{
				if(iLevel+1<nSize) bContinue=_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
				if(bContinue && IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
			}else
			{
				if(IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
				if(bContinue && iLevel+1<nSize) bContinue=_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
			}
			if(!bContinue) break;
			pNodeDest++;
		}
	}else
	{
		pNodeDest=FindKeyNode(pNodeChild,pParent->wChilds,pbyKeys[iLevel]);
		if(pNodeDest)
		{
			pbyKeyArray[iLevel]=pNodeDest->byKey;
			if(bDeepFirst)
			{
				if(iLevel+1<nSize) bContinue=_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
				if(bContinue && IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
			}else
			{
				if(IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
				if(bContinue && iLevel+1<nSize) bContinue=_EnumKeyNode(pbyKeys,nSize,byWild,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
			}
		}
	}
	return bContinue;
}

void CSBufKeyTree::EnumKeyNode(PKEYEQUAL pKeyEqual,int nSize,CBENUMKEY funEnumKey,LPARAM lParam,BOOL bDeepFirst)
{
	ASSERT(nSize>=1);
	BKT_NODE root=GetRoot();
	BYTE *pbyKeyArray=new BYTE[nSize];
	_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,&root,0,pbyKeyArray,bDeepFirst);
	delete []pbyKeyArray;
}

BOOL CSBufKeyTree::_EnumKeyNode(PKEYEQUAL pKeyEqual,int nSize,CBENUMKEY funEnumKey,LPARAM lParam,PBKT_NODE pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst)
{
	ASSERT(nSize>=1 && iLevel<nSize);
	short sLeftSiblings=(short)pParent->wChilds;
	short sCount=pKeyEqual[iLevel].sCount;
	if(sCount==0) sCount=sLeftSiblings;

	BOOL bContinue=TRUE;
	PBKT_NODE pNodeChild=GetChildHead(pParent);
	PBKT_NODE pNodeDest=NULL;
	for(short i=0;i<sCount && sLeftSiblings>0 ;i++)
	{
		if(pKeyEqual[iLevel].sCount!=0)
			pNodeDest=FindKeyNode(pNodeChild,sLeftSiblings,pKeyEqual[iLevel].pbyKeys[i]);
		else
			pNodeDest=pNodeChild;
		if(pNodeDest)
		{
			pbyKeyArray[iLevel]=pNodeDest->byKey;
			if(bDeepFirst)
			{
				if(iLevel+1<nSize) bContinue=_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
				if(bContinue && IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
			}else
			{
				if(IsLeafNode(pNodeDest)) bContinue=funEnumKey(this,pNodeDest,nSize,iLevel,pbyKeyArray,lParam);
				if(bContinue && iLevel+1<nSize) bContinue=_EnumKeyNode(pKeyEqual,nSize,funEnumKey,lParam,pNodeDest,iLevel+1,pbyKeyArray,bDeepFirst);
			}
			sLeftSiblings-=(pNodeDest-pNodeChild)+1;
			pNodeChild=pNodeDest+1;
		}
		if(!bContinue) break;
	}
	return bContinue;
}

//--------------------------------------------------------------------
//	从排好序的兄弟结点中用两分法查找关键字所在节点
//--------------------------------------------------------------------
BKT_NODE * CSBufKeyTree::FindKeyNode(BKT_NODE *pSibling, WORD wSiblings, BYTE byKey)
{
	if(wSiblings==0)
		return NULL;
	if(wSiblings==1)
	{
		if(pSibling->byKey!=byKey)
			return NULL;
		else
			return pSibling;
	}else if(wSiblings==2)
	{
		if(pSibling->byKey==byKey)
			return pSibling;
		else if((pSibling+1)->byKey==byKey)
			return pSibling+1;
		else
			return NULL;
	}else
	{
		WORD wHalf=wSiblings/2;
		BKT_NODE *pMiddle=pSibling+wHalf;
		if(pMiddle->byKey==byKey)
			return pMiddle;
		else if(pMiddle->byKey>byKey)
			return FindKeyNode(pSibling,wHalf,byKey);
		else
			return FindKeyNode(pMiddle+1,wSiblings-wHalf-1,byKey);
	}
}

//---------------------------------------------------------------
//	载入数据
//---------------------------------------------------------------
BOOL CSBufKeyTree::LoadData(FILE *f, BYTE byFlag)
{
	DWORD dwSize=0;
	fread(&dwSize,sizeof(DWORD),1,f);
	BYTE byFlagGet=0;
	fread(&byFlagGet,1,1,f);
	if(byFlag!=byFlagGet) return FALSE;
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

//--------------------------------------------------------
//	释放内存
//--------------------------------------------------------
void CSBufKeyTree::Free()
{
	if(m_pBuffer)
	{
		HeapFree(GetProcessHeap(),0,m_pBuffer);
		m_pBuffer=NULL;
	}
}

//////////////////////////////////////////////////////////////
//	CSKeyTree Implement
//////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CSKeyTree::Dump(HSTREEITEM hsNode,int level)
{
	char tab[6]="     ";
	if(hsNode!=STVI_ROOT)
	{
		KEYDATA *pkeydata=GetItemPt(hsNode);
		for(int i=0;i<level;i++)	TRACE(tab);
		TRACE("%02x:%08x\r\n",pkeydata->c,pkeydata->data);
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
//--------------------------------------------------------------------
BOOL CSKeyTree::AddKey(DWORD data,const BYTE *key,int len)
{
	if(len==-1) len=strlen((char *)key);
	HSTREEITEM hParent=STVI_ROOT;
	for(int i=0;i<len;i++)
	{
		HSTREEITEM hDest=NULL;
		HSTREEITEM hsNode=GetChildItem(hParent);
		KEYDATA *pkeydata=NULL;
		//find a node which value must large key[i]
		while(hsNode)
		{
			pkeydata=GetItemPt(hsNode);
			if(pkeydata->c>=key[i]){
				hDest=hsNode;
				break;
			}
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(hDest!=NULL){
			if(pkeydata->c==key[i]){
				if(i==len-1){
					if(pkeydata->data==m_dwInvalidKey)
					{
						pkeydata->data=data;
						return TRUE;
					}else
						return FALSE;
				}else//i!=len-1
					hParent=hDest;
			}else//key[i]<pkeydata->c,insert the right key to tree
			{
				ASSERT(key[i]<pkeydata->c);
				KEYDATA keydata={key[i],m_dwInvalidKey};
				if(i==len-1) keydata.data=data;
				HSTREEITEM   hInsertAfter=GetPrevSiblingItem(hDest);
				if(hInsertAfter==NULL) hInsertAfter=STVI_FIRST;
				hParent=InsertItem(keydata,hParent,hInsertAfter);
			}
		}else{//hDest==NULL,find no item 's value large key[i],inert to last
			KEYDATA keydata={key[i],m_dwInvalidKey};
			if(i==len-1) keydata.data=data;
			hParent=InsertItem(keydata,hParent,STVI_LAST);
		}
	}
	return TRUE;
}

//--------------------------------------------------------------------
//	判断关键字是否存在
//	key:关键字
//	len:关键字长度
//	pdata:用来返回关键字对应数据的指针
//	return:查到的关键字的节点。
//--------------------------------------------------------------------
HSTREEITEM CSKeyTree::IsKeyExist(const BYTE *key,int len,DWORD *pdata)
{
	HSTREEITEM hParent=STVI_ROOT;
	HSTREEITEM hsNode;
	for(int i=0;i<len;i++)
	{
		hsNode=GetChildItem(hParent);
		BOOL bFind=FALSE;
		KEYDATA  *pkeydata;
		while(hsNode)
		{
			pkeydata=GetItemPt(hsNode);
			if(pkeydata->c==key[i])
			{
				hParent=hsNode;
				bFind=TRUE;
				break;
			}else if(pkeydata->c>key[i])
				break;
			hsNode=GetNextSiblingItem(hsNode);
		}
		if(!bFind) return NULL;
		if(i==len-1&&pdata) *pdata=pkeydata->data;
	}
	return hsNode;
}

//--------------------------------------------------------------------
//	将关键字树信息写到文件树中
//--------------------------------------------------------------------
void CSKeyTree::WriteData(FILE *f)
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
	fwrite(&m_byFlag,1,1,f);//保存索引类型标志

	fwrite(&wChilds,sizeof(WORD),1,f);//第一层节点数
	//保存子节点信息
	WriteItem(&treeTool,treeTool.GetRootItem(),dwCurOffset+sizeof(DWORD)+1,f);
	//写入数据长度
	fseek(f,0,SEEK_END);
	dwSize=ftell(f)-dwCurOffset-sizeof(DWORD)-1;
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
void CSKeyTree::WriteItem(CSTree<FILETREEDATA> *pTreeTool,
						  HSTREEITEM hsItem,
						  DWORD  dwRoot,
						  FILE *f)
{
	BKT_NODE bktNode={0};
	//保存节点头
	HSTREEITEM hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		ftd->dwPosSelf=ftell(f);
		fwrite(&bktNode,sizeof(BKT_NODE),1,f);//写空结点，没有真实数据
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
			m_funWriteData(ftd->keyData.data,f);
		else
			fwrite(&ftd->keyData.data,sizeof(DWORD),1,f);
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
	//保存每一个节点的真实数据及子节点数据
	hSibling=hsItem;
	while(hSibling)
	{
		FILETREEDATA *ftd=pTreeTool->GetItemPt(hSibling);
		HSTREEITEM hChild=pTreeTool->GetChildItem(hSibling);
		DWORD dwEnd=ftell(f);

		bktNode.dwPosData=ftd->dwPosData-dwRoot;
		bktNode.dwPosChild=hChild?(dwEnd-dwRoot):-1;
		bktNode.wChilds=ftd->wChilds;
		bktNode.byKey=ftd->keyData.c;
		fseek(f,ftd->dwPosSelf,SEEK_SET);
		fwrite(&bktNode,sizeof(BKT_NODE),1,f);
		fseek(f,dwEnd,SEEK_SET);
		
		if(hChild) WriteItem(pTreeTool,hChild,dwRoot,f);
		hSibling=pTreeTool->GetNextSiblingItem(hSibling);
	}
}

//----------------------------------------------------------------------
// 采用递归方式构造一棵中间树
//	HSTREEITEM hItemSour:源节点
//	CSTree<FILETREEDATA> *pTreeTool:中间树
//	HSTREEITEM hItemDest:目标节点
//----------------------------------------------------------------------
void CSKeyTree::MakeFileTree(HSTREEITEM hItemSour, CSTree<FILETREEDATA> *pTreeTool, HSTREEITEM hItemDest)
{
	HSTREEITEM hsSibSrc=GetChildItem(hItemSour);
	HSTREEITEM hsSibDest=STVI_FIRST;
	while(hsSibSrc)
	{
		KEYDATA *pkd=GetItemPt(hsSibSrc);
		FILETREEDATA ftd={-1,-1,0,{pkd->c,pkd->data}};
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
//	PBKT_NODE pNode:缓冲区树的父结点
//	FILE *f:文件指针
//	DWORD dwRoot:根节点位置，因为文件树节点保存的位置数据都是相对位置
//------------------------------------------------
void CSKeyTree::LoadItem(HSTREEITEM hParent, PBKT_NODE pNode, FILE *f,DWORD dwRoot)
{
	HSTREEITEM hInsertAfter=STVI_LAST;
	for(WORD i=0;i<pNode->wChilds;i++)
	{
		fseek(f,dwRoot+pNode->dwPosChild+i*sizeof(BKT_NODE),SEEK_SET);
		BKT_NODE node;
		fread(&node,sizeof(BKT_NODE),1,f);
		KEYDATA kd={node.byKey};
		//load data
		fseek(f,dwRoot+node.dwPosData,SEEK_SET);
		if(m_funLoadData)
			m_funLoadData(kd.data,f);
		else
			fread(&kd.data,sizeof(DWORD),1,f);
		hInsertAfter=InsertItem(kd,hParent,hInsertAfter);
		if(node.wChilds!=0) LoadItem(hInsertAfter,&node,f,dwRoot);
	}
}

//--------------------------------------------------
//	从文件缓冲树中载入数据
//--------------------------------------------------
BOOL CSKeyTree::LoadData(FILE *f)
{
	DeleteAllItems();

	DWORD dwOffset=ftell(f);
	DWORD dwSize=0;
	fread(&dwSize,sizeof(DWORD),1,f);
	BYTE byFlag=0;
	fread(&byFlag,1,1,f);
	if(byFlag!=m_byFlag) return FALSE;

	BKT_NODE bktRoot={0};
	bktRoot.dwPosChild=2;
	fread(&bktRoot.wChilds,sizeof(WORD),1,f);
	LoadItem(STVI_ROOT,&bktRoot,f,dwOffset+sizeof(DWORD)+1);

	fseek(f,dwOffset+sizeof(DWORD)+1+dwSize,SEEK_SET);//将文件指针定位到数据尾，便于外部调用接口继续读取后续数据
	return TRUE;
}
