#if !defined(AFX_GROUPINFODLG_H__012A8939_357C_4324_92FE_F24E7AFEB770__INCLUDED_)
#define AFX_GROUPINFODLG_H__012A8939_357C_4324_92FE_F24E7AFEB770__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GroupInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupInfoDlg dialog

class CGroupInfoDlg : public CDialog
{
// Construction
public:
	CGroupInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGroupInfoDlg)
	enum { IDD = IDD_GROUPPROP };
	CString	m_strRemark;
	CString	m_strName;
	CString	m_strEditor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGroupInfoDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GROUPINFODLG_H__012A8939_357C_4324_92FE_F24E7AFEB770__INCLUDED_)
