#pragma once

//编码数据包括2部分：
//1 编码串：形码中是纯编码，拼音中是预测结果+当前在编辑的字拼音=MAX_SENTLEN+7
//2 结果串: 与编码串共用内存
#define MAX_COMP                120
#define MAX_SYLLABLES			(MAX_SENTLEN/2)
#define MAX_INPUT				255 // >(MAX_SENTLEN+7) && >6*MAX_SYLLABLES && UD_MAXCAND

enum COMPMODE
{
	IM_SPELL=0,	//拼音输入模式
	IM_SHAPECODE,	//形码输入模式
};

enum ASTMODE
{//输入时的联想类型
	AST_NULL=0,	//无联想
	AST_CAND,	//词组联想
	AST_ENGLISH,//单词联想
	AST_PHRASEREMIND,//词组提示
};

enum INSTATE
{
	INST_CODING=0,	//编码输入状态
	INST_ENGLISH,	//英文单词输入状态
	INST_DIGITAL,	//数字输入状态
	INST_USERDEF,	//用户自定义输入状态
	INST_LINEIME,	//笔画输入法状态
};

enum SUBSTATE
{
	SBST_NORMAL=0,	//一般状态
	SBST_ASSOCIATE,	//联想信息
	SBST_SENTENCE,	//语句输入状态
};

struct SPELLINFO
{
	char szSpell[7];	//拼音
	BYTE bySpellLen;	//拼音长度
};

//清除缓冲区数据
#define CPC_STATE		(1<<0)
#define CPC_CAND		(1<<1)
#define CPC_COMP		(1<<2)
#define CPC_ENGLISH		(1<<3)
#define CPC_SPELL		(1<<4)
#define CPC_SENTENCE	(1<<5)
#define CPC_ALL			0xFFFFFFFF

struct InputContext
{
	TCHAR szInput[MAX_INPUT];		//当前输入数据
	int   cInput;

	COMPMODE compMode;				//当前的输入类型：形码输入还是拼音输入。
	INSTATE	inState;				//当前输入状态
	SUBSTATE sbState;				//输入子状态
	BOOL	bWebMode;				//网址输入模式

	//重码数据
	short   sCandCount;				//重码数
	LPBYTE  *ppbyCandInfo;			//重码数据表

	//拼音数据
	BYTE	bySyllables;			//拼音音节数
	BYTE	byCaret;				//当前编辑的音节
	BYTE	bySyCaret;				//音节内部编辑光标
	char	szWord[MAX_SYLLABLES][2];	//拼音对应的汉字
	SPELLINFO spellData[MAX_SYLLABLES];	//音节信息
	BYTE	bySelect[MAX_SYLLABLES];	//手动选择重码标志 0-自动预测,1-手动选择单字，其它－手动选择词组
	LPBYTE  pbyBlur[MAX_SYLLABLES];		//拼音的模糊音
	BOOL	bPYBiHua;				//拼音笔画辅助状态标志
	char    szBiHua[50];
	//智能语句
	LPBYTE  pbySentWord[MAX_SENTLEN];	//单字地址
	short   sSentLen;				//句子长度
	short   sSentWords;				//词数
	short	sSentCaret;				//当前编辑位置

	//英文联想
	LPBYTE  pbyEnAstPhrase;			//与联想到的英文对应的词组

	//英文输入
	LPBYTE  pbyEnSpell;				//英文拼写
	LPBYTE  pbyEnPhontic;			//音标数据

	BOOL    bShowTip;				//show tip in comp
};
