#include "StdAfx.h"
#include "ConfigDlg.h"
#include "../../include/version.h"
#include <helper/STime.h>

#pragma warning(disable:4244)
namespace SOUI
{
	CConfigDlg::CConfigDlg(SEventSet *pEvtSet)
		:CSkinAwareWnd(pEvtSet,UIRES.LAYOUT.dlg_config)
	{
	}

	CConfigDlg::~CConfigDlg(void)
	{
	}

	void CConfigDlg::FindAndSetCheck(int id,BOOL bcheck)
	{
		SWindow *pCtrl = FindChildByID(id);
			SASSERT(pCtrl);
			if (pCtrl)
				pCtrl->SetCheck(bcheck);
	}

	void CConfigDlg::FindAndSetText(int id,LPCTSTR text)
	{
		SWindow *pCtrl = FindChildByID(id);
			SASSERT(pCtrl); 
			if (pCtrl)
				pCtrl->SetWindowText(text);
	}

	void CConfigDlg::FindAndSetHotKey(int id,WORD vk_key, WORD modif)
	{
		SHotKeyCtrl *pCtrl = FindChildByID2<SHotKeyCtrl>(id);
			SASSERT(pCtrl);
			if (pCtrl)
				pCtrl->SetHotKey(vk_key, modif);
	}

	void CConfigDlg::InitPageHabit()
	{
		int CtrlId;
		CtrlId = 102;
		//输入法开关
		if (g_SettingsG.bySwitchKey == 0x36)
			CtrlId = 100;
		else if (g_SettingsG.bySwitchKey == 0x2a)
			CtrlId = 101;
		FindAndSetCheck(CtrlId,TRUE);

		//形码状态回车
		CtrlId = g_SettingsG.bEnterClear ? 110 : 111;
		FindAndSetCheck(CtrlId, TRUE);
		//形码状态快捷模式		
		FindAndSetCheck(R.id.key_to_umode, g_SettingsG.bFastUMode);
		//临时拼音开关
		CtrlId = 122;
		switch (g_SettingsG.byTempSpellKey)
		{
		case 0xc0:CtrlId = 120; break;
		case 0xc1:CtrlId = 121; break;
		default:g_SettingsG.byTempSpellKey = 0; break;
		}
		FindAndSetCheck(CtrlId, TRUE);
		//重码自动上屏
		FindAndSetCheck(R.id.cand_auto_input, g_SettingsG.bAutoInput);
		//拼音重码词组优先
		FindAndSetCheck(R.id.cand_py_phrase_first, g_SettingsG.bPYPhraseFirst);
		//op tip
		FindAndSetCheck(R.id.chk_show_op_tip, g_SettingsG.bShowOpTip);
	}

	void CConfigDlg::InitPageHotKey()
	{
		FindAndSetHotKey(R.id.hk_switch_py, g_SettingsG.byHotKeyMode, MOD_CONTROL);
		FindAndSetHotKey(R.id.hk_make_phrase, g_SettingsG.byHotKeyMakeWord, MOD_CONTROL);
		FindAndSetHotKey(R.id.hk_show_table, g_SettingsG.byHotKeyShowRoot, MOD_CONTROL);
		FindAndSetHotKey(R.id.hk_show_comp, g_SettingsG.byHotKeyQuery, MOD_CONTROL);
		FindAndSetHotKey(R.id.hk_show_statusbar, g_SettingsG.byHotKeyHideStatus, MOD_CONTROL);
		FindAndSetHotKey(R.id.hk_input_en, g_SettingsG.byHotKeyEn, MOD_CONTROL);
	}

	void CConfigDlg::InitPageAssociate()
	{
		int CtrlId;
		//联想
		CtrlId = R.id.ass_mode_none +g_SettingsG.byAstMode;
		FindAndSetCheck(CtrlId, TRUE);
		CtrlId= R.id.rate_adjust_disable +g_SettingsG.byRateAdjust;
		FindAndSetCheck(CtrlId, TRUE);
		CtrlId = 600;
		if (g_SettingsG.byForecast == MQC_FORECAST)
			CtrlId = 601;
		else if (g_SettingsG.byForecast == MQC_FCNOCAND)
			CtrlId = 602;
		FindAndSetCheck(CtrlId, TRUE);

		FindAndSetCheck(R.id.chk_auto_comp_promp, g_SettingsG.bAutoPrompt);
		FindAndSetCheck(R.id.chk_auto_dot, g_SettingsG.bAutoDot);
		FindAndSetCheck(R.id.chk_auto_select_cand, g_SettingsG.bAutoMatch);
		FindAndSetCheck(R.id.chk_sent_input, g_SettingsG.bAstSent);
	}


	void CConfigDlg::InitPageCandidate()
	{
		FindAndSetCheck(R.id.chk_enable_23cand_hotkey, g_SettingsG.b23CandKey);

		FindAndSetHotKey(R.id.hk_2_cand, g_SettingsG.by2CandVK, g_SettingsG.by2CandVK >= 0x30 ? 0 : MOD_WIN);
		FindAndSetHotKey(R.id.hk_3_cand, g_SettingsG.by3CandVK, g_SettingsG.by3CandVK >= 0x30 ? 0 : MOD_WIN);
		
		FindAndSetHotKey(R.id.hk_turn_prev, g_SettingsG.byTurnPageUpVK, g_SettingsG.byTurnPageUpVK >= 0x30 ? 0 : MOD_WIN);
		FindAndSetHotKey(R.id.hk_turn_next, g_SettingsG.byTurnPageDownVK, g_SettingsG.byTurnPageDownVK >= 0x30 ? 0 : MOD_WIN);
		
		FindAndSetCheck(R.id.chk_disable_number_to_select_cand, g_SettingsG.bCandSelNoNum);
		FindAndSetCheck(R.id.chk_full_skip_simple, g_SettingsG.bOnlySimpleCode);

		FindAndSetCheck(R.id.gbk_show_only + g_SettingsG.nGbkMode, TRUE);
	}
	
	void CConfigDlg::InitPageMisc()
	{
		FindAndSetCheck(R.id.sound_disable + g_SettingsG.nSoundAlert, TRUE);
	}

	void CConfigDlg::InitPageAbout()
	{
		FindChildByID(R.id.txt_ver)->SetWindowText(VERSION_TSTR);
		WIN32_FIND_DATA wfd;
		TCHAR szPath[1000];
		GetModuleFileName(theModule->GetModule(), szPath, 1000);
		HANDLE h=FindFirstFile(szPath, &wfd);
		FindClose(h);
		FILETIME lft;
		FileTimeToLocalFileTime(&wfd.ftLastWriteTime,&lft);
		SYSTEMTIME tm;
		FileTimeToSystemTime(&lft, &tm);
		CTime time(tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);
		SStringT strTm = time.Format(_T("%Y-%m-%d %H:%M:%S %A"));
		FindChildByID(R.id.txt_build_time)->SetWindowText(strTm);
		
	}

	void CConfigDlg::InitPages()
	{		
		InitPageHabit();
		InitPageHotKey();
		InitPageAssociate();	
		InitPageCandidate();
		InitPageMisc();
		InitPageAbout();
	}

#define GetGroupCheck(id) int CheckId=0;\
SWindow *pCtrl = FindChildByID(id);\
	SASSERT(pCtrl);\
	pCtrl=pCtrl->GetSelectedSiblingInGroup();\
	SASSERT(pCtrl);\
	CheckId=pCtrl->GetID()

	void CConfigDlg::OnClickInputSwitch(int id)
	{
		GetGroupCheck(id);
		switch (CheckId)
		{
		case 100:
			g_SettingsG.bySwitchKey = 0x36;
			break;
		case 101:
			g_SettingsG.bySwitchKey = 0x2a;
			break;
		default:g_SettingsG.bySwitchKey = 0; break;
		}
	}

	void CConfigDlg::OnClickEnter(int id)
	{
		GetGroupCheck(id);
		switch (CheckId)
		{
		case 110:
			g_SettingsG.bEnterClear=TRUE;
			break;
		case 111:
			g_SettingsG.bEnterClear = FALSE;
			break;
		default:
			g_SettingsG.bEnterClear = FALSE;
			break;
		}
	}

	void CConfigDlg::OnClickPYTemp(int id)
	{
		GetGroupCheck(id);
		switch (CheckId)
		{
		case 120:
			g_SettingsG.byTempSpellKey = 0xC0;
			break;
		case 121:
			g_SettingsG.byTempSpellKey = 0xC1;
			break;
		default:
			g_SettingsG.byTempSpellKey = FALSE;
			break;
		}
	}

	void CConfigDlg::OnClickAlertMode(int id)
	{
		GetGroupCheck(id);
		switch (CheckId)
		{
		case R.id.sound_disable:
			g_SettingsG.nSoundAlert = 0;
			break;
		case R.id.sound_wave:
			g_SettingsG.nSoundAlert = 1;
			break;
		case R.id.sound_beep:
		default:
			g_SettingsG.nSoundAlert = 2;
			break;
		}
	}

	void CConfigDlg::OnAutoInput()
	{
		g_SettingsG.bAutoInput=FindChildByID(R.id.cand_auto_input)->IsChecked();
	}

	void CConfigDlg::OnPyPhraseFirst()
	{
		g_SettingsG.bPYPhraseFirst= FindChildByID(R.id.cand_py_phrase_first)->IsChecked();
	}

	void CConfigDlg::OnChkOpTip(EventArgs *e)
	{
		SCheckBox *pCheck = sobj_cast<SCheckBox>(e->sender);
		SASSERT(pCheck);
		g_SettingsG.bShowOpTip = pCheck->IsChecked();
	}

	void CConfigDlg::OnClickAssMode(int id)
	{
		GetGroupCheck(id);
		g_SettingsG.byAstMode = CheckId - 400;		
	}

	void CConfigDlg::OnClickForcast(int id)
	{
		GetGroupCheck(id);

		switch (CheckId)
		{
		case 600:
			g_SettingsG.byForecast = 0;
			break;
		case 601:
			g_SettingsG.byForecast = MQC_FORECAST;
			break;
		case 602:
			g_SettingsG.byForecast = MQC_FCNOCAND;
		}
	}

	void CConfigDlg::OnClickGBK(int id)
	{
		GetGroupCheck(id);

		switch (CheckId)
		{
		case 700:
			g_SettingsG.nGbkMode = CSettingsGlobal::GbkMode::GBK_HIDE;
			break;
		case 701:
			g_SettingsG.nGbkMode = CSettingsGlobal::GbkMode::GBK_SHOW_MANUAL;
			break;
		case 702:
			g_SettingsG.nGbkMode = CSettingsGlobal::GbkMode::GBK_SHOW_NORMAL;
		}
	}

	void CConfigDlg::OnChkFullSkipSimple()
	{
		g_SettingsG.bOnlySimpleCode = FindChildByID(R.id.chk_full_skip_simple)->IsChecked();
	}

	void CConfigDlg::OnClickRateAdjust(int id)
	{
		GetGroupCheck(id);
		g_SettingsG.byRateAdjust = CheckId - 500;
	}
	void CConfigDlg::OnClickAutoCompPromp()
	{
		g_SettingsG.bAutoPrompt= FindChildByID(R.id.chk_auto_comp_promp)->IsChecked();	
	}
	void CConfigDlg::OnClickAutoDot()
	{
		g_SettingsG.bAutoDot=FindChildByID(R.id.chk_auto_dot)->IsChecked();
	}
	void CConfigDlg::OnClickAutoSelectCand()
	{
		g_SettingsG.bAutoMatch = FindChildByID(R.id.chk_auto_select_cand)->IsChecked();
	}
	void CConfigDlg::OnClickSendInput()
	{
		g_SettingsG.bAstSent = FindChildByID(R.id.chk_sent_input)->IsChecked();
	}
	void CConfigDlg::OnDisableNumSelCand()
	{
		g_SettingsG.bCandSelNoNum= FindChildByID(R.id.chk_disable_number_to_select_cand)->IsChecked();
	}
	void CConfigDlg::OnEnable23Cand()
	{
		g_SettingsG.b23CandKey = FindChildByID(R.id.chk_enable_23cand_hotkey)->IsChecked();
	}
	void CConfigDlg::OnHotKeyEvent(EventArgs * pEvt)
	{
		EventSetHotKey *pHotKeyEvt = sobj_cast<EventSetHotKey>(pEvt);
		switch (pEvt->GetID())
		{
			//hotpage
		case R.id.hk_switch_py:
			g_SettingsG.byHotKeyMode = pHotKeyEvt->vKey; break;
		case R.id.hk_make_phrase:
			g_SettingsG.byHotKeyMakeWord = pHotKeyEvt->vKey; break;
		case R.id.hk_show_table:
			g_SettingsG.byHotKeyShowRoot = pHotKeyEvt->vKey; break;
		case R.id.hk_show_comp:
			g_SettingsG.byHotKeyQuery = pHotKeyEvt->vKey; break;
		case R.id.hk_show_statusbar:
			g_SettingsG.byHotKeyHideStatus = pHotKeyEvt->vKey; break;
		case R.id.hk_input_en:
			g_SettingsG.byHotKeyEn = pHotKeyEvt->vKey; break;	
			//other
		case R.id.hk_2_cand:
			g_SettingsG.by2CandVK = pHotKeyEvt->vKey; break;
		case R.id.hk_3_cand:
			g_SettingsG.by3CandVK = pHotKeyEvt->vKey; break;
		case R.id.hk_turn_prev:
			g_SettingsG.byTurnPageUpVK = pHotKeyEvt->vKey; break;
		case R.id.hk_turn_next:
			g_SettingsG.byTurnPageDownVK = pHotKeyEvt->vKey; break;
		}
	}

	void CConfigDlg::OnClose()
	{
		DestroyWindow();
	}

	int CConfigDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if (nRet != 0) return nRet;

		ImmAssociateContext(m_hWnd, (HIMC)NULL);
		InitPages();
		return 0;
	}
}
