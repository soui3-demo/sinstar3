#include "StdAfx.h"
#include "InputWnd.h"

#define SIZE_BELOW 5
#define TIMERID_DELAY 100

namespace SOUI
{
	CInputWnd::CInputWnd(InputContext *pCtx)
		:CImeWnd(UIRES.LAYOUT.wnd_composition)
		,m_bLocated(FALSE)
		,m_nCaretHeight(30)
		,m_pInputContext(pCtx)
		,m_cPageSize(0)
		,m_bShow(FALSE)
		, m_bDraging(FALSE)
		, m_bFollowCaret(TRUE)
	{
	}

	CInputWnd::~CInputWnd(void)
	{
	}

	void CInputWnd::SetFollowCaret(BOOL bFollowCaret)
	{
		m_bFollowCaret = bFollowCaret;
		if (CSimpleWnd::IsWindowVisible() && !bFollowCaret)
		{
			UpdateAnchorPosition();
			SetWindowPos(HWND_TOPMOST, m_ptAnchor.x, m_ptAnchor.y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		}
	}

	void CInputWnd::SetAnchorPosition(CPoint ptAnchor)
	{
		m_ptAnchor = ptAnchor;
	}


	void CInputWnd::MoveTo(CPoint pt,int nCaretHeight)
	{
		SLOG_INFO("pt:" << pt.x <<","<<pt.y<<" caretHeight:"<<nCaretHeight<<" followCaret:"<< m_bFollowCaret);

		if (!m_bFollowCaret)
		{
			return;
		}

		m_bLocated = TRUE;
		m_ptCaret = pt;
		m_nCaretHeight = nCaretHeight;
		SetWindowPos(HWND_TOPMOST,m_ptCaret.x,m_ptCaret.y + m_nCaretHeight + SIZE_BELOW,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
		if(m_bShow && !IsWindowVisible())
		{
			CImeWnd::Show(TRUE);
		}
	}

	void CInputWnd::Show(BOOL bShow, BOOL bClearLocateInfo)
	{
		SLOG_INFO("bShow:"<<bShow<<" located:"<<m_bLocated);
		if(m_bLocated || !m_bFollowCaret)
		{
			if (!m_bFollowCaret && bShow)
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
		int nRet = __super::OnCreate(lpCreateStruct);
		if(nRet != 0) return nRet;
		UpdateUI();
		return 0;
	}


	void CInputWnd::OnBtnNextPage()
	{
		if(GoNextCandidatePage())
			UpdateUI();
	}

	void CInputWnd::OnBtnPrevPage()
	{
		if(GoPrevCandidatePage())
			UpdateUI();
	}


	void CInputWnd::UpdateUI()
	{
		switch(m_pInputContext->inState)
		{
		case INST_CODING:
			if(m_pInputContext->compMode == IM_SPELL)
			{
				SWindow * compSpell = FindChildByID(R.id.comp_spell);
				compSpell->SetVisible(TRUE,TRUE);
				SWindow * pTempFlag = compSpell->FindChildByID(R.id.txt_temp_spell_flag);
				pTempFlag->SetVisible(g_SettingsG.compMode != IM_SPELL,TRUE);
				SSpellView * spellView = compSpell->FindChildByID2<SSpellView>(R.id.txt_comps);
				spellView->UpdateByContext(m_pInputContext);
			}
			else
			{
				SWindow * compNormal = FindChildByID(R.id.comp_normal);
				compNormal->SetVisible(TRUE,TRUE);
				compNormal->FindChildByID(R.id.txt_comps)->SetWindowText(S_CA2T(SStringA(m_pInputContext->szComp,m_pInputContext->cComp)));
			}
			//update sentence input state
			{
				SWindow * compSent = FindChildByID(R.id.comp_sent);
				compSent->SetVisible(m_pInputContext->sbState != SBST_NORMAL,TRUE);
				SWindow *pSentInput = compSent->FindChildByID(R.id.sent_input);
				SWindow *pSentLeft = compSent->FindChildByID(R.id.sent_left);
				SWindow *pSentRight = compSent->FindChildByID(R.id.sent_right);
				if(m_pInputContext->sbState != SBST_NORMAL)
				{//
					SStringT strInput(m_pInputContext->szInput,m_pInputContext->cInput);
					pSentInput->SetWindowText(strInput);
					int nSelLen = int(m_pInputContext->pbySentWord[m_pInputContext->sSentCaret]-m_pInputContext->pbySentWord[0]);
					SStringA strLeft((char*)m_pInputContext->pbySentWord[0],nSelLen);
					pSentLeft->SetWindowText(S_CA2T(strLeft));
					SStringA strRight((char*)m_pInputContext->pbySentWord[m_pInputContext->sSentCaret],m_pInputContext->sSentLen-nSelLen);
					pSentRight->SetWindowText(S_CA2T(strRight));
				}
				else
				{
					pSentInput->SetWindowText(NULL);
					pSentLeft->SetWindowText(NULL);
					pSentRight->SetWindowText(NULL);
				}
			}
			break;
		case INST_USERDEF:
			{
				SWindow * compUmode = FindChildByID(R.id.comp_umode);
				compUmode->SetVisible(TRUE,TRUE);
				compUmode->FindChildByID(R.id.txt_comps)->SetWindowText(S_CA2T(SStringA(m_pInputContext->szComp,m_pInputContext->cComp)));
				SWindow *pCompAutoComplete = compUmode->FindChildByID(R.id.txt_auto_complete);
				if (pCompAutoComplete)
				{
					if (m_pInputContext->cComp > 0)
					{
						SStringA strCompAutoComplete;
						if (m_pInputContext->cCompACLen > m_pInputContext->cComp)
						{
							strCompAutoComplete = SStringA(m_pInputContext->szCompAutoComplete + m_pInputContext->cComp,
								m_pInputContext->cCompACLen - m_pInputContext->cComp);
						}
						pCompAutoComplete->SetWindowText(S_CA2T(strCompAutoComplete));
					}
					else
					{
						pCompAutoComplete->SetWindowText(_T("自定义输入状态"));
					}
				}
			}
			break;
		}
		//update candidate
		if (m_pInputContext->sCandCount>0)
		{
			SWindow * pCandNormal = FindChildByID(R.id.cand_normal);
			pCandNormal->SetVisible(TRUE, TRUE);
			SWindow * pCandContainer = pCandNormal->FindChildByID(R.id.cand_container);

			int nPageSize = GetCandMax(pCandContainer);
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
					pCand2->SetCandData(cWild, S_CA2T(SStringA(m_pInputContext->szComp, m_pInputContext->cComp)), m_pInputContext->ppbyCandInfo[iCand]);
					iCand++;
				}
				pCand = pCand->GetWindow(GSW_NEXTSIBLING);
			}

			while (iCand < iBegin + nPageSize && pCand)
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
		{
			SMutexView * pCandTip = FindChildByID2<SMutexView>(R.id.cand_tip);
			pCandTip->SetVisible(TRUE, TRUE);
			SWindow *pTip = pCandTip->FindChildByID(R.id.txt_tip);
			pTip->SetWindowText(S_CA2T(m_pInputContext->szTip));
		}
	}

	void CInputWnd::UpdateAnchorPosition()
	{
		if (m_ptAnchor.x == -1)
		{
			CRect rcWnd;
			CSimpleWnd::GetWindowRect(&rcWnd);

			CRect rcWorkArea;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);

			m_ptAnchor.x = rcWorkArea.left + (rcWorkArea.Width() - rcWnd.Width()) / 2;
			m_ptAnchor.y = rcWorkArea.bottom - rcWnd.Height();

			SetWindowPos(NULL, m_ptAnchor.x, m_ptAnchor.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	int CInputWnd::GetCandMax(SWindow *pCandContainer) const
	{
		int nRet = 0;
		SWindow * pCand = pCandContainer->GetWindow(GSW_FIRSTCHILD);
		while(pCand)
		{
			if(pCand->IsClass(SCandView::GetClassName()))
			{
				nRet ++;
			}
			pCand = pCand->GetWindow(GSW_NEXTSIBLING);
		}
		return nRet;
	}

	short CInputWnd::SelectCandidate(short iCand)
	{
		if(m_pInputContext->sCandCount == 0) return -1;
		short idx = iCand + m_pInputContext->iCandBegin;
		if(idx >= m_pInputContext->iCandLast) return -1;
		return idx;
	}


	BOOL CInputWnd::GoPrevCandidatePage()
	{
		if(m_cPageSize==0) return FALSE;
		if(m_pInputContext->iCandBegin< m_cPageSize) return FALSE;
		m_pInputContext->iCandBegin -= m_cPageSize;
		return TRUE;
	}


	BOOL CInputWnd::GoNextCandidatePage()
	{
		if(m_pInputContext->iCandLast ==-1) return FALSE;
		if(m_pInputContext->iCandLast >=m_pInputContext->sCandCount) return FALSE;
		m_pInputContext->iCandBegin = m_pInputContext->iCandLast;
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
			Show(FALSE);
			KillTimer(idEvent);
		}else
		{
			SetMsgHandled(FALSE);
		}
	}

	void CInputWnd::OnLButtonDown(UINT nFlags, CPoint point)
	{
		if (m_bFollowCaret)
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
		if (m_bFollowCaret)
		{
			SetMsgHandled(FALSE);
			return;
		}
		m_bDraging = FALSE;
		SWindow::ReleaseCapture();

		CRect rcWnd;
		CSimpleWnd::GetWindowRect(&rcWnd);
		m_ptAnchor = rcWnd.TopLeft();
		//save anchor
		CDataCenter::getSingleton().GetData().m_ptInput = m_ptAnchor;
	}

	void CInputWnd::OnMouseMove(UINT nFlags, CPoint point)
	{
		if (m_bFollowCaret)
		{
			SetMsgHandled(FALSE);
			return;
		}
		__super::OnMouseMove(nFlags, point);
		::SetCursor(GETRESPROVIDER->LoadCursor(_T("sizeall")));
		if (m_bDraging)
		{
			CRect rcWnd;
			CSimpleWnd::GetWindowRect(&rcWnd);
			rcWnd.OffsetRect(point - m_ptClick);
			SetWindowPos(HWND_TOPMOST, rcWnd.left, rcWnd.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
		}

	}

}
