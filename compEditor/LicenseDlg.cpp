// LicenseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LicenseDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg dialog


CLicenseDlg::CLicenseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLicenseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLicenseDlg)
	m_strLicenseDll = _T("");
	//}}AFX_DATA_INIT
}


void CLicenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseDlg)
	DDX_Text(pDX, IDC_LICENSE, m_strLicenseDll);
	DDV_MaxChars(pDX, m_strLicenseDll, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLicenseDlg, CDialog)
	//{{AFX_MSG_MAP(CLicenseDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg message handlers

void CLicenseDlg::OnBrowse() 
{
	CFileDialog openDlg(TRUE,"dll",NULL,0,"启程编码授权文件(*.dll)|*.dll||");
	if(openDlg.DoModal()==IDOK)
	{
		SetDlgItemText(IDC_LICENSE,openDlg.GetPathName());
	}
}
