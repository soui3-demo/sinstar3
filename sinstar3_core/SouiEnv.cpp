#include "StdAfx.h"
#include "SouiEnv.h"
#include "ui/SkinMananger.h"
//从PE文件加载，注意从文件加载路径位置
#ifdef _DEBUG
#define RES_TYPE 0
#else
#define RES_TYPE 1
#endif

// #define RES_TYPE 0   //从文件中加载资源，加载失败再从PE加载
// #define RES_TYPE 1   //从PE资源中加载UI资源
// #define RES_TYPE 2   //从zip包中加载资源


//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP

template<>
CSouiEnv* SSingleton<CSouiEnv>::ms_Singleton = NULL;


CSouiEnv::CSouiEnv(HINSTANCE hInst,LPCTSTR pszWorkDir)
{
	int nRet = 0;

	m_pComMgr = new SComMgr(_T("imgdecoder-png"));

	BOOL bLoaded = FALSE;
	CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
	CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
	bLoaded = m_pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
	SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));
	bLoaded = m_pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
	SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("imgdecoder"));

	pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);

	m_theApp = new SApplication(pRenderFactory, hInst, SINSTART3_WNDCLASS);
	m_theApp->SetAppDir(pszWorkDir);

	m_theApp->RegisterWindowClass<SToggle2>();
	m_theApp->RegisterWindowClass<SCandView>();
	m_theApp->RegisterWindowClass<SSentView>();
	m_theApp->RegisterWindowClass<SEnglishCand>();
	m_theApp->RegisterWindowClass<SPhraseCand>();
	m_theApp->RegisterWindowClass<SSpellView>();
	m_theApp->RegisterWindowClass<SCaption2>();
	m_theApp->RegisterWindowClass<SCompBackground>();
	m_theApp->RegisterWindowClass<SStatusBackground>();
	m_theApp->RegisterWindowClass<SMutexView>();
	m_theApp->RegisterWindowClass<SFlagView>();
	m_theApp->RegisterWindowClass<STabPageEx>();
	m_theApp->RegisterWindowClass<STabCtrlEx>();
	m_theApp->RegisterWindowClass<SLineComp>();

	m_theApp->RegisterSkinClass<SLineSkin>();

	CAutoRefPtr<ILog4zManager> pLogMgr;
	bLoaded = m_pComMgr->CreateLog4z((IObjRef**)&pLogMgr);
	SASSERT_FMT(bLoaded, _T("load ILog4zManager failed!"), _T("log4z"));

	m_theApp->SetLogManager(pLogMgr);

	SStringT strLogDir = pszWorkDir;
	strLogDir += _T("\\log");
	pLogMgr->setLoggerPath(0, S_CT2A(strLogDir));
	pLogMgr->start();

	//从DLL加载系统资源
	CAutoRefPtr<IResProvider> sysResProvider;
	CreateResProvider(RES_PE, (IObjRef**)&sysResProvider);
	sysResProvider->Init((WPARAM)hInst, 0);
	UINT uFlag = m_theApp->LoadSystemNamedResource(sysResProvider);
	SASSERT_FMT(uFlag == 0, L"load system resource failed");
	CAutoRefPtr<IResProvider>   pResProvider;

	BOOL bDebugSkinLoaded = FALSE;
	if (!CDataCenter::getSingletonPtr()->GetData().m_strDebugSkin.IsEmpty())
	{
		CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
		SStringT strPath = CDataCenter::getSingletonPtr()->GetData().m_strDebugSkin;
		bDebugSkinLoaded = pResProvider->Init((LPARAM)(LPCTSTR)strPath, 0);
		if (!bDebugSkinLoaded)
		{//release resprovider.
			pResProvider = NULL;
		}
	}
	
	if(!bDebugSkinLoaded)
	{
#if (RES_TYPE == 0)
		CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);

		//获取调试阶段的皮肤目录位置
		TCHAR szCurrentDir[MAX_PATH] = { 0 };
		GetModuleFileName(hInst, szCurrentDir, sizeof(szCurrentDir));
		LPTSTR lpInsertPos = _tcsstr(szCurrentDir, _T("\\program"));
		SASSERT(lpInsertPos);
		_tcscpy(lpInsertPos, _T("\\..\\sinstar3_core\\uires"));
		if (!pResProvider->Init((LPARAM)szCurrentDir, 0))
		{
			SASSERT(0);
			return;
		}
#else 
		CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
		pResProvider->Init((WPARAM)hInst, 0);
#endif
	}

	m_theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);
	m_theApp->AddResProvider(pResProvider);
	CDataCenter::getSingletonPtr()->GetData().m_defUiDefine = SUiDef::getSingletonPtr()->GetUiDef();
	CDataCenter::getSingletonPtr()->GetData().m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pResProvider);

	if(!bDebugSkinLoaded && !CDataCenter::getSingletonPtr()->GetData().m_strSkin.IsEmpty())
	{
		//SLOG_INFO("apply user skin:"<< CDataCenter::getSingletonPtr()->GetData().m_strSkin);
		IResProvider *pResProvider=NULL;
		CSouiEnv::getSingleton().theComMgr()->CreateResProvider_ZIP((IObjRef**)&pResProvider);
		ZIPRES_PARAM param;
		param.ZipFile(GETRENDERFACTORY, CDataCenter::getSingletonPtr()->GetData().m_strSkin);
		if(pResProvider->Init((WPARAM)&param,0))
		{
			IUiDefInfo * pUiDef = SUiDef::CreateUiDefInfo2(pResProvider,_T("uidef:xml_init"));
			if(!pUiDef->GetSkinPool() && !pUiDef->GetStylePool() && !pUiDef->GetObjDefAttr())
			{//不允许皮肤中存在全局的skin, style and defobjattr数据
				m_theApp->AddResProvider(pResProvider,NULL);

				IUiDefInfo * pBuiltinUidef = SUiDef::getSingleton().GetUiDef();
				pUiDef->SetObjDefAttr(pBuiltinUidef->GetObjDefAttr());
				pUiDef->SetStylePool(pBuiltinUidef->GetStylePool());
				pUiDef->SetSkinPool(pBuiltinUidef->GetSkinPool());
				pUiDef->GetNamedColor().Merge(pBuiltinUidef->GetNamedColor());
				pUiDef->GetNamedString().Merge(pBuiltinUidef->GetNamedString());
				pUiDef->GetNamedDimension().Merge(pBuiltinUidef->GetNamedDimension());

				SUiDef::getSingleton().SetUiDef(pUiDef);
				CDataCenter::getSingletonPtr()->GetData().m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pResProvider);
			}else
			{//外置皮肤中禁止出现全局skin, style and defobjattr表。
				SLOG_WARN("previous skin is invalid");
				CDataCenter::getSingletonPtr()->GetData().m_strSkin.Empty();
			}
			pUiDef->Release();
		}else
		{
			CDataCenter::getSingletonPtr()->GetData().m_strSkin.Empty();
		}
	}
}

CSouiEnv::~CSouiEnv(void)
{
	CDataCenter::getSingletonPtr()->GetData().m_defUiDefine=NULL;

	CAutoRefPtr<ILog4zManager> pLogMgr = m_theApp->GetLogManager();
	if (pLogMgr)
		pLogMgr->stop();


 	delete m_theApp;
	delete m_pComMgr;
}
