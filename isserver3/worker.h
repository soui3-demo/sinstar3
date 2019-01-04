// MyTTS.h: interface for the CWorker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYTTS_H__C7E6BDC8_CD08_4D07_8EA4_9F404B9E0C18__INCLUDED_)
#define AFX_MYTTS_H__C7E6BDC8_CD08_4D07_8EA4_9F404B9E0C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sapi.h>           // SAPI includes
#include <string>
#include <core/SSingleton.h>
#include <interface/STaskLoop-i.h>
#include "iscore-i.h"

class CWorker : public SSingleton<CWorker>
{
public:
	CWorker(LPCTSTR pszDataPath);
	virtual ~CWorker();

	int GetSpeed();
	void SetSpeed(int nSpeed);
	void Stop();
	ULONG GetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize);
	void SetVoice(bool bCh, int iToken);
	int GetVoice(bool bCh);
	void SpeakWText(const WCHAR * pwcText,int nLen,bool bCh);
	void SpeakText(LPCSTR pszText,int nLen,bool bCh);
	void CheckUpdate(const std::string &szConfig,bool bManual);
	void ReportUserInfo();
private:
	void _GetVoice(bool bCh, int &nVoice);
	void _GetSpeed(int &nSpeed);
	void _SetSpeed(int nSpeed);
	void _SpeakText(const std::wstring &buf,bool bCh);
	void _Stop();
	void _SetVoice(bool bCh, int iToken);
	void _GetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize, ULONG &nResult);
	void _CheckUpdate(const std::string &szConfig,bool bManual);
	void _ReportUserInfo();

	void _Init();
	void _Uninit();
	BOOL _IsTTSBusy();
	BOOL _IsInited() { return m_bTtsOK; }

private:
	SStringT		m_strConfigIni;
	SComPtr<ISpVoice>   m_cpVoiceEn;
	SComPtr<ISpVoice>   m_cpVoiceCh;

	SComPtr<IEnumSpObjectTokens> m_cpChTokens;	//中文语音
	SComPtr<IEnumSpObjectTokens> m_cpEnTokens;	//英文语音
	BOOL	m_bTtsOK;
	enum {VOICE_NULL,VOICE_CH,VOICE_EN} m_CurVoice;	//当前正在使用的语音

	int		m_nSpeed;
	int		m_iChVoice, m_iEnVoice;

	CAutoRefPtr<ITaskLoop> m_pTaskLoop;
};

#endif // !defined(AFX_MYTTS_H__C7E6BDC8_CD08_4D07_8EA4_9F404B9E0C18__INCLUDED_)
