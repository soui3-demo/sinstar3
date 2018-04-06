#include "StdAfx.h"
#include "ConfigDlg.h"

namespace SOUI
{
	CConfigDlg::CConfigDlg(void):SHostDialog(UIRES.LAYOUT.dlg_config)
	{
	}

	CConfigDlg::~CConfigDlg(void)
	{
	}

	BOOL CConfigDlg::OnInitDialog( HWND wnd, LPARAM lParam )
	{
		
		return FALSE;
	}

	void CConfigDlg::OnGotoPage( int nID )
	{
		SScrollView *pScrollView = FindChildByID2<SScrollView>(R.id.sv_setting);
		pScrollView->SetViewOrigin(CPoint(0,m_yTitles[nID-R.id.cfg_habit]));
	}

	void CConfigDlg::OnSettingRangeChanged( EventArgs *e )
	{
		SScrollView *pScrollView = sobj_cast<SScrollView>(e->sender);
		CPoint ptOrigin = pScrollView->GetViewOrigin();
		CPoint ptView = pScrollView->GetWindowRect().TopLeft();
		int delta = ptOrigin.y-ptView.y;
		m_yTitles[R.id.title_habit-R.id.title_habit] = FindChildByID(R.id.title_habit)->GetWindowRect().top+delta;
		m_yTitles[R.id.title_hotkey-R.id.title_habit] = FindChildByID(R.id.title_hotkey)->GetWindowRect().top+delta;
		m_yTitles[R.id.title_associate-R.id.title_habit] = FindChildByID(R.id.title_associate)->GetWindowRect().top+delta;
		m_yTitles[R.id.title_candidate-R.id.title_habit] = FindChildByID(R.id.title_candidate)->GetWindowRect().top+delta;
		m_yTitles[R.id.title_about-R.id.title_habit] = FindChildByID(R.id.title_about)->GetWindowRect().top+delta;
	}

}
