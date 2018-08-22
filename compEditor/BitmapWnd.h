// BitmapWnd.h: interface for the CBitmapWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITMAPWND_H__6099C91A_F69C_43D6_B68C_FED395664299__INCLUDED_)
#define AFX_BITMAPWND_H__6099C91A_F69C_43D6_B68C_FED395664299__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UM_BMWND_CLICK	(WM_USER+50)
class CBitmapWnd : public CStatic  
{
public:
	CBitmapWnd();
	virtual ~CBitmapWnd();

	BOOL SetBitmapFile(LPCTSTR pszFileName);

	POINT		m_ptSel;
	COLORREF	m_crSel;
	CString		m_strFileName;
private:
	
	HBITMAP	m_hBmp;

protected:
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_BITMAPWND_H__6099C91A_F69C_43D6_B68C_FED395664299__INCLUDED_)
