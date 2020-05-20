// MyTTS.h: interface for the CWorker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYTTS_H__C7E6BDC8_CD08_4D07_8EA4_9F404B9E0C18__INCLUDED_)
#define AFX_MYTTS_H__C7E6BDC8_CD08_4D07_8EA4_9F404B9E0C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sapi.h>           // SAPI includes
#include <iscore-i.h>
#include <core/SSingleton.h>
#include "threadobject.h"
#include <list>
#include <string>

enum {
	UM_TTS_EVENT = (WM_USER + 100),
	UM_FUN_SPEAK,
	UM_FUN_STOP,
	UM_FUN_SETVOICE,
	UM_FUN_GETTOKENINFO,
	UM_FUN_CHECK_UPDATE,
	UM_FUN_DATA_REPORT,
	UM_FUN_PLAY_SOUND,
};


class CTtsBuffer
{
public:
	enum {MAX_BUFSIZE=20};

	CTtsBuffer();
	LONG OnSpeakText(const std::wstring &input);
	void OnTtsWordBoundary(ULONG nBegin,ULONG nLen);
	void OnTtsEndInputStream();
	void Clear();
private:
	LONG					m_nReadingPos;
	LONG					m_nBufSize;
	std::list<std::wstring>   m_lstBuf;
};

class CWorker : public SNativeWnd
	, protected CThreadObject
	, public SSingleton<CWorker>
{
public:
	CWorker(LPCTSTR pszDataPath);
	virtual ~CWorker();

	void Stop();
	int GetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize);
	void SetVoice(BOOL bCh,int nToken);
	void SpeakWText(const WCHAR * pwcText,int nLen,BOOL bCh);
	void SpeakText(LPCSTR pszText,int nLen,BOOL bCh);
	void ReportUserInfo();
	void CheckUpdate(LPCTSTR pszUri, bool bManual);
	void PlaySoundFromResource(LPCWSTR pszSoundID);
private:
	void _SetSpeed(int nSpeed);
	void _SpeakText(WPARAM wp, LPARAM lp);
	void _Stop();
	BOOL _SetVoice(WPARAM wp, LPARAM lp);
	int  _GetTokensInfo(WPARAM wp, LPARAM lp);
protected:
	virtual UINT Run(LPARAM lp);
protected:
	LRESULT OnTTSEvent(UINT uMsg,WPARAM wp,LPARAM lp);
	LRESULT OnTTSMessage(UINT uMsg, WPARAM wp, LPARAM lp);
	LRESULT OnCheckUpdate(UINT uMsg, WPARAM wp, LPARAM lp);
	LRESULT OnDataReport(UINT uMsg, WPARAM wp, LPARAM lp);
	LRESULT OnPlaySoundFromResource(UINT uMsg, WPARAM wp, LPARAM lp);

	BEGIN_MSG_MAP_EX(CWorker)
		MESSAGE_HANDLER_EX(UM_TTS_EVENT,OnTTSEvent)
		MESSAGE_RANGE_HANDLER_EX(UM_FUN_SPEAK, UM_FUN_GETTOKENINFO,OnTTSMessage)
		MESSAGE_HANDLER_EX(UM_FUN_CHECK_UPDATE, OnCheckUpdate)
		MESSAGE_HANDLER_EX(UM_FUN_DATA_REPORT, OnDataReport)
		MESSAGE_HANDLER_EX(UM_FUN_PLAY_SOUND, OnPlaySoundFromResource)
		CHAIN_MSG_MAP(__super)
	END_MSG_MAP()
private:
	void MoniterTtsEvent(ULONGLONG ullEvent, HWND hWnd, UINT uMsg);
	BOOL Init();
	void Uninit();
	BOOL IsTTSBusy();
	BOOL IsInited() { return m_bInitOK; }

	SComPtr<ISpVoice>   m_cpVoiceEn;
	SComPtr<ISpVoice>   m_cpVoiceCh;

	SComPtr<IEnumSpObjectTokens> m_cpChTokens;	//÷–Œƒ”Ô“Ù
	SComPtr<IEnumSpObjectTokens> m_cpEnTokens;	//”¢Œƒ”Ô“Ù

	CTtsBuffer			m_ttsBuffer;
	int					m_nTtsSpeed;
	BOOL	m_bInitOK;
};

#endif // !defined(AFX_MYTTS_H__C7E6BDC8_CD08_4D07_8EA4_9F404B9E0C18__INCLUDED_)
