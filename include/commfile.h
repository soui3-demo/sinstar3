#pragma once

//提供一个类似文件操作方式的数据缓冲区操作对象
typedef struct tagCOMFILE
{
	int	  nBufSize;	//缓冲区总长度
	BYTE *pbyData;	//数据缓冲区
	int   nLen;		//有效数据长度
	int	  nOffset;	//当前改写位置
}COMFILE,*PCOMFILE;

typedef enum tagCFMODE{CFM_READ=0,CFM_WRITE=1}CFMODE;
#ifdef __cplusplus
extern "C"{
#endif//__cplusplus

COMFILE CF_Init(LPBYTE pbyBuf,int nBufSize,int nValidSize,int nOffset);

int CF_Read(COMFILE *pcf,void *pBuf,int nSize);
BYTE CF_ReadChar(COMFILE *pcf);
int CF_Write(COMFILE *pcf,void *pBuf,int nSize);
int CF_WriteChar(COMFILE *pcf,BYTE byData);
BOOL CF_EOF(COMFILE *pcf);
int CF_Tell(COMFILE *pcf);
int CF_Seek(COMFILE *pcf,int nOffset,int nOrigin);

#ifdef __cplusplus
}
#endif//__cplusplus
