#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <tchar.h>
#include "..\include\iscomm.h"
#include "..\include\isProtocol.h"

static HWND	s_hWndServer=NULL;			//server窗口
static HANDLE	s_hMapDataReq=0;		//MapFile Handle
static HANDLE	s_hMapDataAck=0;		//MapFile Handle
static PMSGDATA	s_pDataReq=NULL;		//MapFile
static PMSGDATA	s_pDataAck=NULL;		//MapFile
static HANDLE	s_hMutexReq=0;			//共享内存访问互斥体,要访问共享内存必须先获得该互斥体的所有权，以防止多个客户端同时写共享内存
static BYTE	s_byBuf[MAX_BUF_ACK];	//保存在本地的缓冲区
static COMPINFO s_CompInfo={"载入...",0};			//当前的编码信息
static FLMINFO  s_flmInfo={0};			//当前的外文库信息
static char	s_szUserDict[50]="无";	//用户词典名称
static UINT    s_uMsgID=0;			//通讯消息ID

static UINT		s_uCount=0;			//访问计数

const UINT ISComm_GetCommMsgID(){
	if(s_uMsgID==0) s_uMsgID=RegisterWindowMessage(MSG_NAME_SINSTAR2);
	return s_uMsgID;
}

//获得配置文件
BOOL ISComm_GetConfig(LPSTR pszConfig)
{
	TCHAR szPathSys[MAX_PATH];
	GetSystemDirectory(szPathSys,MAX_PATH);
	sprintf(pszConfig,"%s\\sinstar2.ini",szPathSys);
	return GetFileAttributes(pszConfig)!=0xFFFFFFFF;
}

//获取服务器运行路径
BOOL ISComm_GetSvrPath(LPSTR pszPath)
{
	char szConfig[MAX_PATH]={0};
	if(!ISComm_GetConfig(szConfig)) return FALSE;
	return GetPrivateProfileString("server","exe",NULL,pszPath,MAX_PATH,szConfig)!=0;
}

//保存服务器运行路径
BOOL ISComm_SetSvrPath(LPSTR pszPath)
{
	char szConfig[MAX_PATH]={0};
	if(!ISComm_GetConfig(szConfig)) return FALSE;
	WritePrivateProfileString("server","exe",pszPath,szConfig);
	return TRUE;
}

//获取服务器数据保存路径
BOOL ISComm_GetDataPath(LPSTR pszPath)
{
	char szConfig[MAX_PATH]={0};
	if(!ISComm_GetConfig(szConfig)) return FALSE;
	return GetPrivateProfileString("server","data",NULL,pszPath,MAX_PATH,szConfig)!=0;
}

void ClearComm()
{
	if(!s_hWndServer) return;
	s_uCount--;
	if(s_uCount==0)
	{
		UnmapViewOfFile(s_pDataReq);
		CloseHandle(s_hMapDataReq);
		UnmapViewOfFile((LPBYTE)s_pDataAck-sizeof(DWORD));
		CloseHandle(s_hMapDataAck);
		CloseHandle(s_hMutexReq);

		s_hWndServer=NULL;
		s_pDataReq=NULL;
		s_hMapDataReq=0;
		s_hMutexReq=0;
		s_pDataAck=NULL;
		s_hMapDataAck=0;

		if(s_CompInfo.hIcon) DeleteObject(s_CompInfo.hIcon);
		s_CompInfo.hIcon=NULL;
	}
}

//从缓冲区中创建一个位图句柄
HBITMAP Helper_CreateBitmapFromBuffer(LPBYTE pBuffer, DWORD cbSize)
{
	HBITMAP hBmp=NULL;
	HDC hdc=NULL;
	HPALETTE hPal=NULL,hOldPal=NULL;
	BITMAPFILEHEADER *pbmfHeader=(BITMAPFILEHEADER *)pBuffer;
	DWORD dwOffset=sizeof(BITMAPFILEHEADER);
	// File type should be 'BM'

	BITMAPINFOHEADER *pbmiHeader = (LPBITMAPINFOHEADER)(pBuffer+dwOffset) ;
	BITMAPINFO * pbmInfo = (LPBITMAPINFO)(pBuffer+dwOffset) ;
	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = pbmiHeader->biClrUsed ? pbmiHeader->biClrUsed : 
		1 << pbmiHeader->biBitCount;

	LPVOID lpDIBBits;
	if (pbmfHeader->bfType != ((WORD) ('M' << 8) | 'B')) return NULL;
	if( pbmInfo->bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(pbmInfo->bmiColors + pbmInfo->bmiHeader.biClrUsed) + 
		((pbmInfo->bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(pbmInfo->bmiColors + nColors);

	hdc=GetDC(NULL);
	// Create the palette
	if( nColors <= 256 )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) malloc(nSize);
		int i;
		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;

		for( i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = pbmInfo->bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = pbmInfo->bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = pbmInfo->bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}
		hPal=CreatePalette(pLP);
		free(pLP);
		hOldPal=(HPALETTE)SelectObject(hdc,hPal);
		RealizePalette(hdc);
	}


	hBmp = CreateDIBitmap( hdc,		// handle to device context 
		pbmiHeader,	// pointer to bitmap size and format data 
		CBM_INIT,	// initialization flag 
		lpDIBBits,	// pointer to initialization data 
		pbmInfo,	// pointer to bitmap color-format data 
		DIB_RGB_COLORS);		// color-data usage 
	if( hPal )
	{
		SelectObject(hdc,hOldPal);
		DeleteObject(hPal);
	}
	ReleaseDC(NULL,hdc);
	return hBmp;
}

PMSGDATA ISComm_OnSeverNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if(wParam==NT_SERVEREXIT)
	{//服务器退出
		ClearComm();
		strcpy(s_CompInfo.szName,"服务退出");
	}else if(wParam==NT_COMPINFO)
	{//广播编码消息
		LPBYTE pData=s_pDataAck->byData;
		memcpy(&s_CompInfo,pData,CISIZE_BASE);
		if(s_CompInfo.hIcon) DeleteObject(s_CompInfo.hIcon);
		s_CompInfo.hIcon=NULL;
		if(s_pDataAck->sSize>CISIZE_BASE)
		{
			pData+=CISIZE_BASE;
			memcpy(&s_CompInfo.crIconKey,pData,sizeof(COLORREF));
			pData+=sizeof(COLORREF);
			s_CompInfo.hIcon=Helper_CreateBitmapFromBuffer(pData,s_pDataAck->sSize-CISIZE_BASE-sizeof(COLORREF));
		}
	}else if(wParam==NT_FLMINFO)
	{//广播的英文库信息
		memcpy(&s_flmInfo,s_pDataAck->byData,s_pDataAck->sSize);
	}else if(wParam==NT_COMPERROR)
	{//编码错误
		strcpy(s_CompInfo.szName,"错误");
	}else if(wParam==NT_UDICTINFO)
	{//用户词典名称
		strcpy(s_szUserDict,s_pDataAck->byData);
	}
	return s_pDataAck;
}

BOOL ISComm_OpenServer()
{
	char szServerPath[MAX_PATH]={0};
	STARTUPINFO         si={0};    
	PROCESS_INFORMATION pi;         
	DWORD dwWaitRet=1;
	HWND  hWndActive=GetActiveWindow();
	//查询任务栏窗口句柄，确保系统已经成功启动。
	HWND  hWndShell=FindWindow("Shell_TrayWnd",NULL);
	if(!hWndShell) return FALSE;
	si.cb  = sizeof(si);  
	si.dwFlags = STARTF_USESHOWWINDOW; 
	si.wShowWindow  = SW_HIDE;
	if(!ISComm_GetSvrPath(szServerPath)) return FALSE;
	//以后台启动方式创建服务器进程
	if(!CreateProcess(szServerPath,NULL,NULL,NULL,FALSE,CREATE_NEW_PROCESS_GROUP,NULL,NULL,&si,&pi))
	{
		return FALSE;
	}
	//等待服务器初始化完成
	dwWaitRet=WaitForInputIdle(pi.hProcess, 10000); 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	//恢复当前活动窗口
	SetForegroundWindow(hWndActive);
	if(dwWaitRet!=0)
	{
		return FALSE;
	}
	return TRUE;
}

//生成一个保证不会被重复的客户ID
HANDLE ISComm_GenerateClientID(DWORD *pdwID)
{
	char szClientID[50];
	DWORD dwID=0;
	HANDLE hEvtClient=0;
	for(;;)
	{
		dwID=GetTickCount();
		sprintf(szClientID,"client-%08X",dwID);
		hEvtClient=CreateEvent(NULL,TRUE,FALSE,szClientID);
		if(GetLastError()==ERROR_ALREADY_EXISTS)
			CloseHandle(hEvtClient);
		else
			break;
	}
	*pdwID=dwID;
	return hEvtClient;
}

//----------------------------------------------
//	初始化通讯,注册服务器
//----------------------------------------------
BOOL ISComm_Login(HWND hWnd)
{

	if(!s_hWndServer)
	{
		LPBYTE pBuf=NULL;
		s_hMutexReq=OpenMutex(MUTEX_ALL_ACCESS,FALSE,NAME_REQ_SYNCOMMUTEX);
		if(!s_hMutexReq)
		{
			if(!ISComm_OpenServer()) return FALSE;
			s_hMutexReq=OpenMutex(MUTEX_ALL_ACCESS,FALSE,NAME_REQ_SYNCOMMUTEX);
		}
		//打开双向通讯数据通道

		s_hMapDataReq=OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE,FALSE,NAME_REQ_MAPFILE);
		s_pDataReq=(PMSGDATA)MapViewOfFile(s_hMapDataReq,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);

		s_hMapDataAck=OpenFileMapping(FILE_MAP_READ,FALSE,NAME_ACK_MAPFILE);
		pBuf=(LPBYTE)MapViewOfFile(s_hMapDataAck,FILE_MAP_READ,0,0,0);
		memcpy(&s_hWndServer,pBuf,sizeof(DWORD));
		s_pDataAck=(PMSGDATA)(pBuf+sizeof(DWORD));
	}
	if(hWnd)
	{
		if(ISComm_SendMsg(CT_LOGIN,NULL,0,hWnd)!=ISACK_SUCCESS)
		{
			return FALSE;
		}
		s_uCount++;
	}
	return TRUE;
}

//----------------------------------------------
//	释放连接,取消注册
//----------------------------------------------
void  ISComm_Logout(HWND hWnd)
{
	if(!s_hWndServer) return ;
	ISComm_SendMsg(CT_LOGOUT,NULL,0,hWnd);
	ClearComm();
}

//------------------------------------------------------
//	向服务器发送一个消息
//------------------------------------------------------
DWORD ISComm_SendMsg(DWORD dwType,LPVOID pData,short sSize,HWND hWnd)
{
	DWORD dwRet=0;
	PMSGDATA pIsBuf=(PMSGDATA)s_pDataReq;
	if(!s_hWndServer) return ISACK_UNINIT;
	if(!IsWindow(s_hWndServer)) return ISACK_SVRCANCEL;
	if(sSize>MAX_BUF_REQ) return ISACK_ERRBUF;

	WaitForSingleObject(s_hMutexReq,INFINITE);
	pIsBuf->sSize=sSize;
	memcpy(pIsBuf->byData,pData,sSize);
	dwRet=SendMessage(s_hWndServer,ISComm_GetCommMsgID(),dwType,(LPARAM)hWnd);
	//提取数据
	pIsBuf=(PMSGDATA)s_pDataAck;
	if(pIsBuf->sSize!=0)
		memcpy(s_byBuf,pIsBuf,pIsBuf->sSize+sizeof(short));
	else
		*(short*)s_byBuf=0;
	ReleaseMutex(s_hMutexReq);
	return dwRet;
}

//使用消息类型的最高位来标识该消息需要立即返回
DWORD ISComm_PostMsg(DWORD dwType,LPVOID pData,short sSize,HWND hWnd)
{
	return ISComm_SendMsg(dwType|0x80000000,pData,sSize,hWnd);
}

DWORD ISComm_SkinChange(HWND hUIWnd)
{
	return ISComm_PostMsg(CT_SKINCHANGE,NULL,0,hUIWnd);
}

PMSGDATA ISComm_GetData()
{
	return (PMSGDATA) s_byBuf;
}

PCOMPINFO ISComm_GetCompInfo()
{
	return &s_CompInfo;
}

PFLMINFO  ISComm_GetFlmInfo()
{
	return &s_flmInfo;
}

char *    ISComm_GetUserDict()
{
	return s_szUserDict;
}

DWORD ISComm_KeyIn(LPCSTR pszText,short sSize,BYTE byMask,HWND hWnd)
{
	if(sSize==-1) sSize=strlen(pszText);
	if(sSize>=MAX_SENTLEN) return ISACK_ERRBUF;

	s_byBuf[0]=byMask;
	memcpy(s_byBuf+1,pszText,sSize);
	return ISComm_PostMsg(CT_KEYIN,(LPVOID)s_byBuf,(short)(sSize+1),hWnd);
}


DWORD ISComm_QueryCand(LPCSTR pszComp,char cCompLen,BYTE byMask,HWND hWnd)
{
	s_byBuf[0]=byMask;
	memcpy(s_byBuf+1,pszComp,cCompLen);
	return ISComm_SendMsg(CT_QUERYCAND,(LPVOID)s_byBuf,(short)(cCompLen+1),hWnd);
}

DWORD ISComm_QueryComp(LPCSTR pszPhrase,char cPhraseLen)
{
	return ISComm_SendMsg(CT_QUERYCOMP,(LPVOID)pszPhrase,cPhraseLen,0);
}

DWORD ISComm_QueryUserDict(LPCTSTR pszkey,char cKeyLen)
{
	return ISComm_SendMsg(CT_QUERYUDICT,(LPVOID)pszkey,cKeyLen,0);
}

DWORD ISComm_Forecast(LPCSTR pszComp,char cCompLen)
{
	return ISComm_SendMsg(CT_FORECAST,(LPVOID)pszComp,cCompLen,0);
}

DWORD ISComm_SpellQueryComp(LPCSTR pszPhrase,char cPhraseLen)
{
	return ISComm_SendMsg(CT_SPELLQUERYCOMP,(LPVOID)pszPhrase,cPhraseLen,0);
}

DWORD ISComm_SpellForecast(char szPrefix[2],LPBYTE *pbyBlurs,BYTE bySyllables)
{
	DWORD dwRet;
	BYTE i;
	short sOffset=0;
	memcpy(s_byBuf,szPrefix,2);
	sOffset+=2;
	s_byBuf[sOffset++]=bySyllables;
	for(i=0;i<bySyllables;i++)
	{
		short sCount;
		memcpy(&sCount,pbyBlurs[i],2);
		memcpy(s_byBuf+sOffset,pbyBlurs[i],sCount*2+2+2);
		sOffset+=sCount*2+2+2;
	}	
	dwRet=ISComm_SendMsg(CT_SPELLFORECAST,s_byBuf,sOffset,0);
	return dwRet;
}

DWORD ISComm_En2Ch(LPCSTR pszWord,char cWordLen)
{
	return ISComm_SendMsg(CT_EN2CH,(LPVOID)pszWord,cWordLen,0);
}

DWORD ISComm_Ch2En(LPCSTR pszWord,char cWordLen)
{
	return ISComm_SendMsg(CT_CH2EN,(LPVOID)pszWord,cWordLen,0);
}

DWORD ISComm_Spell2ID(LPCSTR pszSpell,char cCompLen)
{
	return ISComm_SendMsg(CT_SPELL2ID,(LPVOID)pszSpell,cCompLen,0);
}

DWORD ISComm_SpellGetBlur(LPCSTR pszSpell,char cCompLen)
{
	return ISComm_SendMsg(CT_SPELLGETBLUR,(LPVOID)pszSpell,cCompLen,0);
}

DWORD ISComm_SpellQueryCandEx(LPBYTE *pbyBlur,BYTE bySyllables,BYTE byFlag)
{
	DWORD dwRet;
	short sOffset=0;
	BYTE  i;
	s_byBuf[sOffset++]=byFlag;
	s_byBuf[sOffset++]=bySyllables;
	for(i=0;i<bySyllables;i++)
	{
		short sCount=0;
		memcpy(&sCount,pbyBlur[i],2);
		memcpy(s_byBuf+sOffset,pbyBlur[i],sCount*2+2+2);
		sOffset+=sCount*2+2+2;
	}
	dwRet= ISComm_SendMsg(CT_SPELLQUERYCAND_EX,s_byBuf,sOffset,0);
	return dwRet;
}

DWORD ISComm_TTS(LPCSTR pszText,char cTextLen,BYTE byMask)
{
	s_byBuf[0]=byMask;
	memcpy(s_byBuf+1,pszText,cTextLen);
	return ISComm_PostMsg(CT_TTS,s_byBuf,(short)(cTextLen+1),0);
}

DWORD ISComm_RateAdjust(LPCSTR pszComp,char cCompLen,LPCSTR pszPhrase,char cPhraseLen,BYTE byMode,HWND hWnd)
{
	BYTE *pBuf=s_byBuf;
	short sSize=0;
	pBuf[sSize]=cCompLen;
	sSize++;
	memcpy(pBuf+sSize,pszComp,cCompLen);
	sSize+=cCompLen;
	pBuf[sSize]=cPhraseLen;
	sSize++;
	memcpy(pBuf+sSize,pszPhrase,cPhraseLen);
	sSize+=cPhraseLen;
	pBuf[sSize++]=byMode;
	return ISComm_SendMsg(CT_RATEADJUST,s_byBuf,sSize,hWnd);
}

DWORD ISComm_PhraseDel(LPCSTR pszComp,char cCompLen,LPCSTR pszPhrase,char cPhraseLen,HWND hWnd)
{
	BYTE *pBuf=s_byBuf;
	short sSize=0;
	pBuf[sSize]=cCompLen;
	sSize++;
	memcpy(pBuf+sSize,pszComp,cCompLen);
	sSize+=cCompLen;
	pBuf[sSize]=cPhraseLen;
	sSize++;
	memcpy(pBuf+sSize,pszPhrase,cPhraseLen);
	sSize+=cPhraseLen;
	return ISComm_SendMsg(CT_PHRASEDEL,s_byBuf,sSize,hWnd);
}

DWORD ISComm_EnQueryCand(LPCSTR pszText,char cTextLen)
{
	return ISComm_SendMsg(CT_ENQUERYCAND,(LPVOID)pszText,cTextLen,0);
}

DWORD ISComm_LineQueryCand(LPCSTR pszComp,char cCompLen)
{
	return ISComm_SendMsg(CT_LINEQUERYCAND,(LPVOID)pszComp,cCompLen,0);
}

DWORD ISComm_MakePhrase(LPCSTR pszText,char cTextLen)
{
	return ISComm_SendMsg(CT_MAKEPHRASE,(LPVOID)pszText,cTextLen,0);
}

DWORD ISComm_ShowServer(LPCSTR pszPageName,char cTextLen)
{
	return ISComm_SendMsg(CT_SHOWSERVER,(LPVOID)pszPageName,cTextLen,0);
}

DWORD ISComm_UDQueryCand(LPCSTR pszComp,char cCompLen)
{
	return ISComm_SendMsg(CT_UDQUERYCAND,(LPVOID)pszComp,cCompLen,0);
}

DWORD ISComm_ServerVersion()
{
	return ISComm_SendMsg(CT_SERVERVERSION,NULL,0,0);
}

int ISComm_PhraseRate(LPCSTR pszPhrase,char cPhraseLen)
{
	int nRet=-1;
	if(ISComm_SendMsg(CT_PHRASERATE,(LPVOID)pszPhrase,cPhraseLen,0)==ISACK_SUCCESS)
	{
		PMSGDATA pData=ISComm_GetData();
		nRet=pData->byData[0];
	}
	return nRet;
}

DWORD ISComm_Blur_Set(BOOL bBlur)
{
	return ISComm_SendMsg(CT_BLUR_SET,(LPVOID)&bBlur,4,0);
}

DWORD ISComm_Blur_Add(char *pszSpell1,char *pszSpell2)
{
	char szBuf[50];
	char cLen1=strlen(pszSpell1);
	char cLen2=strlen(pszSpell2);
	if(cLen1>6 || cLen1<0 || cLen2>6 || cLen2<0) return ISACK_ERROR;
	memcpy(szBuf,pszSpell1,cLen1+1);
	memcpy(szBuf+cLen1+1,pszSpell2,cLen2+1);
	return ISComm_SendMsg(CT_BLUR_ADD,(LPVOID)szBuf,(short)(cLen1+1+cLen2+1),0);
}

DWORD ISComm_Blur_Del(char *pszSpell1,char *pszSpell2)
{
	char szBuf[50];
	char cLen1=strlen(pszSpell1);
	char cLen2=strlen(pszSpell2);
	if(cLen1>6 || cLen1<0 || cLen2>6 || cLen2<0) return ISACK_ERROR;
	memcpy(szBuf,pszSpell1,cLen1+1);
	memcpy(szBuf+cLen1+1,pszSpell2,cLen2+1);
	return ISComm_SendMsg(CT_BLUR_DEL,(LPVOID)szBuf,(short)(cLen1+1+cLen2+1),0);
}

DWORD ISComm_Blur_DelAll()
{
	return ISComm_SendMsg(CT_BLUR_DELALL,NULL,0,0);
}

DWORD ISComm_Blur_Query()
{
	return ISComm_SendMsg(CT_BLUR_QUERY,NULL,0,0);
}

DWORD ISComm_SortWordByBiHua(LPCTSTR pszBiHua,char cBiHuaLen,LPCTSTR pszWordList,short nWords)
{
	char *pbuf=NULL;
	DWORD dwRet;
	//检查是否全部为汉字
	int i=0;
	for(;i<nWords;i++)
	{
		if(!(pszWordList[i*2]&0x80)) return ISACK_ERROR;
	}
	pbuf=(char*)malloc(cBiHuaLen+1+nWords*2+1);
	memcpy(pbuf,pszBiHua,cBiHuaLen);
	pbuf[cBiHuaLen]=0;
	memcpy(pbuf+cBiHuaLen+1,pszWordList,nWords*2);
	pbuf[cBiHuaLen+1+nWords*2]=0;
	dwRet=ISComm_SendMsg(CT_SORTWORDBYBIHUA,(LPVOID)pbuf,(short)(cBiHuaLen+1+nWords*2+1),0);
	free(pbuf);
	return dwRet;
}

//查询一个汉字的拼音,为记忆拼音输入词组做准备
DWORD ISComm_QueryWordSpell(char szWord[2])
{
	return ISComm_SendMsg(CT_QUERYWORDSPELL,(LPVOID)szWord,2,0);
}

//记忆一个拼音输入词组
DWORD ISComm_SpellMemoryEx(LPCSTR pszText,char cTextLen,BYTE (*pbySpellID)[2])
{
	short sLen=cTextLen*2+1;
	BYTE *pbuf=(BYTE*)malloc(sLen);
	DWORD dwRet;
	pbuf[0]=cTextLen;
	memcpy(pbuf+1,pszText,cTextLen);
	memcpy(pbuf+1+cTextLen,pbySpellID,cTextLen);
	dwRet=ISComm_PostMsg(CT_SPELLMEM_EX,(LPVOID)pbuf,sLen,0);
	free(pbuf);
	return dwRet;
}

DWORD ISComm_UserDict_List()
{
	return ISComm_SendMsg(CT_USERDICT_LIST,NULL,0,0);
}

DWORD ISComm_UserDict_Open(LPCTSTR pszUserDict)
{
	return ISComm_SendMsg(CT_USERDICT_OPEN,(LPVOID)pszUserDict,(short)strlen(pszUserDict),0);
}

DWORD ISComm_SymbolConvert(char cSymbol,char cType)
{
	char szBuf[2]={cSymbol,cType};
	return ISComm_SendMsg(CT_SYMBOL_CONVERT,(LPVOID)szBuf,2,0);
}

DWORD ISComm_UserDict_Max()
{
	return ISComm_SendMsg(CT_USERDICT_MAX,NULL,0,0);
}

DWORD ISComm_UserDict_Item(DWORD dwItem)
{
	return ISComm_SendMsg(CT_USERDICT_ITEM,(LPVOID)&dwItem,sizeof(DWORD),0);
}

DWORD ISComm_UserDict_Locate(LPCTSTR pszkey,char cKeyLen)
{
	return ISComm_SendMsg(CT_USERDICT_LOCATE,(LPVOID)pszkey,cKeyLen,0);
}

DWORD ISComm_Comp_List()
{
	return ISComm_SendMsg(CT_COMP_LIST,NULL,0,0);
}

DWORD ISComm_Comp_Open(LPCTSTR pszComp)
{
	return ISComm_SendMsg(CT_COMP_OPEN,(LPVOID)pszComp,(short)strlen(pszComp),0);
}

BOOL ISComm_IsSvrWorking()
{
	return s_hWndServer!=0;
}

BOOL  ISComm_CheckComp(LPCTSTR pszComp,char cComplen,BYTE byMask)
{
	char szBuf[MAX_COMPLEN+1];
	if(cComplen<=s_CompInfo.cCodeMax) return TRUE;
	if(cComplen>MAX_COMPLEN) return FALSE;
	szBuf[0]=byMask;
	strncpy(szBuf+1,pszComp,cComplen);
	return ISACK_SUCCESS==ISComm_SendMsg(CT_CHECKCOMP,(LPVOID)szBuf,(short)(cComplen+1),0);
}

DWORD ISComm_Bldsp_Get(BOOL *pbCe2,BOOL *pbCe3,BOOL *pbCa4)
{
	DWORD dwRet=ISComm_SendMsg(CT_DATA_BLDSP_GET,NULL,0,0);
	if(dwRet==ISACK_SUCCESS)
	{
		PMSGDATA pData=ISComm_GetData();
		if(pbCe2) *pbCe2=pData->byData[0]&BLDSP_CE2;
		if(pbCe3) *pbCe3=pData->byData[0]&BLDSP_CE3;
		if(pbCa4) *pbCa4=pData->byData[0]&BLDSP_CA4;
	}
	return dwRet;
}

DWORD ISComm_Bldsp_Set(BYTE byMask,BOOL bCe2,BOOL bCe3,BOOL bCa4)
{
	BYTE byBuf[2]={byMask,0};
	if(byMask&BLDSP_CE2 && bCe2) byBuf[1]|=BLDSP_CE2;
	if(byMask&BLDSP_CE3 && bCe3) byBuf[1]|=BLDSP_CE3;
	if(byMask&BLDSP_CA4 && bCa4) byBuf[1]|=BLDSP_CA4;
	return ISACK_SUCCESS==ISComm_SendMsg(CT_DATA_BLDSP_SET,(LPVOID)byBuf,2,0);
}

BOOL  ISComm_SvrTray_Get()
{
	ISComm_SendMsg(CT_SVRTRAY_GET,NULL,0,0);
	return ISComm_GetData()->byData[0];
}

void  ISComm_SvrTray_Set(BOOL bTray)
{
	BYTE byTray=bTray?1:0;
	ISComm_SendMsg(CT_SVRTRAY_SET,&byTray,1,0);
}

LPCTSTR ISComm_Svr_Pages()
{
	LPCTSTR pszPages=NULL;
	if(ISACK_SUCCESS==ISComm_SendMsg(CT_SVR_PAGES,NULL,0,0))
	{
		PMSGDATA pMsg=ISComm_GetData();
		pszPages=(LPCTSTR)pMsg->byData;
	}
	return pszPages;
}