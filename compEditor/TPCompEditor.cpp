// TPCompEditor.cpp : implementation file
//

#include "stdafx.h"
#include "TPCompEditor.h"
#include "../iscore/compbuilder-i.h"
#include "base64.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTPCompEditor dialog

typedef ICompBuilder* (* FunICompBuilder_Create)();
typedef void (*FunICompBuilder_Destroy)(ICompBuilder* pCompBuilder);
typedef ICompReaderEx * (*FunICompReader_Create)();
typedef void (*FunICompReader_Destroy)(ICompReaderEx* pCompReader);

class CompBuilderLoader
{
public:
	CompBuilderLoader()
	{
		m_hMod = LoadLibrary(_T("iscore.dll"));
		m_funCreate = (FunICompBuilder_Create)GetProcAddress(m_hMod, "ICompBuilder_Create");
		m_funDestroy = (FunICompBuilder_Destroy)GetProcAddress(m_hMod, "ICompBuilder_Destroy");

		m_funCreateReader = (FunICompReader_Create)GetProcAddress(m_hMod,"ICompReader_Create");
	    m_funDestroyReader = (FunICompReader_Destroy)GetProcAddress(m_hMod,"ICompReader_Destroy");

	}

	ICompBuilder * Create() {
		return m_funCreate();
	}

	void  Destroy(ICompBuilder * pCompBuilder) {
		return m_funDestroy(pCompBuilder);
	}

	ICompReaderEx *CreateReader()
	{
		return m_funCreateReader();
	}

	void DestroyReader(ICompReaderEx *pReader)
	{
		m_funDestroyReader(pReader);
	}

	~CompBuilderLoader()
	{
		if(m_hMod) FreeLibrary(m_hMod);
	}
private:
	FunICompBuilder_Create m_funCreate;
	FunICompBuilder_Destroy m_funDestroy;

	FunICompReader_Create  m_funCreateReader;
	FunICompReader_Destroy m_funDestroyReader;

	HMODULE m_hMod;
}s_Loader;

CTPCompEditor::CTPCompEditor(CWnd *pParentWnd)
	: CDialog(CTPCompEditor::IDD,pParentWnd)
{
	//{{AFX_DATA_INIT(CTPCompEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_szLicenseMD5[0]=0;
}


void CTPCompEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTPCompEditor)
	DDX_Control(pDX, IDC_COLORTRANS, m_ctrlColor);
	DDX_Control(pDX, IDC_BMWND, m_bmwIcon);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTPCompEditor, CDialog)
	//{{AFX_MSG_MAP(CTPCompEditor)
	ON_BN_CLICKED(IDC_BROWSECOMP, OnBrowsecomp)
	ON_BN_CLICKED(IDC_BROWSEICON, OnBrowseicon)
	ON_BN_CLICKED(IDC_BROWSE_SAVE, OnBrowseSave)
	ON_BN_CLICKED(IDC_MAKE, OnMake)
	ON_BN_CLICKED(IDC_BROWSE_KEYMAP, OnBrowseKeymap)
	ON_EN_CHANGE(IDC_COMP, OnChangeComp)
	ON_BN_CLICKED(IDC_LICENSE, OnLicense)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_BMWND_CLICK,OnBMWndClick)
	ON_BN_CLICKED(IDC_EXPORT, &CTPCompEditor::OnBnClickedExport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTPCompEditor message handlers

void CTPCompEditor::OnBrowsecomp() 
{
	CString strExt;
	strExt.LoadString(IDS_FORMAT_TXT);
	CFileDialog browseDlg(TRUE,NULL,NULL,0,strExt);
	if(browseDlg.DoModal()!=IDOK) return;
	SetDlgItemText(IDC_COMP,browseDlg.GetPathName());
	CString strSave;
	GetDlgItemText(IDC_SAVEFILE,strSave);
	if(strSave.IsEmpty())
	{
		char szDrive[5],szPath[MAX_PATH],szTitle[MAX_PATH];
		char szSave[MAX_PATH];
		_splitpath(browseDlg.GetPathName(),szDrive,szPath,szTitle,NULL);
		sprintf(szSave,"%s%s%s.cit",szDrive,szPath,szTitle);
		SetDlgItemText(IDC_SAVEFILE,szSave);
	}
}

void CTPCompEditor::OnBrowseicon() 
{
	CString strExt;
	strExt.LoadString(IDS_FORMAT_IMAGE);
	CFileDialog browseDlg(TRUE,NULL,NULL,0,strExt);
	if(browseDlg.DoModal()==IDOK)
	{
		m_strIconFile = browseDlg.GetPathName();
	}
}

void CTPCompEditor::OnBrowseSave() 
{
	CString strExt;
	strExt.LoadString(IDS_FORMAT_CIT);
	CFileDialog browseDlg(FALSE,"cit",NULL,0,strExt);
	if(browseDlg.DoModal()!=IDOK) return;
	SetDlgItemText(IDC_SAVEFILE,browseDlg.GetPathName());		
}


#define MIN_WORDS	2000
#include "unicode2ansi.h"
//-------------------------------------------------------
//	创建编码方案
//------------------------------------------------------
void CTPCompEditor::OnMake() 
{
	COMPHEAD head={0};
	CString strComp,strKeyMap,strSave;

	GetDlgItemText(IDC_COMP,strComp);
	GetDlgItemText(IDC_KEYMAP,strKeyMap);
	GetDlgItemText(IDC_URL,head.szUrl,49);
	GetDlgItemText(IDC_SAVEFILE,strSave);
	head.bSymbolFirst=!(BOOL)IsDlgButtonChecked(IDC_SYMBOL_NOFIRST);
	head.dwEncrypt=IsDlgButtonChecked(IDC_ENCRYPT)?1:0;
	head.dwYinXingMa=IsDlgButtonChecked(IDC_YINXINGMA)?1:0;

	if(strComp.IsEmpty() || GetFileAttributes(strComp)==0xFFFFFFFF)
	{
		AfxMessageBox(IDS_COMPCREATE_NOCOMP);
		return;
	}
	if(strKeyMap.IsEmpty()==FALSE && GetFileAttributes(strKeyMap)==0xFFFFFFFF)
	{
		AfxMessageBox(IDS_COMPCREATE_KEYMAPERR);
		return;
	}

	if(CheckFileUnicode(strComp))
	{
		AfxMessageBox(IDS_ERROR_UNICODE,MB_OK|MB_ICONSTOP);
		return;
	}
	char szWildChar[2];
	BYTE byNumRules;
	char szRules[RULE_MAX*100];

	GetPrivateProfileString("Description","Name","",head.szName,50,strComp);
	GetPrivateProfileString("Description","UsedCodes","",head.szCode,50,strComp);
	GetPrivateProfileString("Description","WildChar","",szWildChar,2,strComp);
	head.cWildChar=szWildChar[0];
	head.cCodeMax=GetPrivateProfileInt("Description","MaxCodes",0,strComp);
	head.dwAutoSelect=GetPrivateProfileInt("Description","AutoSelect",0,strComp)?1:0;//唯一重码自动上屏属性
	head.dwPhraseCompPart=GetPrivateProfileInt("Description","PhraseCompPart",0,strComp)?1:0;//词组可以不是全码标志

	byNumRules=GetPrivateProfileInt("Description","NumRules",0,strComp);
	GetPrivateProfileSection("Rule",szRules,RULE_MAX*100,strComp);	

	ICompBuilder * pCompBuilder = s_Loader.Create();
	ICodingRule * pCodingRule = pCompBuilder->CreateCodingRule();

	if(!pCodingRule->ParseRules(head.cCodeMax,byNumRules,szRules))
	{
		pCompBuilder->DestroyCodingRule(pCodingRule);
		s_Loader.Destroy(pCompBuilder);
		AfxMessageBox(IDS_COMPCREATE_RULE);
		return;
	}
	CFile file(strComp,CFile::modeRead);
	CArchive ar(&file,CArchive::load);

	char szLine[1000];
	//find text section
	char *pBuf=ar.ReadString(szLine,1000);
	while(pBuf && stricmp(pBuf,"[Text]")!=0)
	{
		pBuf=ar.ReadString(szLine,1000);
	}

	//import compostion parts
	pBuf=ar.ReadString(szLine,100);
	while(pBuf)
	{
		if(szLine[0]&0x80)
		{
			char *pszWord=szLine;
			char *pszComp=szLine+2;
			if(pszComp[0]>0)
			{//只支持单字
				char cCompLen=0;
				char *pszCompTemp=pszComp;
				int i=0;
				while(pszComp[i])
				{
					if(pszComp[i]==' ')
					{
						pCompBuilder->AddWord(pszWord,pszCompTemp,cCompLen);
						pszCompTemp+=cCompLen+1;
						cCompLen=0;
					}else
						cCompLen++;
					i++;
				}
				if(cCompLen!=0) pCompBuilder->AddWord(pszWord,pszCompTemp,cCompLen);
			}
		}
		pBuf=ar.ReadString(szLine,100);
	}
	ar.Close();
	file.Close();

	if(pCompBuilder->GetWords()<MIN_WORDS)
	{
		pCompBuilder->DestroyCodingRule(pCodingRule);
		s_Loader.Destroy(pCompBuilder);
		AfxMessageBox(IDS_COMPCREATE_CHAR);
		return ;
	}
	char szDriver[10],szPath[MAX_PATH],szTitle[100];
	_splitpath(strSave,szDriver,szPath,szTitle,NULL);
	BOOL bSuccess= pCompBuilder->Make(strSave,head,
		pCodingRule,
		m_strIconFile.IsEmpty()?NULL:m_strIconFile,
		0,
		strKeyMap.IsEmpty()?NULL:(LPCTSTR)strKeyMap,
		m_szLicenseMD5);

	pCompBuilder->DestroyCodingRule(pCodingRule);
	s_Loader.Destroy(pCompBuilder);

	if(bSuccess)
	{
		if(m_szLicenseMD5[0])
		{
			char szDestKey[MAX_PATH];
			sprintf(szDestKey,"%s%s%s.dll",szDriver,szPath,szTitle);
			CopyFile(m_strKeyDll,szDestKey,FALSE);
		}
		AfxMessageBox(IDS_COMPCREATE_SUCCESS,MB_OK|MB_ICONINFORMATION);
	}else
	{
		AfxMessageBox(IDS_COMPCREATE_ERROR,MB_OK|MB_ICONSTOP);
	}
}

LRESULT CTPCompEditor::OnBMWndClick(WPARAM wParam, LPARAM lParam)
{
	m_ctrlColor.m_crColor=(COLORREF)wParam;
	char szBuf[40];
	sprintf(szBuf,"%u,%u,%u",GetRValue(wParam),GetGValue(wParam),GetBValue(wParam));
	SetDlgItemText(IDC_COLOR_TEXT,szBuf);
	m_ctrlColor.Invalidate(FALSE);
	return 0;
}

void CTPCompEditor::OnBrowseKeymap() 
{
	CString strExt;
	strExt.LoadString(IDS_FORMAT_IMAGE);
	CFileDialog browseDlg(TRUE,NULL,NULL,0,strExt);
	if(browseDlg.DoModal()!=IDOK) return;
	SetDlgItemText(IDC_KEYMAP,browseDlg.GetPathName());	
}

BOOL CTPCompEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckDlgButton(IDC_SYMBOL_NOFIRST,TRUE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTPCompEditor::OnChangeComp() 
{
	char szFileName[MAX_PATH];
	GetDlgItemText(IDC_COMP,szFileName,MAX_PATH);
	if(GetFileAttributes(szFileName)!=0xffffffff)
	{
		int nSymbolFirst=GetPrivateProfileInt("Description","SymbolFirst",2,szFileName);
		if(nSymbolFirst!=2) CheckDlgButton(IDC_SYMBOL_NOFIRST,!nSymbolFirst);
		int nYinXingMa=GetPrivateProfileInt("Description","YinXingMa",2,szFileName);
		if(nYinXingMa!=2) CheckDlgButton(IDC_YINXINGMA,nYinXingMa);
		char szWebSite[200];
		GetPrivateProfileString("Description","WebSite","",szWebSite,200,szFileName);
		SetDlgItemText(IDC_URL,szWebSite);
		char *szIcon=(char*)malloc(32*1024);//max to 32k

		int nLen = GetPrivateProfileSection("Icon2",szIcon,32*1024,szFileName);
		if(nLen)
		{

			char szIconFile[MAX_PATH];
			strcpy(szIconFile,szFileName);
			strcat(szIconFile,".img");

			std::string tmp;
			char *p=szIcon;
			char *pEnd = p+nLen;

			while(p<pEnd)
			{
				tmp+=p;
				p+=strlen(p)+1;
			}

			std::string result;
			result.resize(nLen);
			size_t nUsed=0;
			Base64::DecodeFromArray(&tmp[0],tmp.length(),Base64::DO_LAX,&result,&nUsed);

			FILE *f=fopen(szIconFile,"wb");
			fwrite(&result[0],1,result.length(),f);
			fclose(f);
		}
		free(szIcon);
	}
}

#include "LicenseDlg.h"
#include "md5.h"
typedef int(*FunVerifyUser)(const char * pszPath);

void CTPCompEditor::OnLicense() 
{
	CLicenseDlg lcsDlg;
	if(lcsDlg.DoModal()==IDOK)
	{
		FunVerifyUser funVU=NULL;
		HMODULE hModule=LoadLibrary(lcsDlg.m_strLicenseDll);
		if(!hModule) goto error;
		funVU=(FunVerifyUser)GetProcAddress(hModule,"VerifyUser");
		if(!funVU) goto error;
		FreeLibrary(hModule);
		MD5File(lcsDlg.m_strLicenseDll,m_szLicenseMD5,1<<7);
		m_strKeyDll=lcsDlg.m_strLicenseDll;
		return;
error:
		MessageBox("无效的授权文件");
	}
}

void CTPCompEditor::OnBnClickedExport()
{
	CString strExt;
	strExt.LoadString(IDS_FORMAT_CIT);
	CFileDialog browseDlg(TRUE,NULL,NULL,0,strExt);
	if(browseDlg.DoModal()!=IDOK) return;
	ICompReaderEx *pReader = s_Loader.CreateReader();
	bool bOk = false;
	if(pReader->Load(browseDlg.GetPathName()))
	{
		char szDrive[5],szDir[MAX_PATH];
		_splitpath(browseDlg.GetPathName(),szDrive,szDir,NULL,NULL);
		char szSave[MAX_PATH*2];
		sprintf(szSave,"%s%s%s.txt",szDrive,szDir,pReader->GetProps()->szName);
		if(pReader->Export(szSave))
		{
			ShellExecute(NULL,"open",szSave,NULL,NULL,SW_SHOWDEFAULT);
			bOk = true;
		}
	}
	s_Loader.DestroyReader(pReader);
	if(!bOk)
	{
		AfxMessageBox(IDS_EXPORT_COMP_FAILED,MB_OK|MB_ICONSTOP);
	}
}
