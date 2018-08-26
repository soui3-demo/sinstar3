#if !defined(AFX_PLGROUPPROPDLG_H__7FCF8CFD_9FE4_4184_A322_8C7DCA42ACA9__INCLUDED_)
#define AFX_PLGROUPPROPDLG_H__7FCF8CFD_9FE4_4184_A322_8C7DCA42ACA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PLGroupPropDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPLGroupPropDlg dialog

class CPLGroupPropDlg : public CDialog
{
// Construction
public:
	CPLGroupPropDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPLGroupPropDlg)
	enum { IDD = IDD_GROUPPROP };
	CString	m_strRemark;
	CString	m_strName;
	CString	m_strEditor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPLGroupPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPLGroupPropDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLGROUPPROPDLG_H__7FCF8CFD_9FE4_4184_A322_8C7DCA42ACA9__INCLUDED_)
