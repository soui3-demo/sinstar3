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
		CMyData();
		~CMyData();
		CompInfo	m_compInfo;

		CAutoRefPtr<IUiDefInfo> m_defUiDefine;

		SStringT	m_strSkin;
		CPoint		m_ptStatus;
		CPoint		m_ptInput;

		int			m_cInputCount;
		CRegKey		m_reg;
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
	CDataCenter(void);
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
