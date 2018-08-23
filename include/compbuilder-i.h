#pragma once

#ifdef ISCORE_EXPORTS
#define COMPBUILDER_API __declspec(dllexport)
#else
#define COMPBUILDER_API __declspec(dllimport)
#endif


#define RULE_MAX	8

struct ICodingRule
{
	virtual ~ICodingRule() {};

	virtual short MakeComp(LPBYTE *ppComps, char cSize, BYTE *pbyRet, short sBufSize) = 0;

	//************************************************
	//解析构词规则字符串
	//pszRule format:ce2=pij+pkl+nij+nkl
	//p:正序，n:逆序
	//***********************************************
	virtual BOOL ParseRule(LPCSTR pszRule) = 0;
	virtual BOOL ParseRules(char cCodeMax, BYTE byRules, char *pszBuf) = 0;

	//***************************************************
	//	生成构词规则字符串
	//***************************************************
	virtual BOOL MakeRule(int iRule, char *pszRule) = 0;

	//************************************************
	virtual void WriteEx(FILE *f) = 0;
	//*******************************************************
	//	从磁盘读取
	//******************************************************
	virtual void Load(FILE *f) = 0;
	virtual void LoadEx(FILE *f) = 0;

};

typedef struct tagCOMPHEAD
{
	char szName[50];	//名称
	char szCode[50];	//码元
	char cWildChar;		//万能键
	char cCodeMax;		//最大码长
	char szUrl[50];		//相关网址
	DWORD dwWords;		//已经编码字符数
	BOOL bSymbolFirst;	//标点首编码
	DWORD dwEncrypt : 1;	//加密标志
	DWORD dwYinXingMa : 1;	//音形码优化标志
	DWORD dwPhraseCompPart : 1;	//词组编码不一定是全码
	DWORD dwAutoSelect : 1;	//单一重码自动上屏
	DWORD dwLicense : 1;		//编码需要授权才能使用
	DWORD dwReserved : 27;	//保留,初始化为0
}COMPHEAD;

struct ICompBuilder
{
	virtual ~ICompBuilder() {}

	virtual ICodingRule * CreateCodingRule() = 0;
	virtual void DestroyCodingRule(ICodingRule * pRule) = 0;

	virtual BOOL Make(LPCSTR pszOutput,	//输出文件名
		COMPHEAD headInfo,	//编码信息
		ICodingRule *pRule,	//造词规则
		LPCSTR pszIconFile,//图标文件
		COLORREF crKey,		//图标透明色
		LPCSTR pszKeyMap,	//字根表文件
		const char	szKeyDllMD5[32]//加密算法dll的MD5值,32字节
	) = 0;

	virtual BOOL AddWord(char szWord[2], LPCSTR pszComp, char cCompLen = -1) = 0;
	virtual DWORD GetWords() const = 0;
};

struct ICompReaderEx
{
	virtual ~ICompReaderEx() {}
	virtual BOOL Export(LPCSTR pszFileName) = 0;
	virtual BOOL Load(LPCSTR pszCompFile) = 0;
	virtual const COMPHEAD *GetProps() const = 0;
};

#ifdef __cplusplus
extern "C" {
#endif
	COMPBUILDER_API ICompBuilder * ICompBuilder_Create();
	COMPBUILDER_API void ICompBuilder_Destroy(ICompBuilder* pCompBuilder);

	COMPBUILDER_API ICompReaderEx * ICompReader_Create();
	COMPBUILDER_API void ICompReader_Destroy(ICompReaderEx* pCompReader);
#ifdef __cplusplus
}
#endif
