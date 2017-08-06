#pragma once

template<class T>
class CUnknownImpl: public T
{
public:
	CUnknownImpl():m_cRef(1)
	{

	}

	template<class P1>
	CUnknownImpl(P1 p1):T(p1),m_cRef(1){}

	template<class P1, class P2>
	CUnknownImpl(P1 p1,P2 p2):T(p1,p2),m_cRef(1){}


	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		return T::OnQueryInterface(riid,ppvObject);
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return (ULONG)InterlockedIncrement(&m_cRef);
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)
	{
		LONG ret = InterlockedDecrement(&m_cRef);
		if(ret == 0)
		{
			T::OnFinalRelease();
			delete this;
		}
		return ret;
	}

private:
	volatile LONG m_cRef;
};