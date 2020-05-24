#pragma once

namespace SOUI
{
	class CSearchCfg: public SSingleton<CSearchCfg>
	{
	public:
		struct SearchInfo
		{
			SStringT name;
			SStringT url;
		};
	public:
		CSearchCfg(SStringT strPath);
		~CSearchCfg(void);

		void SetSel(int iSel);
		int  GetSel() const {return m_iSel;}
		SStringT GetSelUrl() const;
		const SArray<SearchInfo> & GetUrls() const;

	protected:
		void SaveToFile();

		SArray<SearchInfo> m_urls;
		int				   m_iSel;
		SStringT		   m_strXmlPath;
	};

}
