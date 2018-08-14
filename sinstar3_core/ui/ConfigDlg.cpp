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

	void CConfigDlg::FindAndSetHotKey(int id,DWORD accel)
	{
		SHotKeyCtrl *pCtrl = FindChildByID2<SHotKeyCtrl>(id);
			SASSERT(pCtrl);
			if (pCtrl)
				pCtrl->SetHotKey(LOWORD(accel), HIWORD(accel));
	}

	WORD Char2VKey(TCHAR wChar)
	{
		TCHAR szBuf[2] = { wChar,0 };
		return CAccelerator::VkFromString(szBuf);
	}

	WORD Vkey2Char(WORD wVK)
	{
		SStringT strName = CAccelerator::GetKeyName(wVK);
		if (strName.GetLength() > 1 || strName.IsEmpty()) return 0;
		return strName[0];
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

		FindAndSetHotKey(R.id.hk_to_sentmode, Char2VKey(g_SettingsG.bySentMode));
	}

	void CConfigDlg::InitPageHotKey()
	{
		FindAndSetHotKey(R.id.hk_switch_py, g_SettingsG.dwHotkeys[HKI_Mode]);
		FindAndSetHotKey(R.id.hk_make_phrase, g_SettingsG.dwHotkeys[HKI_MakePhrase]);
		FindAndSetHotKey(R.id.hk_show_table, g_SettingsG.dwHotkeys[HKI_ShowRoot]);
		FindAndSetHotKey(R.id.hk_show_comp, g_SettingsG.dwHotkeys[HKI_Query]);
		FindAndSetHotKey(R.id.hk_show_statusbar, g_SettingsG.dwHotkeys[HKI_HideStatus]);
		FindAndSetHotKey(R.id.hk_input_en, g_SettingsG.dwHotkeys[HKI_EnSwitch]);
		FindAndSetHotKey(R.id.hk_filter_gbk, g_SettingsG.dwHotkeys[HKI_FilterGbk]);
		FindAndSetHotKey(R.id.hk_tts, g_SettingsG.dwHotkeys[HKI_TTS]);
		FindAndSetHotKey(R.id.hk_record, g_SettingsG.dwHotkeys[HKI_Record]);
		FindAndSetHotKey(R.id.hk_to_umode, g_SettingsG.dwHotkeys[HKI_UDMode]);
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

		FindAndSetText(R.id.edit_webmode_header, S_CA2T(g_SettingsG.szWebHeader));
	}


	void CConfigDlg::InitPageCandidate()
	{
		FindAndSetCheck(R.id.chk_enable_23cand_hotkey, g_SettingsG.b23CandKey);

		FindAndSetHotKey(R.id.hk_2_cand, g_SettingsG.by2CandVK);
		FindAndSetHotKey(R.id.hk_3_cand, g_SettingsG.by3CandVK);
		
		FindAndSetHotKey(R.id.hk_turn_prev, g_SettingsG.byTurnPageUpVK);
		FindAndSetHotKey(R.id.hk_turn_next, g_SettingsG.byTurnPageDownVK);
		
		FindAndSetCheck(R.id.chk_disable_number_to_select_cand, g_SettingsG.bCandSelNoNum);
		FindAndSetCheck(R.id.chk_full_skip_simple, g_SettingsG.bOnlySimpleCode);

		FindAndSetCheck(R.id.gbk_show_only + g_SettingsG.nGbkMode, TRUE);
	}
	
	void CConfigDlg::InitPageMisc()
	{
		FindAndSetCheck(R.id.sound_disable + g_SettingsG.nSoundAlert, TRUE);

		for (int i = 0; i < 6; i++)
		{
			FindAndSetHotKey(R.id.hk_bihua_heng+i, Char2VKey(g_SettingsG.byLineKey[i]));
		}
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
		
		if (ISComm_ServerVersion() == ISACK_SUCCESS)
		{
			PMSGDATA pData = ISComm_GetData();
			BYTE byVer[4];
			memcpy(byVer, pData->byData, 4);
			SStringT strVer =  SStringT().Format(_T("%u.%u.%u.%u"), UINT(byVer[3]), UINT(byVer[2]), UINT(byVer[1]), UINT(byVer[0]));
			FindChildByID(R.id.txt_svr_ver)->SetWindowText(strVer);
		}

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
		SHotKeyCtrl * pHotKeyCtrl = sobj_cast<SHotKeyCtrl>(pEvt->sender);
		SASSERT(pHotKeyCtrl);
		DWORD dwAccel = MAKELONG(pHotKeyEvt->vKey, pHotKeyEvt->wModifiers);
		SLOG_INFO("id:" << pHotKeyCtrl->GetID() << " accel:" << CAccelerator::FormatAccelKey(dwAccel));
		switch (pHotKeyCtrl->GetID())
		{
			//hotpage
		case R.id.hk_switch_py:
			g_SettingsG.dwHotkeys[HKI_Mode] = dwAccel; break;
		case R.id.hk_make_phrase:
			g_SettingsG.dwHotkeys[HKI_MakePhrase] = dwAccel; break;
		case R.id.hk_show_table:
			g_SettingsG.dwHotkeys[HKI_ShowRoot] = dwAccel; break;
		case R.id.hk_show_comp:
			g_SettingsG.dwHotkeys[HKI_Query] = dwAccel; break;
		case R.id.hk_show_statusbar:
			g_SettingsG.dwHotkeys[HKI_HideStatus] = dwAccel; break;
		case R.id.hk_input_en:
			g_SettingsG.dwHotkeys[HKI_EnSwitch] = dwAccel; break;	
		case R.id.hk_filter_gbk:
			g_SettingsG.dwHotkeys[HKI_FilterGbk] = dwAccel; break;
		case R.id.hk_tts:
			g_SettingsG.dwHotkeys[HKI_TTS] = dwAccel; break;
		case R.id.hk_record:
			g_SettingsG.dwHotkeys[HKI_Record] = dwAccel; break;
		case R.id.hk_to_umode:
			g_SettingsG.dwHotkeys[HKI_UDMode] = dwAccel;
		case R.id.hk_2_cand:
			g_SettingsG.by2CandVK = pHotKeyEvt->vKey; break;
		case R.id.hk_3_cand:
			g_SettingsG.by3CandVK = pHotKeyEvt->vKey; break;
		case R.id.hk_turn_prev:
			g_SettingsG.byTurnPageUpVK = pHotKeyEvt->vKey; break;
		case R.id.hk_turn_next:
			g_SettingsG.byTurnPageDownVK = pHotKeyEvt->vKey; break;
		case R.id.hk_bihua_heng:
		case R.id.hk_bihua_shu:
		case R.id.hk_bihua_pie:
		case R.id.hk_bihua_na:
		case R.id.hk_bihua_zhe:
		case R.id.hk_bihua_wild:
			g_SettingsG.byLineKey[pHotKeyCtrl->GetID() - R.id.hk_bihua_heng] = Vkey2Char(pHotKeyEvt->vKey);
			break;
			break;
		case R.id.hk_to_sentmode:
			g_SettingsG.bySentMode = Vkey2Char(pHotKeyEvt->vKey);
			break;
		}
	}

	void CConfigDlg::OnClose()
	{
		DestroyWindow();
	}

	void CConfigDlg::OnWebHeaderNotify(EventArgs * e)
	{
		EventRENotify *e2 = sobj_cast<EventRENotify>(e);
		SASSERT(e2);
		if (e2->iNotify == EN_CHANGE)
		{
			SWindow *pEdit = sobj_cast<SWindow>(e2->sender);
			SStringT str = pEdit->GetWindowText();
			SStringA strA = S_CT2A(str);
			if (strA.GetLength() < 100)
			{
				strcpy(g_SettingsG.szWebHeader, strA);
			}
			
		}
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
