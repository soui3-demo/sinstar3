#include "stdafx.h"
#include "SToolTipEx.h"


namespace SOUI
{

    #define TIMERID_DELAY2   1
    #define TIMERID_SPAN2     2

    STipCtrlEx::STipCtrlEx(void):SHostWnd(UIRES.LAYOUT.wnd_tooltip),m_nDelay(50),m_nShowSpan(5000)
		, m_Tick(0)
		
    {
        m_id.dwHi = m_id.dwLow = 0;
    }

    STipCtrlEx::~STipCtrlEx(void)
    {
    }

    BOOL STipCtrlEx::CreateWnd()
    {
		this->Create(NULL, WS_CLIPCHILDREN | WS_TABSTOP | WS_OVERLAPPED | WS_POPUP, WS_EX_NOACTIVATE, 0, 0, 0, 0);
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
                CPoint pt(GET_X_LPARAM(pMsg->lParam),GET_Y_LPARAM(pMsg->lParam));
                if(!m_rcTarget.PtInRect(pt))
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
                    ::ClientToScreen(pMsg->hwnd,&pt);
                    SetWindowPos(0,pt.x  ,pt.y + 18,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOSENDCHANGING|SWP_NOACTIVATE);
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
        else if(!IsWindowVisible() && !m_strTip.IsEmpty())
        {
			pugi::xml_document xmlDoc;
			if(xmlDoc.load_string(m_strTip))
			{
				InitFromXml(xmlDoc.first_child());
			}else
			{
				SApplication::getSingletonPtr()->LoadXmlDocment(xmlDoc,m_strXmlLayout);
				InitFromXml(xmlDoc.first_child());
				GetRoot()->SetWindowText(m_strTip);
			}
			SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
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

	void STipCtrlEx::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		SHostWnd::OnWindowPosChanging(lpWndPos);
		//if(!(lpWndPos->flags&SWP_NOSIZE) && g_SettingsUI->bMouseFollow)
		//{
		//	CPoint pt = UpdatePosition(m_ptCaret,lpWndPos->cx,lpWndPos->cy);
		//	//auto change pos
		//	lpWndPos->x=pt.x;
		//	lpWndPos->y=pt.y;
		//	lpWndPos->flags&=~SWP_NOMOVE;
		//}
	}


	IToolTip * SToolTipFactory::CreateToolTip(HWND hHost)
	{
		STipCtrlEx *pTipCtrl = new STipCtrlEx;
		if (!pTipCtrl->CreateWnd())
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