//**************************************************************
//	启程链表
//	2006.11.22 增加AdjustNode接口
//	2004.7.5   实现基本功能
//**************************************************************
#ifndef _SLIST_H_
#define _SLIST_H_

#ifndef ASSERT 
#define ASSERT(a) 0
#endif

#define SLISTPOS	DWORD

template<class T>
class CSList
{
	typedef struct tagListNode{
		struct tagListNode *pPrev;
		T		data;
		tagListNode *pNext;
	}LISTNODE,*PLISTNODE;
public:

	CSList()
	{
		m_pHead=m_pTail=NULL;
		m_nCount=0;
	}
	virtual ~CSList(){RemoveAll();};

	BOOL	IsEmpty()
	{
		return m_nCount==0;
	}
	
	//将一个节点调整到另一个节点后
	//SLISTPOS posDest:待调整节点
	//SLISTPOS posAfter:基准节点,NULL表示加到最前
	void AdjustNode(SLISTPOS posDest,SLISTPOS posAfter)
	{
		//从原位置删除
		LISTNODE *pNode=(LISTNODE*)posDest;
		LISTNODE *pAfter=(LISTNODE*)posAfter;
		if(pNode->pPrev==pAfter) return;//不需要调整

		if(pNode->pNext)
		{
			pNode->pNext->pPrev=pNode->pPrev;
		}else
		{
			ASSERT(m_pTail==pNode);
			m_pTail=pNode->pPrev;
		}
		if(pNode->pPrev)
		{
			pNode->pPrev->pNext=pNode->pNext;
		}else
		{
			ASSERT(m_pHead==pNode);
			m_pHead=pNode->pNext;
		}
		//加入到新位置
		if(pAfter)
		{
			LISTNODE *pNext=pAfter->pNext;
			pNode->pPrev=pAfter;
			pAfter->pNext=pNode;
			if(pNext)
			{
				pNext->pPrev=pNode;
				pNode->pNext=pNext;
			}else
			{
				pNode->pNext=NULL;
				m_pTail=pNode;
			}
		}else
		{
			pNode->pPrev=NULL;
			ASSERT(m_pHead) ;
			pNode->pNext=m_pHead;
			m_pHead->pPrev=pNode;
			m_pHead=pNode;
		}
	}

	SLISTPOS AddTail(T data)
	{
		PLISTNODE pNode=new LISTNODE;
		if(!pNode) return NULL;
		pNode->pPrev=pNode->pNext=NULL;
		pNode->data=data;
		m_nCount++;
		if(m_pHead==NULL)
		{
			m_pHead=m_pTail=pNode;
			return (SLISTPOS)pNode;
		}
		ASSERT(m_pTail);
		m_pTail->pNext=pNode;
		pNode->pPrev=m_pTail;
		m_pTail=pNode;
		return (SLISTPOS)pNode;
	}

	SLISTPOS AddHead(T data)
	{
		PLISTNODE pNode=new LISTNODE;
		if(!pNode) return NULL;
		pNode->pPrev=pNode->pNext=NULL;
		pNode->data=data;
		m_nCount++;
		if(m_pHead==NULL)
		{
			m_pHead=m_pTail=pNode;
			return (SLISTPOS)pNode;
		}
		ASSERT(m_pHead);
		m_pHead->pPrev=pNode;
		pNode->pNext=m_pHead;
		m_pHead=pNode;
		return (SLISTPOS)pNode;
	}

	SLISTPOS InsertAfter(SLISTPOS posDest,T data)
	{
		LISTNODE *pDest=(LISTNODE*)posDest;
		if(!pDest) return AddTail(data);
		PLISTNODE pNode=new LISTNODE;
		if(!pNode) return NULL;
		PLISTNODE pPrev,pNext;
		pPrev=pDest;
		pNext=pDest->pNext;
		pNode->pPrev=pPrev;
		pNode->pNext=pNext;
		pNode->data=data;
		if(pPrev){
			pPrev->pNext=pNode;
		}else
		{
			m_pHead=pNode;
		}
		if(pNext)
		{
			pNext->pPrev=pNode;
		}else
		{
			m_pTail=pNode;
		}
		m_nCount++;
		return (SLISTPOS)pNode;
	}

	SLISTPOS InsertBefore(SLISTPOS posDest,T data)
	{
		LISTNODE *pDest=(LISTNODE*)posDest;
		if(!pDest) return AddHead(data);
		PLISTNODE pNode=new LISTNODE;
		if(!pNode) return NULL;
		PLISTNODE pPrev,pNext;
		pPrev=pDest->pPrev;
		pNext=pDest;
		pNode->pPrev=pPrev;
		pNode->pNext=pNext;
		pNode->data=data;
		if(pPrev){
			pPrev->pNext=pNode;
		}else
		{
			m_pHead=pNode;
		}
		if(pNext)
		{
			pNext->pPrev=pNode;
		}else
		{
			m_pTail=pNode;
		}
		m_nCount++;
		return (SLISTPOS)pNode;
	}
	
	T	GetTail()
	{
		ASSERT(m_pTail);
		return m_pTail->data;
	}

	T	GetHead()
	{
		ASSERT(m_pHead);
		return m_pHead->data;
	}
	
	T	IndexAt(int idx)
	{
		SLISTPOS pos=Index2Pos(idx);
		ASSERT(pos);
		return GetAt(pos);
	}
	
	int Pos2Index(SLISTPOS posNode)
	{
		int nRet=-1;
		LISTNODE *pNode=(LISTNODE*)posNode;
		while(pNode)
		{
			nRet++;
			pNode=pNode->pPrev;
		}
		return nRet;
	}
	
	SLISTPOS Index2Pos(int nIndex)
	{
		if(nIndex<0 || nIndex>=m_nCount) return NULL;
		if(nIndex<(m_nCount>>1))
		{
			LISTNODE *pNode=m_pHead;
			int  i=0;
			while(i<nIndex) 
			{
				pNode=pNode->pNext;
				i++;
			}
			return (SLISTPOS)pNode;
		}else
		{
			LISTNODE *pNode=m_pTail;
			int i=m_nCount-1;
			while(i>nIndex)
			{
				pNode=pNode->pPrev;
				i--;
			}
			return (SLISTPOS)pNode;
		}
	}

	static T	GetAt(SLISTPOS posNode)
	{
		ASSERT(posNode);
		LISTNODE *pNode=(LISTNODE*)posNode;
		return pNode->data;
	}

	static void SetAt(SLISTPOS posNode,T data)
	{
		ASSERT(posNode);
		LISTNODE *pNode=(LISTNODE*)posNode;
		pNode->data=data;
	}
	
	static T & ElementAt(SLISTPOS posDest)
	{
		ASSERT(posDest);
		LISTNODE *pNode=(LISTNODE*)posDest;
		return pNode->data;
	}

	static T * GetPtAt(SLISTPOS posDest)
	{
		ASSERT(posDest);
		LISTNODE *pNode=(LISTNODE*)posDest;
		return &pNode->data;
	}

	static T	GetNext(SLISTPOS &posDest)
	{
		ASSERT(posDest);
		LISTNODE *pNode=(LISTNODE*)posDest;
		PLISTNODE pTmp=pNode;
		posDest=(SLISTPOS)pNode->pNext;
		return pTmp->data;
	}

	static void GoNext(SLISTPOS &posDest)
	{
		ASSERT(posDest);
		LISTNODE *pNode=(LISTNODE*)posDest;
		PLISTNODE pTmp=pNode;
		posDest=(SLISTPOS)pNode->pNext;
	}

	static T	GetPrev(SLISTPOS &posDest)
	{
		ASSERT(posDest);
		LISTNODE *pNode=(LISTNODE*)posDest;
		PLISTNODE pTmp=pNode;
		posDest=(SLISTPOS)pNode->pPrev;
		return pTmp->data;
	}

	static void GoPrev(SLISTPOS &posDest)
	{
		ASSERT(posDest);
		LISTNODE *pNode=(LISTNODE*)posDest;
		PLISTNODE pTmp=pNode;
		posDest=(SLISTPOS)pNode->pPrev;
	}

	void RemoveHead()
	{
		if(m_pHead) RemoveAt((SLISTPOS)m_pHead);
	}

	void RemoveTail()
	{
		if(m_pTail) RemoveAt((SLISTPOS)m_pTail);
	}

	void RemoveLeft(SLISTPOS posDest)
	{
		int nRemoveCount=0;
		PLISTNODE pNodeBegin=m_pHead;
		PLISTNODE pNodeEnd=(PLISTNODE)posDest;
		PLISTNODE pPrev=NULL;
		PLISTNODE pTmp=pNodeBegin;
		BOOL bOver=FALSE;
		while(!bOver&&pTmp)
		{
			pPrev=pTmp;
			pTmp=pPrev->pNext;
			bOver=pPrev==pNodeEnd;
			delete pPrev;
			nRemoveCount++;
		}
		if(pTmp)
		{
			pTmp->pPrev=NULL;
			m_pHead=pTmp;
		}else
		{
			m_pHead=m_pTail=NULL;
		}
		m_nCount-=nRemoveCount;
	}

	void RemoveRight(SLISTPOS posDest)
	{
		int nRemoveCount=0;
		PLISTNODE pNodeBegin=m_pTail;
		PLISTNODE pNodeEnd=(PLISTNODE)posDest;
		PLISTNODE pNext=NULL;
		PLISTNODE pTmp=pNodeBegin;
		BOOL bOver=FALSE;
		while(!bOver&&pTmp)
		{
			pNext=pTmp;
			pTmp=pNext->pPrev;
			bOver=pNext==pNodeEnd;
			delete pNext;
			nRemoveCount++;
		}
		if(pTmp)
		{
			pTmp->pNext=NULL;
			m_pTail=pTmp;
		}else
		{
			m_pHead=m_pTail=NULL;
		}
		m_nCount-=nRemoveCount;
	}

	T	RemoveAt(SLISTPOS posDest)
	{
		ASSERT(posDest);
		LISTNODE *pNode=(LISTNODE*)posDest;
		if(pNode->pNext)
		{
			pNode->pNext->pPrev=pNode->pPrev;
		}else
		{
			ASSERT(m_pTail==pNode);
			m_pTail=pNode->pPrev;
		}
		if(pNode->pPrev)
		{
			pNode->pPrev->pNext=pNode->pNext;
		}else
		{
			ASSERT(m_pHead==pNode);
			m_pHead=pNode->pNext;
		}
		T ret=pNode->data;
		delete pNode;
		m_nCount--;
		return ret;
	}

	void	RemoveAll()
	{
		PLISTNODE pNode=m_pHead;
		while(pNode)
		{
			PLISTNODE pTmp=pNode;
			pNode=pTmp->pNext;
			delete pTmp;
		}
		m_pHead=m_pTail=NULL;
		m_nCount=0;
	}
	
	void operator +=(CSList<T> &lstDest)
	{
		SLISTPOS pos=lstDest.GetHeadPosition();
		while(pos) AddTail(lstDest.GetNext(pos));
	}
	
	void CombineHead(CSList<T> *pLst)
	{
		if(pLst->m_nCount==0) return;
		m_nCount+=pLst->m_nCount;
		if(!m_pHead)
		{
			m_pHead=pLst->m_pHead;
			m_pTail=pLst->m_pTail;
		}else
		{
			m_pHead->pPrev=pLst->m_pTail;
			m_pHead->pPrev->pNext=m_pHead;
			m_pHead=pLst->m_pHead;
		}
		pLst->m_nCount=0;
		pLst->m_pHead=NULL;
		pLst->m_pTail=NULL;
	}

	void CombineTail(CSList<T> *pLst)
	{
		if(pLst->m_nCount==0) return;

		m_nCount+=pLst->m_nCount;
		if(!m_pHead)
		{
			m_pHead=pLst->m_pHead;
			m_pTail=pLst->m_pTail;
		}else
		{
			m_pTail->pNext=pLst->m_pHead;
			m_pTail->pNext->pPrev=m_pTail;
			m_pTail=pLst->m_pTail;
		}
		pLst->m_nCount=0;
		pLst->m_pHead=NULL;
		pLst->m_pTail=NULL;
	}

	int		GetCount(){ return m_nCount;}
	SLISTPOS GetHeadPosition() { return (SLISTPOS)m_pHead;}
	SLISTPOS GetTailPosition()	{ return (SLISTPOS)m_pTail;}

private:
	LISTNODE *m_pHead,*m_pTail;
	int		  m_nCount;
};
#endif//_SLIST_H_