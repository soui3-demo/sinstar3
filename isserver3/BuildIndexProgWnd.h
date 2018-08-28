#pragma once

namespace SOUI
{
	class CBuildIndexProgWnd : public SHostWnd
	{
	public:
		CBuildIndexProgWnd();
		~CBuildIndexProgWnd();

		void SetPage(int iPage,int nMax)
		{
			STabCtrl *pTab = FindChildByID2<STabCtrl>(R.id.tab_stage);
			pTab->SetCurSel(iPage);
			SWindow *pPage = pTab->GetPage(iPage);
			SProgress *pProg = pPage->FindChildByID2<SProgress>(R.id.prog_build_index);
			pProg->SetRange(0, nMax);
			pProg->SetValue(0);
			pTab->UpdateWindow();
		}

		void SetProg(int nValue)
		{
			STabCtrl *pTab = FindChildByID2<STabCtrl>(R.id.tab_stage);
			int iCurPage = pTab->GetCurSel();
			SWindow *pPage = pTab->GetPage(iCurPage);
			pPage->FindChildByID2<SProgress>(R.id.prog_build_index)->SetValue(nValue);
			pTab->UpdateWindow();
		}

		virtual void OnFinalMessage(HWND hWnd);
	};


}
