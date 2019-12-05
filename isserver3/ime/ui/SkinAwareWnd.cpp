#include "stdafx.h"
#include "SkinAwareWnd.h"

namespace SOUI{
	CSkinAwareWnd::CSkinAwareWnd(SEventSet *pEvtSets, LPCTSTR pszLayout)
		:SHostWnd(pszLayout)
		, m_pEvtSet(pEvtSets)
		, m_pListener(NULL)
		, m_wndType(IME_UNKNOWN)
		, m_bAutoScale(false)
	{
		m_pEvtSet->subscribeEvent(EventSvrNotify::EventID, Subscriber(&CSkinAwareWnd::OnEvent, this));
		m_pEvtSet->subscribeEvent(EventSetSkin::EventID, Subscriber(&CSkinAwareWnd::OnEvent, this));
	}

	CSkinAwareWnd::~CSkinAwareWnd()
	{
		m_pEvtSet->unsubscribeEvent(EventSvrNotify::EventID, Subscriber(&CSkinAwareWnd::OnEvent, this));
		m_pEvtSet->unsubscribeEvent(EventSetSkin::EventID, Subscriber(&CSkinAwareWnd::OnEvent, this));
	}

	bool CSkinAwareWnd::OnEvent(EventArgs * e)
	{
		return !!_HandleEvent(e);
	}

	void CSkinAwareWnd::OnSetSkin(EventArgs *e)
	{
		SHostWnd::OnDestroy();
		SetWindowPos(0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		CREATESTRUCT cs;
		cs.cx = 0;
		cs.cy = 0;
		OnRecreateUI(&cs);
		e->bubbleUp = true;
	}

	HWND CSkinAwareWnd::Create(HWND hParent)
	{
		return SHostWnd::Create(hParent,WS_POPUP, WS_EX_TOOLWINDOW, 0, 0, 0, 0);
	}

	void CSkinAwareWnd::SetDestroyListener(IDestroyListener * pListener, IMEWNDTYPE type)
	{
		m_pListener = pListener;
		m_wndType = type;
	}

	int CSkinAwareWnd::OnRecreateUI(LPCREATESTRUCT lpCreateStruct)
	{
		m_bAutoScale = false;
		SHostWnd::OnCreate(lpCreateStruct);
		ScaleHost(m_hWnd);
		return 0;
	}

	void CSkinAwareWnd::OnFinalMessage(HWND hWnd)
	{
		__super::OnFinalMessage(hWnd);
		if (m_pListener)
		{
			m_pListener->OnSkinAwareWndDestroy(this);
		}
	}

	bool CSkinAwareWnd::IsDpiAware() const 
	{
		return m_bAutoScale;
	}

	void CSkinAwareWnd::OnUserXmlNode(pugi::xml_node xmlUser)
	{
		if(wcsicmp(xmlUser.name(),L"user")==0)
		{
			m_bAutoScale = xmlUser.attribute(L"autoScale").as_bool();
		}
	}

}
