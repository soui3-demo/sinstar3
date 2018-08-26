// pleditor.h : main header file for the PLEDITOR application
//

#if !defined(AFX_PLEDITOR_H__FE957EEB_855C_461F_B92D_8B75AF1E5E65__INCLUDED_)
#define AFX_PLEDITOR_H__FE957EEB_855C_461F_B92D_8B75AF1E5E65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPleditorApp:
// See pleditor.cpp for the implementation of this class
//

class CPleditorApp : public CWinApp
{
public:
	CPleditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPleditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPleditorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLEDITOR_H__FE957EEB_855C_461F_B92D_8B75AF1E5E65__INCLUDED_)
