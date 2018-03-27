#pragma once

#define KSettingINI L"config.ini"

class CSettingsGlobal
{
public:
	/*GBK重码显示上屏模式*/
	enum GbkMode
	{
		GBK_HIDE = 0,	//有GB重码时自动隐藏
		GBK_SHOW_MANUAL,//显示手动上屏
		GBK_SHOW_NORMAL,//显示正常上屏
	};

	void Load(LPCTSTR pszIniFile);
	void Save(LPCTSTR pszIniFile);

	COMPMODE	compMode;		//当前输入类型
	BOOL	b23CandKey;			// ;+'选23重码
	BYTE	by2CandVK;			// 2 Cand key
	BYTE	by3CandVK;			// 3 Cand Key
	BYTE	bySwitchKey;		// 输入法开关
	BYTE	byTempSpellKey;		// 临时拼音开关
	BYTE	byForecast;			// forecast mask
	TCHAR	byLineKey[6];		// 笔画输入法转换键值
	BYTE	byPntcRed;			// 音标颜色 Red
	BYTE	byPntcGreen;		// 音标颜色 Green
	BYTE	byPntcBlue;			// 音标颜色 Blue
	BYTE	byTransStatus;		// 状态窗口透明度 50~255
	BYTE    byTransInput;		// 输入窗口透明度50~255
	BOOL	bFastUMode;			// 快捷自定义模式切换
	BOOL	bShowOpTip;			// 显示操作提示开关
	TCHAR   szPhonticLeft[50];	// 音标左数据
	TCHAR   szPhonticRight[50];	// 音标右数据
	GbkMode		nGbkMode;			// GBK Show Mode

	BYTE	byRateAdjust;		// 词频调整类型 0-不使用，１－智能调频，２－快速调频
	BYTE	byAstMode;			// 联想类型 (ASTMODE)
	BOOL	bAstSent;			// 语句联想
	BOOL	bAutoMatch;			// 智能选词
	BOOL	bTTSInput;			// 语音较对

	BOOL	bBlendUD;			// 混合检索自定义编码表
	BOOL	bBlendSpWord;		// 混合输入拼音单字
	BOOL	bAutoInput;			// 重码自动上屏

	BYTE	byTurnPageUpVK;		// 重码上翻键
	BYTE	byTurnPageDownVK;	// 重码下翻键

	BYTE	byHotKeyQuery;		// 查询热键
	BYTE	byHotKeyMode;		// 五笔拼音模式切换
	BYTE	byHotKeyEn;			// 英文输入开关
	BYTE	byHotKeyMakeWord;	// 造词热键
	BYTE	byHotKeyShowRoot;	// 字根表显示热键
	BYTE	byHotKeyHideStatus;	// 隐藏状态栏

	BOOL	bPYPhraseFirst;		// 拼音词组优先
	BOOL	bEnterClear;		// 回车清空编码
	int		nSoundAlert;		// 声音提示
	BOOL	bDisableFnKey;		// 禁用功能键
	BOOL	bAutoDot;			// 智能小数点
	BOOL	bAutoPrompt;		// 智能重码提示
	BOOL	bDisableDelWordCand;// 禁止删除单字重码
	char	szWebHeader[100];	// 自动转为英文输入模式的抬头

	BOOL	bCandSelNoNum;		// 禁用数字选择重码
	BOOL	bOnlySimpleCode;	// 出简不出全
	TCHAR	szPlugin[100];		// plugin name
};

class CSettingsLocal
{
public:

	void Load(LPCTSTR pszIniFile);
	void Save(LPCTSTR pszIniFile);

	BOOL	bHideStatus;		// 当前状态栏隐藏状态
	BOOL	bMouseFollow;		// 鼠标跟随开关
	BOOL	bEnglish;			// 英文单词输入开关
	BOOL	bFullStatus;		// 状态栏展开标志
	BOOL	bCharMode;			// 标点模式
	BOOL	bSound;				// 发音标志
	BOOL	bRecord;			// 记录输入语句
	BOOL	bInputBig5;			// 繁体输出标志
};


class CSettingsCompSpec
{
public:
	TCHAR	hkUserDefSwitch;	//切换到用户定义的与编码相关快捷键
	TCHAR	hkSentSwitch;		//切换到语句输入的与编码相关快捷键
};

extern CSettingsGlobal	g_SettingsG;
extern CSettingsLocal	g_SettingsL;
extern CSettingsCompSpec	g_SettingsCompSpec;

extern int				g_nRefCount;