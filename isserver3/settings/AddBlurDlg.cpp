#include "stdafx.h"
#include "AddBlurDlg.h"

namespace SOUI
{

	static const BLUREQUILT KBlurTune[] = { { "z","zh" },{ "c","ch" },{ "s","sh" },{ "n","l" },{ "l","r" } };
	static const BLUREQUILT KBlurRhyme[] = { { "an","ang" },{ "en","eng" },{ "in","ing" },{ "uan","uang" },{ "ian","iang" } };
	static const BLUREQUILT KBlurFull[] = { { "huang","wang" } };

	CAddBlurDlg::CAddBlurDlg():SHostDialog(UIRES.LAYOUT.dlg_addblur)
	{
	}


	CAddBlurDlg::~CAddBlurDlg()
	{
	}


	BOOL CAddBlurDlg::OnInitDialog(HWND wnd, LPARAM lInitParam)
	{
		{
			SComboBox *pCbxBlur = FindChildByID2<SComboBox>(R.id.cbx_tune_blur);
			for (int i = 0; i < ARRAYSIZE(KBlurTune); i++)
			{
				SStringA strBlur = SStringA().Format("%s=%s", KBlurTune[i].szFrom, KBlurTune[i].szTo);
				pCbxBlur->InsertItem(-1, S_CA2T(strBlur), 0, i);
			}
		}
		{
			SComboBox *pCbxBlur = FindChildByID2<SComboBox>(R.id.cbx_rhyme_blur);
			for (int i = 0; i < ARRAYSIZE(KBlurTune); i++)
			{
				SStringA strBlur = SStringA().Format("%s=%s", KBlurRhyme[i].szFrom, KBlurRhyme[i].szTo);
				pCbxBlur->InsertItem(-1, S_CA2T(strBlur), 0, i);
			}
		}

		{
			SComboBox *pCbxBlur = FindChildByID2<SComboBox>(R.id.cbx_full_blur);
			for (int i = 0; i < ARRAYSIZE(KBlurFull); i++)
			{
				SStringA strBlur = SStringA().Format("%s=%s", KBlurFull[i].szFrom, KBlurFull[i].szTo);
				pCbxBlur->InsertItem(-1, S_CA2T(strBlur), 0, i);
			}
		}

		return 0;
	}

	void CAddBlurDlg::OnOK()
	{
		m_strFrom = S_CT2A(FindChildByID(R.id.edit_blur_from)->GetWindowText());
		m_strTo = S_CT2A(FindChildByID(R.id.edit_blur_to)->GetWindowText());
		EndDialog(IDOK);
	}

	void CAddBlurDlg::OnCbxTuneChange(EventArgs * e)
	{
		EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
		SASSERT(e);
		FindChildByID(R.id.edit_blur_from)->SetWindowText(S_CA2T(KBlurTune[e2->nCurSel].szFrom));
		FindChildByID(R.id.edit_blur_to)->SetWindowText(S_CA2T(KBlurTune[e2->nCurSel].szTo));
	}

	void CAddBlurDlg::OnCbxRhymeChange(EventArgs * e)
	{
		EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
		SASSERT(e);
		FindChildByID(R.id.edit_blur_from)->SetWindowText(S_CA2T(KBlurRhyme[e2->nCurSel].szFrom));
		FindChildByID(R.id.edit_blur_to)->SetWindowText(S_CA2T(KBlurRhyme[e2->nCurSel].szTo));
	}

	void CAddBlurDlg::OnCbxFullChange(EventArgs * e)
	{
		EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
		SASSERT(e);
		FindChildByID(R.id.edit_blur_from)->SetWindowText(S_CA2T(KBlurFull[e2->nCurSel].szFrom));
		FindChildByID(R.id.edit_blur_to)->SetWindowText(S_CA2T(KBlurFull[e2->nCurSel].szTo));
	}

}
