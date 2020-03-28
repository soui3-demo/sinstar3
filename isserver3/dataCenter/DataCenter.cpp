#include "StdAfx.h"
#include "DataCenter.h"
#include "../ime/ui/SkinMananger.h"
#include "../IsSvrProxy.h"

namespace SOUI
{
	template<>
	CDataCenter * SSingleton<CDataCenter>::ms_Singleton = NULL;

	CDataCenter::CDataCenter(const SStringT & strDataPath):m_data(strDataPath),m_path(strDataPath)
	{
		InitializeCriticalSection(&m_cs);
	}

	CDataCenter::~CDataCenter(void)
	{
		DeleteCriticalSection(&m_cs);
	}

	void CDataCenter::Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void CDataCenter::Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}

	/*
LRESULT CServerCore::ReqCompList(HWND hClientWnd, PMSGDATA pMsg)
{
	COMFILE cf = CF_Init(m_pMsgAck->byData, MAX_BUF_ACK, 0, 0);
	CF_WriteChar(&cf, 0);
	BYTE byCount = 0;

	char szTitle[255];
	char szFilter[MAX_PATH];
	char szFullPath[MAX_PATH];
	COMPHEAD header = { 0 };
	sprintf(szFilter, "%s\\*.cit", m_data.m_szDataPath);
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA(szFilter, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			if (!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			{
				sprintf(szFullPath, "%s\\%s", m_data.m_szDataPath, fd.cFileName);
				if (CCompReaderEx::ExtractInfo(szFullPath, &header, NULL))
				{
					_splitpath(szFullPath, NULL, NULL, szTitle, NULL);
					if (strcmp(m_strDefComp, szTitle) == 0)
						CF_WriteChar(&cf, 1);
					else
						CF_WriteChar(&cf, 0);
					CF_Write(&cf, szTitle, strlen(szTitle) + 1);
					CF_Write(&cf, header.szName, strlen(header.szName) + 1);
					byCount++;
					if (byCount>20) break;
				}
			}
		} while (FindNextFileA(hFind, &fd));
		FindClose(hFind);
	}
	CF_Seek(&cf, 0, SEEK_SET);
	CF_WriteChar(&cf, byCount);
	m_pMsgAck->sSize = (short)cf.nLen;
	return ISACK_SUCCESS;
}
*/

	const SArray<CNameTypePair>& CDataCenter::UpdateCompList()
	{
		m_compList.RemoveAll();
		m_iSelComp = -1;


		TCHAR szTitle[255];
		TCHAR szFilter[MAX_PATH];
		TCHAR szFullPath[MAX_PATH];
		COMPHEAD header = { 0 };
		_stprintf(szFilter, _T("%s\\*.cit"), GetDataPath());
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(szFilter, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do {
				if (!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					_stprintf(szFullPath, _T("%s\\%s"), GetDataPath(), fd.cFileName);
					if (CIsSvrProxy::GetSvrCore()->ExtractCompInfo(szFullPath, &header, NULL))
					{
						_tsplitpath(szFullPath, NULL, NULL, szTitle, NULL);
						CNameTypePair item;
						item.strType=szTitle;
						item.strName=szFullPath;
						m_compList.Add(item);//todo:hjx
					}
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}

		return m_compList;
	}

	const SArray<CNameTypePair>& CDataCenter::GetCompList() const
	{
		return m_compList;
	}

	int CDataCenter::GetSelectCompIndex() const
	{
		return m_iSelComp;
	}

	SStringT CDataCenter::GetDataPath() const
	{
		return m_path;
	}

	CMyData::CMyData(const SStringT & strDataPath)
		:m_tmInputSpan(0), m_cInputCount(0)
		,m_tmTotalSpan(0), m_cTotalInput(0)
		,m_nScale(100)
	{
		m_compInfo.cWild = 'z';
		m_compInfo.strCompName = _T("加载...");

		pugi::xml_document fontMap;
		if (fontMap.load_file(strDataPath + _T("\\data\\fontmap.xml")))
		{
			pugi::xml_node font = fontMap.child(L"fontmap").child(L"font");
			while (font)
			{
				m_fontMap[font.attribute(L"face").as_string()] = font.attribute(L"file").as_string();
				font = font.next_sibling(L"font");
			}
		}

		CRegKey reg;
		LONG ret = reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ | KEY_WOW64_64KEY);
		if (ret == ERROR_SUCCESS)
		{
			reg.QueryDWORDValue(_T("input_count"), m_cTotalInput);
			reg.QueryDWORDValue(_T("input_span"), m_tmTotalSpan);
			reg.Close();
			if (m_tmTotalSpan == 0) m_cTotalInput = 0;
			if(m_cTotalInput>0x7fffffff || m_tmTotalSpan>0x7fffffff)
			{//clear data
				m_cTotalInput = m_tmTotalSpan = 0;
			}
		}
	}

	CMyData::~CMyData()
	{
		saveSpeed();
	}

	bool CMyData::saveSpeed()
	{
		CRegKey reg;
		LONG ret = reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ|KEY_WRITE | KEY_WOW64_64KEY);
		if(ret != ERROR_SUCCESS)
			return false;
		reg.SetDWORDValue(_T("input_count"), getTotalInput());
		reg.SetDWORDValue(_T("input_span"), getTotalSpan());
		reg.Close();
		return true;
	}

	
	SStringW CMyData::getFontFile(const SStringW & strFace) const
	{
		const SMap<SStringW,SStringW>::CPair *p= m_fontMap.Lookup(strFace);
		if(!p)
			return SStringW();
		else
			return p->m_value;
	}

	DWORD CMyData::getTotalInput() const
	{
		return m_cInputCount + m_cTotalInput;
	}

	DWORD CMyData::getTotalSpan() const
	{
		return m_tmInputSpan+m_tmTotalSpan;
	}

	bool CMyData::changeSkin(const SStringT &strSkin)
	{
		if (!strSkin.IsEmpty())
		{//加载外部皮肤
			SLOG_INFO("step2, prepare for load skin");
			CAutoRefPtr<IResProvider> pResProvider;
			if(strSkin.EndsWith(_T("sskn"),true))
			{
				g_ComMgr2->CreateResProvider_ZIP((IObjRef**)&pResProvider);
				ZIPRES_PARAM param;
				param.ZipFile(GETRENDERFACTORY, strSkin);
				if (!pResProvider->Init((WPARAM)&param, 0))
					return false;
			}else
			{//load folder
				CreateResProvider(RES_FILE,(IObjRef**)&pResProvider);
				if(!pResProvider->Init((WPARAM)(LPCTSTR)strSkin,0))
					return false;
			}

			IUiDefInfo * pUiDef = SUiDef::CreateUiDefInfo2(pResProvider, _T("uidef:xml_init"));
			if (pUiDef->GetSkinPool() || pUiDef->GetStylePool() || pUiDef->GetObjDefAttr())
			{//不允许皮肤中存在全局的skin and style data
				pUiDef->Release();
				return false;
			}

			SLOG_INFO("step3, load external skin ok");

			if (!g_SettingsG->strSkin.IsEmpty())
			{//清除正在使用的外置皮肤。
				SLOG_INFO("step4, remove current in using external skin");

				IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
				SApplication::getSingleton().RemoveResProvider(pLastRes);
			}

			SLOG_INFO("step6, extract skin defined offset");
			m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pResProvider);

			SLOG_INFO("step7, add new skin to sinstar3");
			SApplication::getSingleton().AddResProvider(pResProvider, NULL);

			SLOG_INFO("step8, set uidef");
			pUiDef->GetNamedColor().Merge(m_defUiDefine->GetNamedColor());
			pUiDef->GetNamedString().Merge(m_defUiDefine->GetNamedString());
			pUiDef->GetNamedDimension().Merge(m_defUiDefine->GetNamedDimension());
			SUiDef::getSingleton().SetUiDef(pUiDef,g_SettingsG->strFontDesc.IsEmpty());
			pUiDef->Release();

			//还原skinpool and stylepool.
			SLOG_INFO("step9, restore builtin skin, style and object default pools.");
			SUiDef::getSingleton().GetUiDef()->SetSkinPool(m_defUiDefine->GetSkinPool());
			SUiDef::getSingleton().GetUiDef()->SetStylePool(m_defUiDefine->GetStylePool());
			SUiDef::getSingleton().GetUiDef()->SetObjDefAttr(m_defUiDefine->GetObjDefAttr());
			SLOG_INFO("step10, set external skin ok");
		}
		else if (!g_SettingsG->strSkin.IsEmpty())
		{//清除正在使用的外置皮肤,还原使用系统内置皮肤
			SLOG_INFO("step11, remove external skin");
			IResProvider *pLastRes = SApplication::getSingleton().GetTailResProvider();
			SApplication::getSingleton().RemoveResProvider(pLastRes);

			SLOG_INFO("step12, restore uidef");
			SUiDef::getSingleton().SetUiDef(m_defUiDefine,g_SettingsG->strFontDesc.IsEmpty());

			SLOG_INFO("step13, extract builtin skin defined offset");
			IResProvider *pCurRes = SApplication::getSingleton().GetHeadResProvider();
			CDataCenter::getSingleton().GetData().m_ptSkinOffset = CSkinMananger::ExtractSkinOffset(pCurRes);
		}
		return true;
	}


	static int CharCmp(const void * p1, const void * p2)
	{
		const WCHAR *c1 = (const WCHAR*)p1;
		const WCHAR *c2 = (const WCHAR*)p2;
		return (*c1) - (*c2);
	}

	void CompInfo::SetSvrCompInfo(const COMPHEAD * compInfo)
	{
		cWild = compInfo->cWildChar;
		strCompName = S_CW2T(compInfo->szName);
		wcscpy(szCode,compInfo->szCode);
		nCodeNum = (int)wcslen(szCode);
		qsort(szCode,nCodeNum,sizeof(WCHAR),CharCmp);
	}

	BOOL CompInfo::IsCompChar(char cInput)
	{
		if(cWild == cInput) return TRUE;
		return NULL != bsearch(&cInput,szCode,nCodeNum,1,CharCmp);
	}

}


