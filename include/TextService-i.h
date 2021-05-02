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

// 输入模式
enum EInputMethod
{
	HalfAlphanumeric=100,// 半角英文数字
	HalfNative,			 // 半角汉字
	FullNative,			 // 全角汉字
};

interface ITextService
{
	virtual BOOL InputStringW(LPCWSTR pszBuf, int nLen) = NULL;
	virtual BOOL IsCompositing() const =NULL;
	virtual void StartComposition(UINT64 imeContext)=NULL;

	//使用指定数据替换当前编码串
	// nLeft,nRight: [-1,-1]:在当前光标位置插入
	//				 [0,-1]:替换全部内容
	//				 [left,right]:替换该范围，都不能为负值
	virtual void ReplaceSelCompositionW(UINT64 imeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)=NULL;
	virtual void UpdateResultAndCompositionStringW(UINT64 imeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)=NULL;
	virtual void EndComposition(UINT64 imeContext)=NULL;
	virtual void  SetConversionMode(EInputMethod mode)=NULL;
	virtual EInputMethod GetConversionMode()=NULL;
	virtual void SetOpenStatus(BOOL bOpen)=NULL;
	virtual BOOL GetOpenStatus() const =NULL;
	virtual DWORD GetActiveWnd() const = NULL;
	virtual void UpdateUI(UINT64 imeContext, bool bPageChanged, UINT curPage) {}
};