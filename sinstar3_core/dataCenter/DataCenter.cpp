#include "StdAfx.h"
#include "DataCenter.h"

namespace SOUI
{
	template<>
	CDataCenter * SSingleton<CDataCenter>::ms_Singleton = NULL;

	CDataCenter::CDataCenter(const SStringT & strDataPath):m_data(strDataPath)
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


	const SArray<CNameTypePair>& CDataCenter::UpdateCompList()
	{
		m_compList.RemoveAll();
		m_iSelComp = -1;
		if (ISComm_Comp_List() == ISACK_SUCCESS)
		{
			PMSGDATA pData = ISComm_GetData();
			BYTE i, byCount = pData->byData[0];
			char *pBuf = (char*)pData->byData + 1;
			for (i = 0; i<byCount; i++)
			{
				CNameTypePair pair;
				char *pName = pBuf + 1;
				pair.strName = pName;
				char *pType = pName + pair.strName.GetLength() + 1;
				pair.strType = pType;
				if (pBuf[0]) m_iSelComp = i;
				pBuf = pType + pair.strType.GetLength() + 1;
				m_compList.Add(pair);
			}
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

	CMyData::CMyData(const SStringT & strDataPath):m_tmInputSpan(0), m_cInputCount(0)
	{
		m_reg.Create(HKEY_CURRENT_USER,L"Software\\Setoutsoft\\sinstar3",NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE|KEY_READ|KEY_WOW64_64KEY,0,NULL);

		m_compInfo.cWild = 'z';
		m_compInfo.strCompName = _T("╪сть...");

		TCHAR szSkin[MAX_PATH]={0};
		ULONG nSize = MAX_PATH;
		m_reg.QueryStringValue(_T("skin"),szSkin,&nSize);
		m_strSkin = szSkin;

		DWORD dwPos=-1;
		if (ERROR_SUCCESS == m_reg.QueryDWORDValue(_T("status_pos"), dwPos))
			m_ptStatus = CPoint(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
		else
			m_ptStatus = CPoint(-1, -1);

		if (ERROR_SUCCESS == m_reg.QueryDWORDValue(_T("input_pos"), dwPos))
			m_ptInput = CPoint(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
		else
			m_ptInput = CPoint(-1, -1);

		CRegKey keySvr;
		if(ERROR_SUCCESS == keySvr.Open(HKEY_LOCAL_MACHINE,L"Software\\Setoutsoft\\sinstar3",KEY_READ|KEY_WOW64_64KEY))
		{
			TCHAR szSvrPath[MAX_PATH]={0};
			ULONG nSize = MAX_PATH;
			keySvr.QueryStringValue(_T("path_svr"),szSvrPath,&nSize);
			ISComm_SetSvrPath(szSvrPath);
		}
		pugi::xml_document fontMap;
		if (fontMap.load_file(strDataPath + _T("\\fontmap.xml")))
		{
			pugi::xml_node font = fontMap.child(L"fontmap").child(L"font");
			while (font)
			{
				m_fontMap[font.attribute(L"face").as_string()] = font.attribute(L"file").as_string();
				font = font.next_sibling(L"font");
			}
		}
	}

	CMyData::~CMyData()
	{
		m_reg.SetDWORDValue(_T("status_pos"),MAKELPARAM(m_ptStatus.x,m_ptStatus.y));
		m_reg.SetDWORDValue(_T("input_pos"), MAKELPARAM(m_ptInput.x, m_ptInput.y));
		m_reg.SetStringValue(_T("skin"),m_strSkin);
		m_reg.Close();
	}

	SStringW CMyData::getFontFile(const SStringW & strFace) const
	{
		const SMap<SStringW,SStringW>::CPair *p= m_fontMap.Lookup(strFace);
		if(!p)
			return SStringW();
		else
			return p->m_value;
	}


	static int CharCmp(const void * p1, const void * p2)
	{
		const char *c1 = (const char*)p1;
		const char *c2 = (const char*)p2;
		return (*c1) - (*c2);
	}

	void CompInfo::SetSvrCompInfo(const COMPINFO * compInfo)
	{
		cWild = compInfo->cWildChar;
		strCompName = S_CA2T(compInfo->szName);
		strcpy(szCode,compInfo->szCode);
		nCodeNum = (int)strlen(szCode);
		qsort(szCode,nCodeNum,sizeof(char),CharCmp);
	}

	BOOL CompInfo::IsCompChar(char cInput)
	{
		if(cWild == cInput) return TRUE;
		return NULL != bsearch(&cInput,szCode,nCodeNum,1,CharCmp);
	}

}


