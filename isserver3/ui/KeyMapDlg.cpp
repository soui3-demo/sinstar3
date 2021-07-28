#include "stdafx.h"
#include "KeyMapDlg.h"

namespace SOUI
{
	CKeyMapDlg::CKeyMapDlg(IBitmap *pBmp, LPCWSTR pszName, LPCWSTR pszUrl):SHostWnd(UIRES.LAYOUT.dlg_keymap), m_pBmp(pBmp), m_pListener(NULL)
	{
		m_strName = S_CW2T(pszName);
		m_strUrl = S_CW2T(pszUrl);
	}


	CKeyMapDlg::~CKeyMapDlg()
	{
	}

	void CKeyMapDlg::OnFinalMessage(HWND hWnd)
	{
		__super::OnFinalMessage(hWnd);
		if (m_pListener) m_pListener->OnKeyMapFree(this);
		delete this;
	}

	void CKeyMapDlg::OnClose()
	{
		DestroyWindow();
	}

	BOOL CKeyMapDlg::OnInitDialog(HWND wnd, LPARAM lInitParam)
	{
		CRect rcWnd = GetWindowRect();

		if(m_pBmp)
		{
			SImageWnd * pImg = FindChildByID2<SImageWnd>(R.id.img_keymap);
			pImg->SetImage(m_pBmp);
			FindChildByID(R.id.txt_keymap)->SetVisible(FALSE,TRUE);
			pImg->SetVisible(TRUE,TRUE);
		}
		FindChildByID(R.id.txt_comp_name)->SetWindowText(m_strName);
		FindChildByID(R.id.link_comp_website)->SetAttribute(L"href",m_strUrl);
		UpdateWindow();
		CenterWindow();
		return 0;
	}

	void CKeyMapDlg::OnSBarTransparent(EventArgs *e)
	{
		EventSliderPos *e2 = sobj_cast<EventSliderPos>(e);
		GetRoot()->SetAlpha(e2->nPos*255/100);
	}

}
