#include "stdafx.h"
#include "TextEditorDlg.h"

CTextEditorDlg::CTextEditorDlg(int nMode, const SStringT & strFileName)
	:SHostDialog(UIRES.LAYOUT.dlg_texteditor)
	,m_mode(nMode- FU_USERDEF)
	,m_strFileName(strFileName)
	, m_pSciter(NULL)
	, m_pFindDlg(NULL)
{
}


CTextEditorDlg::~CTextEditorDlg()
{
}

BOOL CTextEditorDlg::OnInitDialog(HWND hWnd, LPARAM lp)
{
	FindChildByID(R.id.txt_edit_userdef + m_mode)->SetVisible(TRUE, TRUE);
	FindChildByID(R.id.tip_edit_userdef + m_mode)->SetVisible(TRUE, TRUE);
	SRealWnd * pRealWnd = FindChildByID2<SRealWnd>(R.id.real_scilexer);
	SASSERT(pRealWnd);
	m_pSciter = (CScintillaWnd *)pRealWnd->GetUserData();
	m_pSciter->SendMessage(SCI_USEPOPUP, 0, 0);

	FILE *f = _tfopen(m_strFileName, _T("rb"));
	if (f)
	{
		fseek(f, 0, SEEK_END);
		int nLen = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *buf = (char*)malloc(nLen + 1);
		fread(buf, 1, nLen, f);
		fclose(f);
		buf[nLen] = 0;
		SStringA utf8 = S_CA2A(buf, CP_ACP, CP_UTF8);
		free(buf);
		m_pSciter->SendMessage(SCI_SETTEXT, 0, (LPARAM)utf8.c_str());
	}
	m_pSciter->UpdateLineNumberWidth();

	return 0;
}

void CTextEditorDlg::OnBtnClose()
{
	EndDialog(IDCANCEL);
}

void CTextEditorDlg::OnBtnSave()
{
	LONG nLen = m_pSciter->SendMessage(SCI_GETTEXTLENGTH, 0,0);
	char *buf = new char[nLen + 1];
	m_pSciter->SendMessage(SCI_GETTEXT, nLen+1, (LPARAM)buf);
	buf[nLen] = 0;
	SStringA str = S_CA2A(SStringA(buf, nLen), CP_UTF8, CP_ACP);
	delete[]buf;
	FILE *f = _tfopen(m_strFileName, _T("w"));
	if (f)
	{
		fwrite(str.c_str(), 1,str.GetLength(), f);
		fclose(f);
	}
	EndDialog(IDOK);
}

void CTextEditorDlg::OnBtnFind()
{
	if(m_pFindDlg==NULL)
	{
		m_pFindDlg = new CFindDlg(this);
		m_pFindDlg->Create(m_hWnd);
		m_pFindDlg->CenterWindow(m_pSciter->m_hWnd);
	}		
	m_pFindDlg->ShowWindow(SW_SHOW);
}

bool CTextEditorDlg::OnFind(const SStringT & strText, bool bFindNext, bool bMatchCase, bool bMatchWholeWord)
{
	int flags = (bMatchCase?SCFIND_MATCHCASE:0) | (bMatchWholeWord?SCFIND_WHOLEWORD:0);
	TextToFind ttf;
	ttf.chrg.cpMin = m_pSciter->SendMessage(SCI_GETCURRENTPOS);
	if(bFindNext)
		ttf.chrg.cpMax = m_pSciter->SendMessage(SCI_GETLENGTH, 0, 0);
	else
		ttf.chrg.cpMax = 0;

	SStringA strUtf8 = S_CT2A(strText,CP_UTF8);
	ttf.lpstrText = (char *)(LPCSTR) strUtf8;
	int nPos = m_pSciter->SendMessage(SCI_FINDTEXT,flags,(LPARAM)&ttf);
	if(nPos==-1) return false;

	if(bFindNext)
		m_pSciter->SendMessage(SCI_SETSEL,nPos,nPos + strUtf8.GetLength());
	else
		m_pSciter->SendMessage(SCI_SETSEL,nPos+ strUtf8.GetLength(),nPos);

	m_pSciter->SetFocus();
	return true;
}

void CTextEditorDlg::OnReplace(const SStringT &strText)
{
	SStringA strUtf8 = S_CT2A(strText,CP_UTF8);
	m_pSciter->SendMessage(SCI_REPLACESEL,0,(LPARAM)strUtf8.c_str());
}
