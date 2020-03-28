// fleditor.h : main header file for the FLEDITOR application
//

#if !defined(AFX_FLEDITOR_H__CAFB976A_49D0_444C_9E74_1624BA9B01DB__INCLUDED_)
#define AFX_FLEDITOR_H__CAFB976A_49D0_444C_9E74_1624BA9B01DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFleditorApp:
// See fleditor.cpp for the implementation of this class
//

class CFleditorApp : public CWinApp
{
public:
	CFleditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFleditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFleditorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLEDITOR_H__CAFB976A_49D0_444C_9E74_1624BA9B01DB__INCLUDED_)
