#include "StdAfx.h"
#include "ConfigDlg.h"
#include <helper/STime.h>
#include <helper/SAdapterBase.h>
#include "../include/cf_helper.hpp"
#include "../include/filehelper.h"
#include "../iscomm/isProtocol.h"
#include "../helper/helper.h"
#include <string>
#include <shellapi.h>
#pragma comment(lib,"version.lib")
#include "AddBlurDlg.h"
#include "souidlgs.h"
#include "../IsSvrProxy.h"

#pragma warning(disable:4244)
namespace SOUI
{
	SStringT Ver2String(WORD wVer[4])
	{
		return SStringT().Format(_T("%u.%u.%u.%u"), wVer[0],wVer[1],wVer[2],wVer[3]);
	}

	class CGroupAdapter : public SMcAdapterBase
	{
	public:
		struct GroupInfo
		{
			int bEnable;
			SStringT strName;
			DWORD dwCount;
			SStringT strEditor;
			SStringT strRemark;
		};

		void AddGroup(const GroupInfo & gi)
		{
			m_arrGroupInfo.Add(gi);
		}

		void RemoveAll()
		{
			m_arrGroupInfo.RemoveAll();
		}
	protected:

		virtual int getCount()
		{
			return m_arrGroupInfo.GetCount();
		}

		virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
		{
			if (pItem->GetChildrenCount() == 0)
			{
				pItem->InitFromXml(xmlTemplate);
			}
			SCheckBox *pCheck = pItem->FindChildByID2<SCheckBox>(R.id.chk_group_name);
			pCheck->SetUserData(position);
			pCheck->SetWindowText(m_arrGroupInfo[position].strName);
			subscribeCheckEvent(pCheck);
			pCheck->GetEventSet()->setMutedState(true);
			pCheck->SetAttribute(L"checked", m_arrGroupInfo[position].bEnable?L"1":L"0");
			pCheck->GetEventSet()->setMutedState(false);

			pItem->FindChildByID(R.id.txt_group_size)->SetWindowText(SStringT().Format(_T("%d"), m_arrGroupInfo[position].dwCount));
			pItem->FindChildByID(R.id.txt_editor)->SetWindowText(m_arrGroupInfo[position].strEditor);
			pItem->FindChildByID(R.id.txt_remark)->SetWindowText(m_arrGroupInfo[position].strRemark);
		}

		virtual SStringW GetColumnName(int iCol) const {
			const wchar_t * pszColNames[] = {
				L"col_group",L"col_size",L"col_editor",L"col_remark"
			};
			return pszColNames[iCol];
		}

		virtual void subscribeCheckEvent(SCheckBox *pCheck)
		{
		}
	protected:
		SArray<GroupInfo> m_arrGroupInfo;
	};

	class CPhraseGroupAdapter: public CGroupAdapter
	{

	protected:
		virtual void subscribeCheckEvent(SCheckBox *pCheck)
		{
			pCheck->GetEventSet()->subscribeEvent(EventSwndStateChanged::EventID, Subscriber(&CPhraseGroupAdapter::OnGroupEnableChanged, this));

		}

		bool OnGroupEnableChanged(EventArgs *e)
		{
			EventSwndStateChanged *e2 = sobj_cast<EventSwndStateChanged>(e);
			if (e2->CheckState(WndState_Check))
			{
				SWindow *pSender = sobj_cast<SWindow>(e->sender);
				int idx = pSender->GetUserData();
				BOOL bEnable = (e2->dwNewState & WndState_Check)?1:0;
				if(ISACK_SUCCESS==CIsSvrProxy::GetSvrCore()->ReqEnablePhraseGroup(m_arrGroupInfo[idx].strName,bEnable))
				{
					m_arrGroupInfo[idx].bEnable=bEnable;
				}
			}
			return false;
		}
	};

	class CCelibGroupAdapter: public CGroupAdapter
	{

	protected:
		virtual void subscribeCheckEvent(SCheckBox *pCheck)
		{
			pCheck->GetEventSet()->subscribeEvent(EventSwndStateChanged::EventID, Subscriber(&CCelibGroupAdapter::OnGroupEnableChanged, this));

		}

		bool OnGroupEnableChanged(EventArgs *e)
		{
			EventSwndStateChanged *e2 = sobj_cast<EventSwndStateChanged>(e);
			if (e2->CheckState(WndState_Check))
			{
				SWindow *pSender = sobj_cast<SWindow>(e->sender);
				int idx = pSender->GetUserData();
				BOOL bEnable = (e2->dwNewState & WndState_Check)?1:0;
				if(ISACK_SUCCESS == CIsSvrProxy::GetSvrCore()->ReqFlmEnableGroup(m_arrGroupInfo[idx].strName,bEnable))
					m_arrGroupInfo[idx].bEnable = bEnable;
			}
			return false;
		}
	};

	class CBlurListAdapter : public SAdapterBase
	{
		struct INDEXINFO
		{
			int iGroup;
			int iIndex;
		};

		struct BLUREQUILTEX : BLUREQUILT
		{
			SStringT strBlur;
		};
	public:
		enum BLURTYPE {
			BLUR_TUNE = 0,
			BLUR_RHYME,
			BLUR_FULL,
		};

		void RemoveAll()
		{
			for (int i = 0; i < 3; i++)
			{
				m_lstBlur[i].RemoveAll();
			}
		}

		void AddBlur(BLURTYPE bt, LPCSTR pszFrom,LPCSTR pszTo)
		{
			BLUREQUILTEX br;
			strcpy_s(br.szFrom, 7, pszFrom);
			strcpy_s(br.szTo, 7, pszTo);
			br.strBlur = S_CA2T(SStringA().Format("%s=%s", pszFrom, pszTo));
			m_lstBlur[bt].Add(br);
		}
		
		void update()
		{
			m_lstIndex.RemoveAll();
			for (int i = 0; i < 3; i++)
			{
				INDEXINFO ii = { i,-1 };
				m_lstIndex.Add(ii);
				for (UINT j = 0; j < m_lstBlur[i].GetCount(); j++)
				{
					ii.iIndex = j;
					m_lstIndex.Add(ii);
				}
			}
			notifyDataSetChanged();
		}

		bool getBlur(int iItem, char szFrom[7], char szTo[7])
		{
			INDEXINFO ii = position2IndexInfo(iItem);
			if (ii.iIndex == -1) return false;
			strcpy(szFrom, m_lstBlur[ii.iGroup][ii.iIndex].szFrom);
			strcpy(szTo, m_lstBlur[ii.iGroup][ii.iIndex].szTo);
			return true;
		}
	protected:
		INDEXINFO position2IndexInfo(int position)
		{
			return m_lstIndex[position];
		}

		void getGroupView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
		{
			if (pItem->GetChildrenCount() == 0)
			{
				pItem->InitFromXml(xmlTemplate.child(L"group"));
			}
			const TCHAR * KGroupName[3] = {
				_T("声母模糊"),
				_T("韵母模糊"),
				_T("全拼模糊"),
			};
			INDEXINFO ii = position2IndexInfo(position);
			pItem->FindChildByID(R.id.txt_blur_group)->SetWindowText(KGroupName[ii.iGroup]);
		}

		void getItemView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
		{
			if (pItem->GetChildrenCount() == 0)
			{
				pItem->InitFromXml(xmlTemplate.child(L"item"));
			}
			INDEXINFO ii = position2IndexInfo(position);
			pItem->FindChildByID(R.id.txt_blur_info)->SetWindowText(m_lstBlur[ii.iGroup][ii.iIndex].strBlur);
		}

		virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate) override
		{
			int viewType = getItemViewType(position, 0);
			if (viewType == 0)
			{
				getGroupView(position, pItem, xmlTemplate);
			}
			else
			{
				getItemView(position, pItem, xmlTemplate);
			}
		}

		virtual int getCount() override
		{
			return (int)m_lstIndex.GetCount();
		}

		virtual int getViewTypeCount()
		{
			return 2;
		}
		
		virtual int getItemViewType(int position, DWORD dwState)
		{
			INDEXINFO ii = m_lstIndex[position];
			return ii.iIndex == -1 ? 0 : 1;
		}
		
	private:
		SArray<BLUREQUILTEX> m_lstBlur[3];
		SArray<INDEXINFO> m_lstIndex;
	};

	CConfigDlg::CConfigDlg(IUpdateIntervalObserver *pObserver)
		:SHostWnd(UIRES.LAYOUT.dlg_config)
		,m_pObserver(pObserver)
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

	void CConfigDlg::FindAndSetSpin(int id, int nValue)
	{
		SSpinButtonCtrl *pCtrl = FindChildByID2<SSpinButtonCtrl>(id);
		SASSERT(pCtrl);
		if (pCtrl)
		{
			SWindow *pBuddy = pCtrl->GetBuddy();
			if(pBuddy) pBuddy->GetEventSet()->setMutedState(true);
			pCtrl->GetEventSet()->setMutedState(true);
			pCtrl->SetValue(nValue);
			pCtrl->GetEventSet()->setMutedState(false);
			if (pBuddy) pBuddy->GetEventSet()->setMutedState(false);
		}
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
		return SAccelerator::VkFromString(szBuf);
	}

	WORD Vkey2Char(WORD wVK)
	{
		SStringT strName = SAccelerator::GetKeyName(wVK);
		if (strName.GetLength() > 1 || strName.IsEmpty()) return 0;
		return strName[0];
	}

	void CConfigDlg::InitPageHabit()
	{
		int CtrlId;
		CtrlId = R.id.ime_switch_disabled;
		//输入法开关
		if (g_SettingsG->bySwitchKey == 0x2a)
			CtrlId = R.id.ime_switch_left_shift;
		else if (g_SettingsG->bySwitchKey == 0x36)
			CtrlId = R.id.ime_switch_right_shift;
		FindAndSetCheck(CtrlId,TRUE);

		//形码状态回车
		CtrlId = g_SettingsG->bEnterClear ? R.id.enter_for_clear : R.id.enter_for_input;
		FindAndSetCheck(CtrlId, TRUE);
		//临时拼音开关
		CtrlId = R.id.py_switch_disabled;
		switch (g_SettingsG->byTempSpellKey)
		{
		case 0xc0:CtrlId = R.id.py_switch_left_ctrl; break;
		case 0xc1:CtrlId = R.id.py_switch_right_ctrl; break;
		default:g_SettingsG->byTempSpellKey = 0; break;
		}
		FindAndSetCheck(CtrlId, TRUE);
		//重码自动上屏
		FindAndSetCheck(R.id.cand_auto_input, g_SettingsG->bAutoInput);
		//拼音重码词组优先
		FindAndSetCheck(R.id.cand_py_phrase_first, g_SettingsG->bPYPhraseFirst);
		//op tip
		FindAndSetCheck(R.id.chk_show_op_tip, g_SettingsG->bShowOpTip);

		FindAndSetCheck(R.id.chk_disable_first_wild,g_SettingsG->bDisableFirstWild);

		FindAndSetCheck(R.id.chk_full_space,g_SettingsG->bFullSpace);
		
		FindAndSetHotKey(R.id.hk_to_sentmode, Char2VKey(g_SettingsG->bySentMode));

		FindAndSetSpin(R.id.spin_delay_time, g_SettingsG->nDelayTime);
		
		FindAndSetSpin(R.id.spin_cand_num,g_SettingsG->nMaxCands);

		SStringT strFontDesc = g_SettingsG->strFontDesc;
		if(strFontDesc.IsEmpty())
			strFontDesc=_T("<皮肤默认>");
		FindAndSetText(R.id.edit_font,strFontDesc);
	}

	void CConfigDlg::InitPageHotKey()
	{
		FindAndSetHotKey(R.id.hk_switch_py, g_SettingsG->dwHotkeys[HKI_Mode]);
		FindAndSetHotKey(R.id.hk_make_phrase, g_SettingsG->dwHotkeys[HKI_MakePhrase]);
		FindAndSetHotKey(R.id.hk_show_table, g_SettingsG->dwHotkeys[HKI_ShowRoot]);
		FindAndSetHotKey(R.id.hk_show_comp, g_SettingsG->dwHotkeys[HKI_Query]);
		FindAndSetHotKey(R.id.hk_show_statusbar, g_SettingsG->dwHotkeys[HKI_HideStatus]);
		FindAndSetHotKey(R.id.hk_input_en, g_SettingsG->dwHotkeys[HKI_EnSwitch]);
		FindAndSetHotKey(R.id.hk_filter_gbk, g_SettingsG->dwHotkeys[HKI_FilterGbk]);
		FindAndSetHotKey(R.id.hk_tts, g_SettingsG->dwHotkeys[HKI_TTS]);
		FindAndSetHotKey(R.id.hk_record, g_SettingsG->dwHotkeys[HKI_Record]);
		FindAndSetHotKey(R.id.hk_to_umode, g_SettingsG->dwHotkeys[HKI_UDMode]);
		FindAndSetHotKey(R.id.hk_switch_tempspell,g_SettingsG->dwHotkeys[HKI_TempSpell]);
	}

	void CConfigDlg::InitPageAssociate()
	{
		int CtrlId;
		//联想
		CtrlId = R.id.ass_mode_none +g_SettingsG->byAstMode;
		FindAndSetCheck(CtrlId, TRUE);
		CtrlId= R.id.rate_adjust_disable +g_SettingsG->byRateAdjust;
		FindAndSetCheck(CtrlId, TRUE);
		CtrlId = 600;
		if (g_SettingsG->byForecast == MQC_FORECAST)
			CtrlId = 601;
		else if (g_SettingsG->byForecast == MQC_FCNOCAND)
			CtrlId = 602;
		FindAndSetCheck(CtrlId, TRUE);

		FindAndSetCheck(R.id.chk_auto_comp_promp, g_SettingsG->bAutoPrompt);
		FindAndSetCheck(R.id.chk_auto_dot, g_SettingsG->bAutoDot);
		FindAndSetCheck(R.id.chk_auto_select_cand, g_SettingsG->bAutoMatch);

		FindAndSetText(R.id.edit_webmode_header, g_SettingsG->szWebHeader);
		
		int nSentMax = CIsSvrProxy::GetSvrCore()->GetSentRecordMax();
		FindAndSetText(R.id.edit_sent_record_max, SStringT().Format(_T("%d"), nSentMax));
		int nPredictLength = CIsSvrProxy::GetSvrCore()->GetMaxPhrasePreictLength();
		FindAndSetSpin(R.id.spin_predict_phrase_maxlength, nPredictLength);

		int nDeepness = CIsSvrProxy::GetSvrCore()->GetMaxPhraseAstDeepness();
		FindAndSetSpin(R.id.spin_phrase_ast_deepness_max, nDeepness);
	}


	void CConfigDlg::InitPageCandidate()
	{
		FindAndSetCheck(R.id.chk_enable_23cand_hotkey, g_SettingsG->b23CandKey);

		FindAndSetHotKey(R.id.hk_2_cand, g_SettingsG->by2CandVK);
		FindAndSetHotKey(R.id.hk_3_cand, g_SettingsG->by3CandVK);
		
		FindAndSetHotKey(R.id.hk_turn_prev, g_SettingsG->byTurnPageUpVK);
		FindAndSetHotKey(R.id.hk_turn_next, g_SettingsG->byTurnPageDownVK);
		
		FindAndSetCheck(R.id.chk_disable_number_to_select_cand, g_SettingsG->bCandSelNoNum);
		FindAndSetCheck(R.id.chk_full_skip_simple, g_SettingsG->bOnlySimpleCode);

		FindAndSetCheck(R.id.gbk_show_only + g_SettingsG->nGbkMode, TRUE);
	}
	
	void CConfigDlg::InitPageMisc()
	{
		FindAndSetCheck(R.id.sound_disable + g_SettingsG->nSoundAlert, TRUE);

		for (int i = 0; i < 6; i++)
		{
			FindAndSetHotKey(R.id.hk_bihua_heng+i, Char2VKey(g_SettingsG->byLineKey[i]));
		}
	}

	void CConfigDlg::InitPageAbout()
	{
		TCHAR szPath[1000];
		GetModuleFileName(NULL, szPath, 1000);

		WIN32_FIND_DATA wfd;
		HANDLE h=FindFirstFile(szPath, &wfd);
		FindClose(h);
		FILETIME lft;
		FileTimeToLocalFileTime(&wfd.ftLastWriteTime,&lft);
		SYSTEMTIME tm;
		FileTimeToSystemTime(&lft, &tm);
		STime time(tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);
		SStringT strTm = time.Format(_T("%Y-%m-%d %H:%M:%S %A"));
		FindChildByID(R.id.txt_build_time)->SetWindowText(strTm);
		
		TCHAR szExe[MAX_PATH];
		GetModuleFileName(NULL,szExe,MAX_PATH);
		WORD ver[4];
		SDpiHelper::PEVersion(szExe,ver[0],ver[1],ver[2],ver[3]);
		SStringT strVer = Ver2String(ver);
		FindChildByID(R.id.txt_svr_ver)->SetWindowText(strVer);

		SWindow *pCheck = FindChildByID(R.id.chk_auto_update);
		int nCheckUpdateInterval = m_pObserver->GetUpdateInterval();
		if(nCheckUpdateInterval != 0)
		{
			pCheck->GetEventSet()->setMutedState(true);
			pCheck->SetCheck(TRUE);
			pCheck->GetEventSet()->setMutedState(false);
			int intervals[] = {0,7,30,90 };
			if(nCheckUpdateInterval>90)
				nCheckUpdateInterval = 90;
			SComboBox *pCbxInterval = FindChildByID2<SComboBox>(R.id.cbx_update_interval);
			int idx = -1;
			for(int i=0;i<ARRAYSIZE(intervals)-1;i++)
			{
				if(nCheckUpdateInterval>intervals[i] && nCheckUpdateInterval<=intervals[i+1])
					idx = i;
			}
			pCbxInterval->SetCurSel(idx);
		}
	}

	void CConfigDlg::InitTtsTokenInfo(bool bChVoice, SComboBox *pCbx)
	{
		int nTokens= CIsSvrProxy::GetInstance()->TtsGetTokensInfo(bChVoice,NULL,0);
		if (nTokens)
		{
			WCHAR (*szToken)[MAX_TOKEN_NAME_LENGHT] = new WCHAR[nTokens][MAX_TOKEN_NAME_LENGHT];
			CIsSvrProxy::GetInstance()->TtsGetTokensInfo(bChVoice,szToken,nTokens);

			for (UINT i = 0; i < nTokens; i++)
			{
				pCbx->InsertItem(-1, S_CW2T(szToken[i]), 0, 0);
			}
			int iSel=CIsSvrProxy::GetInstance()->TtsGetVoice(bChVoice);
		
			if (iSel>=0 && iSel<nTokens)
			{
				pCbx->GetEventSet()->setMutedState(true);
				pCbx->SetCurSel(iSel);
				pCbx->GetEventSet()->setMutedState(false);
			}
			delete []szToken;
		}
	}

	void CConfigDlg::InitPageTTS()
	{
		InitTtsTokenInfo(false, FindChildByID2<SComboBox>(R.id.cbx_tts_en_token));
		InitTtsTokenInfo(true, FindChildByID2<SComboBox>(R.id.cbx_tts_ch_token));
		int nTtsSpeed = CIsSvrProxy::GetInstance()->TtsGetSpeed();
		FindChildByID(R.id.txt_tts_speed)->SetWindowText(SStringT().Format(_T("%d"), nTtsSpeed));
		FindChildByID2<SSliderBar>(R.id.slider_tts_speed)->SetValue(nTtsSpeed);
	}

	void CConfigDlg::InitPinyinBlur(COMFILE & cf, CBlurListAdapter * pBlurAdapter, int iGroup)
	{
		int nCount;
		CF_Read(&cf, &nCount, sizeof(int));
		for (int i = 0; i < nCount; i++)
		{
			char szPY1[7], szPY2[7];
			CF_ReadString(&cf, szPY1, 7);
			CF_ReadString(&cf, szPY2, 7);
			pBlurAdapter->AddBlur((CBlurListAdapter::BLURTYPE)iGroup,szPY1,szPY2);
		}
	}

	void CConfigDlg::InitPinyinBlurListView(SListView *pLvBLur)
	{
		CBlurListAdapter *pAdapter = (CBlurListAdapter*)pLvBLur->GetAdapter();
		pAdapter->RemoveAll();
		
		if (CIsSvrProxy::GetSvrCore()->ReqBlurQuery() == ISACK_SUCCESS)
		{
			PMSGDATA pMsgData = CIsSvrProxy::GetSvrCore()->GetAck();
			COMFILE cf = CF_Init(pMsgData->byData, MAX_BUF_ACK, pMsgData->sSize, 0);
			int bEnableBlur = 0, bZcsBlur = 0;
			CF_Read(&cf, &bEnableBlur, sizeof(int));
			FindAndSetCheck(R.id.chk_py_blur, bEnableBlur);
			CF_Read(&cf, &bZcsBlur, sizeof(int));
			FindAndSetCheck(R.id.chk_jp_zcs, bZcsBlur);

			InitPinyinBlur(cf, pAdapter, CBlurListAdapter::BLUR_TUNE);
			InitPinyinBlur(cf, pAdapter, CBlurListAdapter::BLUR_RHYME);
			InitPinyinBlur(cf, pAdapter, CBlurListAdapter::BLUR_FULL);
		}
		pAdapter->update();
	}

	void CConfigDlg::InitPagePinYin()
	{
		CBlurListAdapter * pAdapter = new CBlurListAdapter;
		SListView *pLvBlur = FindChildByID2<SListView>(R.id.lv_blur);
		SASSERT(pLvBlur);
		pLvBlur->SetAdapter(pAdapter);
		InitPinyinBlurListView(pLvBlur);
		pAdapter->Release();

	}

	void CConfigDlg::InitPhraseLib()
	{
		SMCListView * pLvPhraseLib = FindChildByID2<SMCListView>(R.id.mc_phraselib);
		CPhraseGroupAdapter *pAdapter = new CPhraseGroupAdapter();
		pLvPhraseLib->SetAdapter(pAdapter);
		pAdapter->Release();
		InitPhraseLibListview();
	}

	void CConfigDlg::InitPhraseLibListview()
	{
		int nPhraseGroup = CIsSvrProxy::GetSvrCore()->GetPhraseGroupCount();
		PGROUPINFO pGroupInfo = new GROUPINFO[nPhraseGroup];
		if (CIsSvrProxy::GetSvrCore()->QueryPhraseGroup(pGroupInfo,nPhraseGroup))
		{
			SMCListView * pLvPhraseLib = FindChildByID2<SMCListView>(R.id.mc_phraselib);
			CPhraseGroupAdapter *pAdapter = (CPhraseGroupAdapter*)pLvPhraseLib->GetAdapter();
			pAdapter->RemoveAll();
			for (int i = 0; i < nPhraseGroup; i++)
			{
				CGroupAdapter::GroupInfo gi;
				gi.dwCount = pGroupInfo[i].dwCount;
				gi.bEnable = pGroupInfo[i].bValid;
				gi.strName = pGroupInfo[i].szName;
				gi.strEditor = pGroupInfo[i].szEditor;
				gi.strRemark = pGroupInfo[i].szRemark;
				pAdapter->AddGroup(gi);
			}
			pAdapter->notifyDataSetChanged();
		}
	}

	void CConfigDlg::InitCeLib()
	{
		SMCListView * pLvCeLib = FindChildByID2<SMCListView>(R.id.mc_celib);
		CCelibGroupAdapter *pAdapter = new CCelibGroupAdapter();
		pLvCeLib->SetAdapter(pAdapter);

		CIsSvrProxy::GetSvrCore()->ReqFlmGetInfo();
		PMSGDATA pMsgData = CIsSvrProxy::GetSvrCore()->GetAck();
		COMFILE cf = CF_Init(pMsgData->byData, MAX_BUF_ACK, pMsgData->sSize, 0);
		BOOL bOpen=FALSE;
		CF_ReadT(cf,&bOpen);
		if(bOpen)
		{
			FLMINFO flmInfo;
			CF_ReadT(cf,&flmInfo);
			FindChildByID(R.id.edit_flm_name)->SetWindowText(flmInfo.szName);
			FindChildByID(R.id.edit_flm_addtion)->SetWindowText(flmInfo.szAddition);

			BYTE byGroup = 0;
			CF_ReadT(cf, &byGroup);
			for (BYTE i = 0; i < byGroup; i++)
			{
				CGroupAdapter::GroupInfo gi;
				CF_ReadT(cf, &gi.bEnable);
				std::wstring buf;
				CF_ReadWString(cf, buf);
				gi.strName = buf.c_str();
				CF_ReadT(cf, &gi.dwCount);
				CF_ReadWString(cf, buf);
				gi.strEditor = buf.c_str();
				CF_ReadWString(cf, buf);
				gi.strRemark = buf.c_str();
				pAdapter->AddGroup(gi);
			}
			pAdapter->notifyDataSetChanged();
		}else
		{
			FindChildByID(R.id.edit_flm_name)->SetWindowText(_T("none"));
		}

		pAdapter->Release();

	}


	void CConfigDlg::InitPages()
	{		
		InitPageHabit();
		InitPageHotKey();
		InitPageAssociate();	
		InitPageCandidate();
		InitPageMisc();
		InitPageTTS();
		InitPagePinYin();
		InitPhraseLib();
		InitCeLib();
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
			g_SettingsG->bySwitchKey = 0x2a;
			break;
		case 101:
			g_SettingsG->bySwitchKey = 0x36;
			break;
		default:g_SettingsG->bySwitchKey = 0; break;
		}
	}

	void CConfigDlg::OnClickEnter(int id)
	{
		GetGroupCheck(id);
		switch (CheckId)
		{
		case R.id.enter_for_clear:
			g_SettingsG->bEnterClear=TRUE;
			break;
		case R.id.enter_for_input:
		default:
			g_SettingsG->bEnterClear = FALSE;
			break;
		}
	}

	void CConfigDlg::OnClickPYTemp(int id)
	{
		GetGroupCheck(id);
		switch (CheckId)
		{
		case R.id.py_switch_left_ctrl:
			g_SettingsG->byTempSpellKey = 0xC0;
			break;
		case R.id.py_switch_right_ctrl:
			g_SettingsG->byTempSpellKey = 0xC1;
			break;
		default:
			g_SettingsG->byTempSpellKey = 0;
			break;
		}
	}

	void CConfigDlg::OnClickAlertMode(int id)
	{
		GetGroupCheck(id);
		switch (CheckId)
		{
		case R.id.sound_disable:
			g_SettingsG->nSoundAlert = 0;
			break;
		case R.id.sound_wave:
			g_SettingsG->nSoundAlert = 1;
			break;
		case R.id.sound_beep:
		default:
			g_SettingsG->nSoundAlert = 2;
			break;
		}
	}

	void CConfigDlg::OnAutoInput()
	{
		g_SettingsG->bAutoInput=FindChildByID(R.id.cand_auto_input)->IsChecked();
	}

	void CConfigDlg::OnPyPhraseFirst()
	{
		g_SettingsG->bPYPhraseFirst= FindChildByID(R.id.cand_py_phrase_first)->IsChecked();
	}

	void CConfigDlg::OnDislabeFirstWild()
	{
		g_SettingsG->bDisableFirstWild = FindChildByID(R.id.chk_disable_first_wild)->IsChecked();
	}

	void CConfigDlg::OnFullSpace()
	{
		g_SettingsG->bFullSpace = FindChildByID(R.id.chk_full_space)->IsChecked();
	}

	void CConfigDlg::OnChkOpTip(EventArgs *e)
	{
		SCheckBox *pCheck = sobj_cast<SCheckBox>(e->sender);
		SASSERT(pCheck);
		g_SettingsG->bShowOpTip = pCheck->IsChecked();
	}

	void CConfigDlg::OnClickAssMode(int id)
	{
		GetGroupCheck(id);
		g_SettingsG->byAstMode = CheckId - 400;		
	}

	void CConfigDlg::OnClickForcast(int id)
	{
		GetGroupCheck(id);

		switch (CheckId)
		{
		case 600:
			g_SettingsG->byForecast = 0;
			break;
		case 601:
			g_SettingsG->byForecast = MQC_FORECAST;
			break;
		case 602:
			g_SettingsG->byForecast = MQC_FCNOCAND;
		}
	}

	void CConfigDlg::OnClickGBK(int id)
	{
		GetGroupCheck(id);

		switch (CheckId)
		{
		case 700:
			g_SettingsG->nGbkMode = CSettingsGlobal::GBK_HIDE;
			break;
		case 701:
			g_SettingsG->nGbkMode = CSettingsGlobal::GBK_SHOW_MANUAL;
			break;
		case 702:
			g_SettingsG->nGbkMode = CSettingsGlobal::GBK_SHOW_NORMAL;
		}
	}

	void CConfigDlg::OnChkFullSkipSimple()
	{
		g_SettingsG->bOnlySimpleCode = FindChildByID(R.id.chk_full_skip_simple)->IsChecked();
	}

	void CConfigDlg::OnClickRateAdjust(int id)
	{
		GetGroupCheck(id);
		g_SettingsG->byRateAdjust = CheckId - 500;
	}
	void CConfigDlg::OnClickAutoCompPromp()
	{
		g_SettingsG->bAutoPrompt= FindChildByID(R.id.chk_auto_comp_promp)->IsChecked();	
	}
	void CConfigDlg::OnClickAutoDot()
	{
		g_SettingsG->bAutoDot=FindChildByID(R.id.chk_auto_dot)->IsChecked();
	}
	void CConfigDlg::OnClickAutoSelectCand()
	{
		g_SettingsG->bAutoMatch = FindChildByID(R.id.chk_auto_select_cand)->IsChecked();
	}

	void CConfigDlg::OnDisableNumSelCand()
	{
		g_SettingsG->bCandSelNoNum= FindChildByID(R.id.chk_disable_number_to_select_cand)->IsChecked();
	}
	void CConfigDlg::OnEnable23Cand()
	{
		g_SettingsG->b23CandKey = FindChildByID(R.id.chk_enable_23cand_hotkey)->IsChecked();
	}
	void CConfigDlg::OnHotKeyEvent(EventArgs * pEvt)
	{
		EventSetHotKey *pHotKeyEvt = sobj_cast<EventSetHotKey>(pEvt);
		SHotKeyCtrl * pHotKeyCtrl = sobj_cast<SHotKeyCtrl>(pEvt->sender);
		SASSERT(pHotKeyCtrl);
		DWORD dwAccel = MAKELONG(pHotKeyEvt->vKey, pHotKeyEvt->wModifiers);
		SLOG_INFO("id:" << pHotKeyCtrl->GetID() << " accel:" << SAccelerator::FormatAccelKey(dwAccel));
		switch (pHotKeyCtrl->GetID())
		{
			//hotpage
		case R.id.hk_switch_py:
			g_SettingsG->dwHotkeys[HKI_Mode] = dwAccel; break;
		case R.id.hk_switch_tempspell:
			g_SettingsG->dwHotkeys[HKI_TempSpell] = dwAccel; break;
		case R.id.hk_make_phrase:
			g_SettingsG->dwHotkeys[HKI_MakePhrase] = dwAccel; break;
		case R.id.hk_show_table:
			g_SettingsG->dwHotkeys[HKI_ShowRoot] = dwAccel; break;
		case R.id.hk_show_comp:
			g_SettingsG->dwHotkeys[HKI_Query] = dwAccel; break;
		case R.id.hk_show_statusbar:
			g_SettingsG->dwHotkeys[HKI_HideStatus] = dwAccel; break;
		case R.id.hk_input_en:
			g_SettingsG->dwHotkeys[HKI_EnSwitch] = dwAccel; break;	
		case R.id.hk_filter_gbk:
			g_SettingsG->dwHotkeys[HKI_FilterGbk] = dwAccel; break;
		case R.id.hk_tts:
			g_SettingsG->dwHotkeys[HKI_TTS] = dwAccel; break;
		case R.id.hk_record:
			g_SettingsG->dwHotkeys[HKI_Record] = dwAccel; break;
		case R.id.hk_to_umode:
			g_SettingsG->dwHotkeys[HKI_UDMode] = dwAccel;
		case R.id.hk_2_cand:
			g_SettingsG->by2CandVK = pHotKeyEvt->vKey; break;
		case R.id.hk_3_cand:
			g_SettingsG->by3CandVK = pHotKeyEvt->vKey; break;
		case R.id.hk_turn_prev:
			g_SettingsG->byTurnPageUpVK = pHotKeyEvt->vKey; break;
		case R.id.hk_turn_next:
			g_SettingsG->byTurnPageDownVK = pHotKeyEvt->vKey; break;
		case R.id.hk_bihua_heng:
		case R.id.hk_bihua_shu:
		case R.id.hk_bihua_pie:
		case R.id.hk_bihua_na:
		case R.id.hk_bihua_zhe:
		case R.id.hk_bihua_wild:
			g_SettingsG->byLineKey[pHotKeyCtrl->GetID() - R.id.hk_bihua_heng] = Vkey2Char(pHotKeyEvt->vKey);
			break;
			break;
		case R.id.hk_to_sentmode:
			g_SettingsG->bySentMode = Vkey2Char(pHotKeyEvt->vKey);
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
			if (str.GetLength() < 100)
			{
				wcscpy(g_SettingsG->szWebHeader, str);
			}
			
		}
	}

	void CConfigDlg::OnTtsSpeedChanged(EventArgs * e)
	{
		EventSliderPos *e2 = sobj_cast<EventSliderPos>(e);
		SASSERT(e2);
		FindChildByID(R.id.txt_tts_speed)->SetWindowText(SStringT().Format(_T("%d"), e2->nPos));
		CIsSvrProxy::GetInstance()->TtsSetSpeed(e2->nPos);
	}

	const WCHAR KTTS_SAMPLE_CH[] = L"中文朗读速度测试。";
	const WCHAR KTTS_SAMPLE_EN[] = L"speed test for English speaking.";

	void CConfigDlg::OnTtsChPreview()
	{
		CIsSvrProxy::GetInstance()->TtsSpeakText(KTTS_SAMPLE_CH,-1,true);
	}

	void CConfigDlg::OnTtsEnPreview()
	{
		CIsSvrProxy::GetInstance()->TtsSpeakText(KTTS_SAMPLE_EN,-1,true);
	}

	void CConfigDlg::OnPyBlurClick(EventArgs * e)
	{
		SCheckBox *pCheck = sobj_cast<SCheckBox>(e->sender);
		BOOL bCheck = pCheck->IsChecked();
		CIsSvrProxy::GetSvrCore()->BlurEnable(bCheck);
	}

	void CConfigDlg::OnJPBlurClick(EventArgs * e)
	{
		SCheckBox *pCheck = sobj_cast<SCheckBox>(e->sender);
		BOOL bCheck = pCheck->IsChecked();
		CIsSvrProxy::GetSvrCore()->BlurZCS(bCheck);
	}

	void CConfigDlg::OnTtsChTokenChange(EventArgs * e)
	{
		EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
		CIsSvrProxy::GetInstance()->TtsSetVoice(true,e2->nCurSel);
	}

	void CConfigDlg::OnTtsEnTokenChange(EventArgs * e)
	{
		EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
		CIsSvrProxy::GetInstance()->TtsSetVoice(false,e2->nCurSel);
	}

	void CConfigDlg::OnReNotify(EventArgs * e)
	{
		EventRENotify *e2 = sobj_cast<EventRENotify>(e);
		if (e2->iNotify == EN_CHANGE)
		{
			SRichEdit *pEdit = sobj_cast<SRichEdit>(e->sender);
			SStringT str = pEdit->GetWindowText();
			switch (e2->idFrom)
			{
			case R.id.edit_sent_record_max:
				{
					int nSentMax = _ttoi(str);
					CIsSvrProxy::GetSvrCore()->SetSentRecordMax(nSentMax);
				}
				break;
			}
		}
	}

	void CConfigDlg::OnSpinValue2String(EventArgs * e)
	{
		EventSpinValue2String *e2 = sobj_cast<EventSpinValue2String>(e);
		if(e2->bInit) return;
		switch (e2->idFrom)
		{
		case R.id.spin_predict_phrase_maxlength:
			{
				int nPredictLength = e2->nValue;
				CIsSvrProxy::GetSvrCore()->SetMaxPhrasePreictLength(nPredictLength);
			}
			break;
		case R.id.spin_phrase_ast_deepness_max:
			{
				int nDeepness =  e2->nValue;
				CIsSvrProxy::GetSvrCore()->SetMaxPhraseAstDeepness(nDeepness);
			}
			break;
		case R.id.spin_delay_time:
			g_SettingsG->nDelayTime =  e2->nValue;
			break;
		case R.id.spin_cand_num:
			g_SettingsG->nMaxCands = e2->nValue;
			break;
		}
	}

	void CConfigDlg::OnInstallSysPhraseLib()
	{
		CFileDialogEx dlg(TRUE, _T("spl"), 0, 6, _T("启程词库文件(*.spl)\0*.spl\0All files (*.*)\0*.*\0\0"));
		if (dlg.DoModal() == IDOK)
		{
			::SetCursor(SApplication::getSingleton().LoadCursor(_T("wait")));
			BOOL bRet = CIsSvrProxy::GetSvrCore()->InstallPlt(dlg.m_szFileName);
			::SetCursor(SApplication::getSingleton().LoadCursor(_T("arrow")));
			if (bRet)
			{
				InitPhraseLibListview();
				SMessageBox(m_hWnd, _T("安装成功"), _T("提示"), MB_OK);
			}
			else
			{
				SMessageBox(m_hWnd,_T("安装失败"),_T("提示"),MB_OK|MB_ICONSTOP);
			}
		}
	}

	void CConfigDlg::OnAddBlur()
	{
		CAddBlurDlg addBlurDlg;
		if (addBlurDlg.DoModal() == IDOK)
		{
			
			if (-1 != CIsSvrProxy::GetSvrCore()->BlurAdd(addBlurDlg.m_strFrom, addBlurDlg.m_strTo))
			{
				SListView *pLvBlur = FindChildByID2<SListView>(R.id.lv_blur);
				InitPinyinBlurListView(pLvBlur);
			}
		}
	}

	void CConfigDlg::OnDelBlur()
	{
		SListView *pLvBlur = FindChildByID2<SListView>(R.id.lv_blur);
		int iSel = pLvBlur->GetSel();
		if (iSel != -1)
		{
			CBlurListAdapter *pAdapter = (CBlurListAdapter*)pLvBlur->GetAdapter();
			char szFrom[7], szTo[7];
			if (pAdapter->getBlur(iSel, szFrom, szTo))
			{
				if (-1 != CIsSvrProxy::GetSvrCore()->BlurDel(szFrom, szTo))
				{
					InitPinyinBlurListView(pLvBlur);
				}
			}
		}
	}


	int CConfigDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if (nRet != 0) return nRet;	
		InitPages();
		return 0;
	}


	void CConfigDlg::OnUpdateNow()
	{
		m_pObserver->OnUpdateNow();
		DestroyWindow();
	}

	void CConfigDlg::OnAutoUpdateClick()
	{
		SWindow *pWnd = FindChildByID(R.id.chk_auto_update);
		if (!pWnd->IsChecked())
		{
			m_pObserver->OnUpdateIntervalChanged(0);
		}
		else
		{
			SComboBox * pCbxInterval = FindChildByID2<SComboBox>(R.id.cbx_update_interval);
			int iSel = pCbxInterval->GetCurSel();
			if (iSel == -1)
			{
				pCbxInterval->SetCurSel(0);
			}
			else
			{
				int interval[3] = { 7,30,90 };
				m_pObserver->OnUpdateIntervalChanged(interval[iSel % 3]);
			}
		}
	}

	void CConfigDlg::OnCbUpdateIntervalSelChange(EventArgs * e)
	{
		EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
		SASSERT(e2);
		int iSel = e2->nCurSel;
		if (iSel >= 0 && iSel < 3)
		{
			int intervals[3] = { 7,30,90 };
			m_pObserver->OnUpdateIntervalChanged(intervals[iSel]);
			FindChildByID(R.id.chk_auto_update)->SetCheck(TRUE);
		}
	}

	void CConfigDlg::OnFinalMessage(HWND hWnd)
	{
		__super::OnFinalMessage(hWnd);
		g_SettingsG->SetModified(true);
		g_SettingsUI->SetModified(true);
		delete this;
	}

	void CConfigDlg::OnChangeFont()
	{
		LOGFONT lf={0};
		if(!g_SettingsG->strFontDesc.IsEmpty())
		{
			FontInfo fi = SFontPool::FontInfoFromString(g_SettingsG->strFontDesc);
			_tcscpy(lf.lfFaceName,S_CW2T(fi.strFaceName));
			lf.lfWeight = fi.style.attr.byWeight*4;
			if(lf.lfWeight == 0)
				lf.lfWeight = fi.style.attr.fBold?FW_BOLD:FW_NORMAL;
			lf.lfCharSet = fi.style.attr.byCharset;
			lf.lfHeight = -(short)fi.style.attr.cSize;
			lf.lfItalic = fi.style.attr.fItalic;
			lf.lfUnderline = fi.style.attr.fUnderline;
			lf.lfStrikeOut = fi.style.attr.fStrike;
		}else
		{
			IFontPtr font = SFontPool::getSingletonPtr()->GetFont(L"",100);
			memcpy(&lf,font->LogFont(),sizeof(lf));
		}
		CFontDialog fontDlg(&lf);
		if(fontDlg.DoModal()== IDOK)
		{
			lf = fontDlg.m_lf;
			FontInfo fi;
			fi.strFaceName = lf.lfFaceName;
			fi.style.attr.cSize = abs(lf.lfHeight);
			fi.style.attr.byWeight = lf.lfWeight/4;
			fi.style.attr.byCharset = lf.lfCharSet;
			fi.style.attr.fItalic = lf.lfItalic;
			fi.style.attr.fUnderline = lf.lfUnderline;
			fi.style.attr.fStrike = lf.lfStrikeOut;
			if(lf.lfWeight == FW_BOLD)
			{
				fi.style.attr.fBold = 1;
				fi.style.attr.byWeight = 0;
			}
			g_SettingsG->strFontDesc = SFontPool::FontInfoToString(fi);
			SFontPool::getSingletonPtr()->SetDefFontInfo(fi);
			FindAndSetText(R.id.edit_font,g_SettingsG->strFontDesc);
		}
	}

	void CConfigDlg::OnSkinFont()
	{
		FontInfo fi = SUiDef::getSingletonPtr()->GetUiDef()->GetDefFontInfo();
		FindAndSetText(R.id.edit_font,_T("<皮肤默认>"));
		SFontPool::getSingletonPtr()->SetDefFontInfo(fi);
		g_SettingsG->strFontDesc.Empty();
	}

}
