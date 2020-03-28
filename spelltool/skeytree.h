//************************************************************
//	关键字树
//	黄建雄 2003-10-22
//************************************************************
#ifndef SKEYTREE_H
#define SKEYTREE_H

#include "stree.hpp"

#pragma pack(push,1)
typedef struct tagBKT_NODE{
	DWORD	dwPosChild;			//子节点在缓冲区中的位置
	DWORD	dwPosData;			//数据在缓冲区中的位置
	WORD	wChilds;			//子节点数量,最多可能有256个子节点,因此必须占用2个字节
	BYTE	byKey;				//关键字
}BKT_NODE,*PBKT_NODE;
#pragma pack(pop)

typedef struct tagKEYEQUAL
{
	short sCount;	//等价关键字个数,0代表所有分枝此时pbyKeys==NULL
	BYTE *pbyKeys;	//等价关键字串
}KEYEQUAL,*PKEYEQUAL;

class CSBufKeyTree ;

//关键字枚举回调接口
//CSBufKeyTree *pTree:树对象
//PBKT_NODE pNode:目标节点
//int nSize:		总级别数
//int iLevel:		当前级别
//BYTE *pbyKeys:	关键字串
//LPARAM lParam:	用户数据
//return: FALSE-枚举结束,TRUE-继续枚举
typedef BOOL (*CBENUMKEY)(CSBufKeyTree *pTree,PBKT_NODE pNode,int nSize,int iLevel,BYTE *pbyKeys,LPARAM lParam);

//树的子节点枚举回调接口
//CSBufKeyTree *pTree:树对象
//PBKT_NODE pNode:目标节点
//int iLevel:		当前级别
//BYTE *pbyKeys:	关键字串
//LPARAM lParam:	用户数据
//return: FALSE-枚举结束,TRUE-继续枚举
typedef BOOL (*CBENUMTREE)(CSBufKeyTree *pTree,PBKT_NODE pNode,int iLevel,BYTE *pbyKeys,LPARAM lParam);

class CSBufKeyTree  
{
public:
	void Free();
	BOOL LoadData(FILE *f,BYTE byFlag);

	BOOL EnumTreeNode(PBKT_NODE pNode,CBENUMTREE funEnumTree,LPARAM lParam,int iLevel=0,BYTE *byKeyHead=NULL);
	void EnumKeyNode(BYTE *pbyKeys,int nSize,BYTE byWild,CBENUMKEY funEnumKey,LPARAM lParam,BOOL bDeepFirst);	
	void EnumKeyNode(PKEYEQUAL pKeyEqual,int nSize,CBENUMKEY funEnumKey,LPARAM lParam,BOOL bDeepFirst);
	BKT_NODE * IsKeyExist(BYTE *byKey,int nLen);
	BKT_NODE * GetChildHead(BKT_NODE * pNode)
	{
		if(pNode->wChilds==0) return NULL;
		return (BKT_NODE *)(m_pBuffer+pNode->dwPosChild);
	}

	CSBufKeyTree();
	virtual ~CSBufKeyTree();


	BOOL IsLeafNode(PBKT_NODE pNode)
	{
		return m_funIsLeaf(m_pBuffer+pNode->dwPosData);
	}
	
	void GetItemData(PBKT_NODE pNode,void *pData)
	{
		m_funReadData(m_pBuffer+pNode->dwPosData,pData);
	}
	LPBYTE GetItemDataPtr(PBKT_NODE pNode){return m_pBuffer+pNode->dwPosData;}

	BKT_NODE GetRoot()
	{
		BKT_NODE root;
		root.dwPosData=0xffffffff;	//没有数据
		root.byKey=0;				//绝对不可以使用该数据，它没有意义
		root.dwPosChild=2;
		memcpy(&root.wChilds,m_pBuffer,2);
		return root;
	}
	inline BYTE *GetBuffer()const
	{
		return m_pBuffer;
	}
	//************************************************************
	//	从内存中读取数据
	//************************************************************
	void (*m_funReadData)(BYTE *pBuf,void *pData);
	BOOL (*m_funIsLeaf)(BYTE *pBuf);
	BKT_NODE * FindKeyNode(BKT_NODE *pSibling,WORD wSiblings,BYTE byKey);
protected:
	BOOL _EnumKeyNode(PKEYEQUAL pKeyEqual,int nSize,CBENUMKEY funEnumKey,LPARAM lParam,PBKT_NODE pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst);
	BOOL _EnumKeyNode(BYTE *pbyKeys,int nSize,BYTE byWild,CBENUMKEY funEnumKey,LPARAM lParam,PBKT_NODE pParent,int iLevel,BYTE *pbyKeyArray,BOOL bDeepFirst);

	BYTE	 * m_pBuffer;
	DWORD		m_dwBufSize;
};

typedef struct tagKEYDATA{
	BYTE	c;		//字符
	DWORD	data;	//数据
}KEYDATA;

typedef void (*LOADDATAFUN)(DWORD &data, FILE *f);
typedef void (*WRITEDATAFUN)(DWORD &data, FILE *f);

class CSKeyTree:public CSTree<KEYDATA>
{
	//在存盘时用于构造临时工具树
	typedef struct _FILETREEDATA{
		DWORD	dwPosSelf;		//自己的位置
		DWORD	dwPosData;		//数据位置
		WORD	wChilds;		//子节点个数
		KEYDATA	keyData;
	}FILETREEDATA;

public:
	BOOL LoadData(FILE *f);
	void WriteData(FILE *f);
#ifdef _DEBUG
	//**********************************************************
	//	在输出窗口以文本方式打印树的分支，便于调试时查看树结构
	//**********************************************************
	void Dump(HSTREEITEM hsNode=STVI_ROOT,int level=0);
#endif
	CSKeyTree(BYTE byFlag=0,DWORD dwInvalidKey=0){
		m_byFlag=byFlag;
		m_dwInvalidKey=dwInvalidKey;
		m_funLoadData=m_funWriteData=NULL;
	}
	
	virtual ~CSKeyTree(){
		DeleteAllItems();
	}
	
	//****************************************************
	//	设置映射数据非法值，一个关键字串一般要对应一个数据
	//	如果一个树结点上的映射数据是非法值则表示该节点是中途结点
	//****************************************************
	void SetInvalidKey(DWORD dwInvalidKey){
		m_dwInvalidKey=dwInvalidKey;
	}
	
	//**********************************************************
	//	得到一项的字符
	//**********************************************************
	BYTE GetItemChar(HSTREEITEM hsNode){
		return GetItemPt(hsNode)->c;
	}

	//*********************************************************
	//	得到一项的数据
	//*********************************************************
	DWORD GetItemData(HSTREEITEM hsNode){
		return GetItemPt(hsNode)->data;
	}

	//********************************************************
	//	set key word data
	//*********************************************************
	void SetItemData(HSTREEITEM hsNode, DWORD data)
	{
		GetItemPt(hsNode)->data=data;
	}

	//*****************************************************
	//	添加一个关键字
	//	data:关键字的附加数据
	//	key:关键字字符串
	//	len:关键字字符串长度，－1表示为字符串全长
	//*****************************************************
	BOOL AddKey(DWORD data,const BYTE *key,int len=-1);

	//***************************************************
	//	检测关键字是否存在
	//	key:待测关键字
	//	len:待测关键字长度
	//	pdata:关键字的附加数据，如果关键字存在则将
	//		  该关键字的附加数据复制到该指针指向的地址中
	//***************************************************
	HSTREEITEM IsKeyExist(const BYTE *key,int len,DWORD *pdata=NULL);

	LOADDATAFUN		m_funLoadData;
	WRITEDATAFUN	m_funWriteData;
private:
	void LoadItem(HSTREEITEM hItem,PBKT_NODE pNode,FILE *f,DWORD dwRoot);
	void MakeFileTree(HSTREEITEM hItemSour,CSTree<FILETREEDATA> *pTreeTool,HSTREEITEM hItemDest);
	void WriteItem(CSTree<FILETREEDATA> *pTreeTool,HSTREEITEM hsItem,DWORD  dwRootOffset,FILE *f);
	DWORD	m_dwInvalidKey;
	BYTE	m_byFlag;		//数据标志
};
#endif//#ifndef SKEYTREE_H
