#pragma once

namespace SOUI
{

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

	struct CompInfo
	{
		SStringT strCompName;
		TCHAR    cWild;
	};

	static AutoLocker GetAutoLockerInstance();
public:
	void Lock();
	void Unlock();

	CompInfo & getCompInfo(){return m_compInfo;}
private:
	CompInfo		m_compInfo;
	CRITICAL_SECTION m_cs;
};

}
