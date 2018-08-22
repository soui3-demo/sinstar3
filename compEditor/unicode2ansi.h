#ifndef UNICODE2ANSI_H
#define UNICODE2ANSI_H

#define TMPANSI_FILE "c:\\uni2ansi.tmp"

#define U2A_OK		0	//转换成功
#define U2A_CANCEL	1	//不是UNICODE编码，未转换
#define U2A_FAIL	2	//转换失败

int Unicode2Ansi(BYTE *pBuffer,DWORD len);
BOOL CheckFileUnicode(LPCTSTR pszFileName);

#endif//UNICODE2ANSI_H