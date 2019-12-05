#pragma once
#include <helper/SDpiHelper.hpp>

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

	class CSkinAwareWnd : public SHostWnd,public SDpiHandler<CSkinAwareWnd>
	{
	public:
		CSkinAwareWnd(SEventSet *pEvtSets, LPCTSTR pszLayout);
		virtual ~CSkinAwareWnd();

		HWND Create(HWND hParent = NULL);

		void SetDestroyListener(IDestroyListener *pListener, IMEWNDTYPE type);

		IMEWNDTYPE GetWndType() const { return m_wndType; }
	protected:
		virtual int OnRecreateUI(LPCREATESTRUCT lpCreateStruct);

		virtual void OnFinalMessage(HWND hWnd);

		virtual bool IsDpiAware() const override;

		virtual void OnUserXmlNode(pugi::xml_node xmlUser) override;
	protected:
		bool OnEvent(EventArgs *e);
		SEventSet * m_pEvtSet;
	protected:
		virtual void OnSetSkin(EventArgs * e);
		EVENT_MAP_BEGIN()
			EVENT_HANDLER(EventSetSkin::EventID, OnSetSkin)
		EVENT_MAP_END()

		BEGIN_MSG_MAP_EX(CSkinAwareWnd)
			CHAIN_MSG_MAP(SDpiHandler<CSkinAwareWnd>)
			CHAIN_MSG_MAP(SHostWnd)
		END_MSG_MAP()
	private:
		IMEWNDTYPE		   m_wndType;
		IDestroyListener * m_pListener;
		bool			   m_bAutoScale;
	};


}
