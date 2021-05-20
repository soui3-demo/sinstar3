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

		BOOL IsCompChar(WCHAR cInput);

		CompInfo() :nCodeNum(0) ,cWild(0)
		{}
	};

	class CNameTypePair
	{
	public:
		SStringT strTitle;
		SStringT strName;
	};

	struct SkinInfo
	{
		CPoint		ptOffset;
		SStringW    vertLayout;
		SStringW	horzLayout;
	};

	class CMyData{
	public:
		CMyData(const SStringT & strDataPath);
		~CMyData();
		CompInfo	m_compInfo;

		SStringW getFontFile(const SStringW &strFace) const;
		const SArray<CNameTypePair> & UpdateCompList();
		const SArray<CNameTypePair>& UpdateFlmList();

		SArray<CNameTypePair> m_compList;
		SArray<CNameTypePair> m_flmList;

		SAutoRefPtr<IUiDefInfo> m_defUiDefine;	//uidef info of builtin skin.

		DWORD getTotalInput() const;
		DWORD getTotalSpan() const;

		SkinInfo	m_skinInfo;

		DWORD		m_cTotalInput;
		DWORD		m_tmTotalSpan;
		DWORD		m_cInputCount;
		DWORD		m_tmInputSpan;

		int			m_nScale;

		bool changeSkin(const SStringT &strSkin);
		bool saveSpeed();

		SStringT GetDataPath() const;
	private:
		SMap<SStringW, SStringW> m_fontMap;
		SStringT	m_strDataPath;
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

	SStringT GetDataPath() const;
private:
	CMyData	m_data;
	SStringT m_path;
	CRITICAL_SECTION m_cs;
};

}
