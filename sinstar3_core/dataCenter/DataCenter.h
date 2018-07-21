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
		
		int			m_cInputCount;
		CRegKey		m_reg;
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
private:
	CMyData	m_data;
	
	CRITICAL_SECTION m_cs;
};

}
