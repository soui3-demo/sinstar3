#pragma once

namespace SOUI
{
	struct CompInfo
	{
		SStringT strCompName;
		WCHAR szCode[50];
		int  nCodeNum;
		TCHAR    cWild;

		void SetSvrCompInfo(const COMPHEAD * compHead);

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

		CAutoRefPtr<IUiDefInfo> m_defUiDefine;	//uidef info of builtin skin.

		DWORD getTotalInput() const;
		DWORD getTotalSpan() const;

		CPoint		m_ptSkinOffset;

		DWORD		m_cTotalInput;
		DWORD		m_tmTotalSpan;
		DWORD		m_cInputCount;
		DWORD		m_tmInputSpan;

		int			m_nScale;

		bool changeSkin(const SStringT &strSkin);
		bool saveSpeed();
	private:
		SMap<SStringW, SStringW> m_fontMap;
	};

	class CNameTypePair
	{
	public:
		SStringT strType;
		SStringT strName;
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
