// pleditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pleditor.h"
#include "pleditorDlg.h"
#include "txtlinereader.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
#include "hyperlink.h"
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_ctrlHome;
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
	DDX_Control(pDX, IDC_HOMESITE, m_ctrlHome);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPleditorDlg dialog

CPleditorDlg::CPleditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPleditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPleditorDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bInitList=FALSE;
}

void CPleditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPleditorDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProg);
	DDX_Control(pDX, IDC_LIST_SYSPL, m_lstSysPL);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPleditorDlg, CDialog)
	//{{AFX_MSG_MAP(CPleditorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	ON_BN_CLICKED(IDC_MERGER, OnMerger)
	ON_BN_CLICKED(IDC_GROUPPROP, OnGroupprop)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SYSPL, OnItemchangedListSyspl)
	ON_BN_CLICKED(IDC_LOADCUR, OnLoadcur)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_INSTALL, &CPleditorDlg::OnBnClickedInstall)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPleditorDlg message handlers
void PLEditProgCallback(PROGTYPE type,DWORD dwData,LPARAM lParam)
{
	CPleditorDlg *pDlg=(CPleditorDlg*)lParam;
	switch(type)
	{
	case PT_MAX:
		pDlg->m_ctrlProg.SetRange32(0,dwData);
		break;
	case PT_PROG:
		pDlg->m_ctrlProg.SetPos(dwData);
		break;
	}
}
BOOL CPleditorDlg::OnInitDialog()
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
	m_lstSysPL.SetExtendedStyle(LVS_EX_CHECKBOXES);
	m_lstSysPL.InsertColumn(0,"组名称",0,120);
	m_lstSysPL.InsertColumn(1,"容量",0,60);
	m_lstSysPL.InsertColumn(2,"编辑",0,120);
	m_lstSysPL.InsertColumn(3,"备注",0,120);
	SetDlgItemInt(IDC_RATE_MIN,0);
	SetDlgItemInt(IDC_RATE_MAX,250);
	m_plEditor.SetProgCallBack(PLEditProgCallback,(LPARAM)this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPleditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPleditorDlg::OnPaint() 
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
HCURSOR CPleditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPleditorDlg::OnClear() 
{
	m_plEditor.Free();	
	m_lstSysPL.DeleteAllItems();	
}

//解析文本行
//LPCTSTR pszLine:输入文本行
//int &nBegin:词频开始位置
//int &nEnd:词结束位置
BOOL ParseLine(LPCTSTR pszLine,int &nBegin,int &nEnd)
{
	int i=0;
	while(pszLine[i])
	{
		if(pszLine[i]&0x80)
			i+=2;
		else if(pszLine[i]=='\t' || pszLine[i]==0x20)
		{
			nEnd=i;
			break;
		}else
		{
			i++;
		}
	}
	if(pszLine[i])
	{
		while(pszLine[i] && (pszLine[i]=='\t' || pszLine[i]==0x20)) i++;
		nBegin=i;
		return TRUE;
	}
	return FALSE;
}

void CPleditorDlg::OnImport() 
{
	CFileDialog dlg(TRUE,"txt",NULL,0,"文本文件(*.txt)|*.txt||");
	if(dlg.DoModal()==IDOK)
	{
		BYTE byMin=(BYTE)GetDlgItemInt(IDC_RATE_MIN,NULL,FALSE);
		BYTE byMax=(BYTE)GetDlgItemInt(IDC_RATE_MAX,NULL,FALSE);
		CTxtLineReader f('#');
		if(f.Open(dlg.GetPathName()))
		{
			char szLine[100];
			LPSTR pszBuf=f.ReadString(szLine,100);
			int nEnd,nBegin;
			BYTE byGroup=m_plEditor.AddGroup(dlg.GetFileTitle(),"","");
			m_ctrlProg.SetRange32(0,f.m_file.GetLength());
			m_ctrlProg.SetPos(0);
			while(pszBuf)
			{
				if(szLine[0]=='@')
				{
					if(strncmp(szLine+1,"名称=",5)==0)
					{
						strcpy(m_plEditor.GetGroup()->GetPtAt(byGroup)->szName,szLine+5+1);
					}else if(strncmp(szLine+1,"编辑=",5)==0)
					{
						strcpy(m_plEditor.GetGroup()->GetPtAt(byGroup)->szEditor,szLine+5+1);
					}else if(strncmp(szLine+1,"备注=",5)==0)
					{
						strcpy(m_plEditor.GetGroup()->GetPtAt(byGroup)->szRemark,szLine+5+1);
					}
				}else 
				{
					BYTE byRate=0;
					if(ParseLine(szLine,nBegin,nEnd))
					{
						 byRate=atoi(szLine+nBegin);
						 if(byRate>MAX_RATE) byRate=MAX_RATE;
					}else
					{
						nEnd=strlen(szLine);
					}
					if(byMin<=byRate && byRate<=byMax) m_plEditor.AddWord(szLine,nEnd,byRate,TRUE,byGroup);
				}
				pszBuf=f.ReadString(szLine,100);
				m_ctrlProg.SetPos(f.m_file.GetPosition());
			}
			f.Close();
			m_bInitList=TRUE;
			m_lstSysPL.InsertItem(byGroup,m_plEditor.GetGroup()->GetPtAt(byGroup)->szName);
			m_bInitList=FALSE;
			sprintf(szLine,"%d",m_plEditor.GetGroup()->GetPtAt(byGroup)->dwCount);
			m_lstSysPL.SetItemText(byGroup,1,szLine);
			m_lstSysPL.SetItemText(byGroup,2,m_plEditor.GetGroup()->GetPtAt(byGroup)->szEditor);
			m_lstSysPL.SetItemText(byGroup,3,m_plEditor.GetGroup()->GetPtAt(byGroup)->szRemark);
		}else
		{
			MessageBox("打开文件失败");
		}
	}	
}

void CPleditorDlg::OnMerger() 
{
	int nSelGroup=m_lstSysPL.GetSelectionMark();
	if(nSelGroup==-1) return;
	CFileDialog dlg(TRUE,"txt",NULL,0,"文本文件(*.txt)|*.txt||");
	if(dlg.DoModal()==IDOK)
	{
		BYTE byMin=(BYTE)GetDlgItemInt(IDC_RATE_MIN,NULL,FALSE);
		BYTE byMax=(BYTE)GetDlgItemInt(IDC_RATE_MAX,NULL,FALSE);
		CTxtLineReader f('#');
		if(f.Open(dlg.GetPathName()))
		{
			char szLine[100];
			LPSTR pszBuf=f.ReadString(szLine,100);
			int nEnd,nBegin;
			BYTE byGroup=(BYTE)nSelGroup;
			m_ctrlProg.SetRange32(0,f.m_file.GetLength());
			m_ctrlProg.SetPos(0);
			while(pszBuf)
			{
				if(szLine[0]!='@')
				{
					BYTE byRate=0;
					if(ParseLine(szLine,nBegin,nEnd))
					{
						 byRate=atoi(szLine+nBegin);
						 if(byRate>MAX_RATE) byRate=MAX_RATE;
					}else
					{
						nEnd=strlen(szLine);
					}
					if(byMin<=byRate && byRate<=byMax ) m_plEditor.AddWord(szLine,nEnd,byRate,TRUE,byGroup);
				}
				pszBuf=f.ReadString(szLine,100);
				m_ctrlProg.SetPos(f.m_file.GetPosition());
			}
			f.Close();
			sprintf(szLine,"%d",m_plEditor.GetGroup()->GetPtAt(byGroup)->dwCount);
			m_lstSysPL.SetItemText(byGroup,1,szLine);
		}else
		{
			MessageBox("打开文件失败");
		}
	}		
}

#include "plgrouppropdlg.h"
void CPleditorDlg::OnGroupprop() 
{
	int nSelGroup=m_lstSysPL.GetSelectionMark();
	if(nSelGroup==-1) return;

	GROUPINFO *pplg=m_plEditor.GetGroup()->GetPtAt(nSelGroup);
	CPLGroupPropDlg propDlg;
	propDlg.m_strName=pplg->szName;
	propDlg.m_strEditor=pplg->szEditor;
	propDlg.m_strRemark=pplg->szRemark;
	if(propDlg.DoModal()==IDOK)
	{
		strcpy(pplg->szName,propDlg.m_strName);
		m_lstSysPL.SetItemText(nSelGroup,0,pplg->szName);
		strcpy(pplg->szEditor,propDlg.m_strEditor);
		m_lstSysPL.SetItemText(nSelGroup,2,pplg->szEditor);
		strcpy(pplg->szRemark,propDlg.m_strRemark);
		m_lstSysPL.SetItemText(nSelGroup,3,pplg->szRemark);
	}	
}



void CPleditorDlg::OnLoad() 
{
	m_lstSysPL.DeleteAllItems();
	m_plEditor.Free();
	CFileDialog dlg(TRUE,"spl",NULL,0,"启程输入之星词库文件(*.spl)|*.spl||");
	if(dlg.DoModal()==IDOK)
	{
		if(m_plEditor.Load(dlg.GetPathName()))
		{
			InitList();
		}else
		{
			MessageBox("载入词库失败！");
		}
	}
}

void CPleditorDlg::OnSave() 
{
	CFileDialog dlg(FALSE,"spl",NULL,0,"启程输入法词库文件(*.spl)|*.spl||");
	if(dlg.DoModal()==IDOK)
	{
		if(m_plEditor.Save(dlg.GetPathName()))
			MessageBox("保存完成");
		else
			MessageBox("保存失败");
	}
}

void CPleditorDlg::InitList()
{
	m_bInitList=TRUE;
	char szBuf[50];
	m_lstSysPL.DeleteAllItems();
	for(int i=0;i<m_plEditor.GetGroup()->GetSize();i++)
	{
		GROUPINFO plgi=m_plEditor.GetGroup()->GetAt(i);
		m_lstSysPL.InsertItem(i,plgi.szName);
		sprintf(szBuf,"%u",plgi.dwCount);
		m_lstSysPL.SetItemText(i,1,szBuf);
		m_lstSysPL.SetItemText(i,2,plgi.szEditor);
		m_lstSysPL.SetItemText(i,3,plgi.szRemark);
		m_lstSysPL.SetCheck(i,plgi.bValid);
	}
	m_bInitList=FALSE;
}

void CPleditorDlg::OnItemchangedListSyspl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(!m_bInitList)
	{
		m_plEditor.GetGroup()->GetPtAt(pNMListView->iItem)->bValid=ListView_GetCheckState(m_lstSysPL.m_hWnd,pNMListView->iItem);
	}	
	*pResult = 0;
}

void CPleditorDlg::OnLoadcur() 
{
	char szServerPath[MAX_PATH]={0};
	if(!ISComm_GetDataPathA(szServerPath,MAX_PATH))
	{
		MessageBox("没有找到输入之星的安装路径");
		return;
	}
	
	char szPath[MAX_PATH];
	sprintf(szPath,"%s\\default.spl",szServerPath);
	OnClear();
	if(m_plEditor.Load(szPath))
	{
		InitList();
	}else
	{
		MessageBox("载入词库错误");
	}
}


void CPleditorDlg::OnBnClickedInstall()
{
	CFileDialog dlg(TRUE, "spl", NULL, 0, "启程输入法词库文件(*.spl)|*.spl||");
	if (dlg.DoModal() == IDOK)
	{
		ISComm_Login(m_hWnd);
		wchar_t wszName[MAX_PATH];
		int nLen = MultiByteToWideChar(CP_ACP, 0, dlg.GetPathName(), dlg.GetPathName().GetLength(), wszName, MAX_PATH);
		char szNameUtf8[MAX_PATH];
		nLen = WideCharToMultiByte(CP_UTF8, 0, wszName, nLen, szNameUtf8, MAX_PATH, NULL, NULL);
		szNameUtf8[nLen] = 0;
		DWORD dwRet = ISComm_InstallPhraseLib(szNameUtf8);
		if (dwRet == ISACK_ERROR)
		{
			MessageBox("安装失败");
		}
		else if (dwRet == ISACK_UNINIT)
		{
			MessageBox("服务器正在加载数据，请稍后重试");
		}
		else if (dwRet == ISACK_SUCCESS)
		{
			MessageBox("安装成功");
		}
		ISComm_Logout(m_hWnd);
	}

}
