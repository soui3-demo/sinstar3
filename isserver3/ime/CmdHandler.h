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
	CMD_HOTKEY_MAKEPHRASE = 100,
	CMD_HOTKEY_QUERYINFO,
	CMD_HOTKEY_KEYMAP,
	CMD_HOTKEY_HIDESTATUSBAR,
	CMD_HOTKEY_INPUTMODE,
	CMD_HOTKEY_CHARMODE,
	CMD_HOTKEY_ENGLISHMODE,
	CMD_HOTKEY_FILTERGBK,
	CMD_HOTKEY_TTS,
	CMD_HOTKEY_RECORD,
	CMD_FOLLOWCARET,
	CMD_KEYSPEED,
	CMD_UPDATEMODE,
	CMD_CHANGESKIN,
	CMD_OPENSKINDIR,
	CMD_OPENCONFIG,
	CMD_SHOWTIP,
	CMD_EXECUTETOOL,
	CMD_SYNCUI,
	CMD_OPENHELP,
};

struct TIPINFO
{
	TIPINFO(const SStringT & title=_T(""), const SStringT & tip=_T("")) :strTitle(title), strTip(tip)
	{}
	SStringT strTitle;
	SStringT strTip;
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

protected:
	void OnHotkeyMakePhrase(LPARAM lp);
	void OnHotKeyKeyMap(LPARAM lp);
	void OnHotKeyHideStatusBar(LPARAM lp);
	void OnHotKeyQueryInfo(LPARAM lp);
	void OnHotKeyCharMode(LPARAM lp);
	void OnHotKeyEnglishMode(LPARAM lp);
	void OnHotKeyInputMode(LPARAM lp);
	void OnHotKeyFilterGbk(LPARAM lp);
	void OnFollowCaret(LPARAM lp);
	void OnKeySpeed(LPARAM lp);
	void OnUpdateMode(LPARAM lp);
	void OnHotkeyTTS(LPARAM lp);
	void OnHotkeyRecord(LPARAM lp);
	void OnChangeSkin(LPARAM lp);
	void OnOpenConfig(LPARAM lp);
	void OnOpenSkinDir(LPARAM lp);
	void OnShowTip(LPARAM lp);
	void OnExecuteTool(LPARAM lp);
	void OnSyncUI(LPARAM lp);
	void OnOpenHelp(LPARAM lp);

	BEGIN_MSG_MAP_EX(CCmdHandler)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_TTS,OnHotkeyTTS)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_RECORD,OnHotkeyRecord)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_MAKEPHRASE, OnHotkeyMakePhrase)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_KEYMAP, OnHotKeyKeyMap)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_HIDESTATUSBAR, OnHotKeyHideStatusBar)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_QUERYINFO, OnHotKeyQueryInfo)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_ENGLISHMODE, OnHotKeyEnglishMode)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_INPUTMODE, OnHotKeyInputMode)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_CHARMODE, OnHotKeyCharMode)
		MSG_WM_COMMAND_EX(CMD_HOTKEY_FILTERGBK, OnHotKeyFilterGbk)
		MSG_WM_COMMAND_EX(CMD_FOLLOWCARET, OnFollowCaret)
		MSG_WM_COMMAND_EX(CMD_KEYSPEED, OnKeySpeed)
		MSG_WM_COMMAND_EX(CMD_UPDATEMODE,OnUpdateMode)
		MSG_WM_COMMAND_EX(CMD_CHANGESKIN,OnChangeSkin)
		MSG_WM_COMMAND_EX(CMD_OPENCONFIG,OnOpenConfig)
		MSG_WM_COMMAND_EX(CMD_OPENSKINDIR,OnOpenSkinDir)
		MSG_WM_COMMAND_EX(CMD_SHOWTIP, OnShowTip)
		MSG_WM_COMMAND_EX(CMD_EXECUTETOOL,OnExecuteTool)
		MSG_WM_COMMAND_EX(CMD_SYNCUI,OnSyncUI)
		MSG_WM_COMMAND_EX(CMD_OPENHELP,OnOpenHelp)
	END_MSG_MAP()

private:
	CSinstar3Impl * m_pSinstar3;
};

