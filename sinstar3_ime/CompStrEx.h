#pragma once

#define MAXCOMPSIZE 256

class CCompStrEx :
	public COMPOSITIONSTRING
{
public:
	template<class T>
	void ARRAY_MOVE(T * buf,int nSize,int nBeginOld,int nBeginNew)
	{
		if(nSize>nBeginOld) memmove(buf+nBeginNew,buf+nBeginOld,(nSize-nBeginOld)*sizeof(T));
	}
	template<class T>
	void ARRAY_COPY(T * pDest,const T *pSour,int nSize)
	{
		if(nSize>0) memcpy(pDest,pSour,nSize*sizeof(T));
	}
	template<class T>
	void ARRAY_SET(T *buf,T data,int nSize)
	{
		for(int i=0;i<nSize;i++) buf[i]=data;
	}

	void Init(void);
	void Insert(ISinstar *pBaiduJP3,int nLeft,int nRight,LPCWSTR pszComp,int nLen);
	void MoveCaret(int nPos,BOOL bSet);
	DWORD  GetTargetPos();
 	BYTE				byCompReadAttr[MAXCOMPSIZE*2];
 	DWORD				dwCompReadCls[MAXCOMPSIZE*2+1];
 	TCHAR				szCompReadStr[MAXCOMPSIZE*2];
	BYTE				byCompAttr[MAXCOMPSIZE];
	DWORD				dwCompCls[MAXCOMPSIZE+1];
	TCHAR				szCompStr[MAXCOMPSIZE];					// Æ½¼ÙÃû×Ö·û´®
	TCHAR				szResultStr[MAXCOMPSIZE*2];				//	
	BOOL				bComposing;
};
