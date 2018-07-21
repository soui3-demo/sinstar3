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

#define CMD_MAKEWORD	100

namespace SOUI {
	class STipWnd;
}

class CCmdHandler 
{
public:
	CCmdHandler();
	~CCmdHandler();

	void SetTipWnd(STipWnd * pTipWnd);
protected:
	void OnMakeWord(LPARAM lp);

	BEGIN_MSG_MAP_EX(CCmdHandler)
		MSG_WM_COMMAND_EX(CMD_MAKEWORD,OnMakeWord)
	END_MSG_MAP()

private:
	STipWnd * m_pTipWnd;
};

