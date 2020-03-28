#pragma once

#ifdef ISCORE_EXPORTS
#define ISCORE_API __declspec(dllexport)
#else
#define ISCORE_API __declspec(dllimport)
#endif

#include "global.h"

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

	virtual void OnShowKeyMap(IDataBlock * pCompData, LPCWSTR pszName, LPCWSTR pszUrl) = 0;

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

	virtual BOOL Init(HWND hWnd, IUiMsgHandler * pUiMsgHander, LPCTSTR pszDataPath)=0;
	virtual void Uninit()=0;
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0)=0;

	virtual const COMPHEAD * GetCompHead() const= 0;
	virtual DWORD  GetCompIconData(LPBYTE *ppBuf) = 0;
	virtual PMSGDATA GetAck() = 0;
	virtual LRESULT ReqLogin(HWND hClientWnd)=0;
	virtual HRESULT ReqLogout(HWND hClientWnd)=0;

	virtual bool CanQuitNow() const=0;

	virtual void GetConfigIni(LPTSTR pszBuf, int nLen)=0;
	virtual bool IsDataReady() const=0;

	virtual bool InstallCit(LPCTSTR pszCitPath)=0;
	virtual BOOL CompSwitch(LPCTSTR pszCompFileTitle)=0;

	virtual bool InstallPlt(LPCTSTR pszPltPath)=0;
	virtual BOOL ExtractCompInfo(LPCTSTR pszCompFile,COMPHEAD *pHead,IDataBlock ** ppIconData)=0;

	virtual int GetSentRecordMax()=0;
	virtual void SetSentRecordMax(int nSentMax)=0;
	virtual int GetPhraseGroupCount() const=0;
	virtual BOOL QueryPhraseGroup(PGROUPINFO pInfo,int nSize)=0;
	virtual int GetMaxPhraseAstDeepness()=0;
	virtual void SetMaxPhraseAstDeepness(int nAstDeep)=0;
	virtual int GetMaxPhrasePreictLength()=0;
	virtual void SetMaxPhrasePreictLength(int nForeDeep)=0;
	virtual void ReqTTS(LPCWSTR pszText,int nLen,BOOL bCh)=0;

	virtual void BlurDelAll()=0;
	virtual LRESULT ReqBlurQuery()=0;
	virtual void BlurZCS(BOOL bBlurZCS)=0;
	virtual void BlurEnable(BOOL bEnableBlur)=0;
	virtual int BlurAdd(LPCSTR pszSpell1,LPCSTR pszSpell2)=0;
	virtual int BlurDel(LPCSTR pszSpell1,LPCSTR pszSpell2)=0;
	virtual BOOL CheckComp(LPCWSTR pszComp,char cCompLen,BYTE byMask)=0;

	virtual BOOL UpdateDataFile(LPCTSTR pszPath,int fType)=0;
	virtual BOOL ExportDataFile(int fType,TCHAR pszTxtFileName[MAX_PATH])=0;

	virtual LRESULT ReqEnablePhraseGroup(LPCWSTR pszGroup,BOOL bEnable)=0;

	virtual LRESULT ReqFlmOpen(LPCTSTR pszFilename)=0;
	virtual LRESULT ReqFlmGetInfo()=0;
	virtual LRESULT ReqFlmEnableGroup(LPCWSTR pszGroup,BOOL bEnable)=0;

	virtual LRESULT ReqSymbolConvert(char cSymbol,int nType,bool bFullChar)=0;
	virtual LRESULT ReqPhraseRate(LPCWSTR pszInput,BYTE cPhraseLen)=0;
	virtual LRESULT ReqQueryWordSpell(WCHAR wWord)=0;
	virtual LRESULT ReqSortWordByBiHua(LPCWSTR pszBiHua,char cBiHuaLen,LPWSTR pszWordList,short nWords)=0;
	virtual LRESULT ReqShowKeyMap()=0;
	virtual LRESULT ReqPhraseDel(HWND hClient,LPCWSTR pszComp,char cCompLen,LPCWSTR pszPhrase,BYTE cPhraseLen)=0;
	virtual LRESULT ReqQueryCandUD(LPCWSTR pszComp,char cCompLen)=0;
	virtual LRESULT ReqQueryCandLine(LPCWSTR pszComp,char cCompLen)=0;
	virtual LRESULT ReqSpellMemEx(LPCWSTR pszText,char cSentLen,BYTE (*pbySpellID)[2])=0;
	virtual LRESULT ReqMakePhrase(LPCWSTR pszText,BYTE cTextLen)=0;
	virtual LRESULT ReqRateAdjust(HWND hClientWnd, LPCWSTR pszComp,char cCompLen,LPCWSTR pszPhrase, BYTE cPhraseLen,BYTE byMode)=0;
	virtual LRESULT ReqSpellGetBlur(LPCSTR pszSpell,int nLen)=0;
	virtual LRESULT ReqSpell2ID(LPCSTR pszInput,int nSize)=0;
	virtual LRESULT ReqKeyIn(HWND hClientWnd, LPCWSTR pszText,int nLen,BYTE byMask)=0;
	virtual LRESULT ReqQueryCand(HWND hClientWnd,LPCWSTR pszComp,char cCompLen, BYTE byMask)=0;
	virtual LRESULT ReqQueryCandEn(LPCWSTR pszInput,int nSize)=0;
	virtual LRESULT ReqQueryCandSpellEx(PBLURINFO2 *ppbi,BYTE bySyllables,BOOL bPhraseFirst,BOOL bReverse,BOOL bAssociate)=0;
	virtual LRESULT ReqForecastSpell(WCHAR wPrefix,PBLURINFO2 *ppbi,BYTE bySyllables)=0;
	virtual LRESULT ReqQueryComp(LPCWSTR pszInput,int nSize)=0;
	virtual LRESULT ReqQueryCompSpell(LPCWSTR pszInput,int nSize)=0;
	virtual LRESULT ReqCh2En(LPCWSTR pszInput,int nLen)=0;
	virtual LRESULT ReqEn2Ch(LPCWSTR pszText,int nLen)=0;
};


#ifdef __cplusplus
extern "C" {
#endif
ISCORE_API IServerCore * Iscore_Create();
ISCORE_API void Iscore_Destroy(IServerCore* pCore);
#ifdef __cplusplus
}
#endif