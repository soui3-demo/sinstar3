#pragma once

#include "interface/stooltip-i.h"
#include "core/snativewnd.h"

namespace SOUI
{
    class STipCtrlEx : public IToolTip
                   , public SHostWnd
    {

    public:
        STipCtrlEx(void);
        virtual ~STipCtrlEx(void);

        BOOL CreateWnd();
    public://IToolTip
        /*virtual*/ void RelayEvent(const MSG *pMsg);

        /*virtual*/ void UpdateTip(const TIPID &id, CRect rc,LPCTSTR pszTip,int nScale);

        /*virtual*/ void ClearTip();

        /*virtual*/ BOOL PreTranslateMessage(MSG* pMsg);
    protected:
        virtual void OnFinalMessage(HWND hWnd);
		virtual IToolTip * CreateTooltip() const;
		virtual void DestroyTooltip(IToolTip * pTooltip) const;
	protected:
        void OnTimer(UINT_PTR idEvent);
        void ShowTip(BOOL bShow);
		CPoint AdjustTipPos(CPoint pt) const;

        BEGIN_MSG_MAP_EX(STipCtrlEx)
			MSG_WM_CREATE(OnCreate)
            MSG_WM_TIMER(OnTimer)
			CHAIN_MSG_MAP(SHostWnd)
        END_MSG_MAP()

    protected:
		DWORD		   m_Tick;
        int            m_nDelay;
        int            m_nShowSpan;
        SStringT       m_strTip;
        int            m_nScale;
        CRect          m_rcTarget;
        TIPID          m_id;
		BOOL		   m_bUpdated;
		CPoint		   m_ptTip;
    };

	class SToolTipFactory : public TObjRefImpl<IToolTipFactory>
	{
	public:
		IToolTip * CreateToolTip(HWND hHost);

		void DestroyToolTip(IToolTip *pToolTip);
	};
}

