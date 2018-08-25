// MyTTS.cpp: implementation of the CWorker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "worker.h"
#include <sphelper.h>
#include "WinHttp\HttpClient.h"
#include "UpdateInfoDlg.h"
#include "IsSvrProxy.h"
#include "../helper/helper.h"
#include "Base64.h"

#define ASSERT SASSERT
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const TCHAR * KConfigIni = _T("config.ini");
const TCHAR * KTtsEntry = _T("tts");
const TCHAR * KTtsSpeed = _T("speed");
const TCHAR * KTtsChVoice = _T("ChVoice");
const TCHAR * KTtsEnVoice = _T("EnVoice");

CWorker::CWorker(LPCTSTR pszDataPath):m_bInitOK(FALSE), m_CurVoice(VOICE_NULL)
{
	m_strConfigIni = pszDataPath;
	m_strConfigIni += _T("\\");
	m_strConfigIni += KConfigIni;
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
			hr = cpSpCategory->EnumTokens(L"Language=9", NULL, &m_cpEnTokens);
			if (hr != S_OK) break;
			m_bInitOK = TRUE;

			m_iChVoice = GetPrivateProfileInt(KTtsEntry, KTtsChVoice, 0, m_strConfigIni);
			m_iEnVoice = GetPrivateProfileInt(KTtsEntry, KTtsEnVoice, 0, m_strConfigIni);
			m_nSpeed = GetPrivateProfileInt(KTtsEntry, KTtsSpeed, 0, m_strConfigIni);

			_SetSpeed(m_nSpeed);
			_SetVoice(TRUE, m_iChVoice);
			_SetVoice(FALSE, m_iEnVoice);

			SetMsgOwner(SPEI_END_INPUT_STREAM, m_hWnd, UM_TTS_FINISH);
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
	WritePrivateProfileString(KTtsEntry, KTtsSpeed, SStringT().Format(_T("%d"),m_nSpeed),m_strConfigIni);
	WritePrivateProfileString(KTtsEntry, KTtsChVoice, SStringT().Format(_T("%d"), m_iChVoice), m_strConfigIni);
	WritePrivateProfileString(KTtsEntry, KTtsEnVoice, SStringT().Format(_T("%d"), m_iEnVoice), m_strConfigIni);

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
	SStringW *pStr = new SStringW(pwcText,nLen);
	::PostMessage(m_hWnd, UM_FUN_SPEAK, bCh, (LPARAM)pStr);
}

void CWorker::_SpeakText(WPARAM wp, LPARAM lp)
{
	BOOL bCh = (BOOL)wp;
	SStringW *pStr = (SStringW*)lp;
	if (IsTTSBusy())
		_Stop();
	m_CurVoice = bCh ? VOICE_CH : VOICE_EN;
	HRESULT	hr = (m_CurVoice == VOICE_CH ? m_cpVoiceCh : m_cpVoiceEn)->Speak((*pStr), SPF_ASYNC | SPF_IS_NOT_XML, 0);
	delete pStr;
}

void CWorker::SpeakText(LPCSTR pszText,int nLen,BOOL bCh)
{
	if(nLen==-1) nLen=strlen(pszText);
	SStringA strA(pszText, nLen);
	SStringW strW = S_CA2W(strA);
	SpeakWText(strW, strW.GetLength(), bCh);
}


BOOL CWorker::IsTTSBusy()
{
	ASSERT(m_bInitOK);
	if(m_CurVoice==VOICE_NULL) return FALSE;
	SPVOICESTATUS  spVoiceStatus;
	(m_CurVoice==VOICE_CH?m_cpVoiceCh:m_cpVoiceEn)->GetStatus(&spVoiceStatus,NULL);
	BOOL bBusy=spVoiceStatus.dwRunningState!=SPRS_DONE;
	return bBusy;
}


int CWorker::GetVoice(BOOL bCh)
{
	return bCh?m_iChVoice:m_iEnVoice;
}


void CWorker::SetVoice(BOOL bCh, int nToken)
{
	::PostMessage(m_hWnd, UM_FUN_SETVOICE, (WPARAM)bCh, (LPARAM)nToken);
}

BOOL CWorker::_SetVoice(WPARAM wp, LPARAM lp)
{
	BOOL bCh = (BOOL)wp;
	int nToken = (int)lp;
	if (m_bInitOK) return FALSE;

	
	SComPtr<IEnumSpObjectTokens> pTokens = bCh ? m_cpChTokens : m_cpEnTokens;
	ISpObjectToken* pToken = NULL;
	ULONG count;
	HRESULT hr = pTokens->GetCount(&count);
	if (nToken < 0 || nToken >= count)
		return FALSE;

	hr = pTokens->Item(nToken, &pToken);
	if (hr != S_OK) 
		return FALSE;
	if (bCh)
	{
		hr = m_cpVoiceCh->SetVoice(pToken);
		m_iChVoice = nToken;
	}
	else
	{
		hr = m_cpVoiceEn->SetVoice(pToken);
		m_iEnVoice = nToken;
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
	if (nBufSize < count) return 0;

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
	if (m_CurVoice != VOICE_NULL)
	{
		(m_CurVoice == VOICE_CH ? m_cpVoiceCh : m_cpVoiceEn)->Speak(NULL, SPF_PURGEBEFORESPEAK, 0);
	}
}


void CWorker::SetMsgOwner(ULONGLONG ullEvent, HWND hWnd, UINT uMsg)
{
	_ASSERT(m_bInitOK);
	m_cpVoiceCh->SetInterest( ullEvent,ullEvent );
	m_cpVoiceCh->SetNotifyWindowMessage( hWnd, uMsg, 0, 0 );
	m_cpVoiceEn->SetInterest( ullEvent,ullEvent );
	m_cpVoiceEn->SetNotifyWindowMessage( hWnd, uMsg, 1, 0 );
}

int CWorker::GetSpeed()
{
	return m_nSpeed;
}

void CWorker::SetSpeed(int nSpeed)
{
	PostMessage(UM_FUN_SETSPEED, (WPARAM)nSpeed, 0);
}

void CWorker::_SetSpeed(WPARAM lp)
{
	if(!m_bInitOK) return;
	int nSpeed = (int)lp;
	m_cpVoiceEn->SetRate(nSpeed);
	m_cpVoiceCh->SetRate(nSpeed);
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

LRESULT CWorker::OnTTSMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet= 1;
	switch (uMsg)
	{
	case UM_TTS_FINISH:
		STRACEA("UM_TTS_FINISH");
		break;
	case UM_FUN_SPEAK:
		_SpeakText(wParam, lParam);
		break;
	case UM_FUN_STOP:
		_Stop();
		break;
	case UM_FUN_SETVOICE:
		_SetVoice(wParam, lParam);
		break;
	case UM_FUN_SETSPEED:
		_SetSpeed(wParam);
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
	BOOL bManual = (BOOL)wp;
	
	CIsSvrProxy *pSvrProxy = (CIsSvrProxy*)lp;

	CWinHttp  winHttp;
	char szConfig[MAX_PATH];
	pSvrProxy->m_pCore->GetConfigIni(szConfig, MAX_PATH);
	char szUrl[500];
	GetPrivateProfileStringA("update", "url", "http://soime.cm/sinstar3_update.xml", szUrl, 500, szConfig);
	string strHtml = winHttp.Request(szUrl, Hr_Get);
	pugi::xml_document doc;
	if (doc.load_buffer(strHtml.c_str(), strHtml.length()))
	{
		pugi::xml_node update = doc.root().child(L"update");
		CheckUpdateResult *result = new CheckUpdateResult;
		result->strUrl = update.attribute(L"url").as_string();
		result->strNewUpdateUrl = update.attribute(L"newUpdateUrl").as_string();
		result->strInfo = update.text().as_string();

		SStringW strVerClient = update.attribute(L"version_client").as_string();
		SStringW strVerServer = update.attribute(L"version_server").as_string();
		int a = 0, b = 0, c = 0, d = 0;
		sscanf(S_CW2A(strVerClient), "%d.%d.%d.%d", &a, &b, &c, &d);
		result->dwClientVer = MAKELONG(MAKEWORD(d, c), MAKEWORD(b, a));
		a = b = c = d = 0;
		sscanf(S_CW2A(strVerServer), "%d.%d.%d.%d", &a, &b, &c, &d);
		result->dwServerVer = MAKELONG(MAKEWORD(d, c), MAKEWORD(b, a));

		pSvrProxy->PostMessage(UM_CHECK_UPDATE_RESULT, 0, (LPARAM)result);
	}

	return 0;
}

LRESULT CWorker::OnDataReport(UINT uMsg, WPARAM wp, LPARAM lp)
{
	CRegKey reg;
	LONG ret = reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ | KEY_WOW64_64KEY);
	TCHAR szUerID[100] = { 0 };
	if (ret == ERROR_SUCCESS)
	{
		ULONG len = 100;
		reg.QueryStringValue(_T("userid"), szUerID, &len);
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
			srand(time(NULL));
			_stprintf(szUerID, _T("what_fuck_machine_#%d"), rand());
		}

		CRegKey reg;
		LONG ret = reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\SetoutSoft\\sinstar3"), KEY_READ|KEY_WRITE | KEY_WOW64_64KEY);
		if (ret == ERROR_SUCCESS)
		{
			ULONG len = 100;
			reg.SetStringValue(_T("userid"), szUerID, REG_SZ);
			reg.Close();
		}
	}

	TCHAR szModuleName[MAX_PATH];
	GetModuleFileName(NULL, szModuleName, MAX_PATH);
	
	BYTE byVer[4];
	Helper_PEVersion(szModuleName, (DWORD*)byVer, NULL, NULL);
	SStringT strVer = SStringT().Format(_T("%u.%u.%u.%u"), byVer[3], byVer[2], byVer[1], byVer[0]);

	SStringT strInfo = SStringT().Format(_T("userName=qcshf|guid=%s|softVersion=%s"), szUerID, strVer);
	SStringA strInfoA = S_CT2A(strInfo);
	std::string info64 = Base64::Encode((LPCSTR)strInfoA);

	std::string url = "http://pdftj.mmbangshou.net:8888/web/tj_az?";
	url += info64;

	CWinHttp  winHttp;
	string strResp = winHttp.Request(url.c_str(), Hr_Get);
	SLOG_INFO("data report result:" << strResp.c_str());

	return 0;
}
