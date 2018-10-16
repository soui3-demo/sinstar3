#ifndef ISCOMM_H
#define ISCOMM_H

#include "commfile.h"

//输入之星通讯使用的内存文件映射的名称
#define NAME_REQ_MAPFILE	_T("_sinstar3 req map file")	//客户端向服务器发送请求的数据通道
#define MAX_BUF_REQ_MAP		1<<12						//4K						
#define MAX_BUF_REQ			(MAX_BUF_REQ_MAP)									
#define NAME_ACK_MAPFILE	_T("_sinstar3 ack map file")	//服务器向客户端发送指示的数据通道
#define MAX_BUF_ACK_MAP		1<<15						//32K
#define MAX_BUF_ACK			((MAX_BUF_ACK_MAP)-sizeof(HWND))

//输入之星通讯使用的同步事件名称,客户端通过申请lock该Mutex来保证能够获得数据写入权限，以防止多个客户端同时写入
#define NAME_REQ_SYNCOMMUTEX	_T("_sinstar3 req synchronization mutex")

#define MSG_NAME_SINSTAR3		_T("_sinstar3 communicate msg")	//通讯时使用的消息名称，取代老版本固定的消息ID，方便服务器返回数据

#define SVR_PROXY_NAME			TEXT("_sinstar3_svr_proxy_window_{AC7451C1-3528-42E4-ADF8-E35F3CF24646}")

#pragma pack(push,1)

typedef struct tagMSGDATA{
	short	sSize;		//返回数据长度
	BYTE	byData[1];	//返回数据起始地址
}MSGDATA,*PMSGDATA;

typedef struct tagIMEFLAGDATA
{
	char * pData;
	unsigned int    nLen;
}IMEFLAGDATA;

typedef struct tagCOMPINFO
{
	char szName[50];	//名称
	char szCode[50];	//码元
	char cWildChar;		//万能键
	char cCodeMax;		//最大码长
	char szUrl[50];		//相关网址
	DWORD dwWords;		//已经编码字符数
	BOOL bSymbolFirst;	//标点首编码
	DWORD dwUnused:4;	//服务器使用的标志，对输入法外壳没有意义
	DWORD dwReserved:28;	//保留,初始化为0

	IMEFLAGDATA * pImeFlagData;
}COMPINFO,*PCOMPINFO;

typedef struct tagFLMINFO
{
	char szName[50];
	char szAddition[50];
	char szAddFont[LF_FACESIZE];
}FLMINFO,*PFLMINFO;
#pragma pack(pop)

#define CISIZE_BASE		164		//基本编码信息数据量

#define MAX_SENTLEN		50		//语句最大长度
#define MAX_WORDLEN		30		//词组最大长度
#define MAX_COMPLEN		20		//系统支持的最大码长

#define RATE_USERDEF	0xFF	//自定义编码使用的词频
#define RATE_FORECAST	0xFE	//智能预测得到的词使用的词频
#define RATE_GBK		0xFD	//GBK flag
#define RATE_USERCMD	0xFC	//Command ID
#define RATE_MIXSP		0xFB	//简拼词组
#define RATE_WILD		0xFA	//万能键查询结果
#define RATE_ASSOCIATE	0xF9	//联想重码

//系统回应
#define ISACK_SUCCESS	0
#define ISACK_DELAY		1	
#define ISACK_UNKNOWN	100		//不能识别的消息
#define ISACK_ERRCLIENT	101		//错误的客户窗口句柄
#define ISACK_SVRCANCEL	102		//服务器异常退出
#define ISACK_UNINIT	103		//通讯没有正常初始化
#define ISACK_ERRBUF	104		//数据长度错误
#define ISACK_ERROR		105		//一般性错误
	
//获取重码的特殊返回值
#define MCR_NORMAL		0		//正常获取重码
#define MCR_ASSOCIATE	1		//联想字,包含预测及编码提示
#define MCR_LONGERCOMP	(1<<1)	//存在以当前编码为前缀的长自定义编码
#define MCR_MIXSP		(1<<2)	//混合简拼
#define MCR_USERDEF		(1<<3)	//混合自定义输入
#define MCR_SPCAND		(1<<4)	//输入拼音单字
#define MCR_WILD		(1<<6)	//万能键的查询
#define MCR_AUTOSELECT	(1<<7)	//唯一码上屏

//联想mask
#define MKI_ASTCAND		1		//词组联想
#define MKI_ASTSENT		(1<<1)	//句子联想
#define MKI_ASTENGLISH	(1<<2)	//英文联想
#define MKI_RECORD		(1<<3)	//记录开关
#define MKI_AUTOPICK	(1<<4)	//智能选词时联想后续句子的开关
#define MKI_TTSINPUT	(1<<5)	//朗读输入内容
#define MKI_PHRASEREMIND (1<<6)	//词组提醒

//查询重码mask
#define MQC_NORMAL		0		//查询普通重码
#define MQC_FORECAST	1		//查询语句库的预测结果
#define MQC_FCNOCAND	(1<<1)	//无候选词时预测
#define MQC_MATCH		(1<<2)	//自动将最匹配的重码作为首选码
#define MQC_ONLYSC		(1<<3)	//出简不出全
#define MQC_USERDEF		(1<<4)	//同时查询自定义编码
#define MQC_AUTOPROMPT	(1<<5)	//智能编码提示
#define MQC_SPCAND		(1<<6)	//混合自定义输入

//TTS选项
#define MTTS_EN			1		//使用英文引擎	1
#define MTTS_CH			2		//使用中文引擎

//rateAdjust mode
#define RAM_AUTO		0		//智能调频
#define RAM_FAST		1		//快速调频

#ifdef __cplusplus
extern "C"{
#endif//__cplusplus

BOOL ISComm_GetDataPathW(LPWSTR pszDataPath,int nLength);
BOOL ISComm_GetDataPathA(LPSTR pszDataPath, int nLength);

void ISComm_SetSvrPath(LPCTSTR pszPath);

void ISComm_FreeImeFlagData(IMEFLAGDATA *pData);

const UINT ISComm_GetCommMsgID();

DWORD ISComm_UpdateUserDefData(int nType, LPCSTR pszFilename);
DWORD ISComm_FatctUserDefFileName(int nType);

DWORD ISComm_SendMsg(DWORD dwType,const LPVOID pData,short sSize,HWND hWnd);
DWORD ISComm_PostMsg(DWORD dwType,const LPVOID pData,short sSize,HWND hWnd);
PMSGDATA ISComm_GetData();

BOOL ISComm_Login(HWND hWnd);
void  ISComm_Logout(HWND hWnd);

PCOMPINFO ISComm_GetCompInfo();
PFLMINFO  ISComm_GetFlmInfo();
char *    ISComm_GetUserDict();

PMSGDATA ISComm_OnSeverNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);

DWORD ISComm_KeyIn(LPCSTR pszText,short sSize,BYTE byMask,HWND hWnd);

DWORD ISComm_QueryCand(LPCSTR pszComp,char cCompLen,BYTE byMask,HWND hWnd);
DWORD ISComm_QueryComp(LPCSTR pszPhrase,char cPhraseLen);
DWORD ISComm_Forecast(LPCSTR pszComp,char cCompLen);

DWORD ISComm_SpellQueryComp(LPCSTR pszPhrase,char cPhraseLen);
DWORD ISComm_SpellForecast(char szPrefix[2],LPBYTE *pbyBlurs,BYTE bySyllables);
DWORD ISComm_Spell2ID(LPCSTR pszSpell,char cCompLen);
DWORD ISComm_SpellGetBlur(LPCSTR pszSpell,char cCompLen);
DWORD ISComm_SpellQueryCandEx(LPBYTE *pbyBlur,BYTE bySyllables,BYTE byFlag);
DWORD ISComm_En2Ch(LPCSTR pszWord,char cWordLen);
DWORD ISComm_Ch2En(LPCSTR pszWord,char cWordLen);
DWORD ISComm_UDQueryCand(LPCSTR pszComp,char cCompLen);

DWORD ISComm_TTS(LPCSTR pszText,char cTextLen,BYTE byMask);
DWORD ISComm_RateAdjust(LPCSTR pszComp,char cCompLen,LPCSTR pszPhrase,char cPhraseLen,BYTE byMode,HWND hWnd);
DWORD ISComm_EnQueryCand(LPCSTR pszText,char cTextLen);
DWORD ISComm_MakePhrase(LPCSTR pszText,char cTextLen);
DWORD ISComm_ShowServer(LPCSTR pszPageName,char cTextLen);
DWORD ISComm_ServerVersion();
DWORD ISComm_PhraseDel(LPCSTR pszComp,char cCompLen,LPCSTR pszPhrase,char cPhraseLen,HWND hWnd);
DWORD ISComm_LineQueryCand(LPCSTR pszComp,char cCompLen);
int ISComm_PhraseRate(LPCSTR pszPhrase,char cPhraseLen);

DWORD ISComm_QueryUserDict(LPCSTR pszkey,char cKeyLen);
DWORD ISComm_SortWordByBiHua(LPCSTR pszBiHua,char cBiHuaLen,LPCSTR pszWordList,short nWords);

DWORD ISComm_Blur_Set(BOOL bBlur);
DWORD ISComm_Blur_Add(const char *pszSpell1, const char *pszSpell2);
DWORD ISComm_Blur_Del(const char *pszSpell1, const char *pszSpell2);
DWORD ISComm_Blur_Query();
DWORD ISComm_Blur_DelAll();

DWORD ISComm_QueryWordSpell(char szWord[2]);
DWORD ISComm_SpellMemoryEx(LPCSTR pszText,char cTextLen,BYTE (*pbySpellID)[2]);
DWORD ISComm_SymbolConvert(char cSymbol,char cType);

DWORD ISComm_UserDict_List();
DWORD ISComm_UserDict_Open(LPCSTR pszUserDict);
DWORD ISComm_UserDict_Max();
DWORD ISComm_UserDict_Item(DWORD dwItem);
DWORD ISComm_UserDict_Locate(LPCTSTR pszkey,char cKeyLen);

DWORD ISComm_Comp_List();
DWORD ISComm_Comp_Open(LPCSTR pszComp);
BOOL ISComm_IsSvrWorking();
DWORD ISComm_SkinChange(HWND hUIWnd);

BOOL  ISComm_CheckComp(LPCSTR pszComp,char cComplen,BYTE byMask);
DWORD ISComm_Bldsp_Get(BOOL *pbCe2,BOOL *pbCe3,BOOL *pbCa4);
DWORD ISComm_Bldsp_Set(BYTE byMask,BOOL bCe2,BOOL bCe3,BOOL bCa4);

BOOL  ISComm_SvrTray_Get();
void  ISComm_SvrTray_Set(BOOL bTray);

LPCSTR ISComm_Svr_Pages();


DWORD ISComm_GetTtsTokens();
DWORD ISComm_SetTtsToken(char bCh, int iToken);
DWORD ISComm_GetTtsSpeed();
DWORD ISComm_SetTtsSpeed(int nSpeed);

DWORD ISComm_BlurZCS(int bBlur);

DWORD ISComm_GetMaxPhrasePredictLength();
DWORD ISComm_SetMaxPhrasePredictLength(int nLen);

DWORD ISComm_GetMaxPhraseAssociateDeepness();
DWORD ISComm_SetMaxPhraseAssociateDeepness(int nDeepness);

DWORD ISComm_GetSentRecordMax();
DWORD ISComm_SetSentRecordMax(int nSentRecordMax);

DWORD ISComm_GetSvrAutorun();
DWORD ISComm_SetSvrAutorun(int bAutoRun);

DWORD ISComm_InstallPhraseLib(LPCSTR pszPlnameUtf8);

DWORD ISComm_QueryPhraseGroup();
DWORD ISComm_EnablePhraseGroup(LPCSTR pszGroupName, char bEnable);

DWORD ISComm_ImportUserLib(LPCSTR pszUserLibUtf8);
DWORD ISComm_ExportUserLib(LPCSTR pszUserLibUtf8);

DWORD ISComm_InstallComp(LPCSTR pszCompNameUtf8,char bApplyNow);

DWORD ISComm_Flm_List();
DWORD ISComm_Flm_Open(LPCSTR pszFlmUtf8);
DWORD ISComm_Flm_GetInfo();
DWORD ISComm_Flm_EnableGroup(LPCSTR pszGroup,char bEnable);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//ISCOMM_H