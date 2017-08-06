#include "StdAfx.h"
#include "CompStrEx.h"

void CCompStrEx::Init(void)
{
	BOOL _bComposing=bComposing;
	memset(this, 0, sizeof(CCompStrEx));
	dwSize=sizeof(CCompStrEx);
 	dwCompReadAttrOffset = (LPBYTE)byCompReadAttr - (LPBYTE)this;
 	dwCompReadClauseOffset = (LPBYTE)dwCompReadCls - (LPBYTE)this;
 	dwCompReadStrOffset = (LPBYTE)szCompReadStr - (LPBYTE)this;
	dwCompReadAttrOffset =dwCompAttrOffset = (LPBYTE)byCompAttr - (LPBYTE)this;
	dwCompReadClauseOffset = dwCompClauseOffset = (LPBYTE)dwCompCls - (LPBYTE)this;
	dwCompReadStrOffset =dwCompStrOffset = (LPBYTE)szCompStr - (LPBYTE)this;
	dwResultStrOffset = (LPBYTE)szResultStr - (LPBYTE)this;
	dwPrivateOffset=(LPBYTE)&bComposing-(LPBYTE)this;
	dwPrivateSize=sizeof(BOOL);
	bComposing=_bComposing;
}

void CCompStrEx::Insert(ISinstar *pBaiduJP3,int nLeft,int nRight,LPCWSTR pszComp,int nLen)
{
	int x1,x2;
	if(nLeft==-1 && nRight==-1)
	{
		x1=dwCursorPos;
		x2=dwCursorPos;
	}else if(nLeft==0 && nRight==-1)
	{
		x1=0;
		x2=dwCompStrLen;
	}else
	{
		x1=nLeft;
		x2=nRight;
	}
	//make room for new comp string
	int nEnd1=x2;
	int nEnd2=x1+nLen;
	ARRAY_MOVE(szCompStr,dwCompStrLen,nEnd1,nEnd2);
	//copy new comp string to emptied room
	ARRAY_COPY(szCompStr+x1,pszComp,nLen);
	dwCompStrLen+=nLen-x2+x1;

	dwCursorPos=nEnd2;

	//update attribute array
	dwCompAttrLen=dwCompStrLen;
	if(dwCompStrLen>0)
	{
		int nClauseCount=pBaiduJP3->GetCompositionSegments();
		for(int i=0;i<nClauseCount;i++)
		{
			dwCompCls[i+1]=pBaiduJP3->GetCompositionSegmentEnd(i);
			BYTE byAttr=pBaiduJP3->GetCompositionSegmentAttr(i);
			ARRAY_SET(byCompAttr+dwCompCls[i],byAttr,dwCompCls[i+1]-dwCompCls[i]);
		}
		dwCompClauseLen=(nClauseCount+1)*sizeof(DWORD);
	}else
	{
		dwCompClauseLen=0;
	}

	//make compreadstr a copy of compstr
	ARRAY_COPY(szCompReadStr,szCompStr,dwCompStrLen);
	dwCompReadStrLen=dwCompStrLen;
	ARRAY_COPY(byCompReadAttr,byCompAttr,dwCompAttrLen);
	dwCompReadAttrLen=dwCompStrLen;
	ARRAY_COPY(dwCompReadCls,dwCompCls,dwCompClauseLen);
	dwCompReadClauseLen=dwCompClauseLen;

	szCompStr[dwCompStrLen]=0;
	szCompReadStr[dwCompReadStrLen]=0;
}

void CCompStrEx::MoveCaret(int nPos,BOOL bSet)
{
	if(bSet)
		dwCursorPos=nPos;
	else
		dwCursorPos+=nPos;
	if(dwCursorPos&0xf0000000) 
		dwCursorPos = 0;
	if(dwCursorPos > dwCompStrLen) 
		dwCursorPos = dwCompStrLen;
}

DWORD CCompStrEx::GetTargetPos()
{
	DWORD nRet=-1;
	
	for(DWORD i=0;dwCompClauseLen && i<dwCompClauseLen/sizeof(DWORD)-1;i++)
	{
		if(byCompAttr[dwCompCls[i]]==ATTR_TARGET_CONVERTED || byCompAttr[dwCompCls[i]]== ATTR_TARGET_NOTCONVERTED)
		{
			nRet=dwCompCls[i];
			break;
		}
	}
	return nRet;
}