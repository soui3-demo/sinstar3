#pragma once
#include <string>
#include <sstream>
#include "sinstar-i.h"
#include "TextService-i.h"
#include <interface/SIpcObj-i.h>
#include <helper/sipcparamhelper.hpp>
#include "../sinstar3_tsf/UILess.h"

#ifdef TESTID
#define SINSTAR3_SERVER_HWND _T("sinstar4_server_wnd_{85B55CBC-7D48-4860-BA88-0BE4B073A94F}")
#define SINSTAR3_SHARE_BUF_NAME_FMT _T("sistart4_share_buffer_8085395F-E2FA-4F96-8BD0-FE5D7412CD22_%08x_2_%08x")
#define SINSTAR3_MUTEX _T("sinstar4_mutex_40AC2365-F592-4BE3-8B41-04A6B5337BFB")
#else
#define SINSTAR3_SERVER_HWND _T("sinstar3_server_wnd_{85B55CBC-7D48-4860-BA88-0BE4B073A94F}")
#define SINSTAR3_SHARE_BUF_NAME_FMT _T("sistart3_share_buffer_8085395F-E2FA-4F96-8BD0-FE5D7412CD22_%08x_2_%08x")
#define SINSTAR3_MUTEX _T("sinstar3_mutex_40AC2365-F592-4BE3-8B41-04A6B5337BFB")
#endif
//////////////////////////////////////////////////////////////////
namespace SOUI{

template<>
inline SParamStream & SParamStream::operator<<(const std::string & str)
{
	int nSize = (int)str.size();
	GetBuffer()->Write(&nSize, sizeof(int));
	GetBuffer()->Write(str.c_str(), nSize);
	return *this;
}
template<>
inline SParamStream & SParamStream::operator >> (std::string & str)
{
	int nSize = 0;
	GetBuffer()->Read(&nSize, sizeof(int));
	char *pBuf = new char[nSize];
	GetBuffer()->Read(pBuf, nSize);
	str = std::string(pBuf, nSize);
	delete[]pBuf;
	return *this;
}

////////////////////////////////////////////////////////////////////////
template<>
inline SParamStream & SParamStream::operator<<(const std::wstring & str)
{
	int nSize = (int)str.size();
	GetBuffer()->Write(&nSize, sizeof(int));
	GetBuffer()->Write(str.c_str(), nSize*sizeof(wchar_t));
	return *this;
}
template<>
inline SParamStream & SParamStream::operator >> (std::wstring & str)
{
	int nSize = 0;
	GetBuffer()->Read(&nSize, sizeof(int));
	wchar_t *pBuf = new wchar_t[nSize];
	GetBuffer()->Read(pBuf, nSize*sizeof(wchar_t));
	str = std::wstring(pBuf, nSize);
	delete[]pBuf;
	return *this;
}
}

struct FunParams_Base : SOUI::IFunParams
{
	virtual void ToStream4Input(SOUI::SParamStream &  ps) {}
	virtual void ToStream4Output(SOUI::SParamStream &  ps) {}
	virtual void FromStream4Input(SOUI::SParamStream &  ps) {}
	virtual void FromStream4Output(SOUI::SParamStream &  ps) {}
};


enum {
	ISinstar_Create = SOUI::FUN_ID_START,
	ISinstar_ScaleInfo,	//notify server the active view hwnd and size gotten from client. server read size again and calculate scale factor of client.
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
	ISinstar_ClickLanguageBarIcon,

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
	ITextService_GetActiveWnd,
	ITextService_CandidateListInfo,
	ITextService_UpdateUI,
	ITextService_UpdatePreedit,
};

struct Param_Create : FunParams_Base
{
	std::string strHostPath;
	DWORD  dwVer;
	FUNID(ISinstar_Create)
		PARAMS2(Input, strHostPath,dwVer)
};

struct Param_ScaleInfo : FunParams_Base
{
	DWORD hRefWnd;
	SIZE  szWnd;
	FUNID(ISinstar_ScaleInfo)
		PARAMS2(Input, hRefWnd,szWnd)
};

struct Param_CandidateListInfo : FunParams_Base
{
	void ctxtoParamStream(SOUI::SParamStream& ps, Context& ctx)
	{
		ps << ctx.preedit.str;
		size_t size = ctx.cinfo.candies.size();
		ps.Write(&size,sizeof(size_t));
		/*for (const auto &ite : ctx.cinfo.candies)
		{
			ps << ite.str;
		}
		*/
		for (std::vector<Text>::const_iterator ite=ctx.cinfo.candies.begin();ite!=ctx.cinfo.candies.end();ite++)
		{
			ps << ite->str;
		}
	}
	void ctxformParamStream(SOUI::SParamStream& ps, Context& ctx)
	{
		ps >> ctx.preedit.str;
		size_t size = 0;
		ps.Read(&size, sizeof(size_t));		
		while (size)
		{
			std::wstring word;
			ps >> word;
			ctx.cinfo.candies.push_back(word);
			--size;
		}
	}
#define PARAMS2CTX(type,p1) \
void ToStream4##type(SOUI::SParamStream &  ps){ ctxtoParamStream(ps,p1);}\
void FromStream4##type(SOUI::SParamStream &  ps){ctxformParamStream(ps,p1);}\

	Context ctx;
	FUNID(ITextService_CandidateListInfo)
	PARAMS2CTX(Output, ctx)
};

struct Param_UpdateUI : FunParams_Base
{
	UINT64 imeContext;
	bool bPageChanged;
	UINT curPage;
	FUNID(ITextService_UpdateUI)		
	PARAMS3(Input, imeContext,bPageChanged,curPage)
};

struct Param_ClickLanguageBarIcon : FunParams_Base
{
	TfLBIClick click;
	POINT pt;
	RECT rc;
	FUNID(ISinstar_ClickLanguageBarIcon)		
	PARAMS3(Input, click,pt,rc)
};

struct Param_UpdatePreedit : FunParams_Base
{
	UINT64 imeContext;
	std::wstring strPreedit;
	FUNID(ITextService_UpdatePreedit)
	PARAMS2(Input, imeContext, strPreedit)
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
};

struct Param_OnCompositionStarted : FunParams_Base
{
	bool bShowUI;
	FUNID(ISinstar_OnCompositionStarted)
		PARAMS1(Input, bShowUI)
};


struct Param_OnCompositionTerminated : FunParams_Base
{
	bool bClearCtx;
	FUNID(ISinstar_OnCompositionTerminated)
	PARAMS1(Input, bClearCtx)
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
};

struct Param_OnSetFocusSegmentPosition : FunParams_Base
{
	POINT pt; int nHei;
	FUNID(ISinstar_OnSetFocusSegmentPosition)
	PARAMS2(Input, pt, nHei)
};

struct Param_ProcessKeyStoke : FunParams_Base {
	UINT64 lpImeContext; UINT vkCode; DWORD lParam; BOOL bKeyDown; 
	BYTE byKeyState[256];
	BOOL bEaten;
	FUNID(ISinstar_ProcessKeyStoke)
	PARAMS5(Input, lpImeContext, vkCode, lParam, bKeyDown, byKeyState)
	PARAMS1(Output,bEaten)
};

struct Param_TranslateKey : FunParams_Base
{
	UINT64 lpImeContext; UINT vkCode; UINT uScanCode; BOOL bKeyDown; 
	BYTE byKeyState[256];
	BOOL bEaten;
	FUNID(ISinstar_TranslateKey)
	PARAMS5(Input, lpImeContext, vkCode, uScanCode, bKeyDown, byKeyState)
	PARAMS1(Output, bEaten)
};

struct Param_OnSetFocus : FunParams_Base
{
	BOOL bFocus;
	DWORD dwActiveWnd;
	FUNID(ISinstar_OnSetFocus)
		PARAMS2(Input, bFocus,dwActiveWnd)
};

struct Param_GetCompositionSegments : FunParams_Base
{
	int nSegs;
	FUNID(ISinstar_GetCompositionSegments)
		PARAMS1(Output, nSegs)
};

struct Param_GetCompositionSegmentEnd : FunParams_Base
{
	int iSeg;
	int iEnd;
	FUNID(ISinstar_GetCompositionSegmentEnd)
		PARAMS1(Input,iSeg)
		PARAMS1(Output,iEnd)
};

struct Param_GetCompositionSegmentAttr : FunParams_Base
{
	int iSeg;
	int nAttr;
	FUNID(ISinstar_GetCompositionSegmentAttr)
		PARAMS1(Input, iSeg)
		PARAMS1(Output, nAttr)
};

struct Param_OnOpenStatusChanged : FunParams_Base
{
	BOOL bOpen;
	FUNID(ISinstar_OnOpenStatusChanged)
		PARAMS1(Input, bOpen)
};

struct Param_OnConversionModeChanged : FunParams_Base
{
	EInputMethod uMode;
	FUNID(ISinstar_OnConversionModeChanged)
		PARAMS1(Input, uMode)
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
};


////////////////////////////////////////////////////////////////////////////
struct Param_InputStringW : FunParams_Base
{
	std::wstring buf;
	BOOL bRet;
	FUNID(ITextService_InputStringW)
		PARAMS1(Input,buf)
		PARAMS1(Output,bRet)
};

struct Param_IsCompositing : FunParams_Base
{
	BOOL bRet;
	FUNID(ITextService_IsCompositing)
		PARAMS1(Output,bRet)
};

struct Param_StartComposition : FunParams_Base
{
	UINT64 lpImeContext;
	FUNID(ITextService_StartComposition)
		PARAMS1(Input,lpImeContext)
};

struct Param_ReplaceSelCompositionW : FunParams_Base
{
	UINT64 lpImeContext; int nLeft; int nRight; std::wstring buf;
	FUNID(ITextService_ReplaceSelCompositionW)
		PARAMS4(Input,lpImeContext,nLeft,nRight,buf)
};

struct Param_UpdateResultAndCompositionStringW : FunParams_Base
{
	UINT64 lpImeContext; std::wstring resultStr; std::wstring compStr;
	FUNID(ITextService_UpdateResultAndCompositionStringW)
		PARAMS3(Input, lpImeContext, resultStr, compStr)
};

struct Param_EndComposition : FunParams_Base
{
	UINT64 lpImeContext;
	FUNID(ITextService_EndComposition)
		PARAMS1(Input,lpImeContext)
};


struct Param_SetConversionMode : FunParams_Base
{
	EInputMethod mode;
	FUNID(ITextService_SetConversionMode)
		PARAMS1(Input,mode)
};

struct Param_GetConversionMode : FunParams_Base
{
	EInputMethod mode;
	FUNID(ITextService_GetConversionMode)
		PARAMS1(Output, mode)
};

struct Param_SetOpenStatus : FunParams_Base
{
	BOOL bOpen;
	FUNID(ITextService_SetOpenStatus)
		PARAMS1(Input, bOpen)
};

struct Param_GetOpenStatus : FunParams_Base
{
	BOOL bOpen;
	FUNID(ITextService_GetOpenStatus)
		PARAMS1(Output, bOpen)
};

struct Param_GetActiveWnd : FunParams_Base
{
	DWORD hActive;
	FUNID(ITextService_GetActiveWnd)
		PARAMS1(Output, hActive)
};