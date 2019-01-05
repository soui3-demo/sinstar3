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
#include <helper/SFunctor.hpp>

#include "Base64.h"
#pragma warning(disable:4995)
#define ASSERT SASSERT

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template<>
CWorker * SSingleton<CWorker>::ms_Singleton = NULL;

const TCHAR * KConfigIni = _T("config.ini");
const TCHAR * KTtsEntry = _T("tts");
const TCHAR * KTtsSpeed = _T("speed");
const TCHAR * KTtsChVoice = _T("ChVoice");
const TCHAR * KTtsEnVoice = _T("EnVoice");

CWorker::CWorker(LPCTSTR pszDataPath):m_bTtsOK(FALSE), m_CurVoice(VOICE_NULL)
{
	SNotifyCenter::getSingletonPtr()->addEvent(EVENTID(EventCheckUpdateResult));

	m_strConfigIni = pszDataPath;
	m_strConfigIni += _T("\\");
	m_strConfigIni += KConfigIni;

	g_ComMgr2->CreateTaskLoop((IObjRef**)&m_pTaskLoop);
	m_pTaskLoop->start("isserver3_worker",ITaskLoop::Low);
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_Init,true);
}

CWorker::~CWorker()
{
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_Uninit,true);
	m_pTaskLoop->stop();
	m_pTaskLoop = NULL;
}

//*****************************************
//	初始化TTS引擎，成功返回TRUE,失败返回FALSE
//******************************************
void CWorker::_Init()
{
	CoInitialize(NULL);

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

			m_iChVoice = GetPrivateProfileInt(KTtsEntry, KTtsChVoice, 0, m_strConfigIni);
			m_iEnVoice = GetPrivateProfileInt(KTtsEntry, KTtsEnVoice, 0, m_strConfigIni);
			m_nSpeed = GetPrivateProfileInt(KTtsEntry, KTtsSpeed, 0, m_strConfigIni);

			_SetSpeed(m_nSpeed);
			_SetVoice(TRUE, m_iChVoice);
			_SetVoice(FALSE, m_iEnVoice);

			m_bTtsOK = TRUE;
		} while (0);

		if (!m_bTtsOK)
		{
			m_cpVoiceEn = NULL;
			m_cpVoiceCh = NULL;
			m_cpChTokens = NULL;
			m_cpEnTokens = NULL;
		}
	}catch(...)
	{
		m_cpVoiceEn = NULL;
		m_cpVoiceCh = NULL;
		m_cpChTokens = NULL;
		m_cpEnTokens = NULL;
	}
}

void CWorker::_Uninit()
{
	WritePrivateProfileString(KTtsEntry, KTtsSpeed, SStringT().Format(_T("%d"),m_nSpeed),m_strConfigIni);
	WritePrivateProfileString(KTtsEntry, KTtsChVoice, SStringT().Format(_T("%d"), m_iChVoice), m_strConfigIni);
	WritePrivateProfileString(KTtsEntry, KTtsEnVoice, SStringT().Format(_T("%d"), m_iEnVoice), m_strConfigIni);

	m_cpVoiceEn = NULL;
	m_cpVoiceCh = NULL;
	m_cpChTokens = NULL;
	m_cpEnTokens = NULL;
	m_bTtsOK = FALSE;
	CoUninitialize();
}

void CWorker::SpeakWText(const WCHAR * pwcText,int nLen,bool bCh)
{
	if (nLen == -1) nLen = wcslen(pwcText);
	std::wstring buf(pwcText,nLen);
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_SpeakText,buf,bCh,false);
}

void CWorker::_SpeakText(const std::wstring &buf,bool bCh)
{
	if (_IsTTSBusy())
		_Stop();
	m_CurVoice = bCh ? VOICE_CH : VOICE_EN;
	HRESULT	hr = (m_CurVoice == VOICE_CH ? m_cpVoiceCh : m_cpVoiceEn)->Speak(buf.c_str(), SPF_IS_NOT_XML, 0);
}

void CWorker::SpeakText(LPCSTR pszText,int nLen,bool bCh)
{
	if(nLen==-1) nLen=strlen(pszText);
	SStringA strA(pszText, nLen);
	SStringW strW = S_CA2W(strA);
	SpeakWText(strW, strW.GetLength(), bCh);
}


BOOL CWorker::_IsTTSBusy()
{
	ASSERT(m_bTtsOK);
	if(m_CurVoice==VOICE_NULL) return FALSE;
	SPVOICESTATUS  spVoiceStatus;
	(m_CurVoice==VOICE_CH?m_cpVoiceCh:m_cpVoiceEn)->GetStatus(&spVoiceStatus,NULL);
	BOOL bBusy=spVoiceStatus.dwRunningState!=SPRS_DONE;
	return bBusy;
}


int CWorker::GetVoice(bool bCh)
{
	int nVoices = 0;
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_GetVoice,bCh,&nVoices,true);
	return nVoices;
}


void CWorker::_GetVoice(bool bCh,int *nVoice)
{
	*nVoice =  bCh?m_iChVoice:m_iEnVoice;
}

void CWorker::SetVoice(bool bCh, int iToken)
{
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_SetVoice,bCh,iToken,false);
}

void CWorker::_SetVoice(bool bCh, int iToken)
{
	if (m_bTtsOK) return;

	
	SComPtr<IEnumSpObjectTokens> pTokens = bCh ? m_cpChTokens : m_cpEnTokens;
	ISpObjectToken* pToken = NULL;
	ULONG count;
	HRESULT hr = pTokens->GetCount(&count);
	if (iToken < 0 || iToken >= count)
		return;

	hr = pTokens->Item(iToken, &pToken);
	if (hr != S_OK) 
		return;
	if (bCh)
	{
		hr = m_cpVoiceCh->SetVoice(pToken);
		m_iChVoice = iToken;
	}
	else
	{
		hr = m_cpVoiceEn->SetVoice(pToken);
		m_iEnVoice = iToken;
	}
	pToken->Release();
}

ULONG CWorker::GetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize)
{
	ULONG nRet = 0;
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_GetTokensInfo,bCh,token,nBufSize,&nRet,true);
	return nRet;
}

void CWorker::_GetTokensInfo(bool bCh, wchar_t token[][MAX_TOKEN_NAME_LENGHT], int nBufSize, ULONG *count)
{
	if(!m_bTtsOK)
		return;
	SComPtr<IEnumSpObjectTokens> pTokens=bCh?m_cpChTokens:m_cpEnTokens;
	HRESULT hr=pTokens->GetCount(count);
	if (token == NULL) return;
	if(*count==0) return;
	if (nBufSize < *count) 
	{
		*count = -1;
		return;
	}

	for(int i=0;i<(int)(*count);i++)
	{
		ISpObjectToken * pToken;
		hr=pTokens->Item(i,&pToken);
        CSpDynamicString dstrDesc;
        hr = SpGetDescription(pToken, &dstrDesc);
		pToken->Release();
		wcscpy(token[i], dstrDesc);
	}
}


void CWorker::Stop()
{
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_Stop,false);
}

void CWorker::_Stop()
{
	if (m_CurVoice != VOICE_NULL)
	{
		(m_CurVoice == VOICE_CH ? m_cpVoiceCh : m_cpVoiceEn)->Speak(NULL, SPF_PURGEBEFORESPEAK, 0);
	}
}


int CWorker::GetSpeed()
{
	int nSpeed=0;
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_GetSpeed,&nSpeed,true);
	return nSpeed;
}

void CWorker::_GetSpeed(int *nSpeed)
{
	*nSpeed = m_nSpeed;
}

void CWorker::SetSpeed(int nSpeed)
{
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_SetSpeed,nSpeed,false);
}

void CWorker::_SetSpeed(int nSpeed)
{
	if(!m_bTtsOK) return;
	m_cpVoiceEn->SetRate(nSpeed);
	m_cpVoiceCh->SetRate(nSpeed);
}


void CWorker::CheckUpdate(const std::string &szConfig,bool bManual)
{
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_CheckUpdate,szConfig,bManual,false);
}

void CWorker::_CheckUpdate(const std::string &szConfig,bool bManual)
{
	CWinHttp  winHttp;
	char szUrl[500];
	GetPrivateProfileStringA("update", "url", "http://soime.cm/sinstar3_update.xml", szUrl, 500, szConfig.c_str());
	string strHtml = winHttp.Request(szUrl, Hr_Get);
	pugi::xml_document doc;
	if (doc.load_buffer(strHtml.c_str(), strHtml.length()))
	{
		EventCheckUpdateResult * pEvt = new EventCheckUpdateResult(NULL);
		pEvt->bManual = bManual;

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

static DWORD GetKernelVer()
{
	DWORD dwRet;
	Helper_PEVersion(_T("kernel32.dll"),&dwRet,NULL,NULL);
	return dwRet;
}


void CWorker::ReportUserInfo()
{
	STaskHelper::post(m_pTaskLoop,this,&CWorker::_ReportUserInfo,false);
}

void CWorker::_ReportUserInfo()
{
	::CRegKey reg;
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
		_tcscat(szUerID, _T("_"));
		_tcscat(szUerID, strOsVer);
		::CRegKey reg;
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

	SStringT strInfo = SStringT().Format(_T("userName=qcsrf|guid=%s|softVersion=%s"), szUerID, strVer);
	SStringA strInfoA = S_CT2A(strInfo);
	std::string info64 = Base64::Encode((LPCSTR)strInfoA);

	std::string url = "http://pdftj.mmbangshou.net:8888/web/tj_az?param=";
	url += info64;

	CWinHttp  winHttp;
	string strResp = winHttp.Request(url.c_str(), Hr_Get);
	SLOG_INFO("data report result:" << strResp.c_str());
}
