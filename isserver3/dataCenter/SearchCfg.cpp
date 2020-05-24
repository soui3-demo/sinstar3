#include "stdafx.h"
#include "SearchCfg.h"

#define SEARCH_ENGINE_CFG _T("\\server\\search.xml")
namespace SOUI
{
	template<>
	CSearchCfg* SSingleton<CSearchCfg>::ms_Singleton = NULL;

	CSearchCfg::CSearchCfg(SStringT strPath):m_iSel(-1)
	{
		m_strXmlPath = strPath + SEARCH_ENGINE_CFG;
		pugi::xml_document xmlSearch;
		if (xmlSearch.load_file(m_strXmlPath))
		{
			pugi::xml_node searchEngine = xmlSearch.first_child();
			pugi::xml_node item = searchEngine.child(L"item");
			while (item)
			{
				SearchInfo value;
				value.name=item.attribute(L"name").as_string();
				value.url = item.child_value();//using CDATA value;
				value.url.Trim('\n');
				value.url.Trim('\t');
				value.url.Trim();
				m_urls.Add(value);
				item = item.next_sibling(L"item");
			}
			if(m_urls.GetCount()>0)
			{
				m_iSel = searchEngine.attribute(L"selected").as_int(0);
				if(m_iSel<0 || m_iSel>= m_urls.GetCount()) 
					m_iSel = 0;
			}else
			{
				m_iSel = -1;
			}
		}
	}

	CSearchCfg::~CSearchCfg(void)
	{
	}

	void CSearchCfg::SetSel(int iSel)
	{
		if(iSel<0 || iSel>= m_urls.GetCount())
			return;
		m_iSel = iSel;
		SaveToFile();
	}

	SStringT CSearchCfg::GetSelUrl() const
	{
		if(m_iSel==-1)
			return SStringT();
		else
			return m_urls[m_iSel].url;
	}

	const SArray<CSearchCfg::SearchInfo> & CSearchCfg::GetUrls() const
	{
		return m_urls;
	}

	void CSearchCfg::SaveToFile()
	{
		pugi::xml_document xmlSearch;
		if (xmlSearch.load_file(m_strXmlPath))
		{
			pugi::xml_attribute attr = xmlSearch.first_child().attribute(L"selected");
			if(!attr)
			{
				attr = xmlSearch.first_child().append_attribute(L"selected");
			}
			attr.set_value(m_iSel);
			xmlSearch.save_file(m_strXmlPath);
		}
	}

}
