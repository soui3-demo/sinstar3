#pragma once

typedef enum tagPROGTYPE { PT_MAX = 0, PT_PROG, PT_END } PROGTYPE;

#pragma pack(push,1)
typedef struct tagBLURINFO2
{//适合数据线性排列的结构
	short sCount;	//拼音个数
	short sIndex;	//与输入完全匹配的拼音序号,-1代表全部是模糊音
	BYTE byBlurSpell[1][2];
}BLURINFO2,*PBLURINFO2;
#pragma pack(pop)

typedef struct tagGROUPINFO
{//组信息
	WCHAR szName[50];	//名称
	WCHAR szEditor[50];	//编辑
	WCHAR szRemark[200];	//备注
	DWORD dwCount;		//词数
	BOOL  bValid;		//有效标志
}GROUPINFO,*PGROUPINFO;


//输入之星通讯使用的内存文件映射的名称
#define NAME_REQ_MAPFILE	_T("_sinstar3 req map file")	//客户端向服务器发送请求的数据通道
#define MAX_BUF_REQ_MAP		1<<12						//4K						
#define MAX_BUF_REQ			(MAX_BUF_REQ_MAP)									
#define NAME_ACK_MAPFILE	_T("_sinstar3 ack map file")	//服务器向客户端发送指示的数据通道
#define MAX_BUF_ACK_MAP		1<<15						//32K
#define MAX_BUF_ACK			((MAX_BUF_ACK_MAP)-sizeof(HWND))

#define MSG_NAME_SINSTAR3		_T("_sinstar3 communicate msg")	//通讯时使用的消息名称，取代老版本固定的消息ID，方便服务器返回数据

#define  CLS_SINSTAR3_IME_WND _T("sinstar3_uiwnd")	//the class name must like this. otherwise the ime module maybe crash!!!
#define  UM_GETPROCPATH  (WM_USER+5000)
#define  CDT_RET_PROCPATH (2134)
#define  UM_RECONN		 (WM_USER+5001)

#pragma pack(push,1)

typedef struct tagMSGDATA{
	short	sSize;		//返回数据长度
	BYTE	byData[1];	//返回数据起始地址
}MSGDATA,*PMSGDATA;


typedef struct tagFLMINFO
{
	WCHAR szName[50];
	WCHAR szAddition[50];
	WCHAR szAddFont[LF_FACESIZE+1];
}FLMINFO,*PFLMINFO;
#pragma pack(pop)


typedef struct tagCOMPHEAD
{
	WCHAR szName[50];	//名称
	WCHAR szCode[50];	//码元
	WCHAR cWildChar;		//万能键
	char cCodeMax;		//最大码长
	WCHAR szUrl[50];		//相关网址
	DWORD dwWords;		//已经编码字符数
	BOOL bSymbolFirst;	//标点首编码
	DWORD dwEncrypt : 1;	//加密标志
	DWORD dwYinXingMa : 1;	//音形码优化标志
	DWORD dwPhraseCompPart : 1;	//词组编码不一定是全码
	DWORD dwAutoSelect : 1;	//单一重码自动上屏
	DWORD dwLicense : 1;		//编码需要授权才能使用
	DWORD dwReserved : 27;	//保留,初始化为0
}COMPHEAD;

#define CISIZE_BASE		164		//基本编码信息数据量

#define MAX_SENTLEN		50		//语句最大长度
#define MAX_WORDLEN		30		//词组最大长度
#define MAX_COMPLEN		20		//系统支持的最大码长

#define RATE_USERDEF	0xFF	//自定义编码使用的词频
#define RATE_FORECAST	0xFE	//智能预测得到的词使用的词频
#define RATE_USERCMD	0xFC	//Command ID
#define RATE_MIXSP		0xFB	//简拼词组
#define RATE_WILD		0xFA	//万能键查询结果
#define RATE_ASSOCIATE	0xF9	//联想重码
#define RATE_USERJM		0xF8	//user defined jm.
#define RATE_USERDICT	0xF7	//user lib

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
#define MCR_ASSOCIATE	1		//编码提示
#define MCR_LONGERCOMP	(1<<1)	//存在以当前编码为前缀的长自定义编码
#define MCR_MIXSP		(1<<2)	//混合拼音
#define MCR_USERDEF		(1<<3)	//混合自定义输入
#define MCR_WILD		(1<<4)	//万能键的查询
#define MCR_AUTOSELECT	(1<<5)	//唯一码上屏
#define MCR_FORECAST	(1<<6)  //预测结果

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

#define FU_USERDEF	1		//更新用户定义文件
#define FU_SYMBOL   2  //user define symbol
#define FU_USERCMD	3	//快捷命令文件更新
#define FU_USERJM   4  //user define jm map
#define FU_USERDICT 5  //user dict

/////////////////////////////////////////////////////////////////////////////
//服务器广播的消息
/////////////////////////////////////////////////////////////////////////////
#define UM_SVR_NOTIFY	(WM_USER+9000)
//----------------------------------------------
//	服务器退出
//----------------------------------------------
#define NT_SERVEREXIT	100

//----------------------------------------------
//	当前编码更改
//----------------------------------------------
#define NT_COMPINFO		101

//----------------------------------------------
//	当前英文库更改
//----------------------------------------------
#define NT_FLMINFO  	102

//----------------------------------------------
//	编码错误
//----------------------------------------------
#define NT_COMPERROR	103

//----------------------------------------------
//	服务器处理完当前输入
//----------------------------------------------
#define NT_KEYIN	110