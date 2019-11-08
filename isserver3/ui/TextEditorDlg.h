#pragma once

class CTextEditorDlg : public SHostDialog
{
public:
	CTextEditorDlg(int nMode,const SStringT & strFileName);
	~CTextEditorDlg();

protected:
	void OnClose();
	void OnSave();

	EVENT_MAP_BEGIN()
		EVENT_ID_COMMAND(R.id.btn_close,OnClose)
		EVENT_ID_COMMAND(R.id.btn_save, OnSave)
	EVENT_MAP_END()
	BOOL OnInitDialog(HWND hWnd,LPARAM lp);
	BEGIN_MSG_MAP_EX(CTextEditorDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(SHostDialog)
	END_MSG_MAP()

	int m_mode;
	SStringT	m_strFileName;
	CScintillaWnd *			 m_pSciter;
};

