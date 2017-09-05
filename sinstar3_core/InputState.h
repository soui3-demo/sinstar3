#pragma once

#include "inputContext.h"

interface IInputListener{
	virtual HWND GetHwnd() const = 0;
	virtual void OnInputStart() = 0;
	virtual void OnInputEnd(const SStringT & strInput) = 0;
};

class CInputState
{
	friend class CSinstar3Impl;
public:
	CInputState(void);
	~CInputState(void);

	const InputContext * GetInputContext() const {return &m_ctx;}
	void SetInputListener(IInputListener * pListener){m_pListener=pListener;}

	BOOL HandleKeyDown(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState);
protected:
	virtual void OnInputStart();
	virtual void OnInputEnd(const SStringT &strResult);
private:
	void ClearContext(UINT ccMask);
	//update input mode
	BOOL UpdateInputMode(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState);
	BOOL HandleShapeCodeKeyDown(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState);

	InputContext m_ctx;
	BOOL		 m_bCoding;
	IInputListener * m_pListener;
	byte * m_pbyMsgBuf;

};
