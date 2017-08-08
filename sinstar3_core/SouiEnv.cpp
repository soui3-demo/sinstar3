#include "StdAfx.h"
#include "SouiEnv.h"


//从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 0
// #define RES_TYPE 0   //从文件中加载资源，加载失败再从PE加载
// #define RES_TYPE 1   //从PE资源中加载UI资源
// #define RES_TYPE 2   //从zip包中加载资源

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif


//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP

template<>
CSouiEnv* SSingleton<CSouiEnv>::ms_Singleton = NULL;

CSouiEnv::CSouiEnv(HINSTANCE hInst)
{
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));

	int nRet = 0;

	SComMgr	* pComMgr = new SComMgr;

	//将程序的运行路径修改到项目所在目录所在的目录
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));
	LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
	//_tcscpy(lpInsertPos + 1, _T("..\\SouiWizard1"));
	SetCurrentDirectory(szCurrentDir);

	BOOL bLoaded=FALSE;
	CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
	CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
	bLoaded = pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
	SASSERT_FMT(bLoaded,_T("load interface [render] failed!"));
	bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
	SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));

	pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
	
	m_theApp = new SApplication(pRenderFactory, hInst);
	m_theApp->SetAppDir(L"E:\\sinstar3.git\\sinstar3_core");
	//从DLL加载系统资源
	HMODULE hModSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
	if (hModSysResource)
	{
		CAutoRefPtr<IResProvider> sysResProvider;
		CreateResProvider(RES_PE, (IObjRef**)&sysResProvider);
		sysResProvider->Init((WPARAM)hModSysResource, 0);
		m_theApp->LoadSystemNamedResource(sysResProvider);
		FreeLibrary(hModSysResource);
	}else
	{
		SASSERT(0);
	}

	CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
	CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
	SStringT strPath = m_theApp->GetAppDir();
	strPath+=_T("\\uires");
	if (!pResProvider->Init((LPARAM)(LPCTSTR)strPath, 0))
	{
		SASSERT(0);
		return ;
	}
#else 
	CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
	pResProvider->Init((WPARAM)hInst, 0);
#endif

	m_theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);
	m_theApp->AddResProvider(pResProvider);

	m_imgDecoder = pImgDecoderFactory;
	m_render = pRenderFactory;
}

CSouiEnv::~CSouiEnv(void)
{
	delete m_theApp;
	m_imgDecoder = NULL;
	m_render = NULL;
	OleUninitialize();
}
