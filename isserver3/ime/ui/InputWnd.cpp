#include "StdAfx.h"
#include "InputWnd.h"
#include "../utils.h"
#include "../../worker.h"
#include "../../IsSvrProxy.h"

#define SIZE_BELOW 5
#define TIMERID_DELAY 100

namespace SOUI
{
	CInputWnd::CInputWnd(SEventSet *pEvtSets, InputContext *pCtx, IInputWndListener *pListener)
		:CImeWnd(pEvtSets,UIRES.LAYOUT.wnd_composition)
		,m_bLocated(FALSE)
		,m_nCaretHeight(30)
		,m_pInputContext(pCtx)
		,m_pInputWndListener(pListener)
		,m_cPageSize(0)
		,m_bShow(FALSE)
		, m_bDraging(FALSE)
		, m_pStateWnd(NULL)
	{
	}

	CInputWnd::~CInputWnd(void)
	{
	}

	void CInputWnd::SetStatusWnd(CStatusWnd *pWnd)
	{
		m_pStateWnd = pWnd;
	}

	void CInputWnd::SetAnchorPosition(CPoint ptAnchor)
	{
		m_ptAnchor = ptAnchor;
	}


	CPoint CInputWnd::UpdatePosition(CPoint pt,int wid,int hei)
	{
		CPoint pos = pt - CDataCenter::getSingleton().GetData().m_ptSkinOffset;

		CRect rcWorkArea;
		if(::IsWindow(m_hOwner))
		{
			HMONITOR hMonitor = MonitorFromWindow(m_hOwner, MONITOR_DEFAULTTONEAREST);
			MONITORINFO info = { sizeof(info),0 };
			GetMonitorInfo(hMonitor, &info);
			rcWorkArea = info.rcWork;
			SLOGFMTI("work area: %d,%d,%d,%d",rcWorkArea.left,rcWorkArea.top,rcWorkArea.right,rcWorkArea.bottom);
		}else
		{
			SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rcWorkArea,0);
			SLOGFMTW("!!!!! owner is not a window! work area: %d,%d,%d,%d",rcWorkArea.left,rcWorkArea.top,rcWorkArea.right,rcWorkArea.bottom);
		}
		if(pos.x<rcWorkArea.left)
		{
			pos.x = rcWorkArea.left;
		}
		else if (pos.x + wid > rcWorkArea.right)
		{
			pos.x = rcWorkArea.right - wid;
		}
		if (pos.y + m_nCaretHeight + SIZE_BELOW + hei > rcWorkArea.bottom)
		{
			pos.y = pt.y - hei - SIZE_BELOW;
		}
		else
		{
			pos.y = pos.y + m_nCaretHeight + SIZE_BELOW;
		}
		return pos;
	}

	void CInputWnd::MoveTo(CPoint pt,int nCaretHeight)
	{
		SLOG_INFO("pt:" << pt.x <<","<<pt.y<<" caretHeight:"<<nCaretHeight<<" followCaret:"<< g_SettingsUI->bMouseFollow);

		if(nCaretHeight>0)
		{
			m_ptCaret = pt;
			m_nCaretHeight = nCaretHeight;
		}

		if (!g_SettingsUI->bMouseFollow)
		{
			return;
		}

		m_bLocated = TRUE;		
		
		CRect rcWnd = GetClientRect();
		pt = UpdatePosition(m_ptCaret,rcWnd.Width(),rcWnd.Height());
		SetWindowPos(HWND_TOPMOST, pt.x, pt.y , 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

		if(m_bShow && !IsWindowVisible())
		{
			CImeWnd::Show(TRUE);
		}
	}

	void CInputWnd::Show(BOOL bShow, BOOL bClearLocateInfo)
	{
		SLOG_INFO("bShow:"<<bShow<<" located:"<<m_bLocated);
		if(m_bLocated || !g_SettingsUI->bMouseFollow)
		{
			if (!g_SettingsUI->bMouseFollow && bShow)
			{
				UpdateAnchorPosition();
			}
			CImeWnd::Show(bShow);	
		}
		m_bShow = bShow;
		if(!bShow)
		{
			if(bClearLocateInfo) m_bLocated = FALSE;
		}else
		{
			KillTimer(TIMERID_DELAY);
		}
	}

	int CInputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		return OnRecreateUI(lpCreateStruct);
	}

	int CInputWnd::OnRecreateUI(LPCREATESTRUCT lpCreateStruct)
	{
		int nRet = __super::OnRecreateUI(lpCreateStruct);
		if (nRet != 0) return nRet;
		UpdateUI();
		MoveTo(m_ptCaret, m_nCaretHeight);
		return 0;
	}


	void CInputWnd::OnBtnNextPage()
	{
		if(GoNextCandidatePage())
			UpdateUI();
	}

	void CInputWnd::OnUpdateBtnTooltip(EventArgs * e)
	{
		EventSwndUpdateTooltip *e2 = sobj_cast<EventSwndUpdateTooltip>(e);
		SASSERT(e2);
		SStringT strAccel;
		switch (e->sender->GetID())
		{
		case R.id.btn_prevpage:
			e2->bUpdated = TRUE;
			strAccel = SAccelerator::FormatAccelKey(g_SettingsG->byTurnPageUpVK);
			e2->strToolTip = SStringT().Format(_T("重码上翻页,翻页键:%s"), strAccel);
			break;
		case R.id.btn_nextpage:
			e2->bUpdated = TRUE;
			strAccel = SAccelerator::FormatAccelKey(g_SettingsG->byTurnPageDownVK);
			e2->strToolTip = SStringT().Format(_T("重码下翻页,翻页键:%s"),strAccel);
			break;
		}
	}

	void CInputWnd::OnSwitchTip(EventArgs * e)
	{
		EventSwitchTip *e2 = sobj_cast<EventSwitchTip>(e);
		SASSERT(e2);
		STipView *pTipView = sobj_cast<STipView>(e->sender);
		SASSERT(pTipView);
		m_pInputWndListener->OnSwitchTip(m_pInputContext, e2->bNext);
		pTipView->SetWindowText(m_pInputContext->szTip);
	}

	void CInputWnd::OnContextMenu(EventArgs * e)
	{
		m_pStateWnd->OnMenuClick();
	}

	void CInputWnd::OnBtnPrevPage()
	{
		if(GoPrevCandidatePage())
			UpdateUI();
	}


	void CInputWnd::UpdateUI()
	{
		//update composition string
		switch(m_pInputContext->inState)
		{
		case INST_CODING:
		{
			if(m_pInputContext->sbState == SBST_NORMALSTATE)
			{
				SWindow *pMutexView = NULL;
				if (m_pInputContext->compMode == IM_SPELL)
				{
					pMutexView = FindChildByID(R.id.comp_spell);
					pMutexView->SetVisible(TRUE, TRUE);
					SWindow * pTempFlag = pMutexView->FindChildByID(R.id.txt_temp_spell_flag);
					pTempFlag->SetVisible(g_SettingsG->compMode != IM_SPELL, TRUE);
					SSpellView * spellView = pMutexView->FindChildByID2<SSpellView>(R.id.txt_comps);
					spellView->UpdateByContext(m_pInputContext);
				}
				else
				{
					pMutexView = FindChildByID(R.id.comp_normal);
					pMutexView->SetVisible(TRUE, TRUE);
					pMutexView->FindChildByID(R.id.txt_comps)->SetWindowText(SStringW(m_pInputContext->szComp, m_pInputContext->cComp));
				}
				//update tips
				SWindow *pTip = pMutexView->FindChildByID(R.id.txt_tip);
				if (pTip)
				{
					if (m_pInputContext->sbState == SBST_NORMALSTATE && m_pInputContext->bShowTip)
					{
						pTip->SetVisible(TRUE);
						pTip->SetWindowText(m_pInputContext->szTip);
					}
					else
					{
						pTip->SetVisible(FALSE);
						pTip->SetWindowText(NULL);
					}
				}
			}else
			{//update sentence input state
				SWindow * compSent = FindChildByID(R.id.comp_sent);
				compSent->SetVisible(m_pInputContext->sbState != SBST_NORMALSTATE, TRUE);

				SSentView *pStvSent = compSent->FindChildByID2<SSentView>(R.id.stv_sent);
				SASSERT(pStvSent);

				SStringT strInput(m_pInputContext->szInput, m_pInputContext->cInput);
				SStringT strLeft(m_pInputContext->szSentText, m_pInputContext->sSentCaret);
				SStringT strRight(m_pInputContext->szSentText+m_pInputContext->sSentCaret, m_pInputContext->sSentLen - m_pInputContext->sSentCaret);

				SStringT strAll = strInput + strLeft + strRight;
				pStvSent->SetActive(m_pInputContext->sbState == SBST_SENTENCE);
				pStvSent->SetSent(strAll, strInput.GetLength());
				pStvSent->SetSelCount(strLeft.GetLength());
			}
			break;
		}
		case INST_USERDEF:
			{
				SWindow * compUmode = FindChildByID(R.id.comp_umode);
				compUmode->SetVisible(TRUE,TRUE);
				compUmode->FindChildByID(R.id.txt_comps)->SetWindowText(SStringW(m_pInputContext->szComp,m_pInputContext->cComp));
				SWindow *pCompAutoComplete = compUmode->FindChildByID(R.id.txt_auto_complete);
				if (pCompAutoComplete)
				{
					if (m_pInputContext->cComp > 0)
					{
						SStringW strCompAutoComplete;
						if (m_pInputContext->cCompACLen > m_pInputContext->cComp)
						{
							strCompAutoComplete = SStringW(m_pInputContext->szCompAutoComplete + m_pInputContext->cComp,
								m_pInputContext->cCompACLen - m_pInputContext->cComp);
						}
						pCompAutoComplete->SetWindowText(strCompAutoComplete);
					}
					else
					{
						pCompAutoComplete->SetWindowText(_T("自定义输入状态"));
					}
				}
			}
			break;
		case INST_LINEIME:
			{
				SWindow * compLineIme = FindChildByID(R.id.comp_lineime);
				compLineIme->SetVisible(TRUE, TRUE);
				compLineIme->FindChildByID(R.id.txt_comps)->SetWindowText(SStringW(m_pInputContext->szComp, m_pInputContext->cComp));
			}
			break;
		case INST_ENGLISH:
			{
				SWindow * compEnglish = FindChildByID(R.id.comp_english);
				compEnglish->SetVisible(TRUE, TRUE);

				SStringW strComp(m_pInputContext->szComp, m_pInputContext->cComp);
				compEnglish->FindChildByID(R.id.txt_comps)->SetWindowText(strComp);
				SWindow * autoComp = compEnglish->FindChildByID(R.id.txt_auto_complete);
				if (m_pInputContext->pbyEnSpell)
				{//有英文单词
					SStringW strWord((WCHAR *)(m_pInputContext->pbyEnSpell + 1)+ m_pInputContext->cComp, m_pInputContext->pbyEnSpell[0] - m_pInputContext->cComp);
					autoComp->SetWindowText(strWord);
				}
				else
				{
					autoComp->SetWindowText(NULL);
				}
			}
			break;

		}
		//update candidate
		if(m_pInputContext->inState == INST_ENGLISH)
		{
			SWindow * pCandEnglish = FindChildByID(R.id.cand_english_input);
			pCandEnglish->SetVisible(TRUE, TRUE);
			SWindow * pCandContainer = pCandEnglish->FindChildByID(R.id.cand_container);

			int nCandMax = GetCandMax(pCandContainer, SEnglishCand::GetClassName());
			int nPageSize = GetCandMax2(nCandMax);

			int iBegin = m_pInputContext->iCandBegin;
			int iEnd = smin(iBegin + nPageSize, m_pInputContext->sCandCount);
			m_pInputContext->iCandLast = iEnd;
			m_cPageSize = nPageSize;

			pCandEnglish->FindChildByID(R.id.btn_prevpage)->SetVisible(iBegin>0, TRUE);
			pCandEnglish->FindChildByID(R.id.btn_nextpage)->SetVisible(iEnd<m_pInputContext->sCandCount, TRUE);

			SWindow * pCand = pCandContainer->GetWindow(GSW_FIRSTCHILD);
			int iCand = iBegin;
			while (pCand && iCand<iEnd)
			{
				if (pCand->IsClass(SEnglishCand::GetClassName()))
				{
					SEnglishCand *pCand2 = (SEnglishCand*)pCand;
					pCand2->SetVisible(TRUE, TRUE);
					pCand2->SetCandData(m_pInputContext->ppbyCandInfo[iCand]);
					iCand++;
				}
				pCand = pCand->GetWindow(GSW_NEXTSIBLING);
			}

			while (iCand < iBegin + nCandMax && pCand)
			{
				if (pCand->IsClass(SEnglishCand::GetClassName()))
				{
					SEnglishCand *pCand2 = (SEnglishCand*)pCand;
					pCand2->SetVisible(FALSE, TRUE);
					iCand++;
				}
				pCand = pCand->GetWindow(GSW_NEXTSIBLING);
			}
		}
		else if (m_pInputContext->sbState == SBST_NORMALSTATE)
		{//正在输入状态下的重码.
			SWindow * pCandNormal = FindChildByID(R.id.cand_normal);
			pCandNormal->SetVisible(TRUE, TRUE);
			SWindow * pCandContainer = pCandNormal->FindChildByID(R.id.cand_container);

			int nCandMax = GetCandMax(pCandContainer, SCandView::GetClassName());
			int nPageSize = GetCandMax2(nCandMax);

			int iBegin = m_pInputContext->iCandBegin;
			int iEnd = smin(iBegin + nPageSize, m_pInputContext->sCandCount);
			m_pInputContext->iCandLast = iEnd;
			m_cPageSize = nPageSize;

			pCandNormal->FindChildByID(R.id.btn_prevpage)->SetVisible(iBegin>0, TRUE);
			pCandNormal->FindChildByID(R.id.btn_nextpage)->SetVisible(iEnd<m_pInputContext->sCandCount, TRUE);

			SWindow * pCand = pCandContainer->GetWindow(GSW_FIRSTCHILD);
			int iCand = iBegin;
			TCHAR cWild = m_pInputContext->compMode == IM_SHAPECODE ? (CDataCenter::getSingletonPtr()->GetData().m_compInfo.cWild) : 0;
			while (pCand && iCand<iEnd)
			{
				if (pCand->IsClass(SCandView::GetClassName()))
				{
					SCandView *pCand2 = (SCandView*)pCand;
					pCand2->SetVisible(TRUE, TRUE);
					pCand2->SetCandData(cWild, SStringW(m_pInputContext->szComp, m_pInputContext->cComp), m_pInputContext->ppbyCandInfo[iCand]);
					iCand++;
				}
				pCand = pCand->GetWindow(GSW_NEXTSIBLING);
			}

			while (iCand < iBegin + nCandMax && pCand)
			{
				if (pCand->IsClass(SCandView::GetClassName()))
				{
					SCandView *pCand2 = (SCandView*)pCand;
					pCand2->SetVisible(FALSE, TRUE);
					iCand++;
				}
				pCand = pCand->GetWindow(GSW_NEXTSIBLING);
			}
		}
		else
		{//联想状态下的重码
			if (m_pInputContext->sCandCount == 0)
			{
				SMutexView * pCandTip = FindChildByID2<SMutexView>(R.id.cand_tip);
				pCandTip->SetVisible(TRUE, TRUE);
				SWindow *pTip = pCandTip->FindChildByID(R.id.txt_tip);

				if(m_pInputContext->bShowTip)
				{
					pTip->SetWindowText(m_pInputContext->szTip);
				}
				else
				{
					pTip->SetWindowText(NULL);
				}
			}
			else if (g_SettingsG->byAstMode == AST_ENGLISH)
			{//单词联想
				SWindow * pCandEnglish = FindChildByID(R.id.cand_english);
				pCandEnglish->SetVisible(TRUE, TRUE);
				SWindow * pCandContainer = pCandEnglish->FindChildByID(R.id.cand_container);

				pCandEnglish->FindChildByID(R.id.txt_en_header)->SetWindowText(SStringT((WCHAR*)(m_pInputContext->pbyEnAstPhrase+1), m_pInputContext->pbyEnAstPhrase[0]));

				int nCandMax = GetCandMax(pCandContainer, SEnglishCand::GetClassName());
				int nPageSize = GetCandMax2(nCandMax);
				int iBegin = m_pInputContext->iCandBegin;
				int iEnd = smin(iBegin + nPageSize, m_pInputContext->sCandCount);
				m_pInputContext->iCandLast = iEnd;
				m_cPageSize = nPageSize;

				pCandEnglish->FindChildByID(R.id.btn_prevpage)->SetVisible(iBegin>0, TRUE);
				pCandEnglish->FindChildByID(R.id.btn_nextpage)->SetVisible(iEnd<m_pInputContext->sCandCount, TRUE);

				SWindow * pCand = pCandContainer->GetWindow(GSW_FIRSTCHILD);
				int iCand = iBegin;
				while (pCand && iCand<iEnd)
				{
					if (pCand->IsClass(SEnglishCand::GetClassName()))
					{
						SEnglishCand *pCand2 = (SEnglishCand*)pCand;
						pCand2->SetVisible(TRUE, TRUE);
						pCand2->SetCandData(m_pInputContext->ppbyCandInfo[iCand]);
						iCand++;
					}
					pCand = pCand->GetWindow(GSW_NEXTSIBLING);
				}

				while (iCand < iBegin + nCandMax && pCand)
				{
					if (pCand->IsClass(SEnglishCand::GetClassName()))
					{
						SEnglishCand *pCand2 = (SEnglishCand*)pCand;
						pCand2->SetVisible(FALSE, TRUE);
						iCand++;
					}
					pCand = pCand->GetWindow(GSW_NEXTSIBLING);
				}
			}
			else if (g_SettingsG->byAstMode == AST_CAND)
			{//词组联想
				SWindow * pCandEnglish = FindChildByID(R.id.cand_phrase);
				pCandEnglish->SetVisible(TRUE, TRUE);
				SWindow * pCandContainer = pCandEnglish->FindChildByID(R.id.cand_container);

				int nCandMax = GetCandMax(pCandContainer, SPhraseCand::GetClassName());
				int nPageSize = GetCandMax2(nCandMax);
				int iBegin = m_pInputContext->iCandBegin;
				int iEnd = smin(iBegin + nPageSize, m_pInputContext->sCandCount);
				m_pInputContext->iCandLast = iEnd;
				m_cPageSize = nPageSize;

				pCandEnglish->FindChildByID(R.id.btn_prevpage)->SetVisible(iBegin>0, TRUE);
				pCandEnglish->FindChildByID(R.id.btn_nextpage)->SetVisible(iEnd<m_pInputContext->sCandCount, TRUE);

				SWindow * pCand = pCandContainer->GetWindow(GSW_FIRSTCHILD);
				int iCand = iBegin;
				while (pCand && iCand<iEnd)
				{
					if (pCand->IsClass(SPhraseCand::GetClassName()))
					{
						SPhraseCand *pCand2 = (SPhraseCand*)pCand;
						pCand2->SetVisible(TRUE, TRUE);
						pCand2->SetCandData(m_pInputContext->ppbyCandInfo[iCand]);
						iCand++;
					}
					pCand = pCand->GetWindow(GSW_NEXTSIBLING);
				}

				while (iCand < iBegin + nCandMax && pCand)
				{
					if (pCand->IsClass(SPhraseCand::GetClassName()))
					{
						SPhraseCand *pCand2 = (SPhraseCand*)pCand;
						pCand2->SetVisible(FALSE, TRUE);
						iCand++;
					}
					pCand = pCand->GetWindow(GSW_NEXTSIBLING);
				}
			}
			
		}
	}

	void CInputWnd::UpdateAnchorPosition()
	{
		if (m_ptAnchor.x == -1)
		{
			CRect rcWnd;
			GetNative()->GetWindowRect(&rcWnd);

			HMONITOR hMonitor = MonitorFromWindow(m_hOwner, MONITOR_DEFAULTTONEAREST);
			MONITORINFO info = { sizeof(info),0 };
			GetMonitorInfo(hMonitor, &info);
			CRect rcWorkArea = info.rcWork;

			m_ptAnchor.x = rcWorkArea.left + (rcWorkArea.Width() - rcWnd.Width()) / 2;
			m_ptAnchor.y = rcWorkArea.bottom - rcWnd.Height();

			SetWindowPos(NULL, m_ptAnchor.x, m_ptAnchor.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	int CInputWnd::GetCandMax(SWindow *pCandContainer,LPCWSTR pszCandClass) const
	{
		int nRet = 0;
		SWindow * pCand = pCandContainer->GetWindow(GSW_FIRSTCHILD);
		while(pCand)
		{
			if(pCand->IsClass(pszCandClass))
			{
				nRet ++;
			}
			pCand = pCand->GetWindow(GSW_NEXTSIBLING);
		}
		return nRet;
	}


	int CInputWnd::GetCandMax2(int nCands)
	{
		if(g_SettingsG->bCandSelNoNum)
		{
			if(g_SettingsG->b23CandKey)
				return smin(nCands,3);
			else
				return 1;
		}else
		{
			return smin(g_SettingsG->nMaxCands,nCands);
		}
	}

	short CInputWnd::SelectCandidate(short iCand)
	{
		if(m_pInputContext->sCandCount == 0) return -1;
		short idx = iCand + m_pInputContext->iCandBegin;
		if(idx >= m_pInputContext->iCandLast) return -1;
		return idx;
	}

	void CInputWnd::OnFlmInfo(PFLMINFO pFlmInfo)
	{
		SDispatchMessage(UM_FLMINFO, 0, (LPARAM)pFlmInfo);
	}

	void CInputWnd::OnSetSkin(EventArgs * e)
	{
		__super::OnSetSkin(e);
		PFLMINFO flmInfo = CIsSvrProxy::GetSvrCore()->GetCurrentFlmInfo();
		OnFlmInfo(flmInfo);
	}

	BOOL CInputWnd::GoPrevCandidatePage()
	{
		if(m_cPageSize==0) return FALSE;
		if (m_pInputContext->sCandCount <= m_cPageSize) return FALSE;
		if (m_pInputContext->iCandBegin < m_cPageSize)
		{
			CUtils::SoundPlay(_T("error"));
		}
		else
		{
			m_pInputContext->iCandBegin -= m_cPageSize;
		}
		UpdateUI();
		return TRUE;
	}


	BOOL CInputWnd::GoNextCandidatePage()
	{
		if (m_cPageSize == 0) return FALSE;
		if (m_pInputContext->sCandCount <= m_cPageSize) return FALSE;
		if (m_pInputContext->iCandLast < m_pInputContext->sCandCount)
		{
			m_pInputContext->iCandBegin = m_pInputContext->iCandLast;
		}
		else
		{
			CUtils::SoundPlay(_T("error"));
		}
		UpdateUI();
		return TRUE;
	}

	void CInputWnd::Hide(int nDelay)
	{
		if(nDelay == 0)
		{
			Show(FALSE);
		}else
		{
			SetTimer(TIMERID_DELAY,nDelay);
		}
	}

	void CInputWnd::OnTimer(UINT_PTR idEvent)
	{
		if(idEvent == TIMERID_DELAY)
		{
			if (m_pInputWndListener)
			{
				m_pInputWndListener->OnInputDelayHide();
			}
			Show(FALSE);
			KillTimer(idEvent);
		}else
		{
			SetMsgHandled(FALSE);
		}
	}

	void CInputWnd::OnLButtonDown(UINT nFlags, CPoint point)
	{
		if (g_SettingsUI->bMouseFollow)
		{
			SetMsgHandled(FALSE);
			return;
		}

		SWindow::SetCapture();
		m_ptClick = point;
		m_bDraging = TRUE;
	}

	void CInputWnd::OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (g_SettingsUI->bMouseFollow)
		{
			SetMsgHandled(FALSE);
			return;
		}
		m_bDraging = FALSE;
		SWindow::ReleaseCapture();

		CRect rcWnd;
		GetNative()->GetWindowRect(&rcWnd);
		m_ptAnchor = rcWnd.TopLeft();
		//save anchor
		g_SettingsG->ptInput = m_ptAnchor;
		g_SettingsG->SetModified(true);
	}

	void CInputWnd::OnMouseMove(UINT nFlags, CPoint point)
	{
		if (g_SettingsUI->bMouseFollow)
		{
			SetMsgHandled(FALSE);
			return;
		}
		__super::OnMouseMove(nFlags, point);
		::SetCursor(GETRESPROVIDER->LoadCursor(_T("sizeall")));
		if (m_bDraging)
		{
			CRect rcWnd;
			GetNative()->GetWindowRect(&rcWnd);
			rcWnd.OffsetRect(point - m_ptClick);
			SetWindowPos(HWND_TOPMOST, rcWnd.left, rcWnd.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		}

	}

	void CInputWnd::OnWndClick(EventArgs *e)
	{
		e->bubbleUp=true;
		SStringW strSound = e->sender->GetAttribute(L"cmd_sound");
		if(!strSound.IsEmpty())
		{
			CWorker::getSingletonPtr()->PlaySoundFromResource(strSound);
		}
	}

	void CInputWnd::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		__super::OnWindowPosChanging(lpWndPos);
		if(!(lpWndPos->flags&SWP_NOSIZE) && g_SettingsUI->bMouseFollow)
		{
			CPoint pt = UpdatePosition(m_ptCaret,lpWndPos->cx,lpWndPos->cy);
			//auto change pos
			lpWndPos->x=pt.x;
			lpWndPos->y=pt.y;
			lpWndPos->flags&=~SWP_NOMOVE;
		}
	}

}
