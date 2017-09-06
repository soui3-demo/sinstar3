#pragma once

#include "inputContext.h"

interface IInputListener{
	virtual HWND GetHwnd() const = 0;
	virtual void OnInputStart() = 0;
	virtual void OnInputEnd(const SStringT & strInput,int nDelayMS) = 0;
	virtual BOOL GoNextCandidatePage() = 0;
	virtual BOOL GoPrevCandidatePage() = 0;
	virtual short SelectCandidate(short iCand)=0;
	virtual void CloseInputWnd(int nDelayMS) = 0;
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

	BOOL IsTempSpell() const;
protected:
	virtual void OnInputStart();
	virtual void OnInputEnd(const SStringT &strResult,byte byMask=0,BOOL delay=FALSE);
private:
	BYTE GetKeyinMask(BOOL bAssociate,BYTE byMask);
	void ClearContext(UINT ccMask);

	void KeyIn_Spell_UpdateCandList(InputContext * lpCntxtPriv,BYTE byCaret);
	void KeyIn_Spell_Forecast(InputContext * lpCntxtPriv,BYTE byStartPos);
	BOOL KeyIn_Spell_MoveCaret(InputContext *lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_SyFix(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_Normal(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_ChangeComp(InputContext* lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_InputText(InputContext* lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_GetSpellInput(InputContext * lpCntxtPriv,BYTE bySpellID[MAX_SYLLABLES][2]);
	BOOL KeyIn_Spell_Locate(InputContext *lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_Symbol(InputContext* lpCntxtPriv,BYTE byInput, CONST BYTE* lpbKeyState);

	BOOL KeyIn_All_TurnCandPage(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_All_SelectCand(InputContext * lpCntxtPriv,BYTE byInput,char cCompLen, CONST BYTE * lpbKeyState);
	BOOL KeyIn_InputAndAssociate(InputContext * lpCntxtPriv,const char *pszInput,short sLen,BYTE byMask);
	void GetShapeComp(const char *pInput,char cLen);
	BOOL KeyIn_PYBiHua_ChangComp(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Code_Normal(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Code_ChangeComp(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Code_Symbol(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_All_Sentence(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_All_Associate(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	void KeyIn_Sent_Input(InputContext* lpCntxtPriv);
	BOOL KeyIn_Code_English(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE* lpbKeyState);
	BOOL KeyIn_Digital_ChangeComp(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE* lpbKeyState);
	BOOL KeyIn_UserDef_ChangeComp(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE* lpbKeyState);
	BOOL KeyIn_Line_ChangeComp(InputContext * lpCntxtPriv,BYTE byInput, CONST BYTE * lpbKeyState);
	InputContext m_ctx;
	BOOL		 m_bCoding;
	IInputListener * m_pListener;
	byte * m_pbyMsgBuf;

};
