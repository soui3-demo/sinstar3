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
		SImageWnd * pImg = FindChildByID2<SImageWnd>(R.id.img_keymap);
		pImg->SetImage(m_pBmp);
		FindChildByID(R.id.txt_comp_name)->SetWindowText(m_strName);
		FindChildByID(R.id.link_comp_website)->SetWindowText(m_strUrl);
		return 0;
	}

}
