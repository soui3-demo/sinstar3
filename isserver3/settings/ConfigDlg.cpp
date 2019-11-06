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

#pragma warning(disable:4244)
namespace SOUI
{
	SStringT DwordVer2String(DWORD dwVer)
	{
		BYTE * pByte = (BYTE*)&dwVer;
		return SStringT().Format(_T("%u.%u.%u.%u"), UINT(pByte[3]), UINT(pByte[2]), UINT(pByte[1]), UINT(pByte[0]));
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
			if ((e2->dwOldState & WndState_Check) != (e2->dwNewState & WndState_Check))
			{
				SWindow *pSender = sobj_cast<SWindow>(e->sender);
				int idx = pSender->GetUserData();
				SStringA strName = S_CT2A(m_arrGroupInfo[idx].strName);
				ISComm_EnablePhraseGroup(strName, (e2->dwNewState & WndState_Check)?1:0);
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
			if ((e2->dwOldState & WndState_Check) != (e2->dwNewState & WndState_Check))
			{
				SWindow *pSender = sobj_cast<SWindow>(e->sender);
				int idx = pSender->GetUserData();
				SStringA strName = S_CT2A(m_arrGroupInfo[idx].strName);
				char bEnable = (e2->dwNewState & WndState_Check)?1:0;
				if(ISACK_SUCCESS == ISComm_Flm_EnableGroup(strName,bEnable ))
				{
					m_arrGroupInfo[idx].bEnable = bEnable;
				}
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
				for (int j = 0; j < m_lstBlur[i].GetCount(); j++)
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

		FindAndSetHotKey(R.id.hk_to_sentmode, Char2VKey(g_SettingsG->bySentMode));

		FindAndSetSpin(R.id.spin_delay_time, g_SettingsG->nDelayTime);

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

		FindAndSetText(R.id.edit_webmode_header, S_CA2T(g_SettingsG->szWebHeader));

		if (ISComm_GetSentRecordMax() == ISACK_SUCCESS)
		{
			PMSGDATA pMsgData = ISComm_GetData();
			int nSentMax = *(int*)pMsgData->byData;
			FindAndSetText(R.id.edit_sent_record_max, SStringT().Format(_T("%d"), nSentMax));
		}
		if (ISComm_GetMaxPhrasePredictLength() == ISACK_SUCCESS)
		{
			PMSGDATA pMsgData = ISComm_GetData();
			int nPredictLength = *(int*)pMsgData->byData;
			FindAndSetSpin(R.id.spin_predict_phrase_maxlength, nPredictLength);
		}
		if (ISComm_GetMaxPhraseAssociateDeepness() == ISACK_SUCCESS)
		{
			PMSGDATA pMsgData = ISComm_GetData();
			int nDeepness = *(int*)pMsgData->byData;
			FindAndSetSpin(R.id.spin_phrase_ast_deepness_max, nDeepness);
		}
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
		
		if (ISComm_ServerVersion() == ISACK_SUCCESS)
		{
			PMSGDATA pData = ISComm_GetData();
			SStringT strVer = DwordVer2String(*(DWORD*)pData->byData);
			FindChildByID(R.id.txt_svr_ver)->SetWindowText(strVer);
		}

	}

	static LPBYTE ExtractTtsToken(LPBYTE pBuf, SStringAList & tokens, int &iSelToken)
	{
		int nCount = 0;
		memcpy(&nCount, pBuf, sizeof(int));
		pBuf += sizeof(int);
		for (int i = 0; i < nCount; i++)
		{
			int nLen = 0;
			memcpy(&nLen, pBuf, sizeof(int));
			pBuf += sizeof(int);
			SStringA str;
			char *p = str.GetBufferSetLength(nLen);
			memcpy(p, pBuf, nLen);
			str.ReleaseBuffer();
			tokens.Add(str);
			pBuf += nLen;
		}
		memcpy(&iSelToken, pBuf, sizeof(int));
		pBuf += sizeof(int);
		return pBuf;
	}

	LPBYTE CConfigDlg::InitTtsTokenInfo(LPBYTE pBuf, SComboBox *pCbx)
	{
		SStringAList lstToken;
		int iSelToken = -1;
		LPBYTE p = ExtractTtsToken(pBuf, lstToken, iSelToken);

		if (lstToken.GetCount())
		{
			for (int i = 0; i < lstToken.GetCount(); i++)
			{
				pCbx->InsertItem(-1, S_CA2T(lstToken[i]), 0, 0);
			}
			if (iSelToken < lstToken.GetCount())
			{
				pCbx->GetEventSet()->setMutedState(true);
				pCbx->SetCurSel(iSelToken);
				pCbx->GetEventSet()->setMutedState(false);
			}
		}
		return p;
	}

	void CConfigDlg::InitPageTTS()
	{
		if (ISComm_GetTtsTokens() == ISACK_SUCCESS)
		{
			PMSGDATA pMsgData = ISComm_GetData();
			LPBYTE p = InitTtsTokenInfo(pMsgData->byData, FindChildByID2<SComboBox>(R.id.cbx_tts_en_token));
			InitTtsTokenInfo(p, FindChildByID2<SComboBox>(R.id.cbx_tts_ch_token));
		}
		if (ISComm_GetTtsSpeed() == ISACK_SUCCESS)
		{
			PMSGDATA pMsgData = ISComm_GetData();
			int nSpeed = *(int*)pMsgData->byData;
			FindChildByID(R.id.txt_tts_speed)->SetWindowText(SStringT().Format(_T("%d"), nSpeed));
			FindChildByID2<SSliderBar>(R.id.slider_tts_speed)->SetValue(nSpeed);
		}
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
		if (ISComm_Blur_Query() == ISACK_SUCCESS)
		{
			PMSGDATA pMsgData = ISComm_GetData();
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
		if (ISACK_SUCCESS == ISComm_QueryPhraseGroup())
		{
			SMCListView * pLvPhraseLib = FindChildByID2<SMCListView>(R.id.mc_phraselib);
			CPhraseGroupAdapter *pAdapter = (CPhraseGroupAdapter*)pLvPhraseLib->GetAdapter();
			pAdapter->RemoveAll();

			PMSGDATA pMsgData = ISComm_GetData();
			COMFILE cf = CF_Init(pMsgData->byData, MAX_BUF_ACK, pMsgData->sSize, 0);
			BYTE byCount;
			CF_ReadT(cf, &byCount);
			for (BYTE i = 0; i < byCount; i++)
			{
				CGroupAdapter::GroupInfo gi;
				CF_ReadT(cf, &gi.bEnable);
				std::string buf;
				CF_ReadString(cf, buf);
				gi.strName = S_CA2T(buf.c_str());
				CF_ReadT(cf, &gi.dwCount);
				CF_ReadString(cf, buf);
				gi.strEditor = S_CA2T(buf.c_str());
				CF_ReadString(cf, buf);
				gi.strRemark = S_CA2T(buf.c_str());
				pAdapter->AddGroup(gi);
			}
			pAdapter->notifyDataSetChanged();
		}
	}

	void CConfigDlg::InitCeLib()
	{
		SComboBox * pCbxCelib = FindChildByID2<SComboBox>(R.id.cbx_celib);
		pCbxCelib->InsertItem(0,_T("Close"),0,0);
		if(ISACK_SUCCESS == ISComm_Flm_List())
		{
			PMSGDATA pMsgData = ISComm_GetData();
			COMFILE cf = CF_Init(pMsgData->byData, MAX_BUF_ACK, pMsgData->sSize, 0);

			for(;;)
			{
				std::string strCelib;
				CF_ReadString(cf,strCelib);
				if(strCelib.empty()) break;
				pCbxCelib->InsertItem(-1,S_CA2T(strCelib.c_str()),0,0);
			}
			std::string strCurCelib;
			CF_ReadString(cf,strCurCelib);
			pCbxCelib->GetEventSet()->setMutedState(true);
			if(strCurCelib.empty())
			{
				pCbxCelib->SetCurSel(0);
			}else
			{
				int iItem = pCbxCelib->GetListBox()->FindString(-1,S_CA2T(strCurCelib.c_str()));
				pCbxCelib->SetCurSel(iItem);
			}
			pCbxCelib->GetEventSet()->setMutedState(false);
		}

		SMCListView * pLvCeLib = FindChildByID2<SMCListView>(R.id.mc_celib);
		CCelibGroupAdapter *pAdapter = new CCelibGroupAdapter();
		pLvCeLib->SetAdapter(pAdapter);
		pAdapter->Release();
		InitCeLibListview();
	}

	void CConfigDlg::InitCeLibListview()
	{
		if (ISACK_SUCCESS == ISComm_Flm_GetInfo())
		{
			SMCListView * pLvCeLib = FindChildByID2<SMCListView>(R.id.mc_celib);
			CCelibGroupAdapter *pAdapter = (CCelibGroupAdapter*)pLvCeLib->GetAdapter();
			pAdapter->RemoveAll();

			PMSGDATA pMsgData = ISComm_GetData();
			COMFILE cf = CF_Init(pMsgData->byData, MAX_BUF_ACK, pMsgData->sSize, 0);
			int bOpen;
			CF_ReadT(cf,&bOpen);
			if(!bOpen)
			{
				FindChildByID(R.id.edit_flm_name)->SetWindowText(NULL);
				FindChildByID(R.id.edit_flm_addtion)->SetWindowText(NULL);
			}else
			{
				FLMINFO flmInfo;
				CF_ReadT(cf,&flmInfo);
				FindChildByID(R.id.edit_flm_name)->SetWindowText(S_CA2T(flmInfo.szName));
				FindChildByID(R.id.edit_flm_addtion)->SetWindowText(S_CA2T(flmInfo.szAddition));
				BYTE byCount;
				CF_ReadT(cf, &byCount);
				for (BYTE i = 0; i < byCount; i++)
				{
					CGroupAdapter::GroupInfo gi;
					CF_ReadT(cf, &gi.bEnable);
					std::string buf;
					CF_ReadString(cf, buf);
					gi.strName = S_CA2T(buf.c_str());
					CF_ReadT(cf, &gi.dwCount);
					CF_ReadString(cf, buf);
					gi.strEditor = S_CA2T(buf.c_str());
					CF_ReadString(cf, buf);
					gi.strRemark = S_CA2T(buf.c_str());
					pAdapter->AddGroup(gi);
				}
			}
			pAdapter->notifyDataSetChanged();
		}
	}

	void CConfigDlg::OnCbxFlmChange(EventArgs * e)
	{
		EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
		if(e2->nCurSel == 0)
		{//close flm
			ISComm_Flm_Open(NULL);
		}else
		{
			SComboBox *pCbx = sobj_cast<SComboBox>(e->sender);
			SStringT strFlm = pCbx->GetLBText(e2->nCurSel,TRUE);
			SStringA strFlmUtf8=S_CT2A(strFlm,CP_UTF8);
			ISComm_Flm_Open(strFlmUtf8);
		}
		InitCeLibListview();
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
			g_SettingsG->byTempSpellKey = FALSE;
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
			SStringA strA = S_CT2A(str);
			if (strA.GetLength() < 100)
			{
				strcpy(g_SettingsG->szWebHeader, strA);
			}
			
		}
	}

	void CConfigDlg::OnTtsSpeedChanged(EventArgs * e)
	{
		EventSliderPos *e2 = sobj_cast<EventSliderPos>(e);
		SASSERT(e2);
		FindChildByID(R.id.txt_tts_speed)->SetWindowText(SStringT().Format(_T("%d"), e2->nPos));
		ISComm_SetTtsSpeed(e2->nPos);
	}

	const char KTTS_SAMPLE_CH[] = "中文朗读速度测试。";
	const char KTTS_SAMPLE_EN[] = "speed test for English speaking.";

	void CConfigDlg::OnTtsChPreview()
	{
		ISComm_TTS(KTTS_SAMPLE_CH, sizeof(KTTS_SAMPLE_CH) - 1, MTTS_CH);
	}

	void CConfigDlg::OnTtsEnPreview()
	{
		ISComm_TTS(KTTS_SAMPLE_EN, sizeof(KTTS_SAMPLE_EN) - 1, MTTS_EN);
	}

	void CConfigDlg::OnPyBlurClick(EventArgs * e)
	{
		SCheckBox *pCheck = sobj_cast<SCheckBox>(e->sender);
		BOOL bCheck = pCheck->IsChecked();
		ISComm_Blur_Set(bCheck);
	}

	void CConfigDlg::OnJPBlurClick(EventArgs * e)
	{
		SCheckBox *pCheck = sobj_cast<SCheckBox>(e->sender);
		BOOL bCheck = pCheck->IsChecked();
		ISComm_BlurZCS(bCheck);
	}

	void CConfigDlg::OnTtsChTokenChange(EventArgs * e)
	{
		EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
		ISComm_SetTtsToken(1, e2->nCurSel);
	}

	void CConfigDlg::OnTtsEnTokenChange(EventArgs * e)
	{
		EventCBSelChange *e2 = sobj_cast<EventCBSelChange>(e);
		ISComm_SetTtsToken(0, e2->nCurSel);
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
					ISComm_SetSentRecordMax(nSentMax);
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
				ISComm_SetMaxPhrasePredictLength(nPredictLength);
			}
			break;
		case R.id.spin_phrase_ast_deepness_max:
			{
				int nDeepness =  e2->nValue;
				ISComm_SetMaxPhraseAssociateDeepness(nDeepness);
			}
			break;
		case R.id.spin_delay_time:
			g_SettingsG->nDelayTime =  e2->nValue;
			break;
		}
	}

	void CConfigDlg::OnInstallSysPhraseLib()
	{
		CFileDialogEx dlg(TRUE, _T("spl"), 0, 6, _T("启程词库文件(*.spl)\0*.spl\0All files (*.*)\0*.*\0\0"));
		if (dlg.DoModal() == IDOK)
		{
			SStringA strNameUtf8 = S_CT2A(dlg.m_szFileName, CP_UTF8);
			::SetCursor(SApplication::getSingleton().LoadCursor(_T("wait")));
			DWORD dwRet = ISComm_InstallPhraseLib(strNameUtf8);
			::SetCursor(SApplication::getSingleton().LoadCursor(_T("arrow")));
			if (dwRet == ISACK_ERROR)
			{
				SMessageBox(m_hWnd,_T("安装失败"),_T("提示"),MB_OK|MB_ICONSTOP);
			}
			else if (dwRet == ISACK_UNINIT)
			{
				SMessageBox(m_hWnd, _T("服务器正在加载数据，请稍后重试"), _T("提示"), MB_OK | MB_ICONSTOP);
			}
			else if (dwRet == ISACK_SUCCESS)
			{
				InitPhraseLibListview();
				SMessageBox(m_hWnd, _T("安装成功"), _T("提示"), MB_OK);
			}
		}
	}

	void CConfigDlg::OnImportUserLib()
	{
		CFileDialogEx dlg(TRUE, _T("txt"), 0, 6, _T("词库文件(*.txt)\0*.txt\0All files (*.*)\0*.*\0\0"));
		if (dlg.DoModal() == IDOK)
		{
			SStringA strNameUtf8 = S_CT2A(dlg.m_szFileName, CP_UTF8);
			DWORD dwRet = ISComm_ImportUserLib(strNameUtf8);
			if (dwRet == ISACK_ERROR)
			{
				SMessageBox(m_hWnd, _T("导入失败"), _T("提示"), MB_OK | MB_ICONSTOP);
			}
			else if (dwRet == ISACK_SUCCESS)
			{
				SMessageBox(m_hWnd, _T("导入成功"), _T("提示"), MB_OK);
			}
		}
	}

	void CConfigDlg::OnExportUserLib()
	{
		CFileDialogEx dlg(FALSE, _T("txt"), 0, 6, _T("词库文件(*.txt)\0*.txt\0All files (*.*)\0*.*\0\0"));
		if (dlg.DoModal() == IDOK)
		{
			SStringA strNameUtf8 = S_CT2A(dlg.m_szFileName, CP_UTF8);
			DWORD dwRet = ISComm_ExportUserLib(strNameUtf8);
			if (dwRet == ISACK_ERROR)
			{
				SMessageBox(m_hWnd, _T("导出失败"), _T("提示"), MB_OK | MB_ICONSTOP);
			}
			else if (dwRet == ISACK_SUCCESS)
			{
				ShellExecute(m_hWnd, _T("open"), dlg.m_szFileName, NULL,NULL, SW_SHOWDEFAULT);
			}
		}
	}

	void CConfigDlg::OnAddBlur()
	{
		CAddBlurDlg addBlurDlg;
		if (addBlurDlg.DoModal() == IDOK)
		{
			if (ISACK_SUCCESS == ISComm_Blur_Add(addBlurDlg.m_strFrom, addBlurDlg.m_strTo))
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
				if (ISACK_SUCCESS == ISComm_Blur_Del(szFrom, szTo))
				{
					InitPinyinBlurListView(pLvBlur);
				}
			}
		}
	}

	void CConfigDlg::OnDestroy()
	{
		ISComm_Logout(m_hWnd);
		__super::OnDestroy();
	}

	int CConfigDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnCreate(lpCreateStruct);
		if (nRet != 0) return nRet;	
		ISComm_Login(m_hWnd);
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
		delete this;
	}

}
