#pragma once
#include "FindDlg.h"
#include "whwindow.h"

class CTextEditorDlg : public SHostDialog, IFindListener
	, public CWHRoundRectFrameHelper<CTextEditorDlg>
	, public SDpiHandler<CTextEditorDlg>
{
public:
	CTextEditorDlg(int nMode,const SStringT & strFileName);
	~CTextEditorDlg();

protected:
	virtual bool OnFind(const SStringT & strText, bool bFindNext, bool bMatchCase, bool bMatchWholeWord);
	virtual void OnReplace(const SStringT &strText);
protected:
	void OnBtnClose();
	void OnBtnSave();
	void OnBtnFind();


	EVENT_MAP_BEGIN()
		EVENT_ID_COMMAND(R.id.btn_close,OnBtnClose)
		EVENT_ID_COMMAND(R.id.btn_save, OnBtnSave)
		EVENT_ID_COMMAND(R.id.btn_find,OnBtnFind)
	EVENT_MAP_END()
	BOOL OnInitDialog(HWND hWnd,LPARAM lp);
	BEGIN_MSG_MAP_EX(CTextEditorDlg)
		CHAIN_MSG_MAP(SDpiHandler<CTextEditorDlg>)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CTextEditorDlg>)
		CHAIN_MSG_MAP(SHostDialog)
	END_MSG_MAP()


	int m_mode;
	SStringT	m_strFileName;
	CScintillaWnd *			 m_pSciter;
	CFindDlg   *			 m_pFindDlg;
};

