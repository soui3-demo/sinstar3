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
	BYTE GetKeyinMask(BOOL bAssociate,BYTE byMask);
	void ClearContext(UINT ccMask);
	//update input mode
	BOOL UpdateInputMode(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState);

	BOOL HandleShapeCodeKeyDown(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState);

	BOOL HandleSpellKeyDown(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState);

	void KeyIn_Spell_UpdateCandList(InputContext * lpCntxtPriv,BYTE byCaret);
	void KeyIn_Spell_Forecast(InputContext * lpCntxtPriv,BYTE byStartPos);
	BOOL KeyIn_Spell_MoveCaret(InputContext *lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_SyFix(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_Normal(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_ChangeComp(InputContext* lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_InputText(InputContext* lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_GetSpellInput(InputContext * lpCntxtPriv,BYTE bySpellID[MAX_SYLLABLES][2]);
	BOOL KeyIn_Spell_Locate(InputContext *lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
//	BOOL KeyIn_Spell_Symbol(InputContext * lpCntxtPriv,BYTE byInput, CONST LPBYTE lpbKeyState);
	InputContext m_ctx;
	BOOL		 m_bCoding;
	IInputListener * m_pListener;
	byte * m_pbyMsgBuf;

};
