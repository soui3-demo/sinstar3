// MyTTS.cpp: implementation of the CWorker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "worker.h"
#include <sphelper.h>
#include "WinHttp\HttpClient.h"
#include "ui/UpdateInfoDlg.h"
#include "IsSvrProxy.h"
#include "../helper/helper.h"
#include "Base64.h"
#pragma warning(disable:4995)
#define ASSERT SASSERT

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


template<>
CWorker * SSingleton<CWorker>::ms_Singleton = NULL;

CWorker::CWorker(LPCTSTR pszDataPath):m_bInitOK(FALSE)
{
	SNotifyCenter::getSingletonPtr()->addEvent(EVENTID(EventCheckUpdateResult));
	BeginThread();
}

CWorker::~CWorker()
{
	StopThread();
	::PostMessage(m_hWnd, WM_QUIT, 0, 0);// quit get message.
	JonThread();
}

//*****************************************
//	初始化TTS引擎，成功返回TRUE,失败返回FALSE
//******************************************
BOOL CWorker::Init()
{
	CoInitialize(NULL);
	HWND hWnd = Create(_T("sinstar3_server_tts_host"), WS_POPUP, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL);
	ASSERT(hWnd);

	try
	{
		do {
			SComPtr<ISpObjectTokenCategory> cpSpCategory;
			HRESULT     hr;
			hr = m_cpVoiceEn.CoCreateInstance(CLSID_SpVoice);
			if (hr != S_OK) break;
			hr = m_cpVoiceCh.CoCreateInstance(CLSID_SpVoice);
			if (hr != S_OK) break;
			hr = SpGetCategoryFromId(SPCAT_VOICES, &cpSpCategory);
			if (hr != S_OK) break;
			hr = cpSpCategory->EnumTokens(L"Language=804", NULL, &m_cpChTokens);
			if (hr != S_OK) break;
			hr = cpSpCategory->EnumTokens(L"Language=409", NULL, &m_cpEnTokens);
			if (hr != S_OK) break;
			m_bInitOK = TRUE;


			_SetSpeed(0);
			_SetVoice(TRUE,g_SettingsG->iTtsChVoice);
			_SetVoice(FALSE,g_SettingsG->iTtsEnVoice);
			MoniterTtsEvent(SPFEI(SPEI_END_INPUT_STREAM)|SPFEI(SPEI_WORD_BOUNDARY), m_hWnd, UM_TTS_EVENT);
		} while (0);

		if (!m_bInitOK)
		{
			Uninit();
		}
		return m_bInitOK;
	}catch(...)
	{
		Uninit();
		return FALSE;
	}
}

void CWorker::Uninit()
{
	_Stop();
	m_cpVoiceEn = NULL;
	m_cpVoiceCh = NULL;
	m_cpChTokens = NULL;
	m_cpEnTokens = NULL;

	DestroyWindow();
	CoUninitialize();
}

void CWorker::SpeakWText(const WCHAR * pwcText,int nLen,BOOL bCh)
{
	if (nLen == -1) nLen = wcslen(pwcText);
	std::wstring *pStr = new std::wstring(pwcText,nLen);
	::PostMessage(m_hWnd, UM_FUN_SPEAK, bCh, (LPARAM)pStr);
}

#define MAX_TTS_SPEED 10
void CWorker::_SpeakText(WPARAM wp, LPARAM lp)
{
	BOOL bCh = (BOOL)wp;
	std::wstring *pStr = (std::wstring*)lp;
	LONG nBuf=m_ttsBuffer.OnSpeakText(*pStr);
	if(nBuf>CTtsBuffer::MAX_BUFSIZE)
	{//increase speed.
		if(m_nTtsSpeed==MAX_TTS_SPEED)
		{
			_Stop();
			m_ttsBuffer.Clear();
			m_ttsBuffer.OnSpeakText(*pStr);
		}else
		{
			_SetSpeed(m_nTtsSpeed+1);
		}
	}else if(nBuf<CTtsBuffer::MAX_BUFSIZE/4 && m_nTtsSpeed>0)
	{
		_SetSpeed(m_nTtsSpeed-1);
	}
	HRESULT	hr = (bCh ? m_cpVoiceCh : m_cpVoiceEn)->Speak(pStr->c_str(), SPF_ASYNC | SPF_IS_NOT_XML, 0);
	delete pStr;
}

void CWorker::SpeakText(LPCSTR pszText,int nLen,BOOL bCh)
{
	if(nLen==-1) nLen=strlen(pszText);
	SStringA strA(pszText, nLen);
	SStringW strW = S_CA2W(strA);
	SpeakWText(strW, strW.GetLength(), bCh);
}

void CWorker::ReportUserInfo()
{
	PostMessage(UM_FUN_DATA_REPORT);
}

void CWorker::CheckUpdate(LPCTSTR pszUri, bool bManual)
{
	PostMessage(UM_FUN_CHECK_UPDATE, bManual, (LPARAM)_tcsdup(pszUri));
}


BOOL CWorker::IsTTSBusy()
{
	ASSERT(m_bInitOK);
	{
		SPVOICESTATUS  spVoiceStatus;
		m_cpVoiceCh->GetStatus(&spVoiceStatus,NULL);
		if(spVoiceStatus.dwRunningState!=SPRS_DONE)
			return TRUE;
	}
	{
		SPVOICESTATUS  spVoiceStatus;
		m_cpVoiceEn->GetStatus(&spVoiceStatus,NULL);
		if(spVoiceStatus.dwRunningState!=SPRS_DONE)
			return TRUE;
	}
	return FALSE;
}

void CWorker::SetVoice(BOOL bCh, int nToken)
{
	::PostMessage(m_hWnd, UM_FUN_SETVOICE, (WPARAM)bCh, (LPARAM)nToken);
}

BOOL CWorker::_SetVoice(WPARAM wp, LPARAM lp)
{
	BOOL bCh = (BOOL)wp;
	int nToken = (int)lp;
	if (!m_bInitOK) return FALSE;

	
	SComPtr<IEnumSpObjectTokens> pTokens = bCh ? m_cpChTokens : m_cpEnTokens;
	ISpObjectToken* pToken = NULL;
	ULONG count;
	HRESULT hr = pTokens->GetCount(&count);
	if (nToken < 0 || nToken >= (int)count)
		return FALSE;

	hr = pTokens->Item(nToken, &pToken);
	if (hr != S_OK) 
		return FALSE;
	if (bCh)
	{
		hr = m_cpVoiceCh->SetVoice(pToken);
	}
	else
	{
		hr = m_cpVoiceEn->SetVoice(pToken);
	}
	pToken->Release();
	return TRUE;
}

int CWorker::GetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize)
{
	return ::SendMessage(m_hWnd, UM_FUN_GETTOKENINFO, MAKEWPARAM(bCh,nBufSize), (LPARAM)token);
}

int CWorker::_GetTokensInfo(WPARAM wp,LPARAM lp)
{
	BOOL bCh = LOWORD(wp);
	int nBufSize = HIWORD(wp);

	wchar_t(*token)[MAX_TOKEN_NAME_LENGHT] = (wchar_t(*)[MAX_TOKEN_NAME_LENGHT])lp;

	if(!m_bInitOK) return 0;
	ULONG  count;
	SComPtr<IEnumSpObjectTokens> pTokens=bCh?m_cpChTokens:m_cpEnTokens;
	HRESULT hr=pTokens->GetCount(&count);
	if (token == NULL) return count;
	if(count==0) return 0;
	if (nBufSize < (int)count) return 0;

	for(int i=0;i<(int)count;i++)
	{
		ISpObjectToken * pToken;
		hr=pTokens->Item(i,&pToken);
        CSpDynamicString dstrDesc;
        hr = SpGetDescription(pToken, &dstrDesc);
		pToken->Release();
		wcscpy(token[i], dstrDesc);
	}
	return count;
}


void CWorker::Stop()
{
	::PostMessage(m_hWnd, UM_FUN_STOP, 0, 0);
}

void CWorker::_Stop()
{
	m_cpVoiceCh->Speak(NULL, SPF_PURGEBEFORESPEAK, 0);
	m_cpVoiceEn->Speak(NULL, SPF_PURGEBEFORESPEAK, 0);
}


void CWorker::MoniterTtsEvent(ULONGLONG ullEvent, HWND hWnd, UINT uMsg)
{
	_ASSERT(m_bInitOK);
	m_cpVoiceCh->SetInterest( ullEvent,ullEvent );
	m_cpVoiceCh->SetNotifyWindowMessage( hWnd, uMsg, 0, 0 );
	m_cpVoiceEn->SetInterest( ullEvent,ullEvent );
	m_cpVoiceEn->SetNotifyWindowMessage( hWnd, uMsg, 1, 0 );
}

void CWorker::_SetSpeed(int nSpeed)
{
	m_nTtsSpeed = nSpeed;
	m_cpVoiceEn->SetRate(m_nTtsSpeed);
	m_cpVoiceCh->SetRate(m_nTtsSpeed);
}


UINT CWorker::Run(LPARAM lp)
{
	if (!Init())
		return -1;
	
	MSG msg;
	while (!IsStoped())
	{
		if (GetMessage(&msg, 0, 0, 0))
		{
			DispatchMessage(&msg);
		}
	}
	Uninit();
	return 0;
}

LRESULT CWorker::OnTTSEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SComPtr<ISpVoice> spVoice = wParam==0?m_cpVoiceCh:m_cpVoiceEn;

	SPEVENT eventItem ={0};
	while( spVoice->GetEvents(1, &eventItem, NULL ) == S_OK )
	{
		switch(eventItem.eEventId )
		{
		case SPEI_WORD_BOUNDARY :
			{
				SPVOICESTATUS eventStatus;
				spVoice->GetStatus( &eventStatus, NULL );
				m_ttsBuffer.OnTtsWordBoundary(eventStatus.ulInputWordPos,eventStatus.ulInputWordLen);
			}
			break;
		case SPEI_END_INPUT_STREAM:
			m_ttsBuffer.OnTtsEndInputStream();
			break;
		default:
			break;
		}

		SpClearEvent( &eventItem );
	}
	return 0;
}

LRESULT CWorker::OnTTSMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet= 1;
	switch (uMsg)
	{
	case UM_FUN_SPEAK:
		_SpeakText(wParam, lParam);
		break;
	case UM_FUN_STOP:
		_Stop();
		break;
	case UM_FUN_SETVOICE:
		_SetVoice(wParam, lParam);
		break;
	case UM_FUN_GETTOKENINFO:
		lRet = _GetTokensInfo(wParam, lParam);
		break;
	default:
		break;
	}
	return lRet;
}

LRESULT CWorker::OnCheckUpdate(UINT uMsg, WPARAM wp, LPARAM lp)
{
	CWinHttp  winHttp;
	LPTSTR  pszUri = (LPTSTR)lp;
	SStringA strUrl=S_CT2A(pszUri);
	string strHtml = winHttp.Request(strUrl, Hr_Get);
	free(pszUri);
	if (strHtml.empty())
	{
		EventCheckUpdateResult * pEvt = new EventCheckUpdateResult(NULL);
		pEvt->bServerOK = false;
		pEvt->bManual = wp!=0;
		SNotifyCenter::getSingletonPtr()->FireEventAsync(pEvt);
		pEvt->Release();
	}
	else
	{
		pugi::xml_document doc;
		if (doc.load_buffer(strHtml.c_str(), strHtml.length()))
		{
			EventCheckUpdateResult * pEvt = new EventCheckUpdateResult(NULL);
			pEvt->bManual = wp!=0;
			pEvt->bServerOK = true;
			pugi::xml_node update = doc.root().child(L"update");
			pEvt->strUrl = update.attribute(L"url").as_string();
			pEvt->strNewUpdateUrl = update.attribute(L"newUpdateUrl").as_string();
			pEvt->strInfo = update.child_value();

			SStringW strVerNew = update.attribute(L"version_new").as_string();
			int a = 0, b = 0, c = 0, d = 0;
			sscanf(S_CW2A(strVerNew), "%d.%d.%d.%d", &a, &b, &c, &d);
			pEvt->dwNewVer = MAKELONG(MAKEWORD(d, c), MAKEWORD(b, a));

			SNotifyCenter::getSingletonPtr()->FireEventAsync(pEvt);
			pEvt->Release();
		}
	}

	return 0;
}

static DWORD GetKernelVer()
{
	DWORD dwRet;
	Helper_PEVersion(_T("kernel32.dll"),&dwRet,NULL,NULL);
	return dwRet;
}

LRESULT CWorker::OnDataReport(UINT uMsg, WPARAM wp, LPARAM lp)
{
	::CRegKey reg;
	LONG ret = reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ | KEY_WOW64_64KEY);
	TCHAR szUerID[100] = { 0 };
	TCHAR szOsVer[100] = {0};
	if (ret == ERROR_SUCCESS)
	{
		ULONG len = 100;
		reg.QueryStringValue(_T("userid2"), szUerID, &len);
		reg.Close();
	}
	if(szUerID[0] == 0)
	{
		GUID guid;
		if (S_OK == ::CoCreateGuid(&guid))
		{
			_stprintf(szUerID
				, _T("{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}")
				, guid.Data1
				, guid.Data2
				, guid.Data3
				, guid.Data4[0], guid.Data4[1]
				, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
				, guid.Data4[6], guid.Data4[7]
			);
		}
		else
		{
			srand((UINT)time(NULL));
			_stprintf(szUerID, _T("what_fuck_machine_#%d"), rand());
		}

		::CRegKey reg;
		LONG ret = reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ|KEY_WRITE | KEY_WOW64_64KEY);
		if (ret == ERROR_SUCCESS)
		{
			ULONG len = 100;
			reg.SetStringValue(_T("userid2"), szUerID, REG_SZ);
			reg.Close();
		}
	}

	DWORD dwVer = GetKernelVer();
	BYTE *byVer = (BYTE*)&dwVer;
	SStringT strOsVer;
	if(byVer[3] == 10)
	{
		strOsVer = _T("Win10");
	}else if(byVer[3] == 6)
	{
		switch (byVer[2])
		{
		case 3:strOsVer = _T("Win8.1"); break;
		case 2:strOsVer = _T("Win8"); break;
		case 1:strOsVer = _T("Win7"); break;
		case 0:strOsVer = _T("Vista"); break;
		}
	}
	else if (byVer[3] == 5)
	{
		strOsVer = _T("WinXP");
	}

	TCHAR szModuleName[MAX_PATH];
	GetModuleFileName(NULL, szModuleName, MAX_PATH);
	
	Helper_PEVersion(szModuleName, &dwVer, NULL, NULL);
	SStringT strVer = SStringT().Format(_T("%u.%u.%u.%u"), byVer[3], byVer[2], byVer[1], byVer[0]);

	SStringT strInfo = SStringT().Format(_T("&user_mac=%s&soft_version=%s&user_osversion=%s"), szUerID, strVer,strOsVer);
	std::string url = S_CT2A(g_SettingsG->urlStatistics+strInfo);

	CWinHttp  winHttp;
	string strResp = winHttp.Request(url.c_str(), Hr_Post);
	SLOG_INFO("data report result:" << strResp.c_str());

	return 0;
}

void CWorker::PlaySoundFromResource(LPCWSTR pszSoundID)
{
	PostMessage(UM_FUN_PLAY_SOUND, 0, (LPARAM)_wcsdup(pszSoundID));
}

HRESULT CWorker::OnPlaySoundFromResource(UINT uMsg,WPARAM wp, LPARAM lp)
{
	LPWSTR pszSoundID = (LPWSTR)lp;
	LPWSTR pszName = wcschr(pszSoundID,L':');
	if(pszName)
	{
		*pszName=0;
		pszName++;
		UINT nSize = SApplication::getSingletonPtr()->GetRawBufferSize(pszSoundID,pszName);
		if(nSize)
		{
			char *pBuf = new char[nSize];
			SApplication::getSingletonPtr()->GetRawBuffer(pszSoundID,pszName,pBuf,nSize);
			PlaySound((LPCTSTR)pBuf, NULL, SND_MEMORY);
			delete []pBuf;
		}
	}	
	free(pszSoundID);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
CTtsBuffer::CTtsBuffer():m_nReadingPos(0),m_nBufSize(0)
{

}

LONG CTtsBuffer::OnSpeakText(const std::wstring &input)
{
	m_lstBuf.push_back(input);
	m_nBufSize+=input.length();
	return m_nBufSize - m_nReadingPos;
}

void CTtsBuffer::OnTtsWordBoundary(ULONG nBegin,ULONG nLen)
{
	m_nReadingPos = nBegin+nLen;
	SLOG_INFO("OnTTSEvent, SPEI_WORD_BOUNDARY, start:"<<nBegin<<" end:"<<nLen);
}

void CTtsBuffer::OnTtsEndInputStream()
{
	SASSERT(!m_lstBuf.empty());
	if(m_lstBuf.empty())
		return ;

	std::wstring seg = m_lstBuf.front();
	m_lstBuf.pop_front();
	m_nBufSize -= seg.length();
	m_nReadingPos = 0;
}

void CTtsBuffer::Clear()
{
	m_nReadingPos = 0;
	m_nBufSize = 0;
	m_lstBuf.clear();
}
