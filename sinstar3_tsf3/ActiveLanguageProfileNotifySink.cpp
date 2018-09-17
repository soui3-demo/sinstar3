// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include "Private.h"
#include "Globals.h"
#include "Sinstar3_Tsf.h"
#include "CompositionProcessorEngine.h"

BOOL CSinstar3Tsf::VerifySinstar3TsfCLSID(_In_ REFCLSID clsid)
{
    if (IsEqualCLSID(clsid, Global::Sinstar3TsfCLSID))
    {
        return TRUE;
    }
    return FALSE;
}

//+---------------------------------------------------------------------------
//
// ITfActiveLanguageProfileNotifySink::OnActivated
//
// Sink called by the framework when changes activate language profile.
//----------------------------------------------------------------------------

STDAPI CSinstar3Tsf::OnActivated(_In_ REFCLSID clsid, _In_ REFGUID guidProfile, _In_ BOOL isActivated)
{
	guidProfile;

    if (FALSE == VerifySinstar3TsfCLSID(clsid))
    {
        return S_OK;
    }

    if (isActivated)
    {
        _AddTextProcessorEngine();
    }

    if (nullptr == _pCompositionProcessorEngine)
    {
        return S_OK;
    }

    if (isActivated)
    {
        _pCompositionProcessorEngine->ShowAllLanguageBarIcons();

        _pCompositionProcessorEngine->ConversionModeCompartmentUpdated(_pThreadMgr);
    }
    else
    {
        _DeleteCandidateList(FALSE, nullptr);

        _pCompositionProcessorEngine->HideAllLanguageBarIcons();
    }

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// _InitActiveLanguageProfileNotifySink
//
// Advise a active language profile notify sink.
//----------------------------------------------------------------------------

BOOL CSinstar3Tsf::_InitActiveLanguageProfileNotifySink()
{
    ITfSource* pSource = nullptr;
    BOOL ret = FALSE;

    if (_pThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource) != S_OK)
    {
        return ret;
    }

    if (pSource->AdviseSink(IID_ITfActiveLanguageProfileNotifySink, (ITfActiveLanguageProfileNotifySink *)this, &_activeLanguageProfileNotifySinkCookie) != S_OK)
    {
        _activeLanguageProfileNotifySinkCookie = TF_INVALID_COOKIE;
        goto Exit;
    }

    ret = TRUE;

Exit:
    pSource->Release();
    return ret;
}

//+---------------------------------------------------------------------------
//
// _UninitActiveLanguageProfileNotifySink
//
// Unadvise a active language profile notify sink.  Assumes we have advised one already.
//----------------------------------------------------------------------------

void CSinstar3Tsf::_UninitActiveLanguageProfileNotifySink()
{
    ITfSource* pSource = nullptr;

    if (_activeLanguageProfileNotifySinkCookie == TF_INVALID_COOKIE)
    {
        return; // never Advised
    }

    if (_pThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
    {
        pSource->UnadviseSink(_activeLanguageProfileNotifySinkCookie);
        pSource->Release();
    }

    _activeLanguageProfileNotifySinkCookie = TF_INVALID_COOKIE;
}
