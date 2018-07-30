#pragma once

namespace SOUI
{
	struct CompInfo
	{
		SStringT strCompName;
		char szCode[50];
		int  nCodeNum;
		TCHAR    cWild;

		void SetSvrCompInfo(const COMPINFO * compInfo);

		BOOL IsCompChar(char cInput);
	};


	class CMyData{
	public:
		CMyData(const SStringT & strDataPath);
		~CMyData();
		CompInfo	m_compInfo;

		SStringW getFontFile(const SStringW &strFace) const;

		CAutoRefPtr<IUiDefInfo> m_defUiDefine;

		SStringT	m_strSkin;
		CPoint		m_ptStatus;
		CPoint		m_ptInput;

		int			m_cInputCount;
		DWORD		m_tmInputSpan;
		CRegKey		m_reg;
	private:
		SMap<SStringW, SStringW> m_fontMap;
	};

	class CNameTypePair
	{
	public:
		SStringA strType;
		SStringA strName;
	};

class CDataCenter : public SSingleton<CDataCenter>
{
public:
	CDataCenter(const SStringT & strDataPath );
	~CDataCenter(void);

public:
	void Lock();
	void Unlock();

	CMyData & GetData(){return m_data;}
	const SArray<CNameTypePair> & UpdateCompList();
	const SArray<CNameTypePair> & GetCompList() const;
	int GetSelectCompIndex() const;
private:
	CMyData	m_data;
	
	SArray<CNameTypePair> m_compList;
	int					  m_iSelComp;
	CRITICAL_SECTION m_cs;
};

}
