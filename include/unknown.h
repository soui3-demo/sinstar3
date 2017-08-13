#define IUNKNOWN_BEGIN(iid,cls ) \
	public:\
	STDMETHODIMP_(ULONG) AddRef(void)\
	{\
	return ++_cRef;\
	}\
	STDMETHODIMP_(ULONG) Release(void)\
	{\
	LONG cr = --_cRef;\
	assert(_cRef >= 0);\
	if (_cRef == 0)\
	{\
	delete this;\
	}\
	return cr;\
	}\
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj)\
	{\
	if (ppvObj == NULL)\
	return E_INVALIDARG;\
	*ppvObj = NULL;\
	if(IsEqualIID(riid,iid) || IsEqualIID(riid,IID_IUnknown))\
	{\
	*ppvObj=(cls *)this;\
	}\

#define IUNKNOWN_ADD_IID(iid,cls) \
	if(IsEqualIID(riid,iid))\
		{\
		*ppvObj=(cls *)this;\
		}\

#define IUNKNOWN_END() \
	if (*ppvObj)\
		{\
		AddRef();\
		return S_OK;\
		}\
		return E_NOINTERFACE;\
	}\
	private:\
	ULONG _cRef;\
	public:\

