#include "stdafx.h"
#include "TextServiceProxy.h"
#include "../helper/helper.h"
#include "Sinstar3Impl.h"

CSvrConnection::CSvrConnection(IIpcHandle *pIpcHandle,HWND hSvr,IConntionFocusListener * pFocusListener)
:m_ipcHandle(pIpcHandle)
,m_hSvr(hSvr)
,m_xScale(1.0f)
,m_yScale(1.0f)
,m_pFocusListener(pFocusListener)
{
}

CSvrConnection::~CSvrConnection(void)
{
}

BOOL CSvrConnection::InputStringW(LPCWSTR pszBuf, int nLen)
{
	Param_InputStringW param;
	param.buf = std::wstring(pszBuf, nLen);
	CallFun(&param);
	return param.bRet;
}

BOOL CSvrConnection::IsCompositing() const
{
	Param_IsCompositing param;
	CallFun(&param);
	return param.bRet;
}


void CSvrConnection::StartComposition(UINT64 imeContext)
{
	Param_StartComposition param;
	param.lpImeContext = imeContext;
	CallFun(&param);
}

void CSvrConnection::ReplaceSelCompositionW(UINT64 imeContext,int nLeft,int nRight,const WCHAR *wszComp,int nLen)
{
	Param_ReplaceSelCompositionW param;
	param.lpImeContext = imeContext;
	param.nLeft = nLeft;
	param.nRight = nRight;
	param.buf = std::wstring(wszComp,nLen);
	CallFun(&param);
}

void CSvrConnection::UpdateResultAndCompositionStringW(UINT64 imeContext,const WCHAR *wszResultStr,int nResStrLen,const WCHAR *wszCompStr,int nCompStrLen)
{
	Param_UpdateResultAndCompositionStringW param;
	param.lpImeContext = imeContext;
	param.resultStr = std::wstring(wszResultStr,nResStrLen);
	param.compStr = std::wstring(wszCompStr,nCompStrLen);
	CallFun(&param);
}

void CSvrConnection::EndComposition(UINT64 imeContext)
{
	Param_EndComposition param;
	param.lpImeContext = imeContext;
	CallFun(&param);
}

void CSvrConnection::SetConversionMode(EInputMethod mode)
{
	Param_SetConversionMode param;
	param.mode = mode;
	CallFun(&param);
}

EInputMethod CSvrConnection::GetConversionMode()
{
	Param_GetConversionMode param;
	CallFun(&param);
	return param.mode;
}

void CSvrConnection::SetOpenStatus(BOOL bOpen)
{
	Param_SetOpenStatus param;
	param.bOpen = bOpen;
	CallFun(&param);
}

BOOL CSvrConnection::GetOpenStatus() const
{
	Param_GetOpenStatus param;
	CallFun(&param);
	return param.bOpen;
}


DWORD CSvrConnection::GetActiveWnd() const
{
	Param_GetActiveWnd param;
	CallFun(&param);
	return param.hActive;
}

//////////////////////////////////////////////////////////////////////////
void CSvrConnection::HandleCreate(Param_Create & param)
{
	SASSERT(!m_pSinstar);
	TCHAR szVer[100];
	m_strHostPath = param.strHostPath;
	Helper_VersionString(param.dwVer, szVer);
	SLOG_INFO("create connection, host:" << m_strHostPath.c_str() << " ver:" << szVer);
	m_pSinstar.Attach(new CSinstar3Impl(this, m_hSvr));
}


void CSvrConnection::HandleScaleInfo(Param_ScaleInfo &param)
{
	CRect rcWnd;
	HWND hRefWnd = (HWND)param.hRefWnd;
	if(IsWindow(hRefWnd))
	{
		GetWindowRect((HWND)param.hRefWnd,&rcWnd);
		m_xScale = rcWnd.Width()*1.0f/param.szWnd.cx;
		m_yScale = rcWnd.Height()*1.0f/param.szWnd.cy;
	}else
	{
		SLOG_ERROR("ref hwnd is invalid");
	}
}

void CSvrConnection::HandleDestroy(Param_Destroy & param)
{
	SASSERT(m_pSinstar);
	m_pSinstar = NULL;
}

void CSvrConnection::HandleOnImeSelect(Param_OnImeSelect & param)
{
	m_pSinstar->OnIMESelect(param.bSelect);

}

void CSvrConnection::HandleOnCompositionStarted(Param_OnCompositionStarted &param)
{
	m_pSinstar->OnCompositionStarted();

}

void CSvrConnection::HandleOnCompositionChanged(Param_OnCompositionChanged &param)
{
	m_pSinstar->OnCompositionChanged();

}

void CSvrConnection::HandleOnCompositionTerminated(Param_OnCompositionTerminated &param)
{
	m_pSinstar->OnCompositionTerminated(param.bClearCtx);

}

void CSvrConnection::HandleOnSetCaretPosition(Param_OnSetCaretPosition &param)
{
	CPoint pt = param.pt;
	int nHei = param.nHei;
	pt.x = (int)(pt.x*m_xScale);
	pt.y = (int)(pt.y*m_yScale);
	nHei = (int)(nHei*m_yScale);

	m_pSinstar->OnSetCaretPosition(pt,nHei);

}

void CSvrConnection::HandleOnSetFocusSegmentPosition(Param_OnSetFocusSegmentPosition &param)
{
	m_pSinstar->OnSetFocusSegmentPosition(param.pt,param.nHei);

}

void CSvrConnection::HandleProcessKeyStoke(Param_ProcessKeyStoke &param)
{
	m_pSinstar->ProcessKeyStoke(param.lpImeContext,param.vkCode,param.lParam,param.bKeyDown, param.byKeyState,&param.bEaten);
}

void CSvrConnection::HandleTranslateKey(Param_TranslateKey &param)
{
	m_pSinstar->TranslateKey(param.lpImeContext,param.vkCode,param.uScanCode,param.bKeyDown, param.byKeyState,&param.bEaten);
}

void CSvrConnection::HandleOnSetFocus(Param_OnSetFocus &param)
{
	SLOG_INFO("OnSetFocus, host:"<<m_strHostPath.c_str()<<" bFocus:"<<param.bFocus);
	if(!m_pSinstar) return;
	m_pSinstar->OnSetFocus(param.bFocus,param.dwActiveWnd);
	if(m_pFocusListener)
	{
		if(param.bFocus)
			m_pFocusListener->OnSetFocus(this);
		else
			m_pFocusListener->OnKillFocus(this);
	}
}

void CSvrConnection::HandleGetCompositionSegments(Param_GetCompositionSegments &param)
{
	param.nSegs = m_pSinstar->GetCompositionSegments();

}

void CSvrConnection::HandleGetCompositionSegmentEnd(Param_GetCompositionSegmentEnd &param)
{
	param.iEnd = m_pSinstar->GetCompositionSegmentEnd(param.iSeg);

}

void CSvrConnection::HandleGetCompositionSegmentAttr(Param_GetCompositionSegmentAttr &param)
{
	param.nAttr = m_pSinstar->GetCompositionSegmentAttr(param.iSeg);

}

void CSvrConnection::HandleOnOpenStatusChanged(Param_OnOpenStatusChanged &param)
{
	m_pSinstar->OnOpenStatusChanged(param.bOpen);

}

void CSvrConnection::HandleOnConversionModeChanged(Param_OnConversionModeChanged &param)
{
	m_pSinstar->OnConversionModeChanged(param.uMode);

}

void CSvrConnection::HandleShowHelp(Param_ShowHelp &param)
{
	m_pSinstar->ShowHelp();

}

void CSvrConnection::HandleGetDefInputMode(Param_GetDefInputMode &param)
{
	param.uMode = m_pSinstar->GetDefInputMode();

}

IIpcHandle * CSvrConnection::GetIpcHandle()
{
	return m_ipcHandle;
}

void CSvrConnection::BuildShareBufferName(ULONG_PTR idLocal, ULONG_PTR idRemote, TCHAR szName[MAX_PATH]) const
{
	_stprintf(szName, SINSTAR3_SHARE_BUF_NAME_FMT, (DWORD)(((LPARAM)idLocal)&0xffffffff), (DWORD)(((LPARAM)idRemote) & 0xffffffff));
}

bool CSvrConnection::CallFun(IFunParams * params) const
{
	return m_ipcHandle->CallFun(params);
}

void CSvrConnection::OnSkinChanged()
{
	CSinstar3Impl *pSinstar3 =(CSinstar3Impl*)(ISinstar*)m_pSinstar;
	pSinstar3->OnSkinChanged();
}

int CSvrConnection::GetBufSize() const 
{
	return 1024*2;
}

int CSvrConnection::GetStackSize() const 
{
	return 10;
}
