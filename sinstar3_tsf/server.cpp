#include "stdafx.h"
#include "sinstar3_tsf.h"

// from Register.cpp
BOOL RegisterProfiles();
void UnregisterProfiles();
BOOL RegisterCategories();
void UnregisterCategories();
BOOL RegisterServer();
void UnregisterServer();



//+---------------------------------------------------------------------------
//
//  DllGetClassObject
//
//----------------------------------------------------------------------------

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObj)
{
	theModule->BuildClassFactory();
	return theModule->GetClassObject(rclsid,riid,ppvObj);
}

//+---------------------------------------------------------------------------
//
//  DllCanUnloadNow
//
//----------------------------------------------------------------------------

STDAPI DllCanUnloadNow(void)
{
	return theModule->GetRefCount() == 0? S_OK:S_FALSE;
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
DllRegisterServer分三步注册

1. 注册自己为COM组件

[HKEY_CLASSES_ROOT\CLSID\{21E929DF-F2B7-4023-B374-804EF91ECF3C}]
@="启程输入法"

[HKEY_CLASSES_ROOT\CLSID\{21E929DF-F2B7-4023-B374-804EF91ECF3C}\InProcServer32]
@="d:\\anyplace\\aaa\\navi\\output\\debug\\sinstar3_tsf.dll"
"ThreadingModel"="Apartment"

2. 在CTF注册profile

->Register(...):
[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\CTF\TIP\{21E929DF-F2B7-4023-B374-804EF91ECF3C}]
"Enable"="1"

// 以下设%BDIME_CTF_ROOT% = HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\CTF\TIP\{21E929DF-F2B7-4023-B374-804EF91ECF3C}

->AddLanguageProfile(...):
[%BDIME_CTF_ROOT%\LanguageProfile\0x00000804\{D1BD2208-FE45-4B68-93A9-6963DCC10ECB}]
"Description"="启程输入法"
"IconFile"="d:\\anyplace\\aaa\\navi\\output\\debug\\sinstar3_tsf.dll"
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

