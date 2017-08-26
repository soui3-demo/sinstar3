#pragma  once


#ifdef ENABLE_LANGUAGEBAR
#include <msctf.h>
#endif//ENABLE_LANGUAGEBAR

//定义Composition String一个Range的属性，与TSF定义类型一致
#define RG_ATTR_INPUT  0 
#define RG_ATTR_TARGET_CONVERTED  1 
#define RG_ATTR_CONVERTED  2 
#define RG_ATTR_TARGET_NOTCONVERTED 3 
#define RG_ATTR_INPUT_ERROR  4 
#define RG_ATTR_FIXEDCONVERTED  5 
#define RG_ATTR_OTHER  -1

#ifndef TF_MOD_ALT
#define TF_MOD_ALT                         0x0001
#define TF_MOD_CONTROL                     0x0002
#define TF_MOD_SHIFT                       0x0004
#define TF_MOD_RALT                        0x0008
#define TF_MOD_RCONTROL                    0x0010
#define TF_MOD_RSHIFT                      0x0020
#define TF_MOD_LALT                        0x0040
#define TF_MOD_LCONTROL                    0x0080
#define TF_MOD_LSHIFT                      0x0100
#define TF_MOD_ON_KEYUP                    0x0200
#define TF_MOD_IGNORE_ALL_MODIFIER         0x0400
#endif//TF_MOD_ALT

//热键定义，同TSF的TF_PRESERVEDKEY定义
typedef struct _PRESERVEDKEY
{
	UINT uVKey;
	UINT uModifiers;
}PRESERVEDKEY;

// 输入模式
enum EInputMethod
{
	HalfAlphanumeric=100,// 半角英文数字
	HalfNative,			 // 半角汉字
	FullNative,			 // 全角汉字
};

interface ITextService
{
	virtual BOOL IsCompositing()=NULL;
	virtual void StartComposition(LPVOID lpImeContext)=NULL;

	//使用指定数据替换当前编码串
	// nLeft,nRight: [-1,-1]:在当前光标位置插入
	//				 [0,-1]:替换全部内容
	//				 [left,right]:替换该范围，都不能为负值
	virtual void ReplaceSelCompositionW(LPVOID lpImeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)=NULL;
	virtual void UpdateResultAndCompositionStringW(LPVOID lpImeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)=NULL;
	virtual void EndComposition(LPVOID lpImeContext)=NULL;
	virtual int  MoveCaretPos(LPVOID lpImeContext,int nPos,BOOL bSet)=NULL;
	virtual LPVOID GetImeContext()=NULL;
	virtual BOOL   ReleaseImeContext(LPVOID lpImeContext)=NULL;
	virtual void  SetConversionMode(EInputMethod mode)=NULL;
	virtual EInputMethod GetConversionMode()=NULL;
	virtual BOOL  RegisterIMEHotKey(REFGUID guidHotKey,LPCWSTR pszName,const PRESERVEDKEY *pKey)=NULL;
	virtual BOOL  UnregisterIMEHotKey(REFGUID guidHotKey,const PRESERVEDKEY *pKey)=NULL;
	virtual BOOL SetOpenStatus(LPVOID lpImeContext,BOOL bOpen)=NULL;
	virtual BOOL GetOpenStatus(LPVOID lpImeContext)=NULL;
	virtual LRESULT DoWildRequest(WPARAM wParam,LPARAM lParam)=NULL;
#ifdef ENABLE_LANGUAGEBAR
	virtual BOOL GetLanguageBarItemMgr(ITfLangBarItemMgr **ppLangBarMgr,GUID *pGuidTIP)=NULL;
#endif//ENABLE_LANGUAGEBAR
};