// spelltoolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "spelltool.h"
#include "spelltoolDlg.h"

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
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpelltoolDlg dialog

CSpelltoolDlg::CSpelltoolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpelltoolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpelltoolDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpelltoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpelltoolDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpelltoolDlg, CDialog)
	//{{AFX_MSG_MAP(CSpelltoolDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_IMPORT_SINGLE, OnImportSingle)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_IMPORT_PHRASE, OnImportPhrase)
	ON_BN_CLICKED(IDC_SETDEFSPELL, OnSetdefspell)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_INITRATE, OnInitRate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpelltoolDlg message handlers

BOOL CSpelltoolDlg::OnInitDialog()
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
	SetClassLong(m_hWnd,GCL_STYLE,GetClassLong(m_hWnd,GCL_STYLE)|0x00020000);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSpelltoolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSpelltoolDlg::OnPaint() 
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
HCURSOR CSpelltoolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void CSpelltoolDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}


//导入拼音汉字表
void CSpelltoolDlg::OnSetdefspell() 
{
	CFileDialog openDlg(TRUE,_T("Txt"),_T("拼音汉字表"),0,_T("text file(*.txt)|*.txt||"));
	if(openDlg.DoModal()==IDCANCEL) return;
	m_spellEngine.ImportSpell2Word(openDlg.GetPathName());
}

//导入汉字拼音表
void CSpelltoolDlg::OnImportSingle() 
{
	CFileDialog openDlg(TRUE,_T("txt"),_T("汉字拼音表"),0,_T("text file(*.txt)|*.txt||"));
	if(openDlg.DoModal()==IDCANCEL) return;
	m_spellEngine.ImportWord2Spell(openDlg.GetPathName());
}


//----------------------------------------------------------
//	利用制作的词频表初始化词频
//----------------------------------------------------------
void CSpelltoolDlg::OnInitRate() 
{
	CFileDialog openDlg(TRUE,_T("txt"),_T("字频表250"),0,_T("text file(*.txt)|*.txt||"));
	if(openDlg.DoModal()==IDCANCEL) return;
	m_spellEngine.ImportWordRate(openDlg.GetPathName());
}

//文件类型：文本文件，每行一个词组，如果有多音字则字后根该字的拼音
void CSpelltoolDlg::OnImportPhrase() 
{
	CFileDialog openDlg(TRUE,_T("txt"),_T("多音字上下文"),0,_T("text file(*.txt)|*.txt||"));
	if(openDlg.DoModal()==IDCANCEL) return;
	m_spellEngine.Context_Import(openDlg.GetPathName());
}

void CSpelltoolDlg::OnOpen() 
{
	m_spellEngine.Open(_T("d:\\spell2.dat"));
}

void CSpelltoolDlg::OnSave() 
{
	m_spellEngine.Save(_T("d:\\spell2.dat"));
}



