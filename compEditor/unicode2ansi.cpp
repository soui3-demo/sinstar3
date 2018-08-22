#include "stdafx.h"
#include "unicode2ansi.h"

//*********************************************************
//	为Unicode编码的缓冲区生成一个与应的ANSI编码的文件
//	如果该文件是UNICODE编码则进行转换，否则返回失败
//*********************************************************
int Unicode2Ansi(BYTE *pBuffer,DWORD len)
{
	if(len<2||pBuffer[0]!=0xff||pBuffer[1]!=0xfe) return U2A_CANCEL;
	DWORD index=2;
	BYTE *pTmp=pBuffer+index;
	FILE *f=fopen(TMPANSI_FILE,"wb");
	if(!f) return U2A_FAIL;
	char multibyte[3];
	while(index<len)
	{
		WideCharToMultiByte(CP_ACP,0,(LPCWSTR)pTmp,1,multibyte,3,NULL,NULL);
		if(multibyte[0]<0)
			fwrite(multibyte,2,1,f);
		else
			fwrite(multibyte,1,1,f);
		index+=2;
		pTmp+=2;
	}
	fclose(f);
	return U2A_OK;
}

BOOL CheckFileUnicode(LPCTSTR pszFileName)
{
	CFile f;
	if(!f.Open(pszFileName,CFile::modeRead|CFile::typeBinary)) return FALSE;
	BYTE byBuf[2]={0};
	f.Read(byBuf,2);
	f.Close();
	return byBuf[0]==0xff&&byBuf[1]==0xfe;
}