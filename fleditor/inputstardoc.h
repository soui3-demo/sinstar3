#ifndef _INPUTSTARDOC_H_
#define _INPUTSTARDOC_H_

#define ISSAVEID	'SI'

typedef struct tagSISHEAD{
	WORD	wISID;			// Must "IS" ISSAVEID
	WORD	wFileID;		// 文件类型 ID
	WORD	wVersionMajor;	// 主版本号
	WORD	wVersionMinor;	// 副版本号
	DWORD	dwData;			// 附加数据
}SISHEAD,*PSISHEAD;

BOOL ISDOC_ReadHeader(FILE *f,WORD wFileID,WORD wVerMajor,WORD wVerMinor,DWORD *pdwData=NULL,SISHEAD *pHead=NULL);
void ISDOC_WriteHeader(FILE *f,WORD wFileID,WORD wVerMajor,WORD wVerMinor,DWORD dwData=0,SISHEAD *pHead=NULL);
BOOL ISDOC_ReadHeader2(const void *pBuf,int nLen,WORD wFileID,WORD wVerMajor,WORD wVerMinor,DWORD *pdwData=NULL,SISHEAD *pHead=NULL);

#endif//_INPUTSTARDOC_H_