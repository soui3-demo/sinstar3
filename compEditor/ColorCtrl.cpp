// ColorCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ColorCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorCtrl

CColorCtrl::CColorCtrl()
{
	m_crColor=-1;
}

CColorCtrl::~CColorCtrl()
{
}


BEGIN_MESSAGE_MAP(CColorCtrl, CStatic)
	//{{AFX_MSG_MAP(CColorCtrl)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorCtrl message handlers

void CColorCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	dc.SetBkMode(TRANSPARENT);
	if(m_crColor!=-1)
	{
		CRect rc;
		GetClientRect(&rc);
		dc.FillSolidRect(&rc,m_crColor);
	}
	// Do not call CStatic::OnPaint() for painting messages
}

void CColorCtrl::OnDestroy() 
{
	CStatic::OnDestroy();
	m_crColor=-1;
	// TODO: Add your message handler code here
	
}
