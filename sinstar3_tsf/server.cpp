#include "stdafx.h"
#include "sinstar3_tsf.h"

// from Register.cpp
BOOL RegisterProfiles();
void UnregisterProfiles();
BOOL RegisterCategories();
void UnregisterCategories();
BOOL RegisterServer();
void UnregisterServer();

void FreeGlobalObjects(void);

class CClassFactory;
static CClassFactory *g_ObjectInfo[1] = { NULL };

//+---------------------------------------------------------------------------
//
//  DllAddRef
//
//----------------------------------------------------------------------------

void DllAddRef(void)
{
    InterlockedIncrement(&g_cRefDll);
}

//+---------------------------------------------------------------------------
//
//  DllRelease
//
//----------------------------------------------------------------------------

void DllRelease(void)
{
    if (InterlockedDecrement(&g_cRefDll) < 0) // g_cRefDll == -1 with zero refs
    {
        EnterCriticalSection(&g_cs);

        // need to check ref again after grabbing mutex
        if (g_ObjectInfo[0] != NULL)
        {
            FreeGlobalObjects();
        }
        assert(g_cRefDll == -1);

        LeaveCriticalSection(&g_cs);
    }
}

//+---------------------------------------------------------------------------
//
//  CClassFactory declaration with IClassFactory Interface
//
//----------------------------------------------------------------------------

class CClassFactory : public IClassFactory
{
public:
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // IClassFactory methods
    STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);
    STDMETHODIMP LockServer(BOOL fLock);

    // Constructor
    CClassFactory(REFCLSID rclsid, HRESULT (*pfnCreateInstance)(IUnknown *pUnkOuter, REFIID riid, void **ppvObj))
        : _rclsid(rclsid)
    {
        _pfnCreateInstance = pfnCreateInstance;
    }

public:
    REFCLSID _rclsid;
    HRESULT (*_pfnCreateInstance)(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);
};

//+---------------------------------------------------------------------------
//
//  CClassFactory::QueryInterface
//
//----------------------------------------------------------------------------

STDAPI CClassFactory::QueryInterface(REFIID riid, void **ppvObj)
{
    if (IsEqualIID(riid, IID_IClassFactory) || IsEqualIID(riid, IID_IUnknown))
    {
        *ppvObj = this;
        DllAddRef();
        return NOERROR;
    }
    *ppvObj = NULL;
    return E_NOINTERFACE;
}

//+---------------------------------------------------------------------------
//
//  CClassFactory::AddRef
//
//----------------------------------------------------------------------------

STDAPI_(ULONG) CClassFactory::AddRef()
{
    DllAddRef();
    return g_cRefDll+1; // -1 w/ no refs
}

//+---------------------------------------------------------------------------
//
//  CClassFactory::Release
//
//----------------------------------------------------------------------------

STDAPI_(ULONG) CClassFactory::Release()
{
    DllRelease();
    return g_cRefDll+1; // -1 w/ no refs
}

//+---------------------------------------------------------------------------
//
//  CClassFactory::CreateInstance
//
//----------------------------------------------------------------------------

STDAPI CClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj)
{
    return _pfnCreateInstance(pUnkOuter, riid, ppvObj);
}

//+---------------------------------------------------------------------------
//
//  CClassFactory::LockServer
//
//----------------------------------------------------------------------------

STDAPI CClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
    {
        DllAddRef();
    }
    else
    {
        DllRelease();
    }

    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  BuildGlobalObjects
//
//----------------------------------------------------------------------------

void BuildGlobalObjects(void)
{
    // Build CClassFactory Objects

    g_ObjectInfo[0] = new CClassFactory(c_clsidBaidujpTSF, CBaiduJPTSF::CreateInstance);

    // You can add more object info here.
    // Don't forget to increase number of item for g_ObjectInfo[],
}

//+---------------------------------------------------------------------------
//
//  FreeGlobalObjects
//
//----------------------------------------------------------------------------

void FreeGlobalObjects(void)
{
    // Free CClassFactory Objects
    for (int i = 0; i < _countof(g_ObjectInfo); i++)
    {
        if (NULL != g_ObjectInfo[i])
        {
            delete g_ObjectInfo[i];
            g_ObjectInfo[i] = NULL;
        }
    }
}

//+---------------------------------------------------------------------------
//
//  DllGetClassObject
//
//----------------------------------------------------------------------------

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObj)
{
    if (g_ObjectInfo[0] == NULL)
    {
        EnterCriticalSection(&g_cs);

            // need to check ref again after grabbing mutex
            if (g_ObjectInfo[0] == NULL)
            {
                BuildGlobalObjects();
            }

        LeaveCriticalSection(&g_cs);
    }

    if (IsEqualIID(riid, IID_IClassFactory) ||
        IsEqualIID(riid, IID_IUnknown))
    {
        for (int i = 0; i < _countof(g_ObjectInfo); i++)
        {
            if (NULL != g_ObjectInfo[i] &&
                IsEqualGUID(rclsid, g_ObjectInfo[i]->_rclsid))
            {
                *ppvObj = (void *)g_ObjectInfo[i];
                DllAddRef();    // class factory holds DLL ref count
                return NOERROR;
            }
        }
    }

    *ppvObj = NULL;

    return CLASS_E_CLASSNOTAVAILABLE;
}

//+---------------------------------------------------------------------------
//
//  DllCanUnloadNow
//
//----------------------------------------------------------------------------

STDAPI DllCanUnloadNow(void)
{
    if (g_cRefDll >= 0) // -1 with no refs
        return S_FALSE;

    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  DllUnregisterServer
//
//----------------------------------------------------------------------------

STDAPI DllUnregisterServer(void)
{
    UnregisterProfiles();
    UnregisterCategories();
    UnregisterServer();

    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  DllRegisterServer
//
//----------------------------------------------------------------------------
/**
regsvr32 bdime.dll
regsvr32加载bdime.dll，然后调用其导出函数DllRegisterServer
DllRegisterServer分三步注册

1. 注册自己为COM组件

[HKEY_CLASSES_ROOT\CLSID\{21E929DF-F2B7-4023-B374-804EF91ECF3C}]
@="百度拼音输入法"

[HKEY_CLASSES_ROOT\CLSID\{21E929DF-F2B7-4023-B374-804EF91ECF3C}\InProcServer32]
@="d:\\anyplace\\aaa\\navi\\output\\debug\\bdime.dll"
"ThreadingModel"="Apartment"

2. 在CTF注册profile

->Register(...):
[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\CTF\TIP\{21E929DF-F2B7-4023-B374-804EF91ECF3C}]
"Enable"="1"

// 以下设%BDIME_CTF_ROOT% = HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\CTF\TIP\{21E929DF-F2B7-4023-B374-804EF91ECF3C}

->AddLanguageProfile(...):
[%BDIME_CTF_ROOT%\LanguageProfile\0x00000804\{D1BD2208-FE45-4B68-93A9-6963DCC10ECB}]
"Description"="百度拼音输入法"
"IconFile"="d:\\anyplace\\aaa\\navi\\output\\debug\\bdime.dll"
"IconIndex"=dword:00000000


3. 在CTF注册category
->RegisterCategory(...GUID_TFCAT_TIP_KEYBOARD...):
[%BDIME_CTF_ROOT%\Category\Category\{34745C63-B2F0-4784-8B67-5E12C8701A31}\{21E929DF-F2B7-4023-B374-804EF91ECF3C}]
[%BDIME_CTF_ROOT%\Category\Item\{21E929DF-F2B7-4023-B374-804EF91ECF3C}\{34745C63-B2F0-4784-8B67-5E12C8701A31}]

->RegisterCategory(...GUID_TFCAT_DISPLAYATTRIBUTEPROVIDER...): (已不再注册此类别)
[%BDIME_CTF_ROOT%\Category\Category\{046B8C80-1647-40F7-9B21-B93B81AABC1B}\{21E929DF-F2B7-4023-B374-804EF91ECF3C}]
[%BDIME_CTF_ROOT%\Category\Item\{21E929DF-F2B7-4023-B374-804EF91ECF3C}\{046B8C80-1647-40F7-9B21-B93B81AABC1B}]

*/
STDAPI DllRegisterServer(void)
{
    // register this service's profile with the tsf
    if (!RegisterServer() ||
        !RegisterProfiles() ||
        !RegisterCategories())
    {
        DllUnregisterServer(); // cleanup any loose ends
        return E_FAIL;
    }

    return S_OK;
}

