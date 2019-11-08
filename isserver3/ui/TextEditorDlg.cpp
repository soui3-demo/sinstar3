#include "stdafx.h"
#include "TextEditorDlg.h"


CTextEditorDlg::CTextEditorDlg(int nMode, const SStringT & strFileName)
	:SHostDialog(UIRES.LAYOUT.dlg_texteditor)
	,m_mode(nMode- FU_USERDEF)
	,m_strFileName(strFileName)
	, m_pSciter(NULL)
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
	return 0;
}

void CTextEditorDlg::OnClose()
{
	EndDialog(IDCANCEL);
}

void CTextEditorDlg::OnSave()
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