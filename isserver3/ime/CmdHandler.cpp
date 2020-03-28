#include "stdafx.h"
#include "CmdHandler.h"
#include "Sinstar3Impl.h"
#include "ui/STipWnd.h"
#include "Utils.h"
#include <shellapi.h>
#include "../IsSvrProxy.h"
#include "../dataCenter/DataCenter.h"

using namespace SOUI;
#define MAX_UDICTKEY	40	//最大关键字长度
#define MAX_UDICTPHRASE	1000


CCmdHandler::CCmdHandler(CSinstar3Impl * pSinstar3)
	:m_pSinstar3(pSinstar3)
{
}


CCmdHandler::~CCmdHandler()
{
}

void CCmdHandler::OnHotkeyMakePhrase(LPARAM lp)
{
	WCHAR szBuf[MAX_INPUT];
	int nRet =CUtils::GetClipboardText(m_pSinstar3->m_hWnd, szBuf, MAX_INPUT);
	if (nRet > 0)
	{
		SStringT msg;
		if (nRet<127 && CIsSvrProxy::GetSvrCore()->ReqMakePhrase(szBuf, nRet) == ISACK_SUCCESS)
		{
			PMSGDATA pMsg = CIsSvrProxy::GetSvrCore()->GetAck();
			if (pMsg->byData[0] == 1)
				msg.Format(_T("词\"%s\"已经存在"), szBuf);
			else
				msg.Format(_T("词\"%s\"加入词库"), szBuf);
		}
		else
		{
			msg.Format(_T("造词\"%s\"失败"), szBuf);
		}
		m_pSinstar3->ShowTip(_T("造词"), msg);
	}
}

void CCmdHandler::OnHotKeyKeyMap(LPARAM lp)
{
	CIsSvrProxy::GetSvrCore()->ReqShowKeyMap();
}

void CCmdHandler::OnHotKeyHideStatusBar(LPARAM lp)
{
	g_SettingsUI->bHideStatus = !g_SettingsUI->bHideStatus;
	m_pSinstar3->m_pStatusWnd->Show(m_pSinstar3->IsStatusVisible());
}

void CCmdHandler::OnHotKeyQueryInfo(LPARAM lp)
{
	WCHAR szBuf[MAX_INPUT] = { 0 };
	int  nGet = CUtils::GetClipboardText(m_pSinstar3->m_hWnd, szBuf, MAX_INPUT);
	if(nGet==0)
	{
		InputContext *pCtx = m_pSinstar3->m_inputState.GetInputContext();
		if (pCtx->cInput)
		{
			SStringT strInput(pCtx->szInput, pCtx->cInput);
			SStringW strInputW = S_CT2W(strInput);
			wcscpy(szBuf, strInputW);
			nGet = strInputW.GetLength();
		}
	}
	if (nGet)
	{
		SStringW strBuf = (szBuf);
		TCHAR szRet[128 + 300 + 300] = { 0 };//关键词+拼音+五笔
		TCHAR *p = szRet;
		if (strBuf[0]>128)
		{//中文关键词，可以查询编码数据
			p += _stprintf(p, _T("关键词=%s"), S_CW2T(strBuf));
			if (CIsSvrProxy::GetSvrCore()->ReqQueryComp(strBuf, strBuf.GetLength()) == ISACK_SUCCESS)
			{
				PMSGDATA pData = CIsSvrProxy::GetSvrCore()->GetAck();
				SStringW str((WCHAR*)pData->byData,pData->sSize/2);
				p += _stprintf(p, _T("\n%s=%s"),CDataCenter::getSingleton().GetData().m_compInfo.strCompName, str.c_str());
			}
			else
			{
				p += _stprintf(p, _T("\n%s=查询失败"), CDataCenter::getSingleton().GetData().m_compInfo.strCompName);
			}

			if (CIsSvrProxy::GetSvrCore()->ReqQueryCompSpell(strBuf, strBuf.GetLength()) == ISACK_SUCCESS)
			{
				PMSGDATA pData = CIsSvrProxy::GetSvrCore()->GetAck();
				short i, sCount = 0;
				BYTE *pbyData = pData->byData;
				p += _stprintf(p, _T("\n拼音="));
				memcpy(&sCount, pbyData, 2);
				pbyData += 2;

				if (sCount>10) sCount = 10;//只取前面10个拼音
				for (i = 0; i<sCount; i++)
				{
					char cSize = pbyData[0];
					pbyData++;
					wcsncpy(p, (WCHAR*)pbyData, cSize);
					pbyData += cSize*2;
					p += cSize;
					*p++ = ' ';
				}
				*p = 0;
			}
			else
			{
				p += _stprintf(p, _T("\n拼音=查询失败"));
			}
		}
		else
		{//查询英文单词
			p += _stprintf(p, _T("单词=%s"), (LPCWSTR)strBuf);
			
			if (CIsSvrProxy::GetSvrCore()->ReqEn2Ch(strBuf,strBuf.GetLength()) == ISACK_SUCCESS)
			{
				PMSGDATA pData = CIsSvrProxy::GetSvrCore()->GetAck();
				LPBYTE pbyData = pData->byData;
				BYTE i = 0, byItems = *pbyData++;
				pbyData += pbyData[0]*2+1;//skip phonetic
				p += _stprintf(p, _T("\n中文释意"));
				while (i<byItems)
				{
					p += _stprintf(p, _T("\n    %d:"), i + 1);
					wcsncpy(p, (WCHAR*)(pbyData + 1), pbyData[0]);
					p += pbyData[0]; 
					pbyData += pbyData[0]*2+1;
					i++;
				}
				*p = 0;
			}
			else
			{
				p += _stprintf(p, _T("\n外文词库查询失败!"));
			}
		}

		m_pSinstar3->ShowTip(_T("查询"), szRet);
	}
	else
	{
		CUtils::SoundPlay(_T("error"));
	}
}


void CCmdHandler::OnHotKeyInputMode(LPARAM lp)
{
	InputContext * pCtx = m_pSinstar3->m_inputState.GetInputContext();
	if (g_SettingsG->compMode != pCtx->compMode)
	{
		m_pSinstar3->ShowTip(_T("提示"), _T("临时拼音模式不能切换！请先退出临时拼音"));
	}else
	{
		m_pSinstar3->m_inputState.ClearContext(CPC_ALL);
		if (g_SettingsG->compMode == IM_SHAPECODE)
			g_SettingsG->compMode = IM_SPELL;
		else
			g_SettingsG->compMode = IM_SHAPECODE;
		pCtx->compMode = (COMPMODE)g_SettingsG->compMode;
		pCtx->bShowTip = FALSE;
		m_pSinstar3->m_pInputWnd->UpdateUI();
		m_pSinstar3->m_pStatusWnd->UpdateCompInfo();
	}
}

void CCmdHandler::OnKeySpeed(LPARAM lp)
{
	SStringT msg = _T("还没有输入数据");
	CMyData &data = CDataCenter::getSingleton().GetData();
	if (data.getTotalInput() > 0)
	{
		msg.Format(_T("累计输入汉字: %d 个\n打字速度:%d 字/分钟"), data.getTotalInput(), data.getTotalInput() * 60000 / data.getTotalSpan());
		if (data.m_cInputCount > 0 && data.m_tmInputSpan>0)
			msg+=SStringT().Format(_T("\n本次输入汉字: %d 个\n打字速度:%d 字/分钟"), data.m_cInputCount, data.m_cInputCount * 60000 / data.m_tmInputSpan);
	}
	m_pSinstar3->ShowTip(_T("提示"), msg);
}

void CCmdHandler::OnHotKeyCharMode(LPARAM lp)
{
	g_SettingsUI->bCharMode = !g_SettingsUI->bCharMode;
	g_SettingsUI->SetModified(true);
	OnSyncUI(CStatusWnd::BTN_CHARMODE);
}

void CCmdHandler::OnHotKeyEnglishMode(LPARAM lp)
{
	g_SettingsUI->bEnglish = !g_SettingsUI->bEnglish;
	g_SettingsUI->SetModified(true);
	OnSyncUI(CStatusWnd::BTN_ENGLISHMODE);
}

void CCmdHandler::OnHotKeyFilterGbk(LPARAM lp)
{
	g_SettingsUI->bFilterGbk = !g_SettingsUI->bFilterGbk;
	g_SettingsUI->SetModified(true);
	OnSyncUI(CStatusWnd::BTN_FILTERGBK);
}

void CCmdHandler::OnHotkeyTTS(LPARAM lp)
{
	g_SettingsUI->bSound = !g_SettingsUI->bSound;
	g_SettingsUI->SetModified(true);
	OnSyncUI(CStatusWnd::BTN_SOUND);
}

void CCmdHandler::OnHotkeyRecord(LPARAM lp)
{
	g_SettingsUI->bRecord = !g_SettingsUI->bRecord;
	g_SettingsUI->SetModified(true);
	OnSyncUI(CStatusWnd::BTN_RECORD);
}

void CCmdHandler::OnFollowCaret(LPARAM lp)
{
	g_SettingsUI->bMouseFollow = !g_SettingsUI->bMouseFollow;
	g_SettingsUI->SetModified(true);
	m_pSinstar3->m_pInputWnd->SetFollowCaret(g_SettingsUI->bMouseFollow);
}

void CCmdHandler::OnUpdateMode(LPARAM lp)
{
	m_pSinstar3->m_pStatusWnd->UpdateCompInfo();
}

void CCmdHandler::OnChangeSkin(LPARAM lp)
{
	SStringT *pSkin = (SStringT*)lp;
	if (g_SettingsG->strSkin != *pSkin || g_SettingsG->bEnableDebugSkin)
	{
		m_pSinstar3->ChangeSkin(*pSkin);
	}
}

void CCmdHandler::OnOpenSkinDir(LPARAM lp)
{
	SStringT strPath = CDataCenter::getSingletonPtr()->GetDataPath() + _T("\\skins\\");
	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile(strPath + _T("*.sskn"), &wfd);
	FindClose(h);
	strPath += wfd.cFileName;
	ShellExecute(NULL, _T("open"), _T("explorer"), _T("/select,") + strPath, NULL, SW_SHOWDEFAULT);
}

void CCmdHandler::OnShowTip(LPARAM lp)
{
	TIPINFO *pTi = (TIPINFO*)lp;
	m_pSinstar3->ShowTip(pTi->strTitle, pTi->strTip);
}

void CCmdHandler::OnExecuteTool(LPARAM lp)
{
	SStringT * pPath = (SStringT*)lp;
	ShellExecute(NULL, _T("open"), *pPath, NULL, NULL, SW_SHOWDEFAULT);
}

void CCmdHandler::OnSyncUI(LPARAM lp)
{
	DWORD dwData = (DWORD)lp;
	m_pSinstar3->Broadcast(CMD_SYNCUI, &dwData, sizeof(DWORD));
}

void CCmdHandler::OnOpenHelp(LPARAM lp)
{
	m_pSinstar3->ShowHelp();
}

void CCmdHandler::OnOpenConfig(LPARAM lp)
{
	m_pSinstar3->OpenConfig();
}
