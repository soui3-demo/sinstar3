#if !defined(AFX_TPCOMPEDITOR_H__C7CB8710_B4ED_409F_8761_A71F790FC992__INCLUDED_)
#define AFX_TPCOMPEDITOR_H__C7CB8710_B4ED_409F_8761_A71F790FC992__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TPCompEditor.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CTPCompEditor dialog
#include "BitmapWnd.h"
#include "colorCtrl.h"
class CTPCompEditor : public CDialog
{
// Construction
public:
	CTPCompEditor(CWnd *pParentWnd=NULL);   // standard constructor
	char m_szLicenseMD5[33];
	CString	m_strKeyDll;
// Dialog Data
	//{{AFX_DATA(CTPCompEditor)
	enum { IDD = IDD_TP_COMPEDITOR };
	CColorCtrl	m_ctrlColor;
	CBitmapWnd	m_bmwIcon;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTPCompEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg LRESULT OnBMWndClick(WPARAM wParam,LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CTPCompEditor)
	afx_msg void OnBrowsecomp();
	afx_msg void OnBrowseicon();
	afx_msg void OnBrowseSave();
	afx_msg void OnMake();
	afx_msg void OnBrowseKeymap();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeComp();
	afx_msg void OnLicense();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExport();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TPCOMPEDITOR_H__C7CB8710_B4ED_409F_8761_A71F790FC992__INCLUDED_)
