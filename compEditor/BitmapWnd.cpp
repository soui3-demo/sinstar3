// BitmapWnd.cpp: implementation of the CBitmapWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BitmapWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBitmapWnd::CBitmapWnd()
{
	m_hBmp=NULL;
	m_ptSel.x=m_ptSel.y=0;
	m_crSel=-1;
}

CBitmapWnd::~CBitmapWnd()
{
	if(m_hBmp) DeleteObject(m_hBmp);
}

BEGIN_MESSAGE_MAP(CBitmapWnd, CStatic)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CBitmapWnd::SetBitmapFile(LPCTSTR pszFileName)
{
	if(m_hBmp) DeleteObject(m_hBmp);
	m_hBmp=(HBITMAP)LoadImage(NULL,pszFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(!m_hBmp)
	{
		Invalidate();
		return FALSE;
	}
	Invalidate(FALSE);
	m_strFileName=pszFileName;
	return TRUE;
}

void CBitmapWnd::OnDestroy() 
{
	if(m_hBmp)
	{
		DeleteObject(m_hBmp);
		m_hBmp=NULL;
	}
	m_ptSel.x=m_ptSel.y=0;
	m_crSel=-1;
	CStatic::OnDestroy();
}

void CBitmapWnd::OnPaint()
{
	CPaintDC dc(this);
	HDC memdc=CreateCompatibleDC(dc);
	HGDIOBJ hOldBmp=SelectObject(memdc,m_hBmp);
	COLORREF cr;
	RECT rcCell,rc;
	int i,j;
	GetClientRect(&rc);
	for( i=0;i<16;i++)
	{
		rcCell.top=i*rc.bottom/16;
		rcCell.bottom=(i+1)*rc.bottom/16;
		for( j=0;j<16;j++)
		{
			if(m_hBmp)
				cr=GetPixel(memdc,j,i);
			else
				cr=RGB(255,255,255);
			rcCell.left=j*rc.right/16;
			rcCell.right=(j+1)*rc.right/16;
			dc.FillSolidRect(&rcCell,cr);
		}
	}
	CPen pen;
	pen.CreatePen(PS_SOLID,1,RGB(192,192,192));
	CPen *pOldPen=dc.SelectObject(&pen);
	for( i=1;i<16;i++)
	{
		dc.MoveTo(rc.right*i/16,rc.top);
		dc.LineTo(rc.right*i/16,rc.bottom);
		dc.MoveTo(rc.left,rc.bottom*i/16);
		dc.LineTo(rc.right,rc.bottom*i/16);
	}
	if(m_hBmp)
	{
		rcCell.top=m_ptSel.y*rc.bottom/16;
		rcCell.bottom=(m_ptSel.y+1)*rc.bottom/16;
		rcCell.left=m_ptSel.x*rc.right/16;
		rcCell.right=(m_ptSel.x+1)*rc.right/16;
		dc.DrawFocusRect(&rcCell);
	}
	dc.SelectObject(pOldPen);
	SelectObject(memdc,hOldBmp);
	DeleteDC(memdc);
}

void CBitmapWnd::OnLButtonDown(UINT nFlags,CPoint point)
{
	if(!m_hBmp) return;
	RECT rc;
	GetClientRect(&rc);
	m_ptSel.x=point.x*16/rc.right;
	m_ptSel.y=point.y*16/rc.bottom;
	CDC *pDC=GetDC();
	HDC memdc=CreateCompatibleDC(pDC->m_hDC);
	HGDIOBJ hOldBmp=SelectObject(memdc,m_hBmp);
	m_crSel=GetPixel(memdc,m_ptSel.x,m_ptSel.y);
	SelectObject(memdc,hOldBmp);
	DeleteDC(memdc);
	ReleaseDC(pDC);
	GetParent()->SendMessage(UM_BMWND_CLICK,(WPARAM)m_crSel);
}
