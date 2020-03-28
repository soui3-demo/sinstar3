// spelltool.h : main header file for the SPELLTOOL application
//

#if !defined(AFX_SPELLTOOL_H__6588C077_797B_4F1A_9D84_EBD5FFF04F69__INCLUDED_)
#define AFX_SPELLTOOL_H__6588C077_797B_4F1A_9D84_EBD5FFF04F69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSpelltoolApp:
// See spelltool.cpp for the implementation of this class
//

class CSpelltoolApp : public CWinApp
{
public:
	CSpelltoolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpelltoolApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSpelltoolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPELLTOOL_H__6588C077_797B_4F1A_9D84_EBD5FFF04F69__INCLUDED_)
