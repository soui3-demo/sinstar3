//////////////////////////////////////////////////////////////////////
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
//  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//
//  Copyright (C) 2003  Microsoft Corporation.  All rights reserved.
//
//  Globals.h
//
//          Global variable declarations.
//
//////////////////////////////////////////////////////////////////////

#ifndef GLOBALS_H
#define GLOBALS_H

void DllAddRef();
void DllRelease();

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))
#endif

#define TEXTSERVICE_MODEL   TEXT("Apartment")

#define TEXTSERVICE_ICON_INDEX  0

void DllAddRef();
void DllRelease();

extern HINSTANCE g_hInst;

extern LONG g_cRefDll;

extern CRITICAL_SECTION g_cs;

void Helper_Trace(LPCTSTR pszFormat,...);

BOOL IsRangeCovered(TfEditCookie ec, ITfRange *pRangeTest, ITfRange *pRangeCover);

BOOL AdviseSink(IUnknown *pSource, IUnknown *pSink, REFIID riid, DWORD *pdwCookie);
void UnadviseSink(IUnknown *pSource, DWORD *pdwCookie);

BOOL AdviseSingleSink(TfClientId tfClientId, IUnknown *pSource, IUnknown *pSink, REFIID riid);
void UnadviseSingleSink(TfClientId tfClientId, IUnknown *pSource, REFIID riid);


#endif // GLOBALS_H
