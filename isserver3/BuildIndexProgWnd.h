#pragma once

namespace SOUI
{
	class CBuildIndexProgWnd : public SHostWnd
	{
	public:
		CBuildIndexProgWnd();
		~CBuildIndexProgWnd();

		void SetPage(int iPage)
		{
			FindChildByID2<STabCtrl>(R.id.tab_stage)->SetCurSel(iPage);
			SetProg(0);
		}

		void SetProg(int nValue)
		{
			STabCtrl *pTab = FindChildByID2<STabCtrl>(R.id.tab_stage);
			int iCurPage = pTab->GetCurSel();
			SWindow *pPage = pTab->GetPage(iCurPage);
			pPage->FindChildByID2<SProgress>(R.id.prog_build_index)->SetValue(nValue);
		}

		virtual void OnFinalMessage(HWND hWnd);

		BEGIN_MSG_MAP_EX(CBuildIndexProgWnd)
			CHAIN_MSG_MAP(__super)
		END_MSG_MAP()
	};


}
