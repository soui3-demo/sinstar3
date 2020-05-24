#pragma once

#define CODE_RESTORE	100

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
#define RATE_USERJM		0xF8	//user defined jm.
#define RATE_USERDICT	0xF7	//user dict

enum{
	SENDER_SINSTSR3	= 100,
	SENDER_SKINMANAGER,
};


enum{
	EVENT_BASE = EVT_EXTERNAL_BEGIN,
	EVENT_DRAGMOVE,
	EVENT_SETSKIN,
	EVENT_SVRNOTIFY,
	EVENT_CHECK_UPDATE_RESULT,
	EVENT_CAND_CLICK,
};

namespace SOUI
{
	SEVENT_BEGIN(EventSvrNotify,EVENT_SVRNOTIFY)
		WPARAM wp;
		LPARAM lp;
	SEVENT_END()

	SEVENT_BEGIN(EventSetSkin,EVENT_SETSKIN)
	SEVENT_END()

	SEVENT_BEGIN(EventCheckUpdateResult,EVENT_CHECK_UPDATE_RESULT)
		bool    bServerOK;
		bool	bManual;
		SStringW strUrl;
		SStringW strInfo;
		SStringW strNewUpdateUrl;
		DWORD	 dwNewVer;
		DWORD    dwCurVer;
	SEVENT_END()

	SEVENT_BEGIN(EventCandClick,EVENT_CAND_CLICK)
		SStringW strText;
		BYTE	 byRate;
	SEVENT_END()

}

