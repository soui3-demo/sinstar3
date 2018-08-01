#include "stdafx.h"
#include "CmdHandler.h"
#include "Sinstar3Impl.h"
#include "ui/STipWnd.h"
#include "Utils.h"

using namespace SOUI;
#define MAX_UDICTKEY	40	//最大关键字长度
#define MAX_UDICTPHRASE	1000

CCmdHandler::CCmdHandler(CSinstar3Impl * pSinstar3)
	:m_pSinstar3(pSinstar3)
	,m_pTipWnd(NULL)
{
}


CCmdHandler::~CCmdHandler()
{
}

void CCmdHandler::SetTipWnd(STipWnd * pTipWnd)
{
	m_pTipWnd = pTipWnd;
}

void CCmdHandler::OnMakeWord(LPARAM lp)
{
	WCHAR szBuf[MAX_INPUT];
	int nRet =CUtils::GetClipboardText(m_pSinstar3->m_hWnd, szBuf, MAX_INPUT);
	if (nRet > 0)
	{
		SStringA str = S_CW2A(SStringW(szBuf, nRet));
		char szMsg[300];
		if (str.GetLength()<127 && ISComm_MakePhrase(str, (char)str.GetLength()) == ISACK_SUCCESS)
		{
			PMSGDATA pMsg = ISComm_GetData();
			if (pMsg->byData[0] == 1)
				sprintf(szMsg, "词\"%s\"已经存在", (LPCSTR)str);
			else
				sprintf(szMsg, "词\"%s\"加入词库", (LPCSTR)str);
		}
		else
		{
			sprintf(szMsg, "造词\"%s\"失败", (LPCSTR)str);
		}
		SStringT msg = S_CA2T(szMsg);
		m_pTipWnd->SetTip(_T("造词"), msg);
	}
}

void CCmdHandler::OnKeyMap(LPARAM lp)
{
	ISComm_SendMsg(CT_SHOWKEYMAP, 0, 0, 0);
}

void CCmdHandler::OnHideStatusBar(LPARAM lp)
{
	m_pSinstar3->GetInputContext()->settings.bHideStatus = !m_pSinstar3->GetInputContext()->settings.bHideStatus;
	m_pSinstar3->m_pStatusWnd->Show(!m_pSinstar3->GetInputContext()->settings.bHideStatus);
}

void CCmdHandler::OnQueryInfo(LPARAM lp)
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
		SStringA strBuf = S_CW2A(szBuf);
		char szRet[128 + 300 + 300 + MAX_UDICTPHRASE] = { 0 };//关键词+拼音+五笔+用户词典
		char *p = szRet;
		if (strBuf[0] & 0x80)
		{//中文关键词，可以查询编码数据
			p += sprintf(p, "关键词=%s", (LPCSTR)strBuf);
			if (ISComm_QueryComp(strBuf, nGet) == ISACK_SUCCESS)
			{
				PMSGDATA pData = ISComm_GetData();
				pData->byData[pData->sSize] = 0;
				p += sprintf(p, "\r\n%s=%s", ISComm_GetCompInfo()->szName, (char*)pData->byData);
			}
			else
			{
				p += sprintf(p, "\r\n%s=查询失败", ISComm_GetCompInfo()->szName);
			}

			if (ISComm_SpellQueryComp(strBuf, nGet) == ISACK_SUCCESS)
			{
				PMSGDATA pData = ISComm_GetData();
				short i, sCount = 0;
				BYTE *pbyData = pData->byData;
				p += sprintf(p, "\r\n拼音=");
				memcpy(&sCount, pbyData, 2);
				pbyData += 2;
				if (sCount>10) sCount = 10;//只取前面10个拼音
				for (i = 0; i<sCount; i++)
				{
					char cSize = pbyData[0];
					pbyData++;
					memcpy(p, pbyData, cSize);
					pbyData += cSize;
					p += cSize;
					*p++ = ' ';
				}
				*p = 0;
			}
			else
			{
				p += sprintf(p, "\r\n拼音=查询失败");
			}
		}
		else
		{//查询英文单词
			p += sprintf(p, "\r\n单词=%s", (LPCSTR)strBuf);
			if (ISComm_En2Ch(strBuf, nGet) == ISACK_SUCCESS)
			{
				PMSGDATA pData = ISComm_GetData();
				LPBYTE pbyData = pData->byData;
				BYTE i = 0, byItems = *pbyData++;
				pbyData += pbyData[0] + 1;//skip phonetic
				p += sprintf(p, "\r\n中文释意");
				while (i<byItems)
				{
					p += sprintf(p, "\r\n\t%d:", i + 1);
					memcpy(p, pbyData + 1, pbyData[0]); p += pbyData[0]; pbyData += pbyData[0] + 1;
					i++;
				}
				*p = 0;
			}
			else
			{
				p += sprintf(p, "\r\n外文词库查询失败!");
			}
		}
		if (ISComm_QueryUserDict(strBuf, nGet) == ISACK_SUCCESS)
		{//查询用户词典成功
			PMSGDATA pData = ISComm_GetData();
			if (pData->sSize<1000 + 100)//关键词长度最大100，数据最大长度1000
			{
				char *pbuf = (char*)pData->byData;
				p += sprintf(p, "\r\n用户词典[%s]:%s", ISComm_GetUserDict(), pbuf);
				pbuf += strlen(pbuf) + 1;
				p += sprintf(p, "\r\n\t%s", pbuf);
				if (OpenClipboard(m_pSinstar3->m_hWnd))
				{//将用户词典内容保存到剪贴板
					int nLen = (int)strlen(pbuf);
					HGLOBAL hMem = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, nLen + 1);
					LPSTR pMem = (LPSTR)GlobalLock(hMem);
					strcpy(pMem, pbuf);
					GlobalUnlock(hMem);
					EmptyClipboard();
					SetClipboardData(CF_TEXT, hMem);
					CloseClipboard();
				}
			}
		}
		SStringT msg = S_CA2T(szRet);
		m_pTipWnd->SetTip(_T("查询"), msg);
	}
	else
	{
		CUtils::SoundPlay(_T("error"));
	}
}

void CCmdHandler::OnFollowCaret(LPARAM lp)
{
	m_pSinstar3->GetInputContext()->settings.bMouseFollow = !m_pSinstar3->GetInputContext()->settings.bMouseFollow;
	m_pSinstar3->m_pInputWnd->SetFollowCaret(m_pSinstar3->GetInputContext()->settings.bMouseFollow);
}

void CCmdHandler::OnInputMode(LPARAM lp)
{
	InputContext * pCtx = m_pSinstar3->m_inputState.GetInputContext();
	if (g_SettingsG.compMode != pCtx->compMode)
	{
		m_pTipWnd->SetTip(_T("提示"), _T("临时拼音模式不能切换！请先退出临时拼音"));
	}else
	{
		m_pSinstar3->m_inputState.ClearContext(CPC_ALL);
		if (g_SettingsG.compMode == IM_SHAPECODE)
			g_SettingsG.compMode = IM_SPELL;
		else
			g_SettingsG.compMode = IM_SHAPECODE;
		pCtx->compMode = g_SettingsG.compMode;
		pCtx->bShowTip = FALSE;
		m_pSinstar3->m_pInputWnd->UpdateUI();
		m_pSinstar3->m_pStatusWnd->UpdateCompInfo();
	}
}

void CCmdHandler::OnKeySpeed(LPARAM lp)
{
	SStringT msg = _T("还没有输入数据");
	CMyData &data = CDataCenter::getSingleton().GetData();
	if (data.m_cInputCount>0)
		msg.Format(_T("输入汉字: %d 个\n打字速度:%d 字/分钟"), data.m_cInputCount, data.m_cInputCount * 60000 / data.m_tmInputSpan);

	m_pTipWnd->SetTip(_T("提示"), msg);
}

void CCmdHandler::OnCharMode(LPARAM lp)
{
	m_pSinstar3->GetInputContext()->settings.bCharMode = !m_pSinstar3->GetInputContext()->settings.bCharMode;
	m_pSinstar3->m_pStatusWnd->UpdateToggleStatus(CStatusWnd::BTN_CHARMODE);
}

void CCmdHandler::OnEnglishMode(LPARAM lp)
{
	m_pSinstar3->GetInputContext()->settings.bEnglish = !m_pSinstar3->GetInputContext()->settings.bEnglish;
	m_pSinstar3->m_pStatusWnd->UpdateToggleStatus(CStatusWnd::BTN_ENGLISHMODE);
}

void CCmdHandler::OnUpdateMode(LPARAM lp)
{
	m_pSinstar3->m_pStatusWnd->UpdateCompInfo();
}
