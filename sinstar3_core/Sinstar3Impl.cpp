#include "StdAfx.h"
#include "Sinstar3Impl.h"

#include <initguid.h>


CSinstar3Impl::CSinstar3Impl(ITextService *pTxtSvr)
:m_pTxtSvr(pTxtSvr)
,m_pInputWnd(NULL)
,m_pStatusWnd(NULL)
{
	theModule->AddRef();

 	m_pInputWnd = new CInputWnd();
	m_pStatusWnd = new CStatusWnd();
	m_pStatusWnd->Create();
	m_pInputWnd->Create();

	SOUI::CSimpleWnd::Create(_T("sinstar3_msg_recv"),WS_DISABLED|WS_POPUP,WS_EX_TOOLWINDOW,0,0,0,0,HWND_MESSAGE,NULL);
	ISComm_Login(m_hWnd);
}

CSinstar3Impl::~CSinstar3Impl(void)
{
	ISComm_Logout(m_hWnd);
	SOUI::CSimpleWnd::DestroyWindow();

	m_pInputWnd->DestroyWindow();
	m_pStatusWnd->DestroyWindow();
	delete m_pStatusWnd;
	delete m_pInputWnd;


	theModule->Release();
}


void CSinstar3Impl:: ProcessKeyStoke(LPVOID lpImeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown,BOOL *pbEaten)
{
	*pbEaten = bKeyDown;
	return;
}

void CSinstar3Impl:: TranslateKey(LPVOID lpImeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown,BOOL *pbEaten)
{
	*pbEaten = TRUE;

	if(m_inputState.GetInputContext()->strInput.IsEmpty())
	{
		m_pTxtSvr->StartComposition(lpImeContext);
	}
	if(m_inputState.HandleKeyDown(vkCode,uScanCode))
	{
		m_pInputWnd->OnInputContextChanged(m_inputState.GetInputContext());
	}
	return;

	if(isprint(vkCode))
	{
		vkCode = tolower(vkCode);

		BOOL bCompChar = CDataCenter::GetAutoLockerInstance()->GetData().m_compInfo.IsCompChar((char)vkCode);
		if(bCompChar)
		{
			SStringT strComp = m_pInputWnd->GetCompStr();
			if(strComp.IsEmpty())
			{
				m_pTxtSvr->StartComposition(lpImeContext);
			}

			strComp.Append(vkCode);
			m_pTxtSvr->ReplaceSelCompositionW(lpImeContext,0,-1,strComp,strComp.GetLength());
			QueryCand(strComp);
		}
	}else if(vkCode == VK_ESCAPE || vkCode == VK_RETURN)
	{
		m_pTxtSvr->UpdateResultAndCompositionStringW(lpImeContext,L"启程输入法3",6,NULL,0);
		m_pTxtSvr->EndComposition(lpImeContext);
		m_pInputWnd->SetCompStr(_T(""));
	}else if(vkCode == VK_BACK)
	{
		SStringT strComp = m_pInputWnd->GetCompStr();
		if(strComp.GetLength()>0)
		{
			strComp = strComp.Left(strComp.GetLength()-1);
		}
		m_pTxtSvr->ReplaceSelCompositionW(lpImeContext,0,-1,strComp,strComp.GetLength());
		QueryCand(strComp);
	}
}

void CSinstar3Impl::OnIMESelect(BOOL bSelect)
{
}

void CSinstar3Impl::OnSetCaretPosition(POINT pt,int nHei)
{
	m_pInputWnd->MoveTo(pt,nHei);
}

void CSinstar3Impl::OnSetFocusSegmentPosition(POINT pt,int nHei)
{
}

void CSinstar3Impl::OnCompositionStarted()
{
	m_pInputWnd->Show(TRUE);
}

void CSinstar3Impl::OnCompositionChanged()
{
}

void CSinstar3Impl::OnCompositionTerminated()
{
 	m_pInputWnd->Show(FALSE);
}

void CSinstar3Impl::OnSetFocus(BOOL bFocus)
{
	SLOG_INFO("GetThreadID="<<GetCurrentThreadId()<<" focus="<<bFocus);
	m_pStatusWnd->Show(bFocus);
	m_pInputWnd->Show(bFocus);
}

int  CSinstar3Impl::GetCompositionSegments()
{
	return 0;
}

int  CSinstar3Impl::GetCompositionSegmentEnd(int iSeg)
{
	return 0;
}

int CSinstar3Impl::GetCompositionSegmentAttr(int iSeg)
{
	return 0;
}

BOOL CSinstar3Impl::OnHotkey(LPVOID lpImeContext,REFGUID guidHotKey)
{

	return FALSE;
}

void CSinstar3Impl::OnOpenStatusChanged(BOOL bOpen)
{
}

void CSinstar3Impl::OnConversionModeChanged(EInputMethod nMode)
{

}

void CSinstar3Impl::ShowHelp()
{
}

EInputMethod CSinstar3Impl::GetDefInputMode()
{
	return FullNative;
}

BOOL CSinstar3Impl::ShowCompWnd()
{
	return FALSE;
}

LRESULT CSinstar3Impl::OnWildMessage(WPARAM wParam,LPARAM lParam)
{
	return E_NOTIMPL;
}

void CSinstar3Impl::CheckDefIME()
{

}


void CSinstar3Impl::OnFinalRelease()
{

}

HRESULT CSinstar3Impl::OnQueryInterface(REFIID riid, void **ppvObject)
{
	return E_NOINTERFACE;
}

LRESULT CSinstar3Impl::OnSvrNotify(UINT uMsg, WPARAM wp, LPARAM lp)
{
	PMSGDATA pMsg=ISComm_OnSeverNotify(m_hWnd,wp,lp);
	if(wp == NT_KEYIN)
	{

	}
	else if(wp==NT_COMPINFO)
	{
		CDataCenter::getSingleton().Lock();
		CMyData &myData = CDataCenter::getSingleton().GetData();
		myData.m_compInfo.SetSvrCompInfo(ISComm_GetCompInfo());
		CDataCenter::getSingleton().Unlock();

		EventSvrNotify *evt= new EventSvrNotify(this);
		evt->wp = wp;
		evt->lp = lp;
		SNotifyCenter::getSingleton().FireEventAsync(evt);
		evt->Release();
	}

	return 0;
}

void CSinstar3Impl::QueryCand(const SStringT &strComp)
{
	m_pInputWnd->SetCompStr(strComp);
	SStringA strCompA = S_CT2A(strComp);
	if(strComp.IsEmpty())
	{
		m_pInputWnd->ClearCands();

	}
	else if(ISComm_QueryCand(strCompA,strCompA.GetLength(),0,m_hWnd) == ISACK_SUCCESS)
	{
		PMSGDATA pMsgData=ISComm_GetData();
		LPBYTE pbyData,pCandData;
		short i,sCount,sSingleWords=0;
		short sCandCount = 0;
		pbyData = pMsgData->byData+1;

		SArray<SStringT> arrCands,arrComps;
		memcpy(&sCount,pbyData,2);
		pbyData+=2;
		pCandData=pbyData;
		//先找出单字数量
		for(i=0;i<sCount;i++)
		{
			if(pCandData[1]==2) sSingleWords++;
			pCandData+=pCandData[1]+2;	//偏移词组信息
			pCandData+=pCandData[0]+1;	//偏移编码信息					
		}
		pCandData=pbyData;
		for(i=0;i<sCount;i++)
		{
			BYTE byRate = pCandData[0];
			BYTE * byCandText = pCandData+1;
			arrCands.Add(S_CA2T(SStringA((char*)byCandText+1,byCandText[0])));
			BYTE * byCandComp = byCandText + 1 + byCandText[0];
			arrComps.Add(S_CA2T(SStringA((char*)byCandComp+1,byCandComp[0])));

			pCandData+=pCandData[1]+2;	//偏移词组信息
			pCandData+=pCandData[0]+1;	//偏移编码信息
		}

		m_pInputWnd->SetCandidateInfo(arrCands,arrComps,sCount);
	}
}

