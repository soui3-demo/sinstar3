#pragma once

#define MSG_WM_COMMAND_EX(code, func) \
    if (uMsg == WM_COMMAND && code == HIWORD(wParam)) \
    { \
        SetMsgHandled(TRUE); \
        func(lParam); \
        lResult = 0; \
        if(IsMsgHandled()) \
            return TRUE; \
    }

enum {
	CMD_MAKEWORD = 100,
	CMD_QUERYINFO,
	CMD_KEYMAP,
	CMD_HIDESTATUSBAR,
	CMD_INPUTMODE,
	CMD_FOLLOWCARET,
	CMD_KEYSPEED,
	CMD_CHARMODE,
	CMD_ENGLISHMODE,
};

namespace SOUI {
	class STipWnd;
}

class CSinstar3Impl;
class CCmdHandler 
{
public:
	CCmdHandler(CSinstar3Impl * pSinstar3);
	~CCmdHandler();

	void SetTipWnd(STipWnd * pTipWnd);
protected:
	void OnMakeWord(LPARAM lp);
	void OnKeyMap(LPARAM lp);
	void OnHideStatusBar(LPARAM lp);
	void OnQueryInfo(LPARAM lp);
	void OnFollowCaret(LPARAM lp);
	void OnInputMode(LPARAM lp);
	void OnKeySpeed(LPARAM lp);
	void OnCharMode(LPARAM lp);
	void OnEnglishMode(LPARAM lp);

	BEGIN_MSG_MAP_EX(CCmdHandler)
		MSG_WM_COMMAND_EX(CMD_MAKEWORD, OnMakeWord)
		MSG_WM_COMMAND_EX(CMD_KEYMAP, OnKeyMap)
		MSG_WM_COMMAND_EX(CMD_HIDESTATUSBAR, OnHideStatusBar)
		MSG_WM_COMMAND_EX(CMD_QUERYINFO, OnQueryInfo)
		MSG_WM_COMMAND_EX(CMD_FOLLOWCARET, OnFollowCaret)
		MSG_WM_COMMAND_EX(CMD_INPUTMODE, OnInputMode)
		MSG_WM_COMMAND_EX(CMD_KEYSPEED, OnKeySpeed)
		MSG_WM_COMMAND_EX(CMD_CHARMODE,OnCharMode)
		MSG_WM_COMMAND_EX(CMD_ENGLISHMODE,OnEnglishMode)
	END_MSG_MAP()

private:
	STipWnd * m_pTipWnd;
	CSinstar3Impl * m_pSinstar3;
};

