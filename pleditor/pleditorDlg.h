// pleditorDlg.h : header file
//

#if !defined(AFX_PLEDITORDLG_H__30109CA8_6AA7_4F85_BC74_F0082B098203__INCLUDED_)
#define AFX_PLEDITORDLG_H__30109CA8_6AA7_4F85_BC74_F0082B098203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPleditorDlg dialog
#include "phraselib.h"
class CPleditorDlg : public CDialog
{
// Construction
public:
	void InitList();
	CPleditorDlg(CWnd* pParent = NULL);	// standard constructor
	CPLEditor	m_plEditor;
	BOOL	m_bInitList;
// Dialog Data
	//{{AFX_DATA(CPleditorDlg)
	enum { IDD = IDD_PLEDITOR_DIALOG };
	CProgressCtrl	m_ctrlProg;
	CListCtrl	m_lstSysPL;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPleditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPleditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClear();
	afx_msg void OnImport();
	afx_msg void OnMerger();
	afx_msg void OnGroupprop();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnItemchangedListSyspl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLoadcur();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedInstall();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLEDITORDLG_H__30109CA8_6AA7_4F85_BC74_F0082B098203__INCLUDED_)
