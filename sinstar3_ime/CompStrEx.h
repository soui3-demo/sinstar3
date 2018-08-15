#pragma once

#define MAXCOMPSIZE 256

class CCompStrEx :
	public COMPOSITIONSTRING
{
public:
	void Init(void);
	void Insert(ISinstar *pSinstar3,int nLeft,int nRight,LPCWSTR pszComp,int nLen);
	void MoveCaret(int nPos,BOOL bSet);
	DWORD  GetTargetPos();
 	BYTE				byCompReadAttr[MAXCOMPSIZE*2];
 	DWORD				dwCompReadCls[MAXCOMPSIZE*2+1];
 	TCHAR				szCompReadStr[MAXCOMPSIZE*2];
	BYTE				byCompAttr[MAXCOMPSIZE];
	DWORD				dwCompCls[MAXCOMPSIZE+1];
	TCHAR				szCompStr[MAXCOMPSIZE];					//
	TCHAR				szResultStr[MAXCOMPSIZE*2];				//	
};
