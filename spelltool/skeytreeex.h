//************************************************************
//	关键字树扩展
//	黄建雄 2005-7-4
//************************************************************
#ifndef SKEYTREEEX_H
#define SKEYTREEEX_H

#include "stree.hpp"

#pragma pack(push,1)
typedef struct tagBKT_NODEEX{
	DWORD	dwPosChild;			//子节点在缓冲区中的位置
	DWORD	dwPosData;			//数据在缓冲区中的位置
	WORD	wChilds;			//子节点数量,兄弟节点不能超过65534
	BYTE	byKey[1];			//关键字串
}BKT_NODEEX,*PBKT_NODEEX;
#pragma pack(pop)

typedef struct tagKEYEQUALEX
{
	WORD wCount;	//等价关键字个数 0~65534 ,0代表所有分枝此时pbyKeys==NULL
	BYTE *pbyKeys;	//等价关键字串
}KEYEQUALEX,*PKEYEQUALEX;


class CSBufKeyTreeEx ;

//关键字枚举回调接口
//CSBufKeyTreeEx *pTree:树对象
//PBKT_NODEEX pNode:目标节点
//int nSize:		总级别数
//int iLevel:		当前级别
//BYTE *pbyKeys:	关键字串
//LPARAM lParam:	用户数据
//return: FALSE-枚举结束,TRUE-继续枚举
typedef BOOL (*CBENUMKEYEX)(CSBufKeyTreeEx *pTree,PBKT_NODEEX pNode,int nSize,int iLevel,BYTE *pbyKeys,LPARAM lParam);

//树的子节点枚举回调接口
//CSBufKeyTreeEx *pTree:树对象
//PBKT_NODEEX pNode:目标节点
//int iLevel:		当前级别
//BYTE *pbyKeys:	关键字串
//LPARAM lParam:	用户数据
//return: FALSE-枚举结束,TRUE-继续枚举
typedef BOOL (*CBENUMTREEEX)(CSBufKeyTreeEx *pTree,PBKT_NODEEX pNode,int iLevel,BYTE *pbyKeys,LPARAM lParam);

class CSBufKeyTreeEx  
{
public:
	void Free();
	BOOL LoadData(FILE *f,BYTE byFlag);
	BOOL WriteData(FILE *f,BYTE byFlag);
	
	BOOL EnumTreeNode(PBKT_NODEEX pNode,CBENUMTREEEX funEnumTree,LPARAM lParam,int iLevel=0,BYTE *byKeyHead=NULL);
	void EnumKeyNode(PKEYEQUALEX pKeyEqual,int nSize,CBENUMKEYEX funEnumKey,LPARAM lParam,BOOL bDeepFirst=FALSE);
	BKT_NODEEX * IsKeyExist(BYTE *byKey,int nLen);
	BKT_NODEEX * GetChildHead(BKT_NODEEX * pNode)
	{
		if(pNode->wChilds==0) return NULL;
		return (BKT_NODEEX *)(m_pBuffer+pNode->dwPosChild);
	}

	CSBufKeyTreeEx();
	virtual ~CSBufKeyTreeEx();


	BOOL IsLeafNode(PBKT_NODEEX pNode)
	{
		ASSERT(m_funIsLeaf);
		return m_funIsLeaf(m_pBuffer+pNode->dwPosData);
	}
	
	BOOL GetItemData(PBKT_NODEEX pNode,void *pData)
	{
		ASSERT(m_funReadData);
		return m_funReadData(m_pBuffer+pNode->dwPosData,pData);
	}
	LPBYTE GetItemDataPtr(PBKT_NODEEX pNode){return m_pBuffer+pNode->dwPosData;}
	char GetKeySize(){ return m_cKeySize;}

	BKT_NODEEX GetRoot()
	{
		BKT_NODEEX root;
		root.dwPosData=0xffffffff;	//没有数据
		root.byKey[0]=0;			//绝对不可以使用该数据，它没有意义
		root.dwPosChild=2;
		memcpy(&root.wChilds,m_pBuffer,2);
		return root;
	}
	inline BYTE *GetBuffer()const
	{
		return m_pBuffer;
	}
	inline int NODESIZE(){ return sizeof(BKT_NODEEX)-1+m_cKeySize;}

	inline BKT_NODEEX * NextNode(BKT_NODEEX *pNode,int nOffset=1)
	{
		return (BKT_NODEEX *)((BYTE*)pNode+NODESIZE()*nOffset);
	}
	
	//计算两个兄弟节点间的距离
	inline int NodeSub(PBKT_NODEEX pNode1,PBKT_NODEEX pNode2)
	{
		if(pNode1==pNode2) return 0;
		BYTE *p1=(BYTE*)pNode1;
		BYTE *p2=(BYTE*)pNode2;
		return (p1-p2)/NODESIZE();
	}
	//************************************************************
	//	从内存中读取数据
	//************************************************************
	BOOL (*m_funReadData)(BYTE *pBuf,void *pData);
	BOOL (*m_funIsLeaf)(BYTE *pBuf);

	int MatchTestMax(BYTE *pbyKeys, int nSize ,PBKT_NODEEX *pMatchItem=NULL);
	PBKT_NODEEX FindNextLeaf(BKT_NODEEX * hsParent,BYTE *pbyKeys, int nSize,int *pDeep=NULL);
	PBKT_NODEEX FindKeyNode(BKT_NODEEX *pSibling,WORD wSiblings,BYTE *pbyKey);
protected:
	BOOL _EnumKeyNode(PKEYEQUALEX pKeyEqual,int nSize,CBENUMKEYEX funHandle,LPARAM lParam,PBKT_NODEEX pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst=FALSE);

	char	   m_cKeySize;	//关键字长度
	BYTE	 * m_pBuffer;
	DWORD		m_dwBufSize;
};

typedef struct tagKEYDATAEX{
	DWORD	data;		//数据
	BYTE	byKey[1];	//关键字串
}KEYDATAEX,*PKEYDATAEX;

typedef void (*LOADDATAFUN)(DWORD &data, FILE *f);
typedef void (*WRITEDATAFUN)(DWORD &data, FILE *f);

class CSKeyTreeEx;
//关键字树，关键字长度可变,子节点按关键字从小到大排序，比较函数memcmp
typedef BOOL (*CBENUMCHILD)(CSKeyTreeEx *pKeyTree,HSTREEITEM hsItem,BYTE *pbyKeys,int iLevel,LPARAM lParam);
class CSKeyTreeEx:public CSTree<KEYDATAEX *>
{
	//在存盘时用于构造临时工具树
	typedef struct _FILETREEDATA{
		DWORD	dwPosSelf;		//自己的位置
		DWORD	dwPosData;		//数据位置
		WORD	wChilds;		//子节点个数
		KEYDATAEX *pKEYDATAEX;		//关键字数据
	}FILETREEDATA;

public:
	int MatchTest(BYTE *pbyKeys,int nSize,HSTREEITEM *pMatchItem=NULL);
	BOOL LoadData(FILE *f);
	void WriteData(FILE *f);
#ifdef _DEBUG
	//--------------------------------------------------------------------
	//	在输出窗口以文本方式打印树的分支，便于调试时查看树结构
	//--------------------------------------------------------------------
	void Dump(HSTREEITEM hsNode=STVI_ROOT,int level=0);
#endif
	CSKeyTreeEx(BYTE byFlag=0,DWORD dwInvalidKey=0){
		m_byFlag=byFlag;
		m_dwInvalidKey=dwInvalidKey;
		m_funLoadData=m_funWriteData=NULL;
		m_funFreeData=FreeItem;
		m_cKeySize=1;
	}
	
	virtual ~CSKeyTreeEx(){
		DeleteAllItems();
	}
	
	//--------------------------------------------------------------------
	//	设置映射数据非法值，一个关键字串一般要对应一个数据
	//	如果一个树结点上的映射数据是非法值则表示该节点是中途结点
	//--------------------------------------------------------------------
	void SetInvalidKey(DWORD dwInvalidKey){
		m_dwInvalidKey=dwInvalidKey;
	}
	
	void SetFlag(BYTE byFlag)
	{
		m_byFlag=byFlag;
	}
	void SetKeySize(char cKeySize){ m_cKeySize=cKeySize;}
	//--------------------------------------------------------------------
	//	得到一项的字符
	//--------------------------------------------------------------------
	BYTE * GetItemChar(HSTREEITEM hsNode){
		return GetItem(hsNode)->byKey;
	}

	//--------------------------------------------------------------------
	//	得到一项的数据
	//--------------------------------------------------------------------
	DWORD GetItemData(HSTREEITEM hsNode){
		return GetItem(hsNode)->data;
	}

	//--------------------------------------------------------------------
	//	set key word data
	//--------------------------------------------------------------------
	void SetItemData(HSTREEITEM hsNode, DWORD data)
	{
		GetItem(hsNode)->data=data;
	}

	//--------------------------------------------------------------------
	//	添加一个关键字
	//	data:关键字的附加数据
	//	key:关键字字符串
	//	len:关键字字符串长度，－1表示为字符串全长
	//	return:最后一个加入的节点
	//--------------------------------------------------------------------
	HSTREEITEM AddKey(DWORD data,const BYTE *key,int len=-1);

	//-------------------------------------------------------------------
	//	枚举所有节点
	//	CBENUMCHILD cbEnumChild:枚举回调
	//	BYTE *pKeyBuf:父节点数据串
	//-------------------------------------------------------------------
	BOOL EnumTreeNode(HSTREEITEM hParent,CBENUMCHILD cbEnumChild,LPARAM lParam,int iLevel=0,BYTE *byKeyHead=NULL,BOOL bOnlyLeaf=TRUE);


	//--------------------------------------------------------------------
	//	检测关键字是否存在
	//	key:待测关键字
	//	len:待测关键字长度
	//	pdata:关键字的附加数据，如果关键字存在则将
	//		  该关键字的附加数据复制到该指针指向的地址中
	//--------------------------------------------------------------------
	HSTREEITEM IsKeyExist(const BYTE *key,int len,DWORD *pdata=NULL);


	KEYDATAEX * NewItem(const BYTE *byKey,DWORD dwData)
	{
		KEYDATAEX *pRet= (KEYDATAEX *)malloc(sizeof(DWORD)+m_cKeySize);
		pRet->data=dwData;
		if(byKey) memcpy(pRet->byKey,byKey,m_cKeySize);
		return pRet;
	}

	static void FreeItem(const PKEYDATAEX & pData)
	{
		free(pData);
	}


	LOADDATAFUN		m_funLoadData;
	WRITEDATAFUN	m_funWriteData;
private:
	void LoadItem(HSTREEITEM hItem,PBKT_NODEEX pNode,FILE *f,DWORD dwRoot);
	void MakeFileTree(HSTREEITEM hItemSour,CSTree<FILETREEDATA> *pTreeTool,HSTREEITEM hItemDest);
	void WriteItem(CSTree<FILETREEDATA> *pTreeTool,HSTREEITEM hsItem,DWORD  dwRootOffset,FILE *f);

	DWORD	m_dwInvalidKey;
	BYTE	m_byFlag;		//数据标志
	char	m_cKeySize;		//关键字长度，一般情况不要超过4
};
#endif//#ifndef SKEYTREEEX_H
