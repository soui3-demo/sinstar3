// fleditorDlg.h : header file
//

#if !defined(AFX_FLEDITORDLG_H__B525EB7E_9C9F_47FC_B183_58B384481C90__INCLUDED_)
#define AFX_FLEDITORDLG_H__B525EB7E_9C9F_47FC_B183_58B384481C90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFleditorDlg dialog
#include "foreignlangmap.h"
class CFleditorDlg : public CDialog
{
// Construction
public:
	void InitList();
	CFleditorDlg(CWnd* pParent = NULL);	// standard constructor
	CFLMEditor		m_flmEditor;
	BOOL	m_bInitList;
// Dialog Data
	//{{AFX_DATA(CFleditorDlg)
	enum { IDD = IDD_FLEDITOR_DIALOG };
	CProgressCtrl	m_ctrlProg;
	CListCtrl	m_lstFLMap;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFleditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFleditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClear();
	afx_msg void OnImport();
	afx_msg void OnGroupprop();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnItemchangedListFlmap(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLoadcur();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLEDITORDLG_H__B525EB7E_9C9F_47FC_B183_58B384481C90__INCLUDED_)
