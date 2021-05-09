#define _CRT_SECURE_NO_WARNINGS
#include "..\targetver.h"
#include <windows.h>
#include <assert.h>
#include "commfile.h"


COMFILE CF_Init(LPBYTE pbyBuf,int nBufSize,int nValidSize,int nOffset)
{
	COMFILE f;
	f.pbyData=pbyBuf;
	f.nBufSize=nBufSize;
	f.nLen=nValidSize;
	f.nOffset=nOffset;
	return f;
}

int CF_Read(COMFILE *pcf,void *pBuf,int nSize)
{
	int nReaded=pcf->nLen-pcf->nOffset;
	if(nReaded>nSize) nReaded=nSize;
	if(nReaded==0) return 0;
	memcpy(pBuf,pcf->pbyData+pcf->nOffset ,nReaded);
	pcf->nOffset+=nReaded;
	return nReaded;
}

BYTE CF_ReadChar(COMFILE *pcf)
{
	assert(!CF_EOF(pcf));
	return pcf->pbyData[pcf->nOffset++];
}

int CF_ReadString(COMFILE *pcf,char *pBuf,int nBufLen)
{
	int nLen=(int)strlen((char*)pcf->pbyData+pcf->nOffset);
	assert(nLen<nBufLen-1);
	strncpy(pBuf,(char*)pcf->pbyData+pcf->nOffset,nLen+1);
	pcf->nOffset+=nLen+1;
	return nLen+1;
}

int CF_Write(COMFILE *pcf,const void *pBuf,int nSize)
{
	int nWrited=nSize;
	if(pcf->nOffset+nSize>pcf->nBufSize) nWrited=pcf->nBufSize-pcf->nOffset;
	if(nWrited==0) return 0;
	memcpy(pcf->pbyData+pcf->nOffset,pBuf,nWrited);
	pcf->nOffset+=nWrited;
	if(pcf->nOffset>pcf->nLen) pcf->nLen=pcf->nOffset;
	return nWrited;
}

int CF_WriteChar(COMFILE *pcf,BYTE byData)
{
	return CF_Write(pcf,&byData,1);
}

BOOL CF_EOF(COMFILE *pcf)
{
	return pcf->nOffset==pcf->nLen;
}

int CF_Tell(COMFILE *pcf)
{
	return pcf->nOffset;
}

int CF_Seek(COMFILE *pcf,int nOffset,int nOrigin)
{
	int nNew=pcf->nOffset;
	switch(nOrigin)
	{
	case SEEK_SET:nNew=nOffset;break;
	case SEEK_CUR:nNew+=nOffset;break;
	case SEEK_END:nNew=pcf->nLen+nOffset;break;
	}
	if(nNew>=0 && nNew<=pcf->nLen)
	{
		pcf->nOffset=nNew;
	}
	return pcf->nOffset;
}
