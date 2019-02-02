#include "stdafx.h"
#include "TextServiceProxy.h"
#include "Sinstar3Impl.h"
#include "../helper/helper.h"

CSvrConnection::CSvrConnection(IIpcHandle *pIpcHandle):m_ipcHandle(pIpcHandle)
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

UINT64 CSvrConnection::GetImeContext()
{
	Param_GetImeContext param;
	CallFun(&param);
	return param.lpImeContext;
}

BOOL CSvrConnection::ReleaseImeContext(UINT64 imeContext)
{
	Param_ReleaseImeContext param;
	param.lpImeContext =imeContext;
	CallFun(&param);
	return param.bRet;
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

BOOL CSvrConnection::SetOpenStatus(UINT64 imeContext,BOOL bOpen)
{
	Param_SetOpenStatus param;
	param.lpImeContext = imeContext;
	param.bOpen = bOpen;
	CallFun(&param);
	return param.bRet;
}

BOOL CSvrConnection::GetOpenStatus(UINT64 imeContext) const
{
	Param_GetOpenStatus param;
	param.lpImeContext = imeContext;
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
	m_bDpiAware = param.bDpiAware;
	m_strHostPath = param.strHostPath;
	Helper_VersionString(param.dwVer, szVer);
	SLOG_INFO("create connection, host:" << m_strHostPath.c_str() << " ver:" << szVer);
	m_pSinstar.Attach(new CSinstar3Impl(this, m_hSvr));
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
	if(!m_bDpiAware)
	{
		int nScale = CDataCenter::getSingletonPtr()->GetData().m_nScale;
		pt.x = pt.x * nScale/100;
		pt.y = pt.y * nScale/100;
		nHei = nHei * nScale/100;
	}
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
	m_pSinstar->OnSetFocus(param.bFocus);
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
