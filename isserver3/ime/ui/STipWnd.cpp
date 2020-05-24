#include "stdafx.h"
#include "STipWnd.h"
#include "../../dataCenter/SearchCfg.h"
#include "../../UrlEncoder/Encoder.h"
#include <ShellAPI.h>

namespace SOUI {

	STipWnd::STipWnd(SEventSet *pEvtSets):CImeWnd(pEvtSets,UIRES.LAYOUT.wnd_tip)
	{
	}


	STipWnd::~STipWnd()
	{
	}

	void STipWnd::SetTip(const SStringT & strTitle, const SStringT & strTip,const SStringT &strKey)
	{
		m_strKey = strKey;
		FindChildByID(R.id.btn_search)->SetVisible(!strKey.IsEmpty());
		FindChildByID(R.id.txt_tip_title)->SetWindowTextW(strTitle);
		FindChildByID(R.id.txt_tip_content)->SetWindowTextW(strTip);
		UpdateLayout();

		CRect rcWnd = GetWindowRect();
		CRect rcWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
		int x = (rcWorkArea.Width() - rcWnd.Width()) / 2 + rcWorkArea.left;
		int y = (rcWorkArea.Height() - rcWnd.Height()) / 2 + rcWorkArea.top;

		SetWindowPos(HWND_TOPMOST, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
		GetNative()->SetTimer(TIMER_DELAY_HIDE, 5000, NULL);
	}

	void STipWnd::OnTimer(UINT_PTR idEvent)
	{
		if (idEvent == TIMER_DELAY_HIDE)
		{
			OnClose();
		}
		else
		{
			SetMsgHandled(FALSE);
		}
	}

	void STipWnd::OnMouseMove(UINT nFlags, CPoint point)
	{
		GetNative()->SetTimer(TIMER_DELAY_HIDE, 3000, NULL);
		SetMsgHandled(FALSE);
	}

	void STipWnd::OnClose()
	{
		DestroyWindow();
	}

	int STipWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		return OnRecreateUI(lpCreateStruct);
	}

	void STipWnd::OnSearch()
	{
		SStringT strSearchEngine = CSearchCfg::getSingletonPtr()->GetSelUrl();
		if(strSearchEngine.IsEmpty())
			return;
		SStringA strKey = S_CT2A(m_strKey);
		SStringA fmt = S_CT2A(strSearchEngine);
		std::string urlKey = Encoder::UTF8UrlEncode(strKey.c_str());
		SStringA strUrl = SStringA().Format(fmt,urlKey.c_str());
		ShellExecute(NULL,_T("open"),S_CA2T(strUrl),NULL,NULL,SW_SHOWNORMAL);
	}

}
