#include "StdAfx.h"
#include "Settings.h"
#include <core/SAccelerator.h>
#include "dataCenter/DataCenter.h"

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
	{ HKI_TempSpell,_T("HotKey_TempSpell"),_T("")},
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


const TCHAR * KSettingINI =  _T("config.ini");
const TCHAR * KSession = _T("IME");
const TCHAR * KUI = _T("UI");
const TCHAR * KUrl = _T("url");

void CSettingsGlobal::Save(const SStringT & strDataPath)
{
	if(!IsModified())
		return;
	SStringT strConfigIni = strDataPath + _T("\\server\\") + KSettingINI;

	TCHAR szBuf[MAX_PATH]={0};
	WritePrivateProfileInt(KSession,_T("CompMode"),compMode,strConfigIni);
	WritePrivateProfileInt(KSession,_T("RateAdjust"),byRateAdjust,strConfigIni);
	WritePrivateProfileInt(KSession,_T("23CandKey"),b23CandKey,strConfigIni);

	WritePrivateProfileInt(KSession,_T("2CandVK"),by2CandVK,strConfigIni);
	WritePrivateProfileInt(KSession,_T("3CandVK"),by3CandVK,strConfigIni);
	WritePrivateProfileInt(KSession,_T("TurnPageUpVK"),byTurnPageUpVK,strConfigIni);
	WritePrivateProfileInt(KSession,_T("TurnPageDownVK"),byTurnPageDownVK,strConfigIni);

	for (int i = 0; i < ARRAYSIZE(KHotKeyEntryMap); i++)
	{
		SStringT strHotKey = SAccelerator::FormatAccelKey(dwHotkeys[KHotKeyEntryMap[i].idx]);
		WritePrivateProfileString(KSession, KHotKeyEntryMap[i].szName, strHotKey, strConfigIni);
	}

	WritePrivateProfileInt(KSession,_T("Associate"),byAstMode,strConfigIni);
	WritePrivateProfileInt(KSession,_T("SwitchKey"),bySwitchKey,strConfigIni);
	WritePrivateProfileInt(KSession,_T("TempSpellKey"),byTempSpellKey,strConfigIni);
	WritePrivateProfileInt(KSession,_T("Forecast"),byForecast,strConfigIni);
	WritePrivateProfileInt(KSession,_T("ShowTip"),bShowOpTip,strConfigIni);
	WritePrivateProfileInt(KSession,_T("AutoMatch"),bAutoMatch,strConfigIni);

	szBuf[0]=byLineKey[0];
	WritePrivateProfileString(KSession,_T("LineKey1"),szBuf,strConfigIni);
	szBuf[0]=byLineKey[1];
	WritePrivateProfileString(KSession,_T("LineKey2"),szBuf,strConfigIni);
	szBuf[0]=byLineKey[2];
	WritePrivateProfileString(KSession,_T("LineKey3"),szBuf,strConfigIni);
	szBuf[0]=byLineKey[3];
	WritePrivateProfileString(KSession,_T("LineKey4"),szBuf,strConfigIni);
	szBuf[0]=byLineKey[4];
	WritePrivateProfileString(KSession,_T("LineKey5"),szBuf,strConfigIni);
	szBuf[0]=byLineKey[5];
	WritePrivateProfileString(KSession,_T("LineKey6"),szBuf,strConfigIni);

	WritePrivateProfileInt(KSession,_T("GBKMODE"),nGbkMode,strConfigIni);
	WritePrivateProfileInt(KSession,_T("BlendUD"),bBlendUD,strConfigIni);
	WritePrivateProfileInt(KSession,_T("BlendSpWord"),bBlendSpWord,strConfigIni);

	WritePrivateProfileInt(KSession,_T("AutoInput"),bAutoInput,strConfigIni);
	WritePrivateProfileInt(KSession,_T("PYPhraseFirst"),bPYPhraseFirst,strConfigIni);
	WritePrivateProfileInt(KSession,_T("EnterClear"),bEnterClear,strConfigIni);
	WritePrivateProfileInt(KSession,_T("SoundAlert"),nSoundAlert,strConfigIni);
	WritePrivateProfileInt(KSession,_T("DisableFnKey"),bDisableFnKey,strConfigIni);
	WritePrivateProfileInt(KSession,_T("DisableFirstWild"),bDisableFirstWild,strConfigIni);
	WritePrivateProfileInt(KSession,_T("FullSpace"),bFullSpace,strConfigIni);
	WritePrivateProfileInt(KSession,_T("AutoDot"),bAutoDot,strConfigIni);
	WritePrivateProfileInt(KSession,_T("AutoPrompt"),bAutoPrompt,strConfigIni);
	WritePrivateProfileInt(KSession,_T("DisableDelWordCand"),bDisableDelWordCand,strConfigIni);
	WritePrivateProfileInt(KSession,_T("CandSelNoNum"),bCandSelNoNum,strConfigIni);
	WritePrivateProfileString(KSession,_T("WebHeader"),szWebHeader,strConfigIni);
	WritePrivateProfileInt(KSession,_T("OnlySimpleCode"),bOnlySimpleCode,strConfigIni);
	WritePrivateProfileInt(KSession,_T("delayTime"),nDelayTime,strConfigIni);
	WritePrivateProfileInt(KSession,_T("MaxCandidateNum"),nMaxCands,strConfigIni);
	SStringT strSkinDir = strDataPath + _T("\\skins\\");
	SStringT strSkin2;

	if(strSkin.StartsWith(strSkinDir))
		strSkin2 = strSkin.Right(strSkin.GetLength()-strSkinDir.GetLength());
	
	WritePrivateProfileString(KUI,_T("skin"),strSkin2,strConfigIni);
	WritePrivateProfileString(KUI,_T("InputPosition"),SStringT().Format(_T("%d,%d"),ptInput.x,ptInput.y),strConfigIni);
	WritePrivateProfileString(KUI,_T("StatusPosition"),SStringT().Format(_T("%d,%d"),ptStatus.x,ptStatus.y),strConfigIni);


	WritePrivateProfileString(KUI,_T("font"),strFontDesc.c_str(),strConfigIni);

	SStringT strComp = SOUI::CDataCenter::getSingleton().GetData().m_compInfo.strCompName;
	if(strComp != _T("加载...") )
	{
		SStringT strHotKeyFile = SStringT().Format(_T("%s\\data\\hotkey_%s.txt"), strDataPath, strComp);
		//加载特定的自定义状态及语句输入状态开关
		SStringT strHotKey = SAccelerator::FormatAccelKey(g_SettingsG->dwHotkeys[HKI_UDMode]);
		WritePrivateProfileString(_T("hotkey"), _T("umode"), strHotKey, strHotKeyFile);
		strHotKey = SStringT((TCHAR)g_SettingsG->bySentMode);
		WritePrivateProfileString(_T("hotkey"), _T("sentence"), strHotKey, strHotKeyFile);
	}
	WritePrivateProfileInt(KSession,_T("showTray"),bShowTray,strConfigIni);
	WritePrivateProfileInt(KSession,_T("autoQuit"),bAutoQuit,strConfigIni);
	WritePrivateProfileInt(_T("update"), _T("interval"),nUpdateInterval,strConfigIni);

	SetModified(false);
}

void CSettingsGlobal::Load(const SStringT & strDataPath)
{
	SStringT strConfigIni = strDataPath + _T("\\server\\") + KSettingINI;
	TCHAR szBuf[MAX_PATH];
	compMode = GetPrivateProfileInt(KSession,_T("CompMode"),1,strConfigIni);

	byRateAdjust=GetPrivateProfileInt(KSession,_T("RateAdjust"),1,strConfigIni);
	b23CandKey=GetPrivateProfileInt(KSession,_T("23CandKey"),FALSE,strConfigIni);

	by2CandVK=GetPrivateProfileInt(KSession,_T("2CandVK"),VkKeyScan(';'),strConfigIni);
	by3CandVK=GetPrivateProfileInt(KSession,_T("3CandVK"),VkKeyScan('\''),strConfigIni);
	byTurnPageUpVK=GetPrivateProfileInt(KSession,_T("TurnPageUpVK"),VkKeyScan('-'),strConfigIni);
	byTurnPageDownVK=GetPrivateProfileInt(KSession,_T("TurnPageDownVK"),VkKeyScan('='),strConfigIni);

	memset(dwHotkeys,0,sizeof(dwHotkeys));
	for (int i = 0; i < ARRAYSIZE(KHotKeyEntryMap); i++)
	{
		GetPrivateProfileString(KSession, KHotKeyEntryMap[i].szName, KHotKeyEntryMap[i].szDefault, szBuf, 100, strConfigIni);
		dwHotkeys[KHotKeyEntryMap[i].idx] = SAccelerator::TranslateAccelKey(szBuf);
	}

	byAstMode=GetPrivateProfileInt(KSession,_T("Associate"),2,strConfigIni);//2=english
	bySwitchKey=GetPrivateProfileInt(KSession,_T("SwitchKey"),0x36,strConfigIni);//右SHIFT
	byTempSpellKey=GetPrivateProfileInt(KSession,_T("TempSpellKey"),0xC1,strConfigIni);//右Ctrl
	GetPrivateProfileString(KSession,_T("LineKey1"),_T("g"),szBuf,2,strConfigIni);
	byLineKey[0]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey2"),_T("h"),szBuf,2,strConfigIni);
	byLineKey[1]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey3"),_T("t"),szBuf,2,strConfigIni);
	byLineKey[2]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey4"),_T("y"),szBuf,2,strConfigIni);
	byLineKey[3]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey5"),_T("n"),szBuf,2,strConfigIni);
	byLineKey[4]=szBuf[0];
	GetPrivateProfileString(KSession,_T("LineKey6"),_T("z"),szBuf,2,strConfigIni);
	byLineKey[5]=szBuf[0];
	byForecast=GetPrivateProfileInt(KSession,_T("Forecast"),MQC_FORECAST,strConfigIni);

	bShowOpTip=GetPrivateProfileInt(KSession,_T("ShowTip"),1,strConfigIni);
	bAutoMatch=GetPrivateProfileInt(KSession,_T("AutoMatch"),1,strConfigIni);
	nGbkMode=(GbkMode)GetPrivateProfileInt(KSession,_T("GBKMODE"),1,strConfigIni);
	bBlendUD=GetPrivateProfileInt(KSession,_T("BlendUD"),1,strConfigIni);
	bBlendSpWord=GetPrivateProfileInt(KSession,_T("BlendSpWord"),0,strConfigIni);
	bAutoInput=GetPrivateProfileInt(KSession,_T("AutoInput"),1,strConfigIni);

	bPYPhraseFirst=GetPrivateProfileInt(KSession,_T("PYPhraseFirst"),1,strConfigIni);
	bEnterClear=GetPrivateProfileInt(KSession,_T("EnterClear"),1,strConfigIni);
	nSoundAlert=GetPrivateProfileInt(KSession,_T("SoundAlert"),1,strConfigIni);

	bDisableFnKey=GetPrivateProfileInt(KSession,_T("DisableFnKey"),1,strConfigIni);
	bAutoDot=GetPrivateProfileInt(KSession,_T("AutoDot"),1,strConfigIni);
	bAutoPrompt=GetPrivateProfileInt(KSession,_T("AutoPrompt"),0,strConfigIni);
	bDisableFirstWild = GetPrivateProfileInt(KSession,_T("DisableFirstWild"),0,strConfigIni);
	bFullSpace = GetPrivateProfileInt(KSession,_T("FullSpace"),0,strConfigIni);
	bDisableDelWordCand=GetPrivateProfileInt(KSession,_T("DisableDelWordCand"),1,strConfigIni);
	bCandSelNoNum=GetPrivateProfileInt(KSession,_T("CandSelNoNum"),0,strConfigIni);
	nMaxCands=GetPrivateProfileInt(KSession,_T("MaxCandidateNum"),10,strConfigIni);
	if(nMaxCands<1) nMaxCands = 1;
	if(nMaxCands>10) nMaxCands = 10;
	GetPrivateProfileString(KSession,_T("WebHeader"),_T("www. http mail bbs. ftp:"),szWebHeader,100,strConfigIni);

	bOnlySimpleCode=GetPrivateProfileInt(KSession,_T("OnlySimpleCode"),0,strConfigIni);
	nDelayTime = GetPrivateProfileInt(KSession,_T("delayTime"),5,strConfigIni);
	if(nDelayTime<1) nDelayTime = 1;
	if(nDelayTime>5) nDelayTime = 5;
	GetPrivateProfileString(KUI,_T("skin"),_T(""),szBuf,MAX_PATH,strConfigIni);
	if(szBuf[0]!=0)
	{
		strSkin = strDataPath+_T("\\skins\\")+szBuf;
	}
	GetPrivateProfileString(KUI,_T("InputPosition"),_T("-1,-1"),szBuf,100,strConfigIni);
	_stscanf(szBuf,_T("%d,%d"),&ptInput.x,&ptInput.y);
	GetPrivateProfileString(KUI,_T("StatusPosition"),_T("-1,-1"),szBuf,100,strConfigIni);
	_stscanf(szBuf,_T("%d,%d"),&ptStatus.x,&ptStatus.y);
	GetPrivateProfileString(KUI, _T("font"), _T(""), szBuf, MAX_PATH, strConfigIni);
	strFontDesc = szBuf;

	bEnableDebugSkin = GetPrivateProfileInt(_T("DebugSkin"),_T("enable"),FALSE,strConfigIni);
	if(bEnableDebugSkin)
	{
		GetPrivateProfileString(_T("DebugSkin"),_T("path"),_T(""),szBuf,MAX_PATH,strConfigIni);
		DWORD dwAttr = GetFileAttributes(szBuf) ;
		if(dwAttr == INVALID_FILE_ATTRIBUTES || !(dwAttr&FILE_ATTRIBUTE_DIRECTORY))
		{
			bEnableDebugSkin = FALSE;
		}else
		{
			strDebugSkinPath = szBuf;
		}
	}


	GetPrivateProfileString(KUrl, _T("skin_cloud"), _T("http://www.soui.vip/forum.php?mod=forumdisplay&fid=53"), szBuf, MAX_PATH, strConfigIni);
	urlSkin = szBuf;
	GetPrivateProfileString(KUrl, _T("comp_cloud"), _T("http://www.soui.vip/forum.php?mod=forumdisplay&fid=54"), szBuf, MAX_PATH, strConfigIni);
	urlComp = szBuf;
	GetPrivateProfileString(KUrl, _T("statistics"), _T("https://soime.cn/tongji/skTjApi/api.php?soft_id=6&countype_id=4&ditch_id=100"), szBuf, MAX_PATH, strConfigIni);
	urlStatistics = szBuf;
	GetPrivateProfileString(KUrl, _T("forum"), _T("http://www.soui.vip/forum.php?gid=51"), szBuf, MAX_PATH, strConfigIni);
	urlForum = szBuf;

	bShowTray = GetPrivateProfileInt(KSession,_T("showTray"),1,strConfigIni);
	bAutoQuit = GetPrivateProfileInt(KSession,_T("autoQuit"),0,strConfigIni);
	nUpdateInterval = GetPrivateProfileInt(_T("update"), _T("interval"), 30, strConfigIni);

	SetModified(false);
}


//////////////////////////////////////////////////////////////////////////

void CSettingsUI::Load(const SStringT & strDataPath)
{
	SStringT strConfigIni = strDataPath + _T("\\server\\") + KSettingINI;
	bCharMode=GetPrivateProfileInt(KSession,_T("CharMode"),TRUE,strConfigIni);
	bFullStatus=GetPrivateProfileInt(KSession,_T("FullStatus"),TRUE,strConfigIni);
	bSound=GetPrivateProfileInt(KSession,_T("Sound"),TRUE,strConfigIni);
	bRecord=GetPrivateProfileInt(KSession,_T("Record"),TRUE,strConfigIni);
	bEnglish=GetPrivateProfileInt(KSession,_T("English"),FALSE,strConfigIni);
	bMouseFollow=GetPrivateProfileInt(KSession,_T("MouseFollow"),TRUE,strConfigIni);
	bHideStatus=GetPrivateProfileInt(KSession,_T("HideStatus"),0,strConfigIni);
	bInputBig5=GetPrivateProfileInt(KSession,_T("InputBig5"),0,strConfigIni);
	bFilterGbk = GetPrivateProfileInt(KSession, _T("FilterGbk"), 0, strConfigIni);
	SetModified(false);
}

void CSettingsUI::Save(const SStringT & strDataPath)
{
	if(!IsModified())
		return;
	SStringT strConfigIni = strDataPath + _T("\\server\\") + KSettingINI;
	WritePrivateProfileInt(KSession,_T("CharMode"),bCharMode,strConfigIni);
	WritePrivateProfileInt(KSession,_T("FullStatus"),bFullStatus,strConfigIni);
	WritePrivateProfileInt(KSession,_T("Sound"),bSound,strConfigIni);
	WritePrivateProfileInt(KSession,_T("Record"),bRecord,strConfigIni);
	WritePrivateProfileInt(KSession,_T("English"),bEnglish,strConfigIni);
	WritePrivateProfileInt(KSession,_T("MouseFollow"),bMouseFollow,strConfigIni);
	WritePrivateProfileInt(KSession,_T("HideStatus"),bHideStatus,strConfigIni);
	WritePrivateProfileInt(KSession,_T("InputBig5"),bInputBig5,strConfigIni);
	WritePrivateProfileInt(KSession, _T("FilterGbk"), bFilterGbk, strConfigIni);
	SetModified(false);
}
