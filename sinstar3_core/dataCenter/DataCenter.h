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


	class AutoLocker
	{
		friend class CDataCenter;
	public:

		CDataCenter* operator->() const throw()
		{
			return p;
		}

		operator CDataCenter*() const throw()
		{
			return p;
		}
		CDataCenter& operator*() const
		{
			return *p;
		}
		CDataCenter** operator&() throw()
		{
			SASSERT(p==NULL);
			return &p;
		}
		bool operator!() const throw()
		{
			return (p == NULL);
		}

		AutoLocker & operator=(const AutoLocker &src) throw()
		{
			if(p)
			{
				p->Unlock();
			}
			p=src.p;
			if(p)
			{
				p->Lock();
			}
			return *this;
		}

		AutoLocker(const AutoLocker & src):p(src.p)
		{
			p->Lock();
		}

		~AutoLocker()
		{
			p->Unlock();
		}

	private:
		AutoLocker(CDataCenter *_p):p(_p)
		{
			p->Lock();
		}

		CDataCenter * p;
	};


	static AutoLocker GetAutoLockerInstance();
public:
	void Lock();
	void Unlock();

	CMyData & GetData(){return m_data;}
private:
	CMyData	m_data;
	
	CRITICAL_SECTION m_cs;
};

}
