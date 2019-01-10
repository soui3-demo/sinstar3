#pragma once

namespace SOUI
{
	enum IMEWNDTYPE
	{
		IME_UNKNOWN = 0,
		IME_INPUT,
		IME_STATUS,
		IME_CONFIG,
		IME_TIP,
	};

	class CSkinAwareWnd;
	interface IDestroyListener {
		virtual void OnSkinAwareWndDestroy(CSkinAwareWnd *pWnd) = 0;
	};

	class CSkinAwareWnd : public SHostWnd
	{
	public:
		CSkinAwareWnd(SEventSet *pEvtSets, LPCTSTR pszLayout);
		virtual ~CSkinAwareWnd();

		HWND Create(LPCTSTR pszTitle, HWND hParent = NULL);

		void SetDestroyListener(IDestroyListener *pListener, IMEWNDTYPE type);

		IMEWNDTYPE GetWndType() const { return m_wndType; }
	protected:
		virtual int OnRecreateUI(LPCREATESTRUCT lpCreateStruct);

		virtual void OnFinalMessage(HWND hWnd);
	protected:
		bool OnEvent(EventArgs *e);
		SEventSet * m_pEvtSet;
	protected:
		virtual void OnSetSkin(EventArgs * e);
		EVENT_MAP_BEGIN()
			EVENT_HANDLER(EventSetSkin::EventID, OnSetSkin)
		EVENT_MAP_END()

	private:
		IMEWNDTYPE		   m_wndType;
		IDestroyListener * m_pListener;
	};


}
