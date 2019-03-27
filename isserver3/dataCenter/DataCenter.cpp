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

	CMyData::CMyData(const SStringT & strDataPath):m_tmInputSpan(0), m_cInputCount(0),m_nScale(100)
	{
		m_compInfo.cWild = 'z';
		m_compInfo.strCompName = _T("╪сть...");

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
		strCompName = S_CA2T(compInfo->szName, CP_GB);
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


