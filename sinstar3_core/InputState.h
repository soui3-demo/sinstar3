#pragma once

#include "inputContext.h"

interface ICmdListener {
	virtual void OnCommand(WORD cmd, LPARAM lp) = 0;
	virtual InputContext * GetInputContext() = 0;
};

interface IInputListener: public ICmdListener {
	virtual HWND GetHwnd() const = 0;
	virtual void OnInputStart() = 0;
	virtual void OnInputResult(const SStringT & strResult,const SStringT & strComp=SStringT())=0;
	virtual void OnInputEnd() = 0;
	virtual void UpdateInputWnd() = 0;

	virtual BOOL GoNextCandidatePage() = 0;
	virtual BOOL GoPrevCandidatePage() = 0;
	virtual short SelectCandidate(short iCand)=0;

	virtual void OpenInputWnd() = 0;
	virtual void CloseInputWnd(int nDelayMS) = 0;
	virtual BOOL SetOpenStatus(BOOL bOpen)=0;
	virtual BOOL GetOpenStatus() const =0;
	virtual void UpdateStatusbar() = 0;
};

class CInputState
{
	friend class CSinstar3Impl;
public:
	CInputState(void);
	~CInputState(void);

	InputContext * GetInputContext() {return &m_ctx;}
	void SetInputListener(IInputListener * pListener){m_pListener=pListener;}

	BOOL TestKeyDown(UINT uKey,LPARAM lKeyData,const BYTE * lpbKeyState);
	BOOL HandleKeyDown(UINT vKey,UINT uScanCode,const BYTE * lpbKeyState);

	void OnImeSelect(BOOL bSelect);

	BOOL OnSvrNotify(UINT wp, PMSGDATA pMsgData);

	BOOL IsTypeing() const {return m_bTypeing;}

	BOOL IsTempSpell() const;

	void ClearContext(UINT ccMask);
protected:
	void InputStart();
	void InputResult(const SStringA &strResult,BYTE byAstMask);
	void InputResult(const SStringT &strResult,BYTE byAstMask);
	void InputEnd();
	void InputUpdate();
	void InputOpen();
	void InputHide(BOOL bDelay = FALSE);
	void StatusbarUpdate();
private:
	BOOL Tips_Rand(BOOL bSpell, char * pszBuf);

	BYTE GetKeyinMask(BOOL bAssociate,BYTE byMask);

	void KeyIn_Spell_UpdateCandList(InputContext * lpCntxtPriv,BYTE byCaret);
	void KeyIn_Spell_Forecast(InputContext * lpCntxtPriv,BYTE byStartPos);
	BOOL KeyIn_Spell_MoveCaret(InputContext *lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_SyFix(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_Normal(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_ChangeComp(InputContext* lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_InputText(InputContext* lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_GetSpellInput(InputContext * lpCntxtPriv,BYTE bySpellID[MAX_SYLLABLES][2]);
	BOOL KeyIn_Spell_Locate(InputContext *lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Spell_Symbol(InputContext* lpCntxtPriv,UINT byInput, CONST BYTE* lpbKeyState);

	BOOL KeyIn_All_TurnCandPage(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_All_SelectCand(InputContext * lpCntxtPriv,UINT byInput,char cCompLen, CONST BYTE * lpbKeyState);
	BOOL KeyIn_InputAndAssociate(InputContext * lpCntxtPriv,const char *pszInput,short sLen,BYTE byMask);
	void GetShapeComp(const char *pInput,char cLen);
	BOOL KeyIn_PYBiHua_ChangComp(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Code_Normal(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Code_ChangeComp(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_Code_Symbol(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_All_Sentence(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	BOOL KeyIn_All_Associate(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);
	void KeyIn_Sent_Input(InputContext* lpCntxtPriv);
	BOOL KeyIn_Code_English(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE* lpbKeyState);
	BOOL KeyIn_Digital_ChangeComp(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE* lpbKeyState);
	BOOL KeyIn_UserDef_ChangeComp(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE* lpbKeyState);
	BOOL KeyIn_Line_ChangeComp(InputContext * lpCntxtPriv,UINT byInput, CONST BYTE * lpbKeyState);

	InputContext m_ctx;
	BOOL		 m_bTypeing;
	IInputListener * m_pListener;
	BYTE * m_pbyMsgBuf;
	BOOL		 m_fOpen;

	enum TipType {
		TT_SHAPE=0,
		TT_SPELL,
		TT_BOTH,
		TT_COUNT,
	};

	SArray<SStringA>  m_tips[TT_COUNT];
	BOOL			 m_bUpdateTips;

	DWORD			 m_tmInputStart;
	DWORD			 m_tmInputEnd;
};
