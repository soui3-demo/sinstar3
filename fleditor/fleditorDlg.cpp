// fleditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fleditor.h"
#include "fleditorDlg.h"
#include "hyperlink.h"
#include "TxtReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_ctrlHomeSite;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_HOMESITE, m_ctrlHomeSite);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFleditorDlg dialog

CFleditorDlg::CFleditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFleditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFleditorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bInitList=FALSE;
}

void CFleditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFleditorDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProg);
	DDX_Control(pDX, IDC_LIST_FLMAP, m_lstFLMap);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFleditorDlg, CDialog)
	//{{AFX_MSG_MAP(CFleditorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_GROUPPROP, OnGroupprop)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FLMAP, OnItemchangedListFlmap)
	ON_BN_CLICKED(IDC_LOADCUR, OnLoadcur)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFleditorDlg message handlers
void FLEditProgCallback(PROGTYPE type,DWORD dwData,LPARAM lParam)
{
	CFleditorDlg *pDlg=(CFleditorDlg*)lParam;
	switch(type)
	{
	case PT_MAX:
		pDlg->m_ctrlProg.SetRange32(0,dwData);
		pDlg->m_ctrlProg.SetPos(0);
		break;
	case PT_PROG:
		pDlg->m_ctrlProg.SetPos(dwData);
		break;
	}
}

BOOL CFleditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_lstFLMap.SetExtendedStyle(LVS_EX_CHECKBOXES);
	m_lstFLMap.InsertColumn(0,_T("组名称"),0,120);
	m_lstFLMap.InsertColumn(1,_T("容量"),0,60);
	m_lstFLMap.InsertColumn(2,_T("编辑"),0,120);
	m_lstFLMap.InsertColumn(3,_T("备注"),0,120);
	
	m_flmEditor.SetProgCallBack(FLEditProgCallback,(LPARAM)this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFleditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFleditorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFleditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFleditorDlg::OnClear() 
{
	m_lstFLMap.DeleteAllItems();
	m_flmEditor.Free();
}

void CFleditorDlg::OnImport() 
{
	CFileDialog dlg(TRUE,_T("txt"),NULL,0,_T("文本文件(*.txt)|*.txt||"));
	if(dlg.DoModal()==IDOK)
	{
		CTxtReader f(L'#');
		if(f.Open(dlg.GetPathName()))
		{
			WCHAR szLine[500];
			BYTE byGroup=m_flmEditor.AddGroup(dlg.GetFileTitle(),L"",L"");
			m_ctrlProg.SetRange32(0,f.getLength());
			m_ctrlProg.SetPos(0);
			BOOL bReaded=f.getline(szLine,500);
			while(bReaded)
			{
				if(szLine[0]=='@')
				{
					if(wcsncmp(szLine+1,L"名称=",5)==0)
					{
						wcscpy(m_flmEditor.GetGroup()->GetPtAt(byGroup)->szName,szLine+5+1);
					}else if(wcsncmp(szLine+1,L"编辑=",5)==0)
					{
						wcscpy(m_flmEditor.GetGroup()->GetPtAt(byGroup)->szEditor,szLine+5+1);
					}else if(wcsncmp(szLine+1,L"备注=",5)==0)
					{
						wcscpy(m_flmEditor.GetGroup()->GetPtAt(byGroup)->szRemark,szLine+5+1);
					}
				}else 
				{
					WCHAR szKey[100],szAdd[100];
					CStringArray  cnItem;
					WCHAR *pe=wcsstr(szLine,L"#");
					ASSERT(pe);
					wcsncpy(szKey,szLine,pe-szLine);
					szKey[pe-szLine]=0;
					WCHAR *pb=pe+1;
					pe=wcsstr(pb,L"|");
					ASSERT(pe);
					wcsncpy(szAdd,pb,pe-pb);
					szAdd[pe-pb]=0;
					pb=pe+1;
					while(1)
					{
						pe=wcsstr(pb,L"|");
						if(pe)
						{
							cnItem.Add(CStringW(pb,pe-pb));
							pb=pe+1;
						}else
						{
							cnItem.Add(pb);
							break;
						}
					}
					m_flmEditor.AddItem(byGroup,szKey,szAdd,&cnItem);
				}
				bReaded=f.getline(szLine,500);
				m_ctrlProg.SetPos(f.getReadPos());
			}
			f.Close();
			m_lstFLMap.InsertItem(byGroup,m_flmEditor.GetGroup()->GetPtAt(byGroup)->szName);
			swprintf(szLine,L"%d",m_flmEditor.GetGroup()->GetPtAt(byGroup)->dwCount);
			m_lstFLMap.SetItemText(byGroup,1,szLine);
			m_lstFLMap.SetItemText(byGroup,2,m_flmEditor.GetGroup()->GetPtAt(byGroup)->szEditor);
			m_lstFLMap.SetItemText(byGroup,3,m_flmEditor.GetGroup()->GetPtAt(byGroup)->szRemark);
		}else
		{
			MessageBox(_T("打开文件失败"));
		}
	}		
}

#include "GroupInfoDlg.h"

void CFleditorDlg::OnGroupprop() 
{
	int nSelGroup=m_lstFLMap.GetSelectionMark();
	if(nSelGroup==-1) return;

	GROUPINFO *pplg=m_flmEditor.GetGroup()->GetPtAt(nSelGroup);
	CGroupInfoDlg propDlg;
	propDlg.m_strName=pplg->szName;
	propDlg.m_strEditor=pplg->szEditor;
	propDlg.m_strRemark=pplg->szRemark;
	if(propDlg.DoModal()==IDOK)
	{
		wcscpy(pplg->szName,propDlg.m_strName);
		m_lstFLMap.SetItemText(nSelGroup,0,pplg->szName);
		wcscpy(pplg->szEditor,propDlg.m_strEditor);
		m_lstFLMap.SetItemText(nSelGroup,2,pplg->szEditor);
		wcscpy(pplg->szRemark,propDlg.m_strRemark);
		m_lstFLMap.SetItemText(nSelGroup,3,pplg->szRemark);
	}	
}

void CFleditorDlg::OnLoad() 
{
	m_lstFLMap.DeleteAllItems();
	m_flmEditor.Free();
	CFileDialog dlg(TRUE,_T("flm"),NULL,0,_T("启程输入之星外文词库(*.flm)|*.flm||"));
	if(dlg.DoModal()==IDOK)
	{
		if(m_flmEditor.Load(dlg.GetPathName()))
		{
			InitList();
		}else
		{
			MessageBox(_T("载入词库失败！"));
		}
	}	
}

void CFleditorDlg::OnSave() 
{
	CFileDialog dlg(FALSE,_T("flm"),NULL,0,_T("启程输入之星外文词库(*.flm)|*.flm"));
	if(dlg.DoModal()==IDOK)
	{
		GetDlgItemText(IDC_NAME,m_flmEditor.m_flmInfo.szName,99);
		GetDlgItemText(IDC_ADDITION,m_flmEditor.m_flmInfo.szAddition,99);
		GetDlgItemText(IDC_ADDFONT,m_flmEditor.m_flmInfo.szAddFont,99);
		if(m_flmEditor.Save(dlg.GetPathName()))
			MessageBox(_T("保存完成"));
		else
			MessageBox(_T("保存失败"));
	}	
}

void CFleditorDlg::InitList()
{
	m_bInitList=TRUE;
	TCHAR szBuf[50];
	m_lstFLMap.DeleteAllItems();
	SetDlgItemText(IDC_NAME,m_flmEditor.m_flmInfo.szName);
	SetDlgItemText(IDC_ADDITION,m_flmEditor.m_flmInfo.szAddition);
	SetDlgItemText(IDC_ADDFONT,m_flmEditor.m_flmInfo.szAddFont);
	for(int i=0;i<m_flmEditor.GetGroup()->GetSize();i++)
	{
		GROUPINFO plgi=m_flmEditor.GetGroup()->GetAt(i);
		m_lstFLMap.InsertItem(i,plgi.szName);
		_stprintf(szBuf,_T("%u"),plgi.dwCount);
		m_lstFLMap.SetItemText(i,1,szBuf);
		m_lstFLMap.SetItemText(i,2,plgi.szEditor);
		m_lstFLMap.SetItemText(i,3,plgi.szRemark);
		m_lstFLMap.SetCheck(i,plgi.bValid);
	}
	m_bInitList=FALSE;
}

void CFleditorDlg::OnItemchangedListFlmap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(!m_bInitList)
	{
		m_flmEditor.GetGroup()->GetPtAt(pNMListView->iItem)->bValid=ListView_GetCheckState(m_lstFLMap.m_hWnd,pNMListView->iItem);
	}		
	*pResult = 0;
}

void CFleditorDlg::OnLoadcur() 
{
	HKEY hKey=NULL;
	TCHAR szServerPath[MAX_PATH]={0},szConfig[MAX_PATH],szFlm[MAX_PATH]={0};
	DWORD dwSize=MAX_PATH;
	//if(!ISComm_GetDataPath(szServerPath))
	{
		MessageBox(_T("没有找到输入之星服务器的安装路径！"));
		return;
	}
	_stprintf(szConfig,_T("%s\\config.ini"),szServerPath);
	if(GetPrivateProfileString(_T("settings"),_T("defflm"),NULL,szFlm,MAX_PATH,szConfig)==0)
	{
		MessageBox(_T("当前没有使用外文词库"));
		return;
	}

	TCHAR szPath[MAX_PATH];
	_stprintf(szPath,_T("%s\\%s.flm"),szServerPath,szFlm);
	OnClear();
	if(m_flmEditor.Load(szPath))
	{
		InitList();
	}else
	{
		MessageBox(_T("载入词库错误"));
	}	
}
