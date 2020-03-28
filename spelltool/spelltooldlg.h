// spelltoolDlg.h : header file
//

#if !defined(AFX_SPELLTOOLDLG_H__A299EF48_E76A_4475_831C_44DC4AE3A8A3__INCLUDED_)
#define AFX_SPELLTOOLDLG_H__A299EF48_E76A_4475_831C_44DC4AE3A8A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSpelltoolDlg dialog


#include "TxtReader.h"
#include "spellengine.h"

class CSpelltoolDlg : public CDialog
{
// Construction
public:
	CSpelltoolDlg(CWnd* pParent = NULL);	// standard constructor
	~CSpelltoolDlg()
	{
	}
	CSpellEngine	m_spellEngine;
// Dialog Data
	//{{AFX_DATA(CSpelltoolDlg)
	enum { IDD = IDD_SPELLTOOL_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpelltoolDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSpelltoolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnImportSingle();
	afx_msg void OnDestroy();
	afx_msg void OnImportPhrase();
	afx_msg void OnSetdefspell();
	afx_msg void OnOpen();
	afx_msg void OnSave();
	afx_msg void OnInitRate() ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPELLTOOLDLG_H__A299EF48_E76A_4475_831C_44DC4AE3A8A3__INCLUDED_)
