#pragma once

namespace SOUI
{
	class CKeyMapDlg;
	struct IKeyMapListener {
		virtual void OnKeyMapFree(CKeyMapDlg *pWnd) = 0;
	};
	class CKeyMapDlg: public SHostWnd
	{
	public:
		CKeyMapDlg(IBitmap *pBmp, LPCWSTR pszName, LPCWSTR pszUrl);
		~CKeyMapDlg();

		void SetListener(IKeyMapListener *pListener)
		{
			m_pListener = pListener;
		}
	protected:
		virtual void OnFinalMessage(HWND hWnd);
	protected:
		void OnClose();
		//souiœ˚œ¢
		EVENT_MAP_BEGIN()
			EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_MAP_END()

	protected:
		BOOL OnInitDialog(HWND wnd, LPARAM lInitParam);
		BEGIN_MSG_MAP_EX(CKeyMapDlg)
			MSG_WM_INITDIALOG(OnInitDialog)
			CHAIN_MSG_MAP(__super)
		END_MSG_MAP()

		CAutoRefPtr<IBitmap> m_pBmp;
		SStringT m_strName;
		SStringT m_strUrl;

		IKeyMapListener  * m_pListener;
	};

}
