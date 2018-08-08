//*******************************************************************
//	SArray.hpp
//	模块名称：启程动态数组C++模板类 
//	version:2.3
//  说明：该链表是在1.0版本的基础上强化了数据插入与删除的修正版，
//	本代码提供对动态数组的支持，在内存中程序将数据分块存放，
//　避免了大块内存的申请，同时也和普通的双向链表不同的是本代码提供
//　了对内部数据的快速索引，大大提高了数据访问速度。
//		增强程序对随机操作的支持，在链表的空间利用率<用户指定的比例时自动整理数据
//	程序员也可以重载判断是否需要进行数据整理的虚函数IsNeedCompack来使用自己的规则
//	该链表的主要目标是大量数据的管理，提供一个快速的数据插入删除等接口．在使用它时
//	配置一套好的参数将大大提高程序的执行效率，事实证明，该链表的执行效率大大高于
//	MFC提供的CArray
//		本代码可以任意使用、修改、传播，但作者不对使用该链表造成的后果承担任何直接或间接责任。
//		黄建雄　huangjianxiong@sina.com
//　update
//		2.3 黄建雄		2007-04-12	增加Attach,Detach接口
//		2.2 黄建雄		2007-01-31	改写有重复的二叉查找接口
//		2.1 黄建雄		2006-10-19  添加排序插入接口,修改SetSize中的一个bug
//		2.0 黄建雄		2004-06-02
//		1.0 黄建雄		2003-09-25
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SARRAY_H__B1D40C22_2698_4202_921E_36D447EA4199__INCLUDED_)
#define AFX_SARRAY_H__B1D40C22_2698_4202_921E_36D447EA4199__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef ASSERT
#define ASSERT(a) void()
#endif

#ifndef BOOL
#define BOOL int
#endif

//排序查找类型
#define SFT_ANY		0	//可以在重复的任意位置
#define SFT_HEAD	1	//插入到重复的头部
#define SFT_TAIL	2	//插入到重复的尾部

template<class T>
class CSArray  
{
	typedef struct _SARRAYNODE{
		struct _SARRAYNODE *	pPrevNode;	//前一节点
		T				   *	pData;		//存储实际数据数组的数据块的指针	
		DWORD					dwUsed;		//被占用的空间数,初始时为0，最大值为该结点的长度
		struct _SARRAYNODE *	pNextNode;	//后一节点
	}SARRAYNODE;

	typedef int (*FUNCMP)(T *pData1,T *pData2,LPARAM lParam);

public:
	CSArray(WORD nGrowBy=5){
		m_pCurNode=m_pHeadNode=m_pTailNode=NULL;
		m_nCurIndex=-1;
		m_nCount=0;
		m_nEmpty=0;
		m_nGrowBy=nGrowBy;
		m_byZipKey=10;	//默认压缩阀值为10%
	}

	virtual ~CSArray(){Free();}

	//******************************************
	//	name:Add
	//	function:添加数据
	//	input:　T newElement-新数据
	//  return: 成功=数据索引号,失败=-1
	//	remark: 
	//******************************************
	int Add(T newElement)
	{
		if(m_nGrowBy==0)//
			return -1;
		if(m_pHeadNode==NULL)
		{
			if(!(m_pHeadNode=NewNode())) return -1;
			*m_pHeadNode->pData=newElement;
			m_pHeadNode->dwUsed=1;
			m_nCurIndex=0;
			m_nCount=1;
			m_nEmpty=m_nGrowBy-1;
			m_pCurNode=m_pTailNode=m_pHeadNode;
		}
		else
		{
			if(m_pTailNode->dwUsed==m_nGrowBy)
			{
				SARRAYNODE *pNewNode=NewNode();
				if(!pNewNode) return -1;
				pNewNode->pPrevNode=m_pTailNode;
				m_pTailNode->pNextNode=pNewNode;
				m_pTailNode=pNewNode;
				m_nEmpty+=m_nGrowBy;
			}
			m_pTailNode->pData[m_pTailNode->dwUsed++]=newElement;
			m_nCount++;
			m_nEmpty--;
			m_nCurIndex=m_nCount-m_pTailNode->dwUsed;
			m_pCurNode=m_pTailNode;
		}
		return m_nCount-1;
	}

	//******************************************
	//	name:AddBatch
	//	function:批量添加数据
	//	input:　T *pElement-源数组指针
	//			int count-数组大小
	//  return: BOOL　TRUE－成功；FALSE－失败
	//	remark: 
	//******************************************
	BOOL AddBatch(T *pElement,int count)
	{
		for(int i=0;i<count;i++)
			if(Add(pElement[i])==-1)
				return FALSE;
			return TRUE;
	}


	//******************************************
	//	name:Copy
	//	function:数据复制
	//	input:　CSArray & src-源动态数组
	//  return: 
	//	remark: 使用前请先确保两个对象有相同的数据类型
	//******************************************
	void Copy(CSArray &src )
	{
		T *pt;
		RemoveAll();
		int size=src.GetSize();
		SetSize(size);
		for(UINT i=0;i<m_nCount;i++)
		{
			pt=src.GetPtAt(i);
			SetAt(i,*pt);
		}
	}

	//******************************************
	//	name:GetAt
	//	function:获取数组指定位置的数据
	//	input:　int index-指定位置
	//  return: T 数据
	//	remark: 
	//******************************************
	T GetAt(DWORD index){
		SARRAYNODE      *pDest=NULL;
		ASSERT(index>=0&&index<m_nCount);
		pDest=GetDestSegEntry(index);
		return ((T *)pDest->pData)[index-m_nCurIndex];
	}
	
	//******************************************
	//	name:GetPtAt
	//	function:获取数组指定位置的数据的指针
	//	input:　int index-指定位置
	//  return: T 数据
	//	remark: 提供对内部数据的直接访问，小心使用!!
	//******************************************
	T *GetPtAt(DWORD index){
		SARRAYNODE      *pDest=NULL;
		ASSERT(index>=0&&index<m_nCount);
		pDest=GetDestSegEntry(index);
		return ((T *)pDest->pData)+index-m_nCurIndex;
	}
	
	T & ElementAt(DWORD index)
	{
		SARRAYNODE      *pDest=NULL;
		ASSERT(index>=0&&index<m_nCount);
		pDest=GetDestSegEntry(index);
		return ((T *)pDest->pData)[index-m_nCurIndex];
	}

	T & operator[](DWORD index){ return ElementAt(index);}
	
	//******************************************
	//	name:GetSize
	//	function:获取数组的数据容量
	//	input:　
	//  return: int 数据容量
	//	remark: 
	//******************************************
	int GetSize(){return m_nCount;}
	
	void SetCompackKey(BYTE byKey){ m_byZipKey=byKey;}
	//******************************************
	//	name:SetAt
	//	function:修改数组指定位置的数据
	//	input:　T newElement-新数据
	//			int index-指定索引号
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark: 
	//******************************************
	BOOL SetAt(DWORD index,T &newElement)
	{
		SARRAYNODE      *pDest=NULL;
		if(index<0||index>m_nCount-1)
			return FALSE;
		pDest=GetDestSegEntry(index);
		*(pDest->pData+index-m_nCurIndex)=newElement;
		return TRUE;
	}
	

	//******************************************
	//	name:SortInsert
	//	function:在已经有序的数据插入一个新结点并保持有序
	//	input:	T newElement-待插入的数据
	//			BOOL bRepeat:允许重复
	//			CBCMP funCmp:比较回调接口
	//			LPARAM lParam:用户参数
	//	return:插入位置
	//	2006.10.19
	//******************************************
	int SortInsert(T newElement,BOOL bRepeat=TRUE,FUNCMP funCmp=NULL,LPARAM lParam=0,UINT sfType=SFT_ANY)
	{
		if(m_nCount==0)
		{
			Add(newElement);
			return 0;
		}
		int nInsertPos=-1;
		int nPos=SortFindEx(&newElement,0,m_nCount,funCmp,lParam,sfType,&nInsertPos);
		if(nPos!=-1)
		{
			if(!bRepeat) return -1;
			if(sfType==SFT_TAIL)//找到最后一个与目标相等的元素,需要插入到该元素后
				nPos=nPos++; 
		}else
		{
			ASSERT(nInsertPos!=-1);
			nPos=nInsertPos;
		}
		InsertAt(nPos,newElement);
		return nPos;
	}
	
	//******************************************
	//	name:SortFind
	//	function:查找一个数据在表中的位置，表中数据必须有序
	//	input:	T *pData 比较数据
	//			CBCMP funCmp:比较回调接口
	//			LPARAM lParam:用户参数
	//			int nBegin:开始位置
	//			BOOL bHeadFirst:有相同的元素时确定是第一个还是最后一个
	//	return:数据位置，-1：没有找到
	//	2006.10.19
	//******************************************
	int SortFind(T *pData,FUNCMP funCmp=NULL,LPARAM lParam=0,int nBegin=0,UINT sfType=SFT_ANY)
	{
		if(nBegin>=(int)m_nCount)
			return -1;
		else
			return SortFindEx(pData,nBegin,m_nCount,funCmp,lParam,sfType);
	}

	//************************************************
	//	_SortFindEx
	//	在[nBegin,nEnd)之间查找一个插入位置
	//************************************************
	int SortFindEx(T *pData,int nBegin,int nEnd,FUNCMP funCmp,LPARAM lParam,UINT sfType,int *pInsertPos=NULL)
	{
		ASSERT(nBegin<=nEnd);
		if(nBegin==nEnd) //没有待查空间
		{
			if(pInsertPos) *pInsertPos=nBegin;
			return -1;
		}
		int nMiddle=(nBegin+nEnd)/2;
		T *pMiddle=GetPtAt(nMiddle);
		int nCmp=ItemCompare(pData,pMiddle,funCmp,lParam);
		if(nCmp==0)
		{
			if(sfType==SFT_ANY) 
				return nMiddle;
			int nRet=-1;
			if(sfType==SFT_HEAD)
				nRet=_SortFindHead(pData,nBegin,nMiddle,funCmp,lParam);
			else //if(sfType==SFT_TAIL)
				nRet=_SortFindTail(pData,nMiddle+1,nEnd,funCmp,lParam);
			if(nRet==-1) nRet=nMiddle;
			return nRet;
		}
		if(nBegin==nEnd-1)
		{//只有一个元素空间
			if(pInsertPos)
			{
				*pInsertPos=nMiddle;
				if(nCmp>0) (*pInsertPos)++;
			}
			return -1;
		}
		if(nCmp>0)
			return SortFindEx(pData,nMiddle+1,nEnd,funCmp,lParam,sfType,pInsertPos);
		else
			return SortFindEx(pData,nBegin,nMiddle,funCmp,lParam,sfType,pInsertPos);
	}

	//******************************************
	//	name:InsertAt
	//	function:在数组指定位置插入一个新数据
	//	input:　int index-指定索引号
	//			T newElement-待插入的数据
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark: 
	//******************************************
	int InsertAt(DWORD index,T newElement)
	{
		if(index>m_nCount)
			return -1;
		if(index==m_nCount) 
			return Add(newElement);
		SARRAYNODE *pDest=GetDestSegEntry(index);
		if(pDest->dwUsed==m_nGrowBy)
		{
			SARRAYNODE *pAddNode=NewNode();
			if(!pAddNode) return -1;
			pAddNode->dwUsed=pDest->dwUsed-(WORD)(index-m_nCurIndex);
			memcpy(pAddNode->pData,
				pDest->pData+index-m_nCurIndex,
				sizeof(T)*pAddNode->dwUsed);
			*(pDest->pData+index-m_nCurIndex)=newElement;
			pDest->dwUsed=(WORD)(index-m_nCurIndex+1);
			SARRAYNODE *pNext=pDest->pNextNode;
			pDest->pNextNode=pAddNode;
			pAddNode->pPrevNode=pDest;
			if(pNext){
				pNext->pPrevNode=pAddNode;
				pAddNode->pNextNode=pNext;
			}else
			{
				m_pTailNode=pAddNode;
			}
			m_nEmpty+=m_nGrowBy-1;
		}else
		{
			memmove(pDest->pData+index-m_nCurIndex+1,
				pDest->pData+index-m_nCurIndex,
				(pDest->dwUsed-(index-m_nCurIndex))*sizeof(T));
			*(pDest->pData+index-m_nCurIndex)=newElement;
			pDest->dwUsed++;
			m_nEmpty--;
		}
		m_nCount++;
		if(IsNeedCompack()) Compack();
		return index;
	}

	//******************************************
	//	name:RemoveAt
	//	function:删除数组中指定索引号中包含的数据
	//	input:　int index-指定索引号
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark: 
	//******************************************
	BOOL RemoveAt(DWORD index)
	{
		if(index<0||index>=m_nCount)
			return FALSE;
		SARRAYNODE *pDest=GetDestSegEntry(index);
		m_nCount--;
		if(pDest->dwUsed==1)
		{
			SARRAYNODE *pPrev=pDest->pPrevNode;
			SARRAYNODE *pNext=pDest->pNextNode;
			if(!pPrev) 
			{
				m_pHeadNode=pNext;
				if(m_pHeadNode) m_pHeadNode->pPrevNode=NULL;
			}else
			{
				pPrev->pNextNode=pNext;
			}
			if(!pNext)
			{
				m_pTailNode=pPrev;
				if(m_pTailNode) m_pTailNode->pNextNode=NULL;
			}else
			{
				pNext->pPrevNode=pPrev;
			}
			if(pDest==m_pCurNode)
			{
				m_pCurNode=m_pHeadNode;
				m_nCurIndex=m_pHeadNode?0:-1;
			}
			FreeNode(pDest);
			m_nEmpty-=(m_nGrowBy-1);
		}else
		{
			memmove(pDest->pData+index-m_nCurIndex,
				pDest->pData+index-m_nCurIndex+1,
				sizeof(T)*(pDest->dwUsed-(index-m_nCurIndex+1)));
			pDest->dwUsed--;
			m_nEmpty++;
			if(IsNeedCompack()) Compack();
		}
		return TRUE;
	}

	//******************************************
	//	name:SetSize()
	//	function:设置数据的容量
	//	input:　int size -数据的容量
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark:只允许扩大容量
	//******************************************
	BOOL SetSize(DWORD size){
		SARRAYNODE *pNewNode=NULL;
		if(m_nCount>=size)
			return FALSE;
		if(m_pTailNode)
		{
			if((WORD)(size-m_nCount)<=m_nGrowBy-m_pTailNode->dwUsed)
			{//neet not to enlarge the buffer
				m_pTailNode->dwUsed+=(WORD)(size-m_nCount);
				m_nEmpty-=size-m_nCount;
				m_nCount=size;
				return TRUE;
			}else if(m_pTailNode->dwUsed!=m_nGrowBy)
			{//fill the tail node to full
				m_nEmpty+=m_nGrowBy-m_pTailNode->dwUsed;
				m_nCount+=m_nGrowBy-m_pTailNode->dwUsed;
				m_pTailNode->dwUsed=m_nGrowBy;
			}
		}
		int newsegs=(size-m_nCount+m_nGrowBy-1)/m_nGrowBy;
		for(int i=0;i<newsegs;i++)
		{
			pNewNode=NewNode();
			if(!pNewNode) return FALSE;
			pNewNode->dwUsed=(i<newsegs-1)?m_nGrowBy:(WORD)(size-m_nCount);
			if(!m_pHeadNode)
			{
				m_pHeadNode=m_pTailNode=m_pCurNode=pNewNode;
				m_nCount+=pNewNode->dwUsed;
				m_nCurIndex=0;
				continue;
			}
			ASSERT(m_pTailNode);
			m_pTailNode->pNextNode=pNewNode;
			pNewNode->pPrevNode=m_pTailNode;
			m_pTailNode=pNewNode;
			m_nCount+=pNewNode->dwUsed;
		}
		return TRUE;
	}
	
	//******************************************
	//	name:SetGrowBy()
	//	function:设置数据增长幅度
	//	input:　
	//  return: 
	//	remark: 在初始化时使用
	//******************************************
	void SetGrowBy(WORD nGrowBy)
	{
		ASSERT(m_nCount==0);
		m_nGrowBy=nGrowBy;
	}

	//******************************************
	//	name:RemoveAll()
	//	function:清空对象中的数据
	//	input:　
	//  return: BOOL TURE-成功；FALSE－失败
	//	remark:
	//******************************************
	BOOL RemoveAll(){
		if(m_pHeadNode==NULL)
			return TRUE;
		Free();
		m_pCurNode=m_pHeadNode=m_pTailNode=NULL;
		m_nCurIndex=-1;
		m_nCount=0;
		m_nEmpty=0;
		return TRUE;
	}
	
	//*********************************************
	// name: Attach
	// function:将一个外部数组交给CSArray管理,当排列有序时可以利用CSArray提供了2分查询功能
	// input: T *pData,数据区; int nLen,数据长度
	// return:BOOL TURE-成功；FALSE－失败
	// remark:数组为空时可用,外部数据需要使用new分配内存,否则需要调用Detach后自己手动释放内存
	//*********************************************
	BOOL Attach(T *pData,int nLen)
	{
		if(m_pHeadNode!=NULL) return FALSE;
		m_pCurNode=m_pHeadNode=m_pTailNode=new SARRAYNODE;
		m_pHeadNode->pNextNode=NULL;
		m_pHeadNode->pPrevNode=NULL;
		m_pHeadNode->pData=pData;
		m_pHeadNode->dwUsed=nLen;
		m_nGrowBy=nLen;
		m_nCount=nLen;
		m_nCurIndex=0;
		return TRUE;
	}
	
	//*********************************************
	// name: Detach
	// function:提取内部数据
	// param:
	//   int *pnLen[output]:数据长度
	// return:
	//	 NULL-失败,其它-数据指针
	// remark:一般与Attach配合使用
	//*********************************************
	T *Detach(int *pnLen)
	{
		if(m_pHeadNode==NULL) return NULL;
		if(m_pHeadNode!=m_pTailNode) return NULL;
		if(pnLen) *pnLen=m_nCount;
		T *pRet=m_pHeadNode->pData;
		delete m_pHeadNode;
		m_pHeadNode=m_pTailNode=m_pCurNode=NULL;
		m_nCount=0;
		m_nCurIndex=0;
		return pRet;
	}

	//*********************************************
	//  name:Compack
	//	function :压缩数据
	//	remark:将链表中各结点段的空余空间进行整理，释放多余的节点
	//*********************************************
	void Compack()
	{
		SARRAYNODE *pTmp1,*pTmp2=NULL;
		if(m_pHeadNode==m_pTailNode) return;
		pTmp1=FindNotFullNode(m_pHeadNode,m_pTailNode);
		while(pTmp1)
		{
			if(pTmp2==NULL)	pTmp2=pTmp1->pNextNode;
			ASSERT(pTmp2);
			while(pTmp1->dwUsed!=(DWORD)m_nGrowBy&&pTmp2)
			{
				if(pTmp2->dwUsed<=(m_nGrowBy-pTmp1->dwUsed))
				{//the node can free
					memcpy(pTmp1->pData+pTmp1->dwUsed,
						pTmp2->pData,
						sizeof(T)*pTmp2->dwUsed);
					pTmp1->dwUsed+=pTmp2->dwUsed;
					if(m_pTailNode==pTmp2)
					{
						m_pTailNode=pTmp2->pPrevNode;
						m_pTailNode->pNextNode=NULL;
						FreeNode(pTmp2);
						pTmp2=NULL;
					}else
					{
						pTmp2->pPrevNode->pNextNode=pTmp2->pNextNode;
						pTmp2->pNextNode->pPrevNode=pTmp2->pPrevNode;
						SARRAYNODE *p=pTmp2->pNextNode;
						FreeNode(pTmp2);
						pTmp2=p;
					}
				}else
				{//dest node remain data
					DWORD dwMoveItems=m_nGrowBy-pTmp1->dwUsed;
					memcpy(pTmp1->pData+pTmp1->dwUsed,
						pTmp2->pData,
						sizeof(T)*dwMoveItems);
					memmove(pTmp2->pData,
						pTmp2->pData+dwMoveItems,
						sizeof(T)*(pTmp2->dwUsed-dwMoveItems));
					pTmp2->dwUsed-=(WORD)dwMoveItems;
					pTmp1->dwUsed=m_nGrowBy;
				}
			}//while
			SARRAYNODE *pFind=FindNotFullNode(pTmp1,pTmp2);
			if(!pFind)
			{
				pTmp1=FindNotFullNode(pTmp1,m_pTailNode);
			}else
			{
				pTmp1=pFind;
			}
			pTmp2=NULL;
		}//while
		m_pCurNode=m_pHeadNode;//avoid the current node been deleted
		m_nCurIndex=0;
		if(m_pTailNode)
			m_nEmpty=m_nGrowBy-m_pTailNode->dwUsed;
		else
			m_nEmpty=0;
	}	
protected:


	//********************************************
	//	name:IsNeedCompack
	//	function:判断是否需要对数据进行压缩
	//*********************************************
	virtual BOOL IsNeedCompack()
	{
		if(m_nEmpty<m_nGrowBy) return FALSE;
		return ((m_nEmpty-m_nGrowBy)*100>m_nCount*m_byZipKey);
	}
	//******************************************
	//	name:Free()
	//	function:释放对象占用的空间，
	//	input:
	//  return:void
	//	remark:内部使用,外部要清空对象请使用RemoveAll()接口
	//******************************************
	virtual void Free()
	{
		SARRAYNODE *temp1,*temp2;
		temp1=m_pHeadNode;
		while(temp1!=NULL)
		{
			temp2=temp1->pNextNode;
			FreeNode(temp1);
			temp1=temp2;
		}
	}

	//T *pItem1 :测试用的数据
	//T *pItem2 :来自数组中的数据
	int ItemCompare(T *pItem1,T *pItem2,FUNCMP funCmp,LPARAM lParam)
	{
		if(funCmp)
			return funCmp(pItem1,pItem2,lParam);
		else
			return memcmp(pItem1,pItem2,sizeof(T));
	}
	//************************************************
	//	_SortFindHead
	//	在[nBegin,nEnd)之间搜索第一个与pData相等的元素
	//************************************************
	int _SortFindHead(T *pData,int nBegin,int nEnd,FUNCMP funCmp,LPARAM lParam)
	{
		ASSERT(nBegin<=nEnd);
		if(nBegin==nEnd) return -1;
		int nMid=(nBegin+nEnd)/2;
		T *pMiddle=GetPtAt(nMid);
		int nCmp=ItemCompare(pData,pMiddle,funCmp,lParam);
		if(nCmp==0)
		{
			if(nBegin==nEnd-1)	//只有一个元素
				return nBegin;
			int nRet=_SortFindHead(pData,nBegin,nMid,funCmp,lParam);
			if(nRet==-1) nRet=nMid;
			return nRet;
		}else
		{
			return _SortFindHead(pData,nMid+1,nEnd,funCmp,lParam);
		}
	}

	//************************************************
	//	_SortFindTail
	//	在[nBegin,nEnd)之间搜索最后一个与pData相等的元素
	//************************************************
	int _SortFindTail(T *pData,int nBegin,int nEnd,FUNCMP funCmp,LPARAM lParam)
	{
		ASSERT(nBegin<=nEnd);
		if(nBegin==nEnd) return -1;
		int nMid=(nBegin+nEnd)/2;
		T *pMiddle=GetPtAt(nMid);
		int nCmp=ItemCompare(pData,pMiddle,funCmp,lParam);
		if(nCmp==0)
		{
			if(nBegin==nEnd-1)	//只有一个元素
				return nBegin;
			int nRet=_SortFindTail(pData,nMid+1,nEnd,funCmp,lParam);
			if(nRet==-1) nRet=nMid;
			return nRet;
		}else
		{
			return _SortFindTail(pData,nBegin,nMid,funCmp,lParam);
		}
	}

	//************************************************
	//	FreeNode
	//	释放该结点占用的内存
	//************************************************
	void FreeNode(SARRAYNODE *pNode)
	{
		delete []pNode->pData;
		delete pNode;
	}
	//*********************************************
	//	查找有空闲空间的结点
	//*********************************************
	SARRAYNODE *FindNotFullNode(SARRAYNODE *pBegin,SARRAYNODE *pEnd)
	{
		SARRAYNODE *pRet=pBegin;
		if(!pEnd) pEnd=m_pTailNode;
		while(pRet&&pRet->dwUsed==m_nGrowBy&&pRet!=pEnd) pRet=pRet->pNextNode;
		if(pRet==m_pTailNode||pRet==pEnd) pRet=NULL;
		return pRet;
	}

	//*******************************************
	//	name NewNode
	//	function:为一个新结点分配空间
	//********************************************
	SARRAYNODE *NewNode()
	{
		SARRAYNODE *pRet=new SARRAYNODE;
		if(!pRet) return NULL;
		pRet->pPrevNode=pRet->pNextNode=NULL;
		pRet->dwUsed=0;
		pRet->pData=new T[m_nGrowBy];
		if(pRet->pData==NULL)
		{
			delete pRet;
			return NULL;
		}
		return pRet;
	}
	//******************************************
	//	name:GetDestSegEntry()
	//  function:获取数据所在链表的节点指针
	//	input:	int index -数据索引
	//  return: SARRAYNODE * -数据所在链表的节点指针
	//	remark:内部使用，
	//******************************************
	SARRAYNODE * GetDestSegEntry(DWORD index)
	{
		SARRAYNODE * ret=NULL;
		int			i=0;
		DWORD			offset=0;
		if(index<m_nCurIndex)// dest pData is in before cur pData segment 
		{
			if(m_nCurIndex>2*index)
			{	//find the seg from head;
				ret=m_pHeadNode;
				while(offset+ret->dwUsed<=index)
				{
					offset+=ret->dwUsed;
					ret=ret->pNextNode;
				}
			}else	
			{	//find the seg from cur seg;
				ret=m_pCurNode;
				offset=m_nCurIndex;
				while(offset>index)
				{
					ret=ret->pPrevNode;
					offset-=ret->dwUsed;
				}
			}
			m_nCurIndex=offset;
		}else if(index>=(m_nCurIndex+m_pCurNode->dwUsed))
		{
			if(m_nCurIndex+m_nCount<2*index)
			{//find the seg from cur
				ret=m_pCurNode;
				offset=m_nCurIndex;
				while(offset+ret->dwUsed<=index)
				{
					offset+=ret->dwUsed;
					ret=ret->pNextNode;
				}
			}else
			{//find the seg from tail
				ret=m_pTailNode;
				offset=m_nCount-ret->dwUsed;;
				while(offset>index)
				{
					ret=ret->pPrevNode;
					offset-=ret->dwUsed;
				}
			}
			m_nCurIndex=offset;
		}else
		{//in cur pData seg
			ret=m_pCurNode;
		}
		m_pCurNode=ret;
		return ret;
	}
	
/////////////////////////////////////////////////////////////////////
//  private data
	DWORD m_nCurIndex;	//当前节点的第一个元素的索引号
	DWORD m_nCount;		//数组对象包括的数据数
	DWORD m_nGrowBy;	//每次增长的尺寸
	DWORD m_nEmpty;		//空出的空间数量

	BYTE m_byZipKey;	//压缩阀值
	SARRAYNODE * m_pCurNode;	//链表中当前节点的指针，在数据检索时确定从向个方向搜索
	SARRAYNODE * m_pHeadNode;	//头节点
	SARRAYNODE * m_pTailNode;	//尾结点
};

#endif // !defined(AFX_SARRAY_H__B1D40C22_2698_4202_921E_36D447EA4199__INCLUDED_)
