#include "stdafx.h"
#include "inputstardoc.h"

BOOL ISDOC_ReadHeader(FILE *f,WORD wFileID,WORD wVerMajor,WORD wVerMinor,DWORD *pdwData,SISHEAD *pHead)
{
	SISHEAD head;
	if(sizeof(SISHEAD)!=fread(&head,1,sizeof(SISHEAD),f))
		return FALSE;
	if(ISSAVEID!=head.wISID ||
		wFileID!=head.wFileID ||
		wVerMajor>head.wVersionMajor ||
		wVerMinor>head.wVersionMinor )
		return FALSE;
	if(pdwData) *pdwData=head.dwData;
	if(pHead) *pHead=head;
	return TRUE;
}

void ISDOC_WriteHeader(FILE *f,WORD wFileID,WORD wVerMajor,WORD wVerMinor,DWORD dwData,SISHEAD *pHead)
{
	SISHEAD head;
	head.wISID=	ISSAVEID;
	head.wFileID=wFileID;
	head.wVersionMajor=wVerMajor;
	head.wVersionMinor=wVerMinor;
	head.dwData=dwData;
	fwrite(&head,sizeof(head),1,f);
	if(pHead) *pHead=head;
}