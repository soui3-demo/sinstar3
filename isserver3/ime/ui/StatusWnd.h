#pragma once

#include "ImeWnd.h"
#include "SkinMananger.h"
#include "SToolsMgr.h"

interface ICmdListener;

namespace SOUI
{
	class CStatusWnd :
		public CImeWnd
	{
	public:
		enum {
			BTN_CHARMODE=1<<0,
			BTN_RECORD=1<<1,
			BTN_SOUND=1<<2,
			BTN_ENGLISHMODE = 1<<3,
			BTN_FILTERGBK = 1<<4,
			BTN_STATUSMODE = 1<<5,
			BTN_ALL=0xffffffff,
		};

		enum AnchorModeH{
			AMH_NULL=0,
			AMH_LEFT=1,
			AMH_RIGHT=1<<1,
		};

		enum AnchorModeV{
			AMV_NULL=0,
			AMV_TOP = 1<<8,
			AMV_BOTTOM = 1<<9,
		};

		CStatusWnd(SEventSet *pEvtSets, ICmdListener *pListener);
		~CStatusWnd(void);

		HWND Create(LPCTSTR pszWndName);
		void SetOwner(HWND hOwner);
		void UpdateCompInfo();
		void UpdateToggleStatus(DWORD flags,BOOL bInit=FALSE);
		void UpdateMode();
	protected:
		virtual void OnReposition(CPoint pt);
		virtual bool onRootResize(EventArgs *e);

		void UpdateAnchorMode();

		int OnRecreateUI(LPCREATESTRUCT lpCreateStruct);

		bool SwitchToggle(int nID, BOOL  bToggle);
	protected:
		int OnCreate(LPCREATESTRUCT lpCreateStruct);

		void OnRButtonUp(UINT nFlags,CPoint pt);

		void OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu);

		BEGIN_MSG_MAP_EX(CStatusWnd)
			MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_RBUTTONUP(OnRButtonUp)
			CHAIN_MSG_MAP(CImeWnd)
		END_MSG_MAP()

	protected:
		void OnEditUserDefData(int nType);

		void ShowServerExit();
		void OnSvrNotify(EventArgs *e);

		void OnBtnExtend();
		void OnBtnShrink();
		void OnBtnMakePhrase();
		void OnSwitchCharMode(EventArgs *e);
		void OnSwitchRecord(EventArgs *e);
		void OnSwitchSound(EventArgs *e);
		void OnSwitchEnglish(EventArgs *e);
		void OnSwitchFilterGbk(EventArgs *e);
		void OnLogoClick();
		void OnUpdateBtnTooltip(EventArgs *e);
		void OnMenuClick();
		void OnHelpClick();
		void OnQueryClick();

		EVENT_MAP_BEGIN()
			EVENT_HANDLER(EventSvrNotify::EventID, OnSvrNotify)
			EVENT_HANDLER(EventSwndUpdateTooltip::EventID, OnUpdateBtnTooltip)
			EVENT_ID_COMMAND(R.id.btn_status_shrink, OnBtnShrink)
			EVENT_ID_COMMAND(R.id.btn_status_extend, OnBtnExtend)
			EVENT_ID_COMMAND(R.id.btn_make_phrase, OnBtnMakePhrase)
			EVENT_ID_HANDLER(R.id.btn_charmode, EventCmd::EventID, OnSwitchCharMode)
			EVENT_ID_HANDLER(R.id.btn_record, EventCmd::EventID, OnSwitchRecord)
			EVENT_ID_HANDLER(R.id.btn_sound, EventCmd::EventID, OnSwitchSound)
			EVENT_ID_HANDLER(R.id.btn_english,EventCmd::EventID,OnSwitchEnglish)
			EVENT_ID_HANDLER(R.id.btn_filter_gbk, EventCmd::EventID, OnSwitchFilterGbk)
			EVENT_ID_COMMAND(R.id.img_logo, OnLogoClick)
			EVENT_ID_COMMAND(R.id.btn_menu,OnMenuClick)
			EVENT_ID_COMMAND(R.id.btn_help,OnHelpClick)
			EVENT_ID_COMMAND(R.id.btn_query,OnQueryClick)
		EVENT_MAP_END()

	protected:
		void UpdateCompInfo2(SWindow *pParent);
		void UpdateUI();
	protected:
		CSkinMananger       m_skinManager;
		SToolsMgr			m_toolManager;
		ICmdListener     *  m_pCmdListener;

		int					m_anchorMode;
		HWND				m_hOwner;
	};

}
