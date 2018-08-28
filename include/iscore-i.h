#pragma once

#ifdef ISCORE_EXPORTS
#define ISCORE_API __declspec(dllexport)
#else
#define ISCORE_API __declspec(dllimport)
#endif

typedef enum tagPROGTYPE { PT_MAX = 0, PT_PROG, PT_END } PROGTYPE;
struct IBuildIndexProgressListener
{
	virtual void OnBuildShapePhraseIndex(PROGTYPE uType, unsigned int dwData) = 0;
	virtual void OnBuildSpellPhraseIndex(PROGTYPE uType, unsigned int dwData) = 0;
	virtual void OnBuildSpellPhraseIndex2(PROGTYPE uType, unsigned int dwData) = 0;
	virtual void OnImportUserDict(PROGTYPE uType, unsigned int dwData) = 0;
};

struct IDataBlock {
	virtual void Release() = 0;
	virtual void * GetData() const = 0;
	virtual int GetLength() const = 0;
};

#define MAX_TOKEN_NAME_LENGHT	200

struct IUiMsgHandler : IBuildIndexProgressListener 
{
	virtual void OnClientActive() = 0;
	virtual void OnClientLogin() = 0;
	virtual void OnClientLogout() = 0;

	virtual void OnShowTray(bool bTray) = 0;

	virtual void OnShowKeyMap(IDataBlock * pCompData, LPCSTR pszName, LPCSTR pszUrl) = 0;

	virtual int  TtsGetSpeed() = 0;
	virtual void TtsSetSpeed(int nSpeed) = 0;
	virtual void TtsSpeakText(const wchar_t* pText, int nLen, bool bChinese) = 0;
	virtual int  TtsGetVoice(bool bCh) = 0;
	virtual void TtsSetVoice(bool bCh, int iToken) = 0;
	virtual int TtsGetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT],int nBufSize) = 0;

	virtual DWORD OnQueryVersion() const = 0;
};


struct IServerCore {
	virtual ~IServerCore() {}

	virtual BOOL Init(HWND hWnd, IUiMsgHandler * pUiMsgHander, LPCSTR pszDataPath) = 0;

	virtual void Uninit() = 0;

	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) = 0;

	virtual bool CanQuitNow() const = 0;

	virtual bool IsShowTray() const = 0;

	virtual bool IsAutoQuit() const = 0;

	virtual void SetAutoQuit(bool bAutoQuit) = 0;

	virtual void GetConfigIni(LPSTR pszBuf, int nLen) = 0;
};


#ifdef __cplusplus
extern "C" {
#endif
ISCORE_API IServerCore * Iscore_Create();
ISCORE_API void Iscore_Destroy(IServerCore* pCore);
#ifdef __cplusplus
}
#endif