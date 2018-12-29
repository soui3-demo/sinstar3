#pragma once
#include <IpcObject.h>
#include <string>
#include <sstream>
#include "sinstar-i.h"
#include "TextService-i.h"

using namespace std;

#include "paramstream.hpp"
#include "paramhelper.hpp"

#define SINSTAR3_SERVER_HWND _T("sinstar3_server_wnd_{85B55CBC-7D48-4860-BA88-0BE4B073A94F}")

//////////////////////////////////////////////////////////////////
template<class _Traits> inline
basic_ostream<char, _Traits>& operator<<(
	basic_ostream<char, _Traits>& _Ostr,
	const UINT64 & value)
{
	char szBuf[50];
	sprintf(szBuf, "0x%i64u", value);
	_Ostr << szBuf;
	return _Ostr;
}
//////////////////////////////////////////////////////////////////
template<>
inline CParamStream & CParamStream::operator<<(const string & str)
{
	int nSize = (int)str.size();
	GetBuffer()->Write((const BYTE*)&nSize, sizeof(int));
	GetBuffer()->Write((const BYTE*)str.c_str(), nSize);
	return *this;
}
template<>
inline CParamStream & CParamStream::operator >> (string & str)
{
	int nSize = 0;
	GetBuffer()->Read((BYTE*)&nSize, sizeof(int));
	char *pBuf = new char[nSize];
	GetBuffer()->Read((BYTE*)pBuf, nSize);
	str = string(pBuf, nSize);
	delete[]pBuf;
	return *this;
}

////////////////////////////////////////////////////////////////////////
template<>
inline CParamStream & CParamStream::operator<<(const wstring & str)
{
	int nSize = (int)str.size();
	GetBuffer()->Write((const BYTE*)&nSize, sizeof(int));
	GetBuffer()->Write((const BYTE*)str.c_str(), nSize*sizeof(wchar_t));
	return *this;
}
template<>
inline CParamStream & CParamStream::operator >> (wstring & str)
{
	int nSize = 0;
	GetBuffer()->Read((BYTE*)&nSize, sizeof(int));
	wchar_t *pBuf = new wchar_t[nSize];
	GetBuffer()->Read((BYTE*)pBuf, nSize*sizeof(wchar_t));
	str = wstring(pBuf, nSize);
	delete[]pBuf;
	return *this;
}

template<class _Traits> inline
basic_ostream<char, _Traits>& operator<<(
	basic_ostream<char, _Traits>& _Ostr,
	const wstring & value)
{
	char szBuf[500];
	int nRet = WideCharToMultiByte(CP_ACP, 0, value.c_str(), (int)value.size(), szBuf, 500, NULL, NULL);
	szBuf[nRet] = 0;
	_Ostr << szBuf;
	return _Ostr;
}

//////////////////////////////////////////////////////////////////////
template<>
inline CParamStream & CParamStream::operator<<(const POINT & pt)
{
	GetBuffer()->Write((const BYTE*)&pt.x, sizeof(int));
	GetBuffer()->Write((const BYTE*)&pt.y, sizeof(int));
	return *this;
}
template<>
inline CParamStream & CParamStream::operator >> (POINT & pt)
{
	int tmp = 0;
	GetBuffer()->Read((BYTE*)&tmp, sizeof(int));
	pt.x = tmp;
	GetBuffer()->Read((BYTE*)&tmp, sizeof(int));
	pt.y = tmp;
	return *this;
}

template<class _Traits> inline
basic_ostream<char, _Traits>& operator<<(
	basic_ostream<char, _Traits>& _Ostr,
	const POINT & pt)
{
	_Ostr << "(" << pt.x << "," << pt.y << ")";
	return _Ostr;
}

enum {
	ISinstar_Create = FUN_ID_START,
	ISinstar_Destroy,
	ISinstar_OnImeSelect,
	ISinstar_OnCompositionStarted,
	ISinstar_OnCompositionChanged,
	ISinstar_OnCompositionTerminated,
	ISinstar_OnSetCaretPosition,
	ISinstar_OnSetFocusSegmentPosition,
	ISinstar_ProcessKeyStoke,
	ISinstar_TranslateKey,
	ISinstar_OnSetFocus,
	ISinstar_GetCompositionSegments,
	ISinstar_GetCompositionSegmentEnd,
	ISinstar_GetCompositionSegmentAttr,
	ISinstar_OnOpenStatusChanged,
	ISinstar_OnConversionModeChanged,
	ISinstar_ShowHelp,
	ISinstar_GetDefInputMode,

	ITextService_InputStringW = ISinstar_GetDefInputMode + 100,
	ITextService_IsCompositing,
	ITextService_StartComposition,
	ITextService_ReplaceSelCompositionW,
	ITextService_UpdateResultAndCompositionStringW,
	ITextService_EndComposition,
	ITextService_GetImeContext,
	ITextService_ReleaseImeContext,
	ITextService_SetConversionMode,
	ITextService_GetConversionMode,
	ITextService_SetOpenStatus,
	ITextService_GetOpenStatus,
};

struct Param_Create : FunParams_Base
{
	HWND hOwner;
	FUNID(ISinstar_Create)
		PARAMS1(Input, hOwner)
		TOSTR1(hOwner)
};

struct Param_Destroy : FunParams_Base
{
	FUNID(ISinstar_Destroy)
};

struct Param_OnImeSelect : FunParams_Base
{
	BOOL bSelect;
	FUNID(ISinstar_OnImeSelect)
	PARAMS1(Input, bSelect)
	TOSTR1(bSelect)
};

struct Param_OnCompositionStarted : FunParams_Base
{
	FUNID(ISinstar_OnCompositionStarted)
};


struct Param_OnCompositionTerminated : FunParams_Base
{
	bool bClearCtx;
	FUNID(ISinstar_OnCompositionTerminated)
	PARAMS1(Input, bClearCtx)
	TOSTR1(bClearCtx)
};

struct Param_OnCompositionChanged : FunParams_Base
{
	FUNID(ISinstar_OnCompositionChanged)
};

struct Param_OnSetCaretPosition : FunParams_Base
{
	POINT pt;
	int nHei;
	FUNID(ISinstar_OnSetCaretPosition)
	PARAMS2(Input, pt,nHei)
	TOSTR2(pt, nHei)
};

struct Param_OnSetFocusSegmentPosition : FunParams_Base
{
	POINT pt; int nHei;
	FUNID(ISinstar_OnSetFocusSegmentPosition)
	PARAMS2(Input, pt, nHei)
	TOSTR2(pt, nHei)
};

struct Param_ProcessKeyStoke : FunParams_Base {
	UINT64 lpImeContext; UINT vkCode; LPARAM lParam; BOOL bKeyDown; 
	BOOL bEaten;
	FUNID(ISinstar_ProcessKeyStoke)
	TOSTR5(lpImeContext, vkCode, lParam, bKeyDown, bEaten)
	PARAMS4(Input, lpImeContext, vkCode, lParam, bKeyDown)
	PARAMS1(Output,bEaten)
};

struct Param_TranslateKey : FunParams_Base
{
	UINT64 lpImeContext; UINT vkCode; UINT uScanCode; BOOL bKeyDown; 
	BOOL bEaten;
	FUNID(ISinstar_TranslateKey)
	PARAMS4(Input, lpImeContext, vkCode, uScanCode, bKeyDown)
	PARAMS1(Output, bEaten)
	TOSTR5(lpImeContext, vkCode, uScanCode, bKeyDown, bEaten)
};

struct Param_OnSetFocus : FunParams_Base
{
	BOOL bFocus;
	FUNID(ISinstar_OnSetFocus)
		PARAMS1(Input, bFocus)
		TOSTR1(bFocus)
};

struct Param_GetCompositionSegments : FunParams_Base
{
	int nSegs;
	FUNID(ISinstar_GetCompositionSegments)
		PARAMS1(Output, nSegs)
		TOSTR1(nSegs)
};

struct Param_GetCompositionSegmentEnd : FunParams_Base
{
	int iSeg;
	int iEnd;
	FUNID(ISinstar_GetCompositionSegmentEnd)
		PARAMS1(Input,iSeg)
		PARAMS1(Output,iEnd)
		TOSTR2(iSeg,iEnd)
};

struct Param_GetCompositionSegmentAttr : FunParams_Base
{
	int iSeg;
	int nAttr;
	FUNID(ISinstar_GetCompositionSegmentAttr)
		PARAMS1(Input, iSeg)
		PARAMS1(Output, nAttr)
		TOSTR2(iSeg, nAttr)
};

struct Param_OnOpenStatusChanged : FunParams_Base
{
	BOOL bOpen;
	FUNID(ISinstar_OnOpenStatusChanged)
		PARAMS1(Input, bOpen)
		TOSTR1(bOpen)
};

struct Param_OnConversionModeChanged : FunParams_Base
{
	EInputMethod uMode;
	FUNID(ISinstar_OnConversionModeChanged)
		PARAMS1(Input, uMode)
		TOSTR1(uMode)
};

struct Param_ShowHelp : FunParams_Base
{
	FUNID(ISinstar_ShowHelp)
};

struct Param_GetDefInputMode : FunParams_Base
{
	EInputMethod uMode;
	FUNID(ISinstar_GetDefInputMode)
		PARAMS1(Output,uMode)
		TOSTR1(uMode)
};


////////////////////////////////////////////////////////////////////////////
struct Param_InputStringW : FunParams_Base
{
	wstring buf;
	BOOL bRet;
	FUNID(ITextService_InputStringW)
		PARAMS1(Input,buf)
		PARAMS1(Output,bRet)
		TOSTR2(buf,bRet)
};

struct Param_IsCompositing : FunParams_Base
{
	BOOL bRet;
	FUNID(ITextService_IsCompositing)
		PARAMS1(Output,bRet)
		TOSTR1(bRet)
};

struct Param_StartComposition : FunParams_Base
{
	UINT64 lpImeContext;
	FUNID(ITextService_StartComposition)
		PARAMS1(Input,lpImeContext)
		TOSTR1(lpImeContext)
};

struct Param_ReplaceSelCompositionW : FunParams_Base
{
	UINT64 lpImeContext; int nLeft; int nRight; wstring buf;
	FUNID(ITextService_ReplaceSelCompositionW)
		PARAMS4(Input,lpImeContext,nLeft,nRight,buf)
		TOSTR4(lpImeContext, nLeft, nRight, buf)
};

struct Param_UpdateResultAndCompositionStringW : FunParams_Base
{
	UINT64 lpImeContext; wstring resultStr; wstring compStr;
	FUNID(ITextService_UpdateResultAndCompositionStringW)
		PARAMS3(Input, lpImeContext, resultStr, compStr)
		TOSTR3(lpImeContext, resultStr, compStr)
};

struct Param_EndComposition : FunParams_Base
{
	UINT64 lpImeContext;
	FUNID(ITextService_EndComposition)
		PARAMS1(Input,lpImeContext)
		TOSTR1(lpImeContext)
};

struct Param_GetImeContext : FunParams_Base
{
	UINT64 lpImeContext;
	FUNID(ITextService_GetImeContext)
		PARAMS1(Output,lpImeContext)
		TOSTR1(lpImeContext)
};

struct Param_ReleaseImeContext : FunParams_Base
{
	UINT64 lpImeContext;
	BOOL bRet;
	FUNID(ITextService_ReleaseImeContext)
		PARAMS1(Input, lpImeContext)
		PARAMS1(Output,bRet)
		TOSTR2(lpImeContext,bRet)
};

struct Param_SetConversionMode : FunParams_Base
{
	EInputMethod mode;
	FUNID(ITextService_SetConversionMode)
		PARAMS1(Input,mode)
		TOSTR1(mode)
};

struct Param_GetConversionMode : FunParams_Base
{
	EInputMethod mode;
	FUNID(ITextService_GetConversionMode)
		PARAMS1(Output, mode)
		TOSTR1(mode)
};

struct Param_SetOpenStatus : FunParams_Base
{
	UINT64 lpImeContext;
	BOOL bOpen;
	BOOL bRet;
	FUNID(ITextService_SetOpenStatus)
		PARAMS2(Input,lpImeContext,bOpen)
		PARAMS1(Output,bRet)
		TOSTR3(lpImeContext,bOpen,bRet)
};

struct Param_GetOpenStatus : FunParams_Base
{
	UINT64 lpImeContext;
	BOOL bOpen;
	FUNID(ITextService_GetOpenStatus)
		PARAMS1(Input, lpImeContext)
		PARAMS1(Output, bOpen)
		TOSTR2(lpImeContext, bOpen)
};