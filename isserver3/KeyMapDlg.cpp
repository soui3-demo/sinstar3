#include "stdafx.h"
#include "KeyMapDlg.h"

namespace SOUI
{
	CKeyMapDlg::CKeyMapDlg(IBitmap *pBmp, LPCSTR pszName, LPCSTR pszUrl):SHostWnd(UIRES.LAYOUT.dlg_keymap), m_pBmp(pBmp), m_pListener(NULL)
	{
		m_strName = S_CA2T(pszName);
		m_strUrl = S_CA2T(pszUrl);
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
		CSize szImg = m_pBmp->Size();
		szImg.cx = min(szImg.cx, 1024)+10;
		szImg.cy = min(szImg.cy, 768)+rcWnd.Height();

		SetWindowPos(NULL, 0, 0, szImg.cx, szImg.cy, SWP_NOZORDER | SWP_NOMOVE);
		SImageWnd * pImg = FindChildByID2<SImageWnd>(R.id.img_keymap);
		pImg->SetImage(m_pBmp);
		FindChildByID(R.id.txt_comp_name)->SetWindowText(m_strName);
		FindChildByID(R.id.link_comp_website)->SetAttribute(L"href",m_strUrl);
		return 0;
	}

}
