#include "StdAfx.h"
#include "Settings.h"
#include <core/Accelerator.h>

CSettingsGlobal	*g_SettingsG = NULL;
CSettingsUI  *g_SettingsUI = NULL;
struct HotKeyEntry {
	int idx;
	LPCTSTR szName;
	LPCTSTR szDefault;
};

const static HotKeyEntry KHotKeyEntryMap[] = {
	{ HKI_CharMode,_T("HotKey_CharMode"),_T("shift+space") },
	{ HKI_Query,_T("HotKey_Query"),_T("ctrl+/") },
	{ HKI_Mode,_T("HotKey_Mode") ,_T("ctrl+q") },
	{ HKI_EnSwitch,_T("HotKey_EnSwitch") ,_T("ctrl+e") },
	{ HKI_MakePhrase,_T("HotKey_MakePhrase"),_T("ctrl+m") },
	{ HKI_ShowRoot,_T("HotKey_ShowRoot") ,_T("ctrl+k") },
	{ HKI_HideStatus,_T("HotKey_HideStatus"),_T("ctrl+y") },
	{ HKI_FilterGbk,_T("HotKey_FilterGbk"),_T("ctrl+alt+g") },
	{ HKI_Record,_T("HotKey_Record"),_T("ctrl+alt+r") },
	{ HKI_TTS,_T("HotKey_TTS"),_T("ctrl+alt+t") },
};


BOOL WritePrivateProfileInt(
							LPCTSTR lpAppName,  // pointer to section name
							LPCTSTR lpKeyName,  // pointer to key name
							int nKeyValue,   // value to write
							LPCTSTR lpFileName  // pointer to initialization filename
							)
{
	return WritePrivateProfileString(lpAppName,lpKeyName,SStringT().Format(_T("%d"),nKeyValue),lpFileName);
}


const TCHAR * KSession = _T("IME");

void CSettingsGlobal::Save(LPCTSTR pszIniFile)
{
	TCHAR szBuf[100]={0};
	WritePrivateProfileInt(KSession,_T("CompMode"),compMode,pszIniFile);
	WritePrivateProfileInt(KSession,_T("RateAdjust"),byRateAdjust,pszIniFile);
	WritePrivateProfileInt(KSession,_T("23CandKey"),b23CandKey,pszIniFile);

	WritePrivateProfileInt(KSession,_T("2CandVK"),by2CandVK,pszIniFile);
	WritePrivateProfileInt(KSession,_T("3CandVK"),by3CandVK,pszIniFile);
	WritePrivateProfileInt(KSession,_T("TurnPageUpVK"),byTurnPageUpVK,pszIniFile);
	WritePrivateProfileInt(KSession,_T("TurnPageDownVK"),byTurnPageDownVK,pszIniFile);

	for (int i = 0; i < ARRAYSIZE(KHotKeyEntryMap); i++)
	{
		SStringT strHotKey = CAccelerator::FormatAccelKey(dwHotkeys[KHotKeyEntryMap[i].idx]);
		WritePrivateProfileString(KSession, KHotKeyEntryMap[i].szName, strHotKey, pszIniFile);
	}

	WritePrivateProfileInt(KSession,_T("Associate"),byAstMode,pszIniFile);
	WritePrivateProfileInt(KSession,_T("SwitchKey"),bySwitchKey,pszIniFile);
	WritePrivateProfileInt(KSession,_T("TempSpellKey"),byTempSpellKey,pszIniFile);
	WritePrivateProfileInt(KSession,_T("Forecast"),byForecast,pszIniFile);
	WritePrivateProfileInt(KSession,_T("ShowTip"),bShowOpTip,pszIniFile);
	WritePrivateProfileInt(KSession,_T("AutoMatch"),bAutoMatch,pszIniFile);

	szBuf[0]=byLineKey[0];
	WritePrivateProfileString(KSession,_T("LineKey1"),szBuf,pszIniFile);
	szBuf[0]=byLineKey[1];
	WritePrivateProfileString(KSession,_T("LineKey2"),szBuf,pszIniFile);
	szBuf[0]=byLineKey[2];
	WritePrivateProfileString(KSession,_T("LineKey3"),szBuf,pszIniFile);
	szBuf[0]=byLineKey[3];
	WritePrivateProfileString(KSession,_T("LineKey4"),szBuf,pszIniFile);
	szBuf[0]=byLineKey[4];
	WritePrivateProfileString(KSession,_T("LineKey5"),szBuf,pszIniFile);
	szBuf[0]=byLineKey[5];
	WritePrivateProfileString(KSession,_T("LineKey6"),szBuf,pszIniFile);

	WritePrivateProfileInt(KSession,_T("GBKMODE"),nGbkMode,pszIniFile);
	WritePrivateProfileInt(KSession,_T("BlendUD"),bBlendUD,pszIniFile);
	WritePrivateProfileInt(KSession,_T("BlendSpWord"),bBlendSpWord,pszIniFile);

	WritePrivateProfileInt(KSession,_T("AutoInput"),bAutoInput,pszIniFile);
	WritePrivateProfileInt(KSession,_T("PYPhraseFirst"),bPYPhraseFirst,pszIniFile);
	WritePrivateProfileInt(KSession,_T("EnterClear"),bEnterClear,pszIniFile);
	WritePrivateProfileInt(KSession,_T("SoundAlert"),nSoundAlert,pszIniFile);
	WritePrivateProfileInt(KSession,_T("DisableFnKey"),bDisableFnKey,pszIniFile);
	WritePrivateProfileInt(KSession,_T("AutoDot"),bAutoDot,pszIniFile);
	WritePrivateProfileInt(KSession,_T("AutoPrompt"),bAutoPrompt,pszIniFile);
	WritePrivateProfileInt(KSession,_T("DisableDelWordCand"),bDisableDelWordCand,pszIniFile);
	WritePrivateProfileInt(KSession,_T("CandSelNoNum"),bCandSelNoNum,pszIniFile);
	WritePrivateProfileString(KSession,_T("WebHeader"),S_CA2T(szWebHeader),pszIniFile);
	WritePrivateProfileInt(KSession,_T("OnlySimpleCode"),bOnlySimpleCode,pszIniFile);
	WritePrivateProfileInt(KSession,_T("delayTime"),nDelayTime,pszIniFile);

}

void CSettingsGlobal::Load(LPCTSTR pszIniFile)
{
	TCHAR szBuf[100];
	compMode = GetPrivateProfileInt(KSession,_T("CompMode"),1,pszIniFile);

	byRateAdjust=GetPrivateProfileInt(KSession,_T("RateAdjust"),1,pszIniFile);
	b23CandKey=GetPrivateProfileInt(KSession,_T("23CandKey"),FALSE,pszIniFile);

	by2CandVK=GetPrivateProfileInt(KSession,_T("2CandVK"),VkKeyScan(';'),pszIniFile);
	by3CandVK=GetPrivateProfileInt(KSession,_T("3CandVK"),VkKeyScan('\''),pszIniFile);
	byTurnPageUpVK=GetPrivateProfileInt(KSession,_T("TurnPageUpVK"),VkKeyScan('-'),pszIniFile);
	byTurnPageDownVK=GetPrivateProfileInt(KSession,_T("TurnPageDownVK"),VkKeyScan('='),pszIniFile);

	for (int i = 0; i < ARRAYSIZE(KHotKeyEntryMap); i++)
	{
		GetPrivateProfileString(KSession, KHotKeyEntryMap[i].szName, KHotKeyEntryMap[i].szDefault, szBuf, 100, pszIniFile);
		dwHotkeys[KHotKeyEntryMap[i].idx] = CAccelerator::TranslateAccelKey(szBuf);
	}

	byAstMode=GetPrivateProfileInt(KSession,_T("Associate"),2,pszIniFile);//2=english
	bySwitchKey=GetPrivateProfileInt(KSession,_T("SwitchKey"),0x36,pszIniFile);//ÓÒSHIFT
	byTempSpellKey=GetPrivateProfileInt(KSession,_T("TempSpellKey"),0xC1,pszIniFile);//ÓÒCtrl
	GetPrivateProfileString(KSession,_T("LineKey1"),_T("g"),szBuf,2,pszIniFile);
	byLineKey[0]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey2"),_T("h"),szBuf,2,pszIniFile);
	byLineKey[1]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey3"),_T("t"),szBuf,2,pszIniFile);
	byLineKey[2]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey4"),_T("y"),szBuf,2,pszIniFile);
	byLineKey[3]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey5"),_T("n"),szBuf,2,pszIniFile);
	byLineKey[4]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey6"),_T("z"),szBuf,2,pszIniFile);
	byLineKey[5]=szBuf[0];
	byForecast=GetPrivateProfileInt(KSession,_T("Forecast"),MQC_FORECAST,pszIniFile);

	bShowOpTip=GetPrivateProfileInt(KSession,_T("ShowTip"),1,pszIniFile);
	bAutoMatch=GetPrivateProfileInt(KSession,_T("AutoMatch"),1,pszIniFile);
	nGbkMode=(GbkMode)GetPrivateProfileInt(KSession,_T("GBKMODE"),1,pszIniFile);
	bBlendUD=GetPrivateProfileInt(KSession,_T("BlendUD"),1,pszIniFile);
	bBlendSpWord=GetPrivateProfileInt(KSession,_T("BlendSpWord"),0,pszIniFile);
	bAutoInput=GetPrivateProfileInt(KSession,_T("AutoInput"),1,pszIniFile);

	bPYPhraseFirst=GetPrivateProfileInt(KSession,_T("PYPhraseFirst"),1,pszIniFile);
	bEnterClear=GetPrivateProfileInt(KSession,_T("EnterClear"),1,pszIniFile);
	nSoundAlert=GetPrivateProfileInt(KSession,_T("SoundAlert"),1,pszIniFile);

	bDisableFnKey=GetPrivateProfileInt(KSession,_T("DisableFnKey"),1,pszIniFile);
	bAutoDot=GetPrivateProfileInt(KSession,_T("AutoDot"),1,pszIniFile);
	bAutoPrompt=GetPrivateProfileInt(KSession,_T("AutoPrompt"),0,pszIniFile);

	bDisableDelWordCand=GetPrivateProfileInt(KSession,_T("DisableDelWordCand"),1,pszIniFile);
	bCandSelNoNum=GetPrivateProfileInt(KSession,_T("CandSelNoNum"),0,pszIniFile);

	TCHAR szTmp[100]={0};
	GetPrivateProfileString(KSession,_T("WebHeader"),_T("www. http mail bbs. ftp:"),szTmp,100,pszIniFile);
	SStringA strHeader = S_CT2A(szTmp);
	strcpy(szWebHeader,strHeader);

	bOnlySimpleCode=GetPrivateProfileInt(KSession,_T("OnlySimpleCode"),0,pszIniFile);
	nDelayTime = GetPrivateProfileInt(KSession,_T("delayTime"),5,pszIniFile);
}


//////////////////////////////////////////////////////////////////////////

void CSettingsUI::Load(LPCTSTR pszIniFile)
{
	bCharMode=GetPrivateProfileInt(KSession,_T("CharMode"),TRUE,pszIniFile);
	bFullStatus=GetPrivateProfileInt(KSession,_T("FullStatus"),TRUE,pszIniFile);
	bSound=GetPrivateProfileInt(KSession,_T("Sound"),TRUE,pszIniFile);
	bRecord=GetPrivateProfileInt(KSession,_T("Record"),TRUE,pszIniFile);
	bEnglish=GetPrivateProfileInt(KSession,_T("English"),FALSE,pszIniFile);
	bMouseFollow=GetPrivateProfileInt(KSession,_T("MouseFollow"),TRUE,pszIniFile);
	bHideStatus=GetPrivateProfileInt(KSession,_T("HideStatus"),0,pszIniFile);
	bInputBig5=GetPrivateProfileInt(KSession,_T("InputBig5"),0,pszIniFile);
	bFilterGbk = GetPrivateProfileInt(KSession, _T("FilterGbk"), 0, pszIniFile);
	GetPrivateProfileString(KSession, _T("skin"), _T(""), szSkin,MAX_PATH, pszIniFile);
}

void CSettingsUI::Save(LPCTSTR pszIniFile)
{
	WritePrivateProfileInt(KSession,_T("CharMode"),bCharMode,pszIniFile);
	WritePrivateProfileInt(KSession,_T("FullStatus"),bFullStatus,pszIniFile);
	WritePrivateProfileInt(KSession,_T("Sound"),bSound,pszIniFile);
	WritePrivateProfileInt(KSession,_T("Record"),bRecord,pszIniFile);
	WritePrivateProfileInt(KSession,_T("English"),bEnglish,pszIniFile);
	WritePrivateProfileInt(KSession,_T("MouseFollow"),bMouseFollow,pszIniFile);
	WritePrivateProfileInt(KSession,_T("HideStatus"),bHideStatus,pszIniFile);
	WritePrivateProfileInt(KSession,_T("InputBig5"),bInputBig5,pszIniFile);
	WritePrivateProfileInt(KSession, _T("FilterGbk"), bFilterGbk, pszIniFile);
	WritePrivateProfileString(KSession, _T("skin"), szSkin, pszIniFile);
}
