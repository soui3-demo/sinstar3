// GroupInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fleditor.h"
#include "GroupInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupInfoDlg dialog


CGroupInfoDlg::CGroupInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGroupInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGroupInfoDlg)
	m_strRemark = _T("");
	m_strName = _T("");
	m_strEditor = _T("");
	//}}AFX_DATA_INIT
}


void CGroupInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupInfoDlg)
	DDX_Text(pDX, IDC_REMARK, m_strRemark);
	DDV_MaxChars(pDX, m_strRemark, 200);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 50);
	DDX_Text(pDX, IDC_EDITOR, m_strEditor);
	DDV_MaxChars(pDX, m_strEditor, 50);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CGroupInfoDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupInfoDlg message handlers
