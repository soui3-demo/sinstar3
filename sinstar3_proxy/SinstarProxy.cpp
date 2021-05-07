#include "stdafx.h"
#include "SinstarProxy.h"
#include <shellapi.h>
#include "../helper/helper.h"
#include <assert.h>
#include <SIpcObject/ShareMemBuffer.h>

using namespace SOUI;

namespace SOUI{
	namespace IPC
	{
		BOOL SCreateInstance(IObjRef ** ppIpcFactory);
	}
}

bool CSinstarProxy::isInBlackList(LPCTSTR pszBlacklistFile)
{
	bool bRet = false;
	wchar_t szPath[MAX_PATH+1];
	GetModuleFileNameW(NULL,szPath,MAX_PATH);
	LPWSTR pszName = wcsrchr(szPath,L'\\');
	assert(pszName);
	pszName++;
	wchar_t *p=pszName;
	while(*p)
	{
		if(isupper(*p))
			*p = tolower(*p);
		p++;
	}

	const LPCWSTR KBuiltinBlackList[]={
		L"logonui.exe",
		L"svchost.exe",
		//L"searchui.exe",
	};

	for(int i=0;i<ARRAYSIZE(KBuiltinBlackList);i++)
	{
		if(wcscmp(pszName,KBuiltinBlackList[i])==0)
			return true;
	}

	wchar_t *wcsBuf=NULL;
	FILE * f = _tfopen(pszBlacklistFile,_T("rb"));
	if(!f)
		return false;

	fseek(f,0,SEEK_END);
	long len = ftell(f);
	if(len==0)
	{
		return false;
		fclose(f);
	}
	fseek(f,0,SEEK_SET);
	char * buf=(char*)malloc(len+1);
	fread(buf,1,len,f);
	buf[len]=0;
	fclose(f);

	wcsBuf=(wchar_t*)malloc((len+100)*sizeof(wchar_t));
	if(buf[0]==0xef && buf[1]==0xbb && buf[2]==0xbf)
	{//utf8 { 0xef,0xbb,0xbf }
		len = MultiByteToWideChar(CP_UTF8,0,buf,len,wcsBuf,len+100);
	}else if(buf[0]==0xfe && buf[1]==0xff)
	{//utf16
		memcpy(wcsBuf,buf,len);
		len/=sizeof(wchar_t);
	}else
	{//ansi
		len = MultiByteToWideChar(CP_ACP,0,buf,len,wcsBuf,len+100);
	}
	wcsBuf[len]=0;
	//make lower
	for(int i=0;i<len;i++)
	{
		if(isupper(wcsBuf[i]))
			wcsBuf[i] = tolower(wcsBuf[i]);
	}
	free(buf);

	bRet = wcsstr(wcsBuf,pszName)!=NULL;
	free(wcsBuf);

	return bRet;
}


CSinstarProxy::CSinstarProxy(ITextService *pTxtService):m_conn(pTxtService),m_hSvr(0)
{

}


CSinstarProxy::~CSinstarProxy()
{
	Param_Destroy param;
	m_conn.CallFun(&param);
	m_conn.GetIpcHandle()->Disconnect((ULONG_PTR)m_hSvr);
}

BOOL CSinstarProxy::Init(HWND hClient, LPCTSTR pszSvrPath)
{
	HWND hSvr = FindWindow(NULL, SINSTAR3_SERVER_HWND);
	if (!hSvr)
	{
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = _T("open");
		ShExecInfo.lpFile = pszSvrPath;
		ShExecInfo.lpParameters = NULL;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;
		if (!::ShellExecuteEx(&ShExecInfo))
			return FALSE;
		SASSERT(ShExecInfo.hProcess);
		WaitForInputIdle(ShExecInfo.hProcess, 1000);
		CloseHandle(ShExecInfo.hProcess);
		for (int i = 0; i < 300 && !hSvr; i++)
		{
			Sleep(10);
			hSvr = FindWindow(NULL, SINSTAR3_SERVER_HWND);
		}
	}
	if (!hSvr)
	{
		return FALSE;
	}
	if(m_conn.GetIpcHandle()->ConnectTo((ULONG_PTR)hClient,(ULONG_PTR)hSvr) != 0)
	{
		return FALSE;
	}
	m_hSvr = hSvr;
	Param_Create param;
	HMODULE hMod = GetModuleHandle(NULL);
	if (hMod)
	{
		TCHAR szBuf[MAX_PATH];
		GetModuleFileName(hMod, szBuf, MAX_PATH);
		char szBufA[MAX_PATH];
		GetModuleFileNameA(hMod, szBufA, MAX_PATH);
		param.strHostPath = szBufA;
		Helper_PEVersion(szBuf, &param.dwVer, NULL, NULL);
	}

	m_conn.CallFun(&param);
	
	return TRUE;
}

void CSinstarProxy::NotifyScaleInfo(HWND hRefWnd)
{
	Param_ScaleInfo param;
	RECT rc={0};
	GetWindowRect(hRefWnd,&rc);
	param.hRefWnd = (DWORD)(ULONG_PTR)hRefWnd;
	param.szWnd.cx = rc.right - rc.left;
	param.szWnd.cy = rc.bottom - rc.top;
	m_conn.CallFun(&param);
}

void CSinstarProxy::GetCandidateListInfo(Context &_ctx)
{
	Param_CandidateListInfo param;
	m_conn.CallFun(&param);
	_ctx = param.ctx;
}

void CSinstarProxy::OnIMESelect(BOOL bSelect)
{
	Param_OnImeSelect param;
	param.bSelect = bSelect;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnCompositionStarted(bool bCanShowUI)
{
	Param_OnCompositionStarted param;
	param.bShowUI = bCanShowUI;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnCompositionStarted()
{
	Param_OnCompositionStarted param;
	param.bShowUI = true;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnCompositionChanged()
{
	Param_OnCompositionChanged param;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnCompositionTerminated(bool bClearCtx)
{
	Param_OnCompositionTerminated param;
	param.bClearCtx = bClearCtx;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnSetCaretPosition(POINT pt, int nHei)
{
	Param_OnSetCaretPosition param;
	param.pt = pt;
	param.nHei = nHei;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnSetFocusSegmentPosition(POINT pt, int nHei)
{
	Param_OnSetFocusSegmentPosition param;
	param.pt = pt;
	param.nHei = nHei;
	m_conn.CallFun(&param);
}

void CSinstarProxy::ProcessKeyStoke(UINT64 imeContext, UINT vkCode, LPARAM lParam, BOOL bKeyDown, BYTE byKeyState[256], BOOL * pbEaten)
{
	Param_ProcessKeyStoke param;
	param.lpImeContext = imeContext;
	param.vkCode = vkCode;
	param.lParam = (DWORD)lParam;
	param.bKeyDown = bKeyDown;
	memcpy(param.byKeyState, byKeyState, 256);
	param.bEaten = false;
	m_conn.CallFun(&param);
	*pbEaten = param.bEaten;
}

void CSinstarProxy::TranslateKey(UINT64 imeContext, UINT vkCode, UINT uScanCode, BOOL bKeyDown, BYTE byKeyState[256], BOOL * pbEaten)
{
	Param_TranslateKey param;
	param.lpImeContext = imeContext;
	param.vkCode = vkCode;
	param.uScanCode = uScanCode;
	param.bKeyDown = bKeyDown;
	memcpy(param.byKeyState, byKeyState, 256);
	param.bEaten = false;
	m_conn.CallFun(&param);
	*pbEaten = param.bEaten;
}

void CSinstarProxy::OnSetFocus(BOOL bFocus,DWORD dwActiveWnd)
{
	Param_OnSetFocus param;
	param.bFocus = bFocus;
	param.dwActiveWnd = dwActiveWnd;
	m_conn.CallFun(&param);
}

int CSinstarProxy::GetCompositionSegments()
{
	Param_GetCompositionSegments param;
	param.nSegs = 0;
	m_conn.CallFun(&param);
	return param.nSegs;
}

int CSinstarProxy::GetCompositionSegmentEnd(int iSeg)
{
	Param_GetCompositionSegmentEnd param;
	param.iSeg = iSeg;
	param.iEnd = -1;
	m_conn.CallFun(&param);
	return param.iEnd;
}

int CSinstarProxy::GetCompositionSegmentAttr(int iSeg)
{
	Param_GetCompositionSegmentAttr param;
	param.iSeg = iSeg;
	param.nAttr = 0;
	m_conn.CallFun(&param);
	return param.nAttr;
}

void CSinstarProxy::OnOpenStatusChanged(BOOL bOpen)
{
	Param_OnOpenStatusChanged param;
	param.bOpen = bOpen;
	m_conn.CallFun(&param);
}

void CSinstarProxy::OnConversionModeChanged(EInputMethod uMode)
{
	Param_OnConversionModeChanged param;
	param.uMode = uMode;
	m_conn.CallFun(&param);
}

void CSinstarProxy::ShowHelp()
{
	Param_ShowHelp param;
	m_conn.CallFun(&param);
}

EInputMethod CSinstarProxy::GetDefInputMode()
{
	Param_GetDefInputMode param;
	param.uMode = HalfAlphanumeric;
	m_conn.CallFun(&param);
	return param.uMode;
}

////////////////////////////////////////////////////////////////////////////////
void CClientConnection::OnInputStringW( Param_InputStringW & param)
{
	m_pTxtService->InputStringW(param.buf.c_str(), (int)param.buf.length());
}

void CClientConnection::OnStartComposition( Param_StartComposition & param)
{
	m_pTxtService->StartComposition(param.lpImeContext);
}

void CClientConnection::OnReplaceSelCompositionW( Param_ReplaceSelCompositionW & param)
{
	m_pTxtService->ReplaceSelCompositionW(param.lpImeContext, param.nLeft, param.nRight, param.buf.c_str(), (int)param.buf.length());
}

void CClientConnection::OnUpdateResultAndCompositionStringW( Param_UpdateResultAndCompositionStringW & param)
{
	m_pTxtService->UpdateResultAndCompositionStringW(param.lpImeContext, param.resultStr.c_str(), (int)param.resultStr.length(), param.compStr.c_str(), (int)param.compStr.length());
}

void CClientConnection::OnEndComposition( Param_EndComposition & param)
{
	m_pTxtService->EndComposition(param.lpImeContext);
}

void CClientConnection::OnSetConversionMode( Param_SetConversionMode & param)
{
	m_pTxtService->SetConversionMode(param.mode);
}

void CClientConnection::OnIsCompositing( Param_IsCompositing & param)
{
	param.bRet = m_pTxtService->IsCompositing();
}

void CClientConnection::OnGetConversionMode( Param_GetConversionMode & param)
{
	param.mode = m_pTxtService->GetConversionMode();

}

void CClientConnection::OnSetOpenStatus( Param_SetOpenStatus & param)
{
	m_pTxtService->SetOpenStatus(param.bOpen);
}

void CClientConnection::OnGetOpenStatus( Param_GetOpenStatus & param)
{
	param.bOpen = m_pTxtService->GetOpenStatus();
}

void CClientConnection::OnGetActiveWnd(Param_GetActiveWnd &param)
{
	param.hActive = m_pTxtService->GetActiveWnd();
}

void CClientConnection::OnUpdateUI(Param_UpdateUI& param)
{
	m_pTxtService->UpdateUI(param.imeContext,param.bPageChanged,param.curPage);
}

void CClientConnection::OnUpdatePreedit(Param_UpdatePreedit& param)
{
	m_pTxtService->UpdatePreedit(param.imeContext,param.strPreedit);
}

SOUI::IIpcHandle * CClientConnection::GetIpcHandle()
{
	return m_ipcHandle;
}

void CClientConnection::BuildShareBufferName(ULONG_PTR idLocal, ULONG_PTR idRemote, TCHAR szName[MAX_PATH]) const 
{
	_stprintf(szName, SINSTAR3_SHARE_BUF_NAME_FMT, (DWORD)(((LPARAM)idLocal)&0xffffffff), (DWORD)(((LPARAM)idRemote) & 0xffffffff));
}

bool CClientConnection::CallFun(SOUI::IFunParams *params) const
{
	SASSERT(m_ipcHandle);
	return m_ipcHandle->CallFun(params);
}

CClientConnection::CClientConnection(ITextService * pTxtService) :m_pTxtService(pTxtService)
{
	CAutoRefPtr<IIpcFactory> ipcFactory;
	IPC::SCreateInstance((IObjRef**)&ipcFactory);
	ipcFactory->CreateIpcHandle(&m_ipcHandle);
	m_ipcHandle->SetIpcConnection(this);
}

int CClientConnection::GetBufSize() const
{
	return 1024*2;
}

int CClientConnection::GetStackSize() const
{
	return 10;
}

