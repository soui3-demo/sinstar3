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


BOOL IsRangeCovered(TfEditCookie ec, ITfRange *pRangeTest, ITfRange *pRangeCover);

BOOL AdviseSink(IUnknown *pSource, IUnknown *pSink, REFIID riid, DWORD *pdwCookie);
void UnadviseSink(IUnknown *pSource, DWORD *pdwCookie);

BOOL AdviseSingleSink(TfClientId tfClientId, IUnknown *pSource, IUnknown *pSink, REFIID riid);
void UnadviseSingleSink(TfClientId tfClientId, IUnknown *pSource, REFIID riid);


#endif // GLOBALS_H
