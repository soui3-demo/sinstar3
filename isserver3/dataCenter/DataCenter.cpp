#include "StdAfx.h"
#include "DataCenter.h"

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

	SStringT CDataCenter::GetDataPath() const
	{
		return m_path;
	}

	CMyData::CMyData(const SStringT & strDataPath):m_tmInputSpan(0), m_cInputCount(0)
	{
		m_compInfo.cWild = 'z';
		m_compInfo.strCompName = _T("╪сть...");
		m_ptStatus = 
		m_ptInput = 
		m_ptSpchar =CPoint(-1, -1);

		CRegKey		reg;
		if(ERROR_SUCCESS == reg.Create(HKEY_CURRENT_USER,L"Software\\Setoutsoft\\sinstar3",NULL,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WOW64_64KEY,0,NULL))
		{
			TCHAR szSkin[MAX_PATH]={0};
			{
				ULONG nSize = MAX_PATH;
				if (ERROR_SUCCESS == reg.QueryStringValue(_T("skin"), szSkin, &nSize))
					m_strSkin = szSkin;
			}

			DWORD dwPos=-1;
			if (ERROR_SUCCESS == reg.QueryDWORDValue(_T("status_pos"), dwPos))
				m_ptStatus = CPoint(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));

			if (ERROR_SUCCESS == reg.QueryDWORDValue(_T("input_pos"), dwPos))
				m_ptInput = CPoint(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));

			if (ERROR_SUCCESS == reg.QueryDWORDValue(_T("spchar_pos"), dwPos))
				m_ptSpchar = CPoint(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
		}

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
	}

	CMyData::~CMyData()
	{
		CRegKey		m_reg;
		if(ERROR_SUCCESS == m_reg.Create(HKEY_CURRENT_USER,L"Software\\Setoutsoft\\sinstar3",NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE|KEY_WOW64_64KEY,0,NULL))
		{
			m_reg.SetDWORDValue(_T("status_pos"),MAKELPARAM(m_ptStatus.x,m_ptStatus.y));
			m_reg.SetDWORDValue(_T("input_pos"), MAKELPARAM(m_ptInput.x, m_ptInput.y));
			m_reg.SetDWORDValue(_T("spchar_pos"), MAKELPARAM(m_ptSpchar.x, m_ptSpchar.y));
			m_reg.SetStringValue(_T("skin"),m_strSkin);
			m_reg.Close();
		}
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


