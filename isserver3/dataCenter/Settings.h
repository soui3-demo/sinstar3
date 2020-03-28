#pragma once


enum HotKeyIndex{
	HKI_CharMode = 0,	// 标点模式切换
	HKI_Query,			// 查询热键
	HKI_Mode,			// 五笔拼音模式切换
	HKI_EnSwitch,		// 英文输入开关
	HKI_MakePhrase,		// 造词热键
	HKI_ShowRoot,		// 字根表显示热键
	HKI_HideStatus,		// 隐藏状态栏
	HKI_FilterGbk,		// filter gbk
	HKI_Record,			// record input
	HKI_TTS,			// tts
	HKI_UDMode,			// user define mode
	HKI_TempSpell,		// temp spell
	HKI_COUNT,

	HKI_AdjustRate,		// 快速调频: ctrl+重码号 
	HKI_DelCandidate,	// 在线删词: Ctrl+Shift+重码号 
};

class CModifiedMark
{
public:
	CModifiedMark():m_bModified(false){}
	bool IsModified() const{
		return m_bModified;
	}
	void SetModified(bool bModified)
	{
		m_bModified = bModified;
	}
private:
	bool m_bModified;
};
class CSettingsGlobal: public CModifiedMark
{
public:
	/*GBK重码显示上屏模式*/
	enum GbkMode
	{
		GBK_HIDE = 0,	//有GB重码时自动隐藏
		GBK_SHOW_MANUAL,//显示手动上屏
		GBK_SHOW_NORMAL,//显示正常上屏
	};

	void Load(const SStringT & strDataPath);
	void Save(const SStringT & strDataPath);
	int		compMode;		//当前输入类型
	BOOL	b23CandKey;			// ;+'选23重码
	BYTE	by2CandVK;			// 2 Cand key
	BYTE	by3CandVK;			// 3 Cand Key
	BYTE	bySwitchKey;		// 输入法开关
	BYTE	byTempSpellKey;		// 临时拼音开关
	BYTE	byForecast;			// forecast mask
	TCHAR	byLineKey[6];		// 笔画输入法转换键值
	BOOL	bShowOpTip;			// 显示操作提示开关
	GbkMode		nGbkMode;			// GBK Show Mode

	BYTE	byRateAdjust;		// 词频调整类型 0-不使用，１－智能调频，２－快速调频
	BYTE	byAstMode;			// 联想类型 (ASTMODE)
	BOOL	bAutoMatch;			// 智能选词

	BOOL	bBlendUD;			// 混合检索自定义编码表
	BOOL	bBlendSpWord;		// 混合输入拼音单字
	BOOL	bAutoInput;			// 重码自动上屏

	BYTE	byTurnPageUpVK;		// 重码上翻键
	BYTE	byTurnPageDownVK;	// 重码下翻键
	DWORD   dwHotkeys[HKI_COUNT];

	BOOL	bPYPhraseFirst;		// 拼音词组优先
	BOOL	bEnterClear;		// 回车清空编码
	int		nSoundAlert;		// 声音提示
	BOOL	bDisableFnKey;		// 禁用功能键
	BOOL	bAutoDot;			// 智能小数点
	BOOL	bAutoPrompt;		// 智能重码提示
	BOOL	bDisableDelWordCand;// 禁止删除单字重码
	WCHAR	szWebHeader[100];	// 自动转为英文输入模式的抬头

	BOOL	bCandSelNoNum;		// 禁用数字选择重码
	BOOL	bOnlySimpleCode;	// 出简不出全
	BOOL	bDisableFirstWild;	// 禁止首码万能键
	BOOL	bFullSpace;			// full space.
	BYTE    bySentMode;
	int		nMaxCands;			// max candidate number.
	int		nDelayTime;			//delay timer for closing composition window in seconds.

	SStringT strFontDesc;		//font description.

	BOOL	bEnableDebugSkin;	//enable debug skin. default is false
	SStringT   strDebugSkinPath;//debug skin path.

	SStringT   strSkin;   //skin
	POINT	ptInput;			//input window pos for not caret follow.
	POINT   ptStatus;			//status window pos

	SStringT urlSkin;
	SStringT urlComp;
	SStringT urlForum;
	SStringT urlStatistics;

	BOOL	bShowTray;
	BOOL	bAutoQuit;
	int		nUpdateInterval;
	TCHAR   szUpdateDate[100];
	TCHAR	szUpdateUrl[MAX_PATH];
};

class CSettingsUI: public CModifiedMark
{
public:
	void Load(const SStringT & strDataPath);
	void Save(const SStringT & strDataPath);

	BOOL	bHideStatus;		// 当前状态栏隐藏状态
	BOOL	bMouseFollow;		// 鼠标跟随开关
	BOOL	bEnglish;			// 英文单词输入开关
	BOOL	bFullStatus;		// 状态栏展开标志
	BOOL	bCharMode;			// 标点模式
	BOOL	bSound;				// 语音较对
	BOOL	bRecord;			// 记录输入语句
	BOOL	bInputBig5;			// 繁体输出标志
	BOOL    bFilterGbk;			// filter gbk

};

extern CSettingsGlobal	*g_SettingsG;
extern CSettingsUI   *g_SettingsUI;