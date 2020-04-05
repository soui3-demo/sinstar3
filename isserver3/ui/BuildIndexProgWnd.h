#pragma once

namespace SOUI
{
	class CBuildIndexProgWnd : public SHostWnd, public SDpiHandler<CBuildIndexProgWnd>
	{
	public:
		CBuildIndexProgWnd();
		~CBuildIndexProgWnd();

		void SetPage(int iPage,int nMax);

		void SetProg(int nValue);

	protected:
		virtual void OnFinalMessage(HWND hWnd);
		
		BEGIN_MSG_MAP_EX(CBuildIndexProgWnd)
			CHAIN_MSG_MAP(SDpiHandler<CBuildIndexProgWnd>)
			CHAIN_MSG_MAP(SHostWnd)
		END_MSG_MAP()

	};


}
