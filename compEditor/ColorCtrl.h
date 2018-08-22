#if !defined(AFX_COLORCTRL_H__524D8C7E_3814_4BE7_A5A5_954C135DFDDC__INCLUDED_)
#define AFX_COLORCTRL_H__524D8C7E_3814_4BE7_A5A5_954C135DFDDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorCtrl window

class CColorCtrl : public CStatic
{
// Construction
public:
	CColorCtrl();

// Attributes
public:
	COLORREF	m_crColor;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorCtrl)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCTRL_H__524D8C7E_3814_4BE7_A5A5_954C135DFDDC__INCLUDED_)
