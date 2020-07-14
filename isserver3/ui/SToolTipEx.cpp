#include "stdafx.h"
#include "SToolTipEx.h"


namespace SOUI
{

    #define TIMERID_DELAY2   1
    #define TIMERID_SPAN2     2

    STipCtrlEx::STipCtrlEx(void):SHostWnd(UIRES.LAYOUT.wnd_tooltip),m_nDelay(200),m_nShowSpan(5000)
		, m_Tick(0),m_bUpdated(FALSE)
		
    {
        m_id.dwHi = m_id.dwLow = 0;
    }

    STipCtrlEx::~STipCtrlEx(void)
    {
    }

    BOOL STipCtrlEx::CreateWnd(HWND hHost)
    {
		m_hHost = hHost;
		this->Create(NULL, WS_CLIPCHILDREN | WS_TABSTOP | WS_OVERLAPPED | WS_POPUP, WS_EX_NOACTIVATE|WS_EX_TOPMOST, 0, 0, 0, 0);
        return TRUE;
    }

    void STipCtrlEx::RelayEvent( const MSG *pMsg )
    {
        switch(pMsg->message)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
            OnTimer(TIMERID_SPAN2);//hide tip
            break;
        case WM_MOUSEMOVE:
            {
                m_ptTip = CPoint(GET_X_LPARAM(pMsg->lParam),GET_Y_LPARAM(pMsg->lParam));
                if(!m_rcTarget.PtInRect(m_ptTip))
                {
                    OnTimer(TIMERID_SPAN2);//hide tip
                }
                else if(/*!IsWindowVisible() &&*/ !m_strTip.IsEmpty())
                {
					DWORD t = ::GetTickCount();
					if ((t - m_Tick) > DWORD(m_nDelay))
					{
						KillTimer(TIMERID_DELAY2);
						SetTimer(TIMERID_DELAY2, m_nDelay);
						m_Tick = t;
					}
                    ::ClientToScreen(pMsg->hwnd,&m_ptTip);
					m_ptTip.y+=18;
					CPoint pt = AdjustTipPos(m_ptTip);
                    SetWindowPos(HWND_TOPMOST,pt.x,pt.y,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
                }
            }
            break;
        }
    }

    void STipCtrlEx::UpdateTip(const TIPID &id, CRect rc,LPCTSTR pszTip, int nScale)
    {
        if(m_id.dwHi == id.dwHi && m_id.dwLow== id.dwLow) return;

        m_id = id;
        m_rcTarget=rc;
        m_strTip=pszTip;
		m_bUpdated = TRUE;
		if(m_nScale != nScale)
		{
			m_nScale = nScale;
			SDispatchMessage(UM_SETSCALE, nScale, 0);
		}

        if(IsWindowVisible())
            ShowTip(TRUE);
    }

    void STipCtrlEx::ClearTip()
    {
        m_id.dwHi = m_id.dwLow =0;
        ShowTip(FALSE);
    }

    void STipCtrlEx::ShowTip(BOOL bShow)
    {
        if(!bShow)
        {
            ShowWindow(SW_HIDE);
            m_rcTarget.SetRect(0,0,0,0);
            m_strTip=_T("");
        }
        else if(!IsWindowVisible() && m_bUpdated && !m_strTip.IsEmpty())
        {
			m_bUpdated = FALSE;
			pugi::xml_document xmlDoc;
			DestroyAllChildren();
			GetRoot()->SetWindowText(L"");

			if(xmlDoc.load_string(m_strTip))
			{
				pugi::xml_node root = xmlDoc.first_child();
				if(_wcsicmp(root.name(),L"soui")==0)
				{
					InitFromXml(xmlDoc.first_child());
				}else
				{
					pugi::xml_document xmlDoc2;
					SApplication::getSingletonPtr()->LoadXmlDocment(xmlDoc2,m_strXmlLayout);
					InitFromXml(xmlDoc2.first_child());
					CreateChildren(xmlDoc);
					UpdateLayout();
				}
			}else
			{
				SApplication::getSingletonPtr()->LoadXmlDocment(xmlDoc,m_strXmlLayout);
				InitFromXml(xmlDoc.first_child());
				GetRoot()->SetWindowText(m_strTip);
			}
			CPoint pt = AdjustTipPos(m_ptTip);
			SetWindowPos(HWND_TOPMOST,pt.x,pt.y,0,0,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
        }
    }

    void STipCtrlEx::OnTimer( UINT_PTR idEvent )
    {
        switch(idEvent)
        {
        case TIMERID_DELAY2:
            KillTimer(TIMERID_DELAY2);       
            ShowTip(TRUE);
            SetTimer(TIMERID_SPAN2,m_nShowSpan);
            break;
        case TIMERID_SPAN2:
            ShowTip(FALSE);
            KillTimer(TIMERID_SPAN2);
            break;
        }
    }

    BOOL STipCtrlEx::PreTranslateMessage( MSG* pMsg )
    {
        if(IsWindow()) RelayEvent(pMsg);
        return FALSE;
    }


	void STipCtrlEx::OnFinalMessage( HWND hWnd )
    {
        SNativeWnd::OnFinalMessage(hWnd);
        delete this;
    }

	IToolTip * STipCtrlEx::CreateTooltip() const
	{
		return NULL;//disable tip for tip window.
	}

	void STipCtrlEx::DestroyTooltip(IToolTip * pTooltip) const
	{
	}

	CPoint STipCtrlEx::AdjustTipPos(CPoint pt) const
	{

		HMONITOR hMonitor;
		MONITORINFO mi;
		hMonitor = MonitorFromWindow(m_hHost, MONITOR_DEFAULTTONEAREST);	 
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);
		CRect rcWnd = GetWindowRect();
		if(pt.x+rcWnd.Width()>mi.rcMonitor.right)
			pt.x = mi.rcMonitor.right-rcWnd.Width();
		if(pt.y+rcWnd.Height()>mi.rcMonitor.bottom)
			pt.y = mi.rcMonitor.bottom-rcWnd.Height();
		return pt;
	}


	IToolTip * SToolTipFactory::CreateToolTip(HWND hHost)
	{
		STipCtrlEx *pTipCtrl = new STipCtrlEx;
		if (!pTipCtrl->CreateWnd(hHost))
		{
			delete pTipCtrl;
			return NULL;
		}
		return pTipCtrl;
	}

	void SToolTipFactory::DestroyToolTip(IToolTip *pToolTip)
	{
		if (pToolTip)
		{
			STipCtrlEx *pTipCtrl = (STipCtrlEx *)pToolTip;
			pTipCtrl->DestroyWindow();
		}
	}

}//namespace SOUI