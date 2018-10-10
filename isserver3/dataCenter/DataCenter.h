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

		CompInfo() :nCodeNum(0) ,cWild(0)
		{}
	};


	class CMyData{
	public:
		CMyData(const SStringT & strDataPath);
		~CMyData();
		CompInfo	m_compInfo;

		SStringW getFontFile(const SStringW &strFace) const;

		CAutoRefPtr<IUiDefInfo> m_defUiDefine;

		SStringT	m_strSkin;

		SStringT	m_strDebugSkin;

		CPoint		m_ptStatus;
		CPoint		m_ptInput;
		CPoint		m_ptSpchar;
		CPoint		m_ptSkinOffset;

		int			m_cInputCount;
		DWORD		m_tmInputSpan;
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

	SStringT GetDataPath() const;
private:
	CMyData	m_data;
	SStringT m_path;
	SArray<CNameTypePair> m_compList;
	int					  m_iSelComp;
	CRITICAL_SECTION m_cs;
};

}
