#include "StdAfx.h"
#include "InputState.h"
#include "Utils.h"
#include <ShellAPI.h>
#include "../IsSvrProxy.h"

#pragma warning(disable:4311 4302)
static const BYTE KCompKey[] ={0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,        // 00-0F
0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,        // 10-1F
1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,        // 20-2F
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,        // 30-3F
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,        // 40-4F
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,        // 50-5F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 60-6F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 70-7F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 80-8F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 90-9F
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // A0-AF
0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,        // B0-BF
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // C0-CF
0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,        // D0-DF
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // E0-EF
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};       // F0-FF


static const int HKI_LoadDebugSkin	 = -2;
static const int HKI_UnloadDebugSkin = -3;
//符号处理
//BYTE byInput:键盘输入
SStringW CInputState::Symbol_Convert(InputContext * lpCntxtPriv,UINT byInput,const BYTE * lpbKeyState)
{
	WCHAR pBuf[100];
	int nRet=0;
	{
		char cType=3;
		if(byInput=='\'')
		{
			static BOOL bLeft1=TRUE;
			cType=bLeft1?1:2;
			bLeft1=!bLeft1;
		}else if(byInput=='\"')
		{
			static BOOL bLeft2=TRUE;
			cType=bLeft2?1:2;
			bLeft2=!bLeft2;
		}
		if(byInput == 0x20 )
		{
			if(g_SettingsG->bFullSpace)
			{//full space
				wcscpy(pBuf,L"　");
				nRet = 1;
			}else
			{
				pBuf[0]=0x20;
				nRet=1;
			}
		}else if(CIsSvrProxy::GetSvrCore()->ReqSymbolConvert((char)byInput,cType,g_SettingsUI->bCharMode?1:0)==ISACK_SUCCESS)
		{
			PMSGDATA pMsg=CIsSvrProxy::GetSvrCore()->GetAck();
			if(pMsg->sSize<20)
			{
				nRet=pMsg->sSize/2-1;
				wcscpy(pBuf,(WCHAR*)pMsg->byData);
				if(wcsncmp(pBuf+nRet-5,L"$left",5)==0)
				{//解释光标移动函数
					nRet-=5;
					pBuf[nRet]=0;
					m_pListener->DelayCaretLeft();
				}
			}
		}
	}
	if(!nRet)
	{
		pBuf[0]=byInput;
		nRet=1;
	}

	return SStringW(pBuf,nRet);
}

BOOL IsDigitEx(WCHAR c)
{
	if(isdigit(c)) return TRUE;
	return c=='.';
}

//判断当前状态下数字键区是不是有较编码,自定义状态及笔画输入状态下支持数字键盘当作编码
BOOL KeyIn_IsNumCode(InputContext * lpCntxtPriv)
{
	return (lpCntxtPriv->inState == INST_USERDEF
		&& ((lpCntxtPriv->cComp>0 && IsDigitEx(lpCntxtPriv->szComp[0])) || lpCntxtPriv->cComp == 0)
		);
}

BOOL KeyIn_Code_IsMaxCode(InputContext * lpCntxtPriv)
{
	if(lpCntxtPriv->cComp==0) return FALSE;
	return (lpCntxtPriv->cComp>=CIsSvrProxy::GetSvrCore()->GetCompHead()->cCodeMax &&  lpCntxtPriv->byCandType==MCR_NORMAL);
}

BOOL KeyIn_Code_IsMaxCode2(InputContext * lpCntxtPriv)
{
	if (lpCntxtPriv->cComp == 0) return FALSE;
	return (lpCntxtPriv->cComp >= CIsSvrProxy::GetSvrCore()->GetCompHead()->cCodeMax &&  (lpCntxtPriv->byCandType&(MCR_LONGERCOMP|MCR_MIXSP)) == 0);
}

BOOL KeyIn_Code_IsValidComp(InputContext * lpCntxtPriv,char cInput)
{
	BYTE byMask=0;
	if(lpCntxtPriv->cComp==MAX_COMP-1) return FALSE;
	if(g_SettingsG->bBlendUD) byMask|=MQC_USERDEF;
	if(g_SettingsG->bBlendSpell) byMask|=MQC_SPCAND;
	lpCntxtPriv->szComp[lpCntxtPriv->cComp]=cInput;
	return CIsSvrProxy::GetSvrCore()->CheckComp(lpCntxtPriv->szComp,lpCntxtPriv->cComp+1,byMask);
}

BOOL KeyIn_IsCoding(InputContext * lpCntxtPriv)
{
	BOOL bOpen=FALSE;
	if(lpCntxtPriv->inState==INST_CODING)
	{//编码输入
		if(lpCntxtPriv->sbState==SBST_NORMALSTATE)
		{
			if(lpCntxtPriv->compMode==IM_SHAPECODE)
			{//形码
				bOpen=lpCntxtPriv->cComp!=0;
			}else//if(lpCntxtPriv->compMode==IM_SPELL)
			{//拼音
				bOpen=(lpCntxtPriv->bySyllables>1 || lpCntxtPriv->spellData[0].bySpellLen>0);
				if(g_SettingsG->compMode==IM_SHAPECODE) bOpen=TRUE;
			}
		}else if(lpCntxtPriv->sbState==SBST_SENTENCE)
			bOpen=TRUE;
	}else if(lpCntxtPriv->inState==INST_ENGLISH)
	{//英文输入
		bOpen=lpCntxtPriv->cComp!=0;
	}else if(lpCntxtPriv->inState==INST_USERDEF || lpCntxtPriv->inState==INST_LINEIME)
	{
		bOpen=TRUE;
	}
	return bOpen;
}

CInputState::CInputState(void)
:m_pListener(NULL)
,m_fOpen(FALSE)
,m_bUpdateTips(TRUE)
,m_bPressOther(FALSE)
,m_bPressShift(FALSE)
,m_bPressCtrl(FALSE)

{
	memset(&m_ctx,0,sizeof(InputContext));
	ClearContext(CPC_ALL);
	m_ctx.compMode  = (COMPMODE)g_SettingsG->compMode;
	m_ctx.bShowTip = FALSE;
	m_pbyMsgBuf=(LPBYTE)malloc(MAX_BUF_ACK);
}

CInputState::~CInputState(void)
{
	free(m_pbyMsgBuf);
}

int CInputState::TestHotKey(UINT uVk, const BYTE * lpbKeyState) const
{
	if (uVk == VK_CONTROL || uVk == VK_SHIFT || uVk == VK_MENU)
		return -1;
	if (!m_pListener->GetOpenStatus() || lpbKeyState[VK_CAPITAL]&0x01)
		return -1;
	int iRet = -1;
	for (int i = 0; i < HKI_COUNT; i++)
	{
		DWORD dwHotkey = g_SettingsG->dwHotkeys[i];
		WORD wModifier = HIWORD(dwHotkey);
		WORD wVk = LOWORD(dwHotkey);
		if (wVk == uVk)
		{
			if (((wModifier & MOD_ALT)!=0) ^ ((lpbKeyState[VK_MENU] & 0x80)!=0))
				continue;
			if (((wModifier & MOD_CONTROL) != 0) ^ ((lpbKeyState[VK_CONTROL] & 0x80) != 0))
				continue;
			if (((wModifier & MOD_SHIFT) != 0) ^ ((lpbKeyState[VK_SHIFT] & 0x80) != 0))
				continue;
			iRet = i;
			break;
		}
	}
	if(iRet != -1)
	{
		if ((m_ctx.cComp > 0 && iRet!=HKI_Repeat) || m_ctx.inState == INST_USERDEF)
			iRet = -1;
		if(iRet == HKI_UDMode && (g_SettingsG->compMode == IM_SPELL || IsTempSpell()))
			iRet = -1;
	}else//iRet == -1
	{
		if((lpbKeyState[VK_CONTROL] & 0x80) && (lpbKeyState[VK_MENU] & 0x80) && g_SettingsG->bEnableDebugSkin)
		{
			if(uVk == 'F')
			{//load debug skin
				return HKI_LoadDebugSkin;
			}else if(uVk='U')
			{
				return HKI_UnloadDebugSkin;
			}
		}

		if(m_ctx.sCandCount && ((uVk>='0' && uVk<='9')||(uVk>=VK_NUMPAD0 && uVk<=VK_NUMPAD9)))
		{//number
			if(lpbKeyState[VK_CONTROL] & 0x80)
			{
				iRet = (lpbKeyState[VK_SHIFT] & 0x80)?HKI_DelCandidate:HKI_AdjustRate;
			}
		}
	}

	return iRet;
}

void CInputState::Tips_Init()
{
	if (m_bUpdateTips)
	{
		for (int i = 0; i < TT_COUNT; i++)
		{
			m_tips[i].RemoveAll();
		}
		pugi::xml_document xmlTips;
		const wchar_t * groups[] = {
			L"all",L"spell",L"shape"
		};
		if (xmlTips.load_file(CDataCenter::getSingletonPtr()->GetDataPath() + _T("\\data\\tips.xml")))
		{
			pugi::xml_node tips = xmlTips.child(L"tips");
			for (int i = 0; i < 3; i++)
			{
				pugi::xml_node tip = tips.child(groups[i]).child(L"tip");
				while (tip)
				{
					m_tips[i].Add(S_CW2T(tip.attribute(L"value").as_string()));
					tip = tip.next_sibling(L"tip");
				}

			}
		}
		m_bUpdateTips = FALSE;
	}
}

int CInputState::Tips_Rand(BOOL bSpell, TCHAR *pszBuf)
{
	Tips_Init();
	if (bSpell)
	{
		int total = (int) (m_tips[TT_SPELL].GetCount() + m_tips[TT_BOTH].GetCount());
		int idx = rand() % total;
		if (idx < (int)m_tips[TT_SPELL].GetCount())
			_tcscpy(pszBuf, m_tips[TT_SPELL][idx]);
		else
			_tcscpy(pszBuf, m_tips[TT_BOTH][idx - m_tips[TT_SPELL].GetCount()]);
		return idx;
	}
	else
	{
		int total = (int) (m_tips[TT_SHAPE].GetCount() + m_tips[TT_BOTH].GetCount());
		int idx = rand() % total;
		if (idx < (int)m_tips[TT_SHAPE].GetCount())
			_tcscpy(pszBuf, m_tips[TT_SHAPE][idx]);
		else
			_tcscpy(pszBuf, m_tips[TT_BOTH][idx - m_tips[TT_SHAPE].GetCount()]);
		return idx;
	}
}

int CInputState::Tips_Next(BOOL bSpell,TCHAR *pszBuf, int iTip, bool bNext)
{
	Tips_Init();
	int idx = 0;
	if (bSpell)
	{
		int total = (int)(m_tips[TT_SPELL].GetCount() + m_tips[TT_BOTH].GetCount());
		if (iTip == -1)
		{
			idx = bNext ? 0 : total - 1;
		}
		else
		{
			idx = bNext ? (iTip + 1) : (iTip - 1);
		}
		idx = (idx+total)%total;
		if (idx < (int)m_tips[TT_SPELL].GetCount())
			_tcscpy_s(pszBuf, 200,m_tips[TT_SPELL][idx]);
		else
			_tcscpy_s(pszBuf, 200,m_tips[TT_BOTH][idx - m_tips[TT_SPELL].GetCount()]);
		return idx;
	}
	else
	{
		int total = (int)(m_tips[TT_SHAPE].GetCount() + m_tips[TT_BOTH].GetCount());
		if (iTip == -1)
		{
			idx = bNext ? 0 : total - 1;
		}
		else
		{
			idx = bNext ? (iTip + 1) : (iTip - 1);
		}
		idx = (idx+total)%total;
		if (idx < (int)m_tips[TT_SHAPE].GetCount())
			_tcscpy_s(pszBuf,200, m_tips[TT_SHAPE][idx]);
		else
			_tcscpy_s(pszBuf, 200,  m_tips[TT_BOTH][idx - m_tips[TT_SHAPE].GetCount()]);
	}
	SLOG_INFO("iTip:" << iTip << " bNext:" << bNext<<" idx:"<<idx);
	return idx;
}

void CInputState::GetShapeComp(const WCHAR *pInput,char cLen)
{
	if(CIsSvrProxy::GetSvrCore()->ReqQueryComp(pInput,cLen)==ISACK_SUCCESS)
	{
		PMSGDATA pData=CIsSvrProxy::GetSvrCore()->GetAck();		
		swprintf(m_ctx.szTip,L"词\"%s\"的编码=%s",pInput,SStringW((WCHAR*)pData->byData,pData->sSize/2));
	}else
	{
		swprintf(m_ctx.szTip,L"查询词\"%s\"的编码失败",pInput);
	}
}

#define MKI_ALL (MKI_RECORD|MKI_TTSINPUT|MKI_AUTOPICK)
BYTE CInputState::GetKeyinMask(BOOL bAssociate,BYTE byMask)
{
	BYTE byRet=0;
	if(g_SettingsG->bAutoMatch) byRet|=(MKI_AUTOPICK&byMask);
	if(g_SettingsUI->bRecord) byRet|=(MKI_RECORD&byMask);
	if(g_SettingsUI->bSound) byRet|=(MKI_TTSINPUT&byMask);
	if(bAssociate)
	{
		if(g_SettingsUI->bRecord) byRet|=MKI_ASTSENT;
		switch(g_SettingsG->byAstMode)
		{
		case AST_CAND:byRet |= MKI_ASTCAND; break;
		case AST_ENGLISH:byRet|=MKI_ASTENGLISH;break;
		case AST_PHRASEREMIND:byRet|=MKI_PHRASEREMIND;break;
		default:break;
		}
	}
	return byRet;
}

void CInputState::ClearContext(UINT dwMask)
{
	SLOG_INFO("dwMask:"<<dwMask);
	if(dwMask&CPC_COMP)
	{
		m_ctx.szComp[0]=0;
		m_ctx.cComp = 0;
	}
	if(dwMask&CPC_ENGLISH)
	{
		m_ctx.pbyEnSpell=NULL;
		m_ctx.pbyEnPhontic=NULL;
	}
	if(dwMask&CPC_STATE)
	{
		m_ctx.inState=INST_CODING;
		m_ctx.sbState=SBST_NORMALSTATE;
	}
	if(dwMask&CPC_CAND)
	{
		if(m_ctx.ppbyCandInfo)
		{
			free(m_ctx.ppbyCandInfo);
			m_ctx.ppbyCandInfo=NULL;
		}
		m_ctx.sCandCount=0;
		m_ctx.iCandBegin = 0;
		m_ctx.iCandLast = -1;
		m_ctx.pbyEnAstPhrase=NULL;
		m_ctx.byCandType = MCR_NORMAL;
	}
	if(dwMask&CPC_SENTENCE)
	{
		m_ctx.sSentCaret=m_ctx.sSentLen=0;
		m_ctx.sSentLen=0;
		memset(m_ctx.szSentText,0,sizeof(LPBYTE)*MAX_SENTLEN);
	}
	if(dwMask&CPC_SPELL)
	{
		BYTE i;
		for(i=0;i<m_ctx.bySyllables;i++)
		{
			if(m_ctx.pbyBlur[i]) free(m_ctx.pbyBlur[i]);
			m_ctx.pbyBlur[i]=NULL;
		}
		memset(m_ctx.szWord,VK_SPACE,2*MAX_SYLLABLES);
		memset(m_ctx.bySelect,0,sizeof(BOOL)*MAX_SYLLABLES);
		memset(m_ctx.spellData,0,sizeof(SPELLINFO)*MAX_SYLLABLES);
		m_ctx.bySyllables=1;
		m_ctx.bySyCaret=0xFF;
		m_ctx.byCaret=0;
	}
	if(dwMask & CPC_TIP)
	{
		m_ctx.szTip[0]=0;
		m_ctx.bShowTip=FALSE;
	}
	if (dwMask & CPC_UDCOMP)
	{
		m_ctx.cCompACLen = 0;
	}
	if (dwMask & CPC_INPUT)
	{
		m_ctx.cInput = 0;
	}
}


void CInputState::InputStart()
{
	SLOG_INFO("");
	m_pListener->OnInputStart();

	DWORD tmCur = GetTickCount();
	if (tmCur - m_tmInputEnd > 10 * 1000)
	{
		m_tmInputStart = tmCur;
	}
	else
	{
		m_tmInputStart = m_tmInputEnd;
	}
}

void CInputState::InputResult(const SStringW &strResult,BYTE byAstMask)
{
	SLOG_INFO("result:"<<strResult<<" astMask:"<<byAstMask);

	SASSERT(m_pListener);
	SStringW strTemp = strResult;
	if (g_SettingsUI->bInputBig5)
	{
		int nLen = CUtils::GB2GIB5(strResult, strResult.GetLength(), NULL, 0);
		WCHAR *pBig5 = new WCHAR[nLen+1];
		CUtils::GB2GIB5(strResult, strResult.GetLength(), pBig5, nLen+1);
		strTemp = SStringW(pBig5);
		delete[]pBig5;
	}
	{
		m_pListener->OnInputResult(strTemp);
		_tcscpy(m_ctx.szInput, strTemp);
		m_ctx.cInput = strTemp.GetLength();
	}

	if(byAstMask!=0)
	{
		SStringW strResult = strTemp;
		KeyIn_InputAndAssociate(&m_ctx,strResult,(short)strResult.GetLength(),byAstMask);
	}else
	{
		InputHide(FALSE);
	}

	m_tmInputEnd = GetTickCount();
	CDataCenter::getSingletonPtr()->GetData().m_tmInputSpan += m_tmInputEnd - m_tmInputStart;
	CDataCenter::getSingletonPtr()->GetData().m_cInputCount+= strTemp.GetLength();
}



void CInputState::InputEnd()
{
	SLOG_INFO("");
	m_pListener->OnInputEnd();
}

void CInputState::InputUpdate()
{
	SLOG_INFO("");
	m_pListener->UpdateInputWnd();
}


void CInputState::InputOpen()
{
	SLOG_INFO("");
	m_pListener->OpenInputWnd();

}

void CInputState::InputHide(BOOL bDelay)
{
	SLOG_INFO("delay:"<<bDelay);
	if(m_pListener->IsCompositing()) 
		m_pListener->OnInputEnd();
	m_pListener->CloseInputWnd(bDelay);
}

void CInputState::StatusbarUpdate()
{
	m_pListener->OnCommand(CMD_UPDATEMODE,0);
}

BOOL CInputState::HandleKeyDown(UINT uVKey,UINT uScanCode,const BYTE * lpbKeyState)
{
	SLOG_INFO("uVKey:"<<uVKey<<" uScanCode:"<<uScanCode);
	//首先使用VK处理快捷键及重码翻页键
	int iHotKey = TestHotKey(uVKey, lpbKeyState);
	if (iHotKey != -1)
	{
		if(iHotKey == HKI_LoadDebugSkin || iHotKey == HKI_UnloadDebugSkin)
		{//load debug skin
			return TRUE;
		}
		if(iHotKey == HKI_UDMode)
		{//切换到用户自定义输入状态
			ClearContext(CPC_ALL);
			m_ctx.inState = INST_USERDEF;
			if (!m_pListener->IsCompositing())
			{
				InputStart();
				InputOpen();
			}
			InputUpdate();
		}else if(iHotKey == HKI_Repeat)
		{
			return KeyIn_RepeatInput(&m_ctx,lpbKeyState);
		}
		if(iHotKey != HKI_AdjustRate && iHotKey != HKI_DelCandidate)//调频，删词交给后面逻辑处理。
			return TRUE;
	}

	BOOL bHandle=FALSE;
	InputContext * lpCntxtPriv = &m_ctx;
	if(!bHandle && lpCntxtPriv && lpCntxtPriv->sCandCount && lpCntxtPriv->sbState!=SBST_SENTENCE)
	{//处理重码
		BYTE byCandIndex=0;
		if(uVKey==VK_SPACE) 
		{//空格
			byCandIndex='1';
		}else if((uVKey>='0' && uVKey<='9') || (uVKey>=VK_NUMPAD0 && uVKey<=VK_NUMPAD9))
		{//数字键
			if((lpbKeyState[VK_CONTROL]&0x80) ||
				(!(lpbKeyState[VK_SHIFT]&0x80) //未按下Shift的情况
				&& !KeyIn_IsNumCode(lpCntxtPriv) //数字不是编码状态
				&& (!g_SettingsG->bCandSelNoNum || g_SettingsG->compMode==IM_SPELL) //未禁止数字选择重码
				)//允许数字选择重码
				)
			{//获得输入数字的ASCII码
				if((uVKey>=VK_NUMPAD0 && uVKey<=VK_NUMPAD9))
					byCandIndex=uVKey-VK_NUMPAD0+'0';
				else
					byCandIndex=uVKey;
			}
		}else if(g_SettingsG->b23CandKey && !(lpbKeyState[VK_SHIFT]&0x80))
		{//SHIFT 模式下不进入
			UINT uVk=MapVirtualKey(uScanCode,1);
			if(uVk==g_SettingsG->by2CandVK && lpCntxtPriv->sCandCount>=2) byCandIndex='2';
			if(uVk==g_SettingsG->by3CandVK && lpCntxtPriv->sCandCount>=3) byCandIndex='3';
		}
		if(byCandIndex )
		{
			if(!((lpCntxtPriv->sbState==SBST_ASSOCIATE && g_SettingsG->byAstMode==AST_ENGLISH && !(lpbKeyState[VK_CONTROL]&0x80))//英文联想状态只有按下Ctrl才进入编码选择
				|| (m_ctx.compMode==IM_SPELL && lpCntxtPriv->inState==INST_CODING && (uVKey==VK_SPACE || uVKey=='\'') ) )	//拼音输入状态下的输入键及手动音节  0xde=VkKeyScan('\'')
				)
			{
				bHandle=KeyIn_All_SelectCand(lpCntxtPriv,byCandIndex,0,lpbKeyState);
			}
		}
		if(!bHandle) bHandle=KeyIn_All_TurnCandPage(lpCntxtPriv,uVKey,lpbKeyState);
	}


	//处理拼音的音节移动
	if(!bHandle && lpCntxtPriv->compMode==IM_SPELL && lpCntxtPriv->inState==INST_CODING)
	{
		bHandle=KeyIn_Spell_MoveCaret(lpCntxtPriv,uVKey,lpbKeyState);
		if(!bHandle && uVKey==VK_DELETE) bHandle=KeyIn_Spell_SyFix(lpCntxtPriv,uVKey,lpbKeyState);//处理VK_DELETE
	}

	WCHAR wChar = 0;
	int test = ToUnicode(uVKey,uScanCode,lpbKeyState,&wChar,1,0);

	if(!bHandle && uVKey && test!=0)
	{
		uVKey = wChar;
		if(lpCntxtPriv->inState==INST_CODING)
		{//先做状态转换前处理
			BOOL bReadyEn=FALSE;
			BOOL bReadyDgt=FALSE;
			if(lpCntxtPriv->sbState==SBST_NORMALSTATE)
			{
				if(lpCntxtPriv->compMode==IM_SPELL)
				{
					if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
					{
						bReadyEn=TRUE;
						bReadyDgt=TRUE;
					}
				}else
				{
					if(lpCntxtPriv->cComp==0)
					{
						bReadyEn=TRUE;
						bReadyDgt=TRUE;
					}
				}
			}else if(lpCntxtPriv->sbState==SBST_ASSOCIATE)
			{
				bReadyEn=TRUE;
				bReadyDgt=TRUE;
				if((g_SettingsG->byAstMode==AST_CAND ||(g_SettingsG->byAstMode==AST_ENGLISH &&(lpbKeyState[VK_CONTROL]&0x80))) && lpCntxtPriv->sCandCount)
					bReadyDgt=FALSE;
			}
			if(IsTempSpell() && (bReadyEn || bReadyDgt) && (isdigit(uVKey) || isupper(uVKey)))
			{//temp spell mode
				CUtils::SoundPlay(_T("error"));
				return FALSE;
			}
			if((bReadyEn || bReadyDgt) && lpCntxtPriv->bShowTip) //关闭tip
				lpCntxtPriv->bShowTip=FALSE;
			if(bReadyEn && uVKey>='A' && uVKey<='Z' && (lpbKeyState[VK_CAPITAL]&0x01)== 0)
			{//大写输入，则切换到英文状态
				ClearContext(CPC_ALL);
				if(g_SettingsUI->bEnglish)
				{
					lpCntxtPriv->inState=INST_ENGLISH;
					//确保打开输入窗口
					InputStart();
					InputOpen();
				}
			}else if(bReadyDgt && uVKey>='0' && uVKey<='9')
			{//数字输入，进入数字输入状态
				ClearContext(CPC_ALL);
				lpCntxtPriv->inState=INST_DIGITAL;
				InputHide(FALSE);
			}
		}
		if(lpCntxtPriv->inState==INST_CODING)
		{
			if(lpCntxtPriv->sbState==SBST_NORMALSTATE)
			{//编码输入
				if(lpCntxtPriv->compMode==IM_SPELL)
				{//拼音状态
					bHandle=KeyIn_Spell_Normal(lpCntxtPriv,uVKey,lpbKeyState);
				}else
				{
					bHandle=KeyIn_Code_Normal(lpCntxtPriv,uVKey,lpbKeyState);
				}
			}else if(lpCntxtPriv->sbState==SBST_ASSOCIATE)
			{//联想状态
				bHandle=KeyIn_All_Associate(lpCntxtPriv,uVKey,lpbKeyState);
			}else if(lpCntxtPriv->sbState==SBST_SENTENCE)
			{//句子状态
				bHandle=KeyIn_All_Sentence(lpCntxtPriv,uVKey,lpbKeyState);
				if(!bHandle) KeyIn_Code_Symbol(lpCntxtPriv,uVKey,lpbKeyState);
			}
		}else if(lpCntxtPriv->inState==INST_ENGLISH)
		{//英文单词状态
			bHandle=KeyIn_Code_English(lpCntxtPriv,uVKey,lpbKeyState);
		}else if(lpCntxtPriv->inState==INST_DIGITAL)
		{//数字输入状态
			bHandle=KeyIn_Digital_ChangeComp(lpCntxtPriv,uVKey,lpbKeyState);
		}else if(lpCntxtPriv->inState==INST_USERDEF)
		{//用户自定义输入状态
			bHandle=KeyIn_UserDef_ChangeComp(lpCntxtPriv,uVKey,lpbKeyState);
		}else if(lpCntxtPriv->inState==INST_LINEIME)
		{//笔画输入状态
			BOOL bInputKey=FALSE;
			if(uVKey>=VK_NUMPAD1&&uVKey<=VK_NUMPAD6)
			{//小键盘输入
				bInputKey=TRUE;
			}else
			{//设置的转换键
				int i;
				for( i=0;i<6;i++)
				{
					if(g_SettingsG->byLineKey[i]==uVKey)
					{
						uVKey=VK_NUMPAD1+i;
						bInputKey=TRUE;
						break;
					}
				}
				if(uVKey==VK_ESCAPE || uVKey==VK_RETURN || uVKey==VK_BACK) bInputKey=TRUE;
			}
			if(bInputKey) bHandle=KeyIn_Line_ChangeComp(lpCntxtPriv,uVKey,lpbKeyState);
		}
		if(!bHandle) CUtils::SoundPlay(_T("error"));
	}
	return bHandle;
}

//拼音缓冲区的移动
void SpellBuf_Move(InputContext * lpCntxtPriv,BYTE byBegin,BYTE bySize,char cDistance)
{
	memmove(lpCntxtPriv->pbyBlur+byBegin+cDistance,lpCntxtPriv->pbyBlur+byBegin,sizeof(LPBYTE)*bySize);
	memmove(lpCntxtPriv->spellData+byBegin+cDistance,lpCntxtPriv->spellData+byBegin,sizeof(SPELLINFO)*bySize);
	memmove(lpCntxtPriv->bySelect+byBegin+cDistance,lpCntxtPriv->bySelect+byBegin,sizeof(BYTE)*bySize);
	memmove(lpCntxtPriv->szWord+byBegin+cDistance,lpCntxtPriv->szWord+byBegin,2*bySize);
}

//清空指定音节的数据
void SpellBuf_ClearSyllable(InputContext * lpCntxtPriv,BYTE bySyllable)
{
	memset(lpCntxtPriv->pbyBlur+bySyllable,0,sizeof(LPBYTE));
	memset(lpCntxtPriv->spellData+bySyllable,0,sizeof(SPELLINFO));
	memset(lpCntxtPriv->bySelect+bySyllable,0,sizeof(BYTE));
	memset(lpCntxtPriv->szWord+bySyllable,VK_SPACE,2);
}

BOOL CInputState::KeyIn_Test_RepeatInput(InputContext *  lpCntxtPriv,const BYTE * lpbKeyState)
{
	if(KeyIn_IsCoding(lpCntxtPriv))
	{
		return lpCntxtPriv->cComp>0;
	}else
	{
		return lpCntxtPriv->cInput>0;
	}
}

BOOL CInputState::KeyIn_RepeatInput(InputContext *  lpCntxtPriv,const BYTE * lpbKeyState)
{
	if(KeyIn_IsCoding(lpCntxtPriv))
	{//编码上屏
		SStringW strResult;
		if (lpCntxtPriv->cComp == 1 && (lpCntxtPriv->szComp[0]<'a' || lpCntxtPriv->szComp[0]>'z'))
		{
			strResult += Symbol_Convert(&m_ctx, lpCntxtPriv->szComp[0], lpbKeyState);
		}
		else
		{
			strResult=SStringW(lpCntxtPriv->szComp, lpCntxtPriv->cComp);
		}
		InputResult(strResult,GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT));

		InputEnd();
		InputHide(FALSE);
		ClearContext(CPC_ALL);
		return TRUE;
	}
	else if (lpCntxtPriv->cInput > 0)
	{
		InputStart();
		InputResult(SStringT(lpCntxtPriv->szInput,lpCntxtPriv->cInput), GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT));
		InputEnd();
		return TRUE;
	}else
	{
		return FALSE;
	}
}

//拼音状态下更新候选词窗口
void CInputState::KeyIn_Spell_UpdateCandList(InputContext *  lpCntxtPriv,BYTE byCaret)
{
	DWORD dwRet=ISACK_ERROR;
	ClearContext(CPC_CAND);
	if(lpCntxtPriv->spellData[byCaret].bySpellLen==0) return;

	BOOL bPhraseFirst = g_SettingsG->bPYPhraseFirst;
	BOOL bReverse = FALSE;
	BOOL bAssociate = FALSE;

	if(byCaret==lpCntxtPriv->bySyllables-1)
	{
		bReverse=TRUE;//光标在最后，逆向查询词组
		if(lpCntxtPriv->bySyllables>=4) bAssociate=TRUE;	//不完整词组联想
		dwRet=CIsSvrProxy::GetSvrCore()->ReqQueryCandSpellEx((PBLURINFO2 *)lpCntxtPriv->pbyBlur,lpCntxtPriv->bySyllables,bPhraseFirst,bReverse,bAssociate);
	}else
	{
		if(byCaret==0 && lpCntxtPriv->bySyllables>=4) bAssociate=TRUE;//不完整词组联想
		dwRet=CIsSvrProxy::GetSvrCore()->ReqQueryCandSpellEx((PBLURINFO2 *)lpCntxtPriv->pbyBlur+byCaret,lpCntxtPriv->bySyllables-byCaret,bPhraseFirst,bReverse,bAssociate);
	}

	if(dwRet==ISACK_SUCCESS)
	{
		PMSGDATA pMsgData=CIsSvrProxy::GetSvrCore()->GetAck();
		short i,sCount=0,sValidCount=0;
		LPBYTE pBuf=m_pbyMsgBuf;
		memcpy(pBuf,pMsgData->byData,pMsgData->sSize);
		memcpy(&sCount,pBuf,2);
		pBuf+=2;
		lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(sCount*sizeof(LPBYTE));
		for(i=0;i<sCount;i++)
		{
			char nLen = pBuf[2];
			if(byCaret==(lpCntxtPriv->bySyllables-1))
			{
				if(wcsncmp(lpCntxtPriv->szWord+lpCntxtPriv->bySyllables-nLen,(WCHAR*)(pBuf+3),nLen)!=0) //去掉与预测相同的重码
					lpCntxtPriv->ppbyCandInfo[sValidCount++]=pBuf;
			}else
			{
				if(wcsncmp(lpCntxtPriv->szWord+byCaret,(WCHAR*)(pBuf+3),nLen)!=0)
					lpCntxtPriv->ppbyCandInfo[sValidCount++]=pBuf;
			}
			pBuf+=nLen*2+3;
		}
		lpCntxtPriv->sCandCount=sValidCount;
	}
}

//拼音状态下移到编辑音节
BOOL CInputState::KeyIn_Spell_MoveCaret(InputContext *lpCntxtPriv, UINT byInput,
										CONST BYTE * lpbKeyState)
{
	BOOL bRet=TRUE;
	if(lpCntxtPriv->bySyCaret==0xFF)
	{
		switch(byInput)
		{
		case VK_LEFT:
			if(lpCntxtPriv->byCaret>0)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,lpCntxtPriv->byCaret-1,1,1);
					SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->byCaret-1);
				}
				lpCntxtPriv->byCaret--;
			}
			break;
		case VK_HOME:
			if(lpCntxtPriv->byCaret>0)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,0,lpCntxtPriv->byCaret,1);
					SpellBuf_ClearSyllable(lpCntxtPriv,0);
				}
				lpCntxtPriv->byCaret=0;
			}
			break;
		case VK_RIGHT:
			if(lpCntxtPriv->byCaret<lpCntxtPriv->bySyllables-1)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,lpCntxtPriv->byCaret+1,1,-1);
					SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->byCaret+1);
				}
				lpCntxtPriv->byCaret++;
			}
			break;
		case VK_END:
			if(lpCntxtPriv->byCaret<lpCntxtPriv->bySyllables-1)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,lpCntxtPriv->byCaret+1,lpCntxtPriv->bySyllables-(lpCntxtPriv->byCaret+1),-1);
					SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->bySyllables-1);
				}
				lpCntxtPriv->byCaret=lpCntxtPriv->bySyllables-1;
			}
			break;
		default:
			bRet=FALSE;
			break;
		}
		if(bRet) 
		{
			KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
		}
	}else
	{
		bRet=FALSE;
		//处理方向键
		if(byInput==VK_LEFT)
		{
			if(lpCntxtPriv->bySyCaret>1) 
				lpCntxtPriv->bySyCaret--;
			else
				lpCntxtPriv->bySyCaret = lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen;
			bRet=TRUE;
		}else if(byInput==VK_RIGHT)
		{
			if(lpCntxtPriv->bySyCaret<lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen)
				lpCntxtPriv->bySyCaret++;
			else
				lpCntxtPriv->bySyCaret = 1;
			bRet=TRUE;
		}else if(byInput==VK_HOME)
		{
			lpCntxtPriv->bySyCaret=1;
			bRet=TRUE;
		}else if(byInput==VK_END)
		{
			lpCntxtPriv->bySyCaret=lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen;
			bRet=TRUE;
		}
	}
	return bRet;
}

//从byStartPos音节开始预测，如果开始音节的前一个音节是用户选择的单字，则该字会作为预测的前缀发给服务器
void CInputState::KeyIn_Spell_Forecast(InputContext * lpCntxtPriv,BYTE byStartPos)
{
	BYTE byEnd=byStartPos;
	while(byEnd<lpCntxtPriv->bySyllables)
	{
		//分别找到预测的起始点和结束点
		BYTE byBegin=byEnd;
		while(lpCntxtPriv->bySelect[byBegin]!=0 && byBegin<lpCntxtPriv->bySyllables) byBegin++;
		byEnd=byBegin;
		while(lpCntxtPriv->bySelect[byEnd]==0 && byEnd<lpCntxtPriv->bySyllables) byEnd++;
		if(byEnd>byBegin)
		{
			WCHAR szPrefix=0;
			if(byBegin>0 && lpCntxtPriv->bySelect[byBegin-1]==1)
				szPrefix = lpCntxtPriv->szWord[byBegin-1];
			if(CIsSvrProxy::GetSvrCore()->ReqForecastSpell(szPrefix,(PBLURINFO2*)lpCntxtPriv->pbyBlur+byBegin,byEnd-byBegin)==ISACK_SUCCESS)
			{
				PMSGDATA pMsgData=CIsSvrProxy::GetSvrCore()->GetAck();
				BYTE byPrefix=pMsgData->byData[0];
				BYTE *pBuf=pMsgData->byData+1;
				BYTE iPhrase,byPhrases=*pBuf++;
				WCHAR *pszWord=lpCntxtPriv->szWord+byBegin;
				if(byPrefix==1)
				{//前缀有效
					wcsncpy(pszWord,(WCHAR*)(pBuf+2)+1,pBuf[1]-1);
					pszWord+=pBuf[1]-1;
					pBuf+=pBuf[1]*2+2;
					byPhrases--;
				}
				for(iPhrase=0;iPhrase<byPhrases;iPhrase++)
				{//后续词组
					wcsncpy(pszWord,(WCHAR*)(pBuf+2),pBuf[1]);
					pszWord+=pBuf[1];
					pBuf+=pBuf[1]*2+2;
				}
			}
		}
	}
}


BOOL CInputState::KeyIn_Spell_SyFix(InputContext * lpCntxtPriv,UINT byInput,
									CONST BYTE * lpbKeyState)
{
	if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0) return FALSE;
	if(lpCntxtPriv->bySyCaret==0xFF)
	{
		if(byInput==VK_RETURN && !(lpbKeyState[VK_SHIFT]&0x80))
		{
			lpCntxtPriv->bySyCaret=lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen;
			return TRUE;
		}
	}else
	{
		BOOL bRet=FALSE;
		if(byInput==VK_RETURN || byInput==VK_ESCAPE)
		{//退出音节修改状态
			if(lpCntxtPriv->sCandCount)
				lpCntxtPriv->bySyCaret=0xFF;
			else
			{
				lpCntxtPriv->bShowTip=TRUE;
				_tcscpy(lpCntxtPriv->szTip,_T("音节错误!"));
			}
			return TRUE;
		}else
		{//处理修改键
			BOOL bModified=FALSE;
			SPELLINFO * lpSpi=lpCntxtPriv->spellData+lpCntxtPriv->byCaret;
			if(byInput==VK_BACK)
			{
				if(lpCntxtPriv->bySyCaret>0)
				{
					memmove(lpSpi->szSpell+lpCntxtPriv->bySyCaret-1,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret);
					lpSpi->bySpellLen--;
					lpCntxtPriv->bySyCaret--;
					bModified=TRUE;
				}
				bRet=TRUE;
			}else if(byInput==VK_DELETE)
			{
				if(lpCntxtPriv->bySyCaret<lpSpi->bySpellLen)
				{
					memmove(lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret+1,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret-1);
					lpSpi->bySpellLen--;
					bModified=TRUE;
				}
				bRet=TRUE;
			}else if(byInput>='a' && byInput<='z')
			{//编码输入
				if(lpSpi->bySpellLen<6)
				{
					memmove(lpSpi->szSpell+lpCntxtPriv->bySyCaret+1,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret);
					lpSpi->szSpell[lpCntxtPriv->bySyCaret]=byInput;
					lpSpi->bySpellLen++;
					lpCntxtPriv->bySyCaret++;
					bModified=TRUE;
				}
				bRet=TRUE;
			}
			if(bModified)
			{//当前音节被修改，更新数据
				lpCntxtPriv->bShowTip=FALSE;
				if(lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret])
				{//删除原有数据
					free(lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret]);
					lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret]=NULL;
					lpCntxtPriv->szWord[lpCntxtPriv->byCaret]=VK_SPACE;
					ClearContext(CPC_CAND);//清除当前重码
				}
				lpCntxtPriv->bySelect[lpCntxtPriv->byCaret]=0;
				if(lpSpi->bySpellLen==0)
				{//自动退出音节编辑状态
					lpCntxtPriv->bySyCaret=0xFF;
				}else if(CIsSvrProxy::GetSvrCore()->ReqSpell2ID(lpSpi->szSpell,lpSpi->bySpellLen)==ISACK_SUCCESS)
				{//更新当前正在编辑的音节
					CIsSvrProxy::GetSvrCore()->ReqSpellGetBlur(lpSpi->szSpell,lpSpi->bySpellLen);
					PMSGDATA pData=CIsSvrProxy::GetSvrCore()->GetAck();
					lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret]=(LPBYTE)malloc(pData->sSize);
					memcpy(lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret],pData->byData,pData->sSize);
					//重新预测本句的候选句
					KeyIn_Spell_Forecast(lpCntxtPriv,0);
					//获取当前插入符位置的重码
					KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
				}
			}
			return bRet;
		}
	}
	return FALSE;
}

//拼音状态下改变编码
BOOL CInputState::KeyIn_Spell_ChangeComp(InputContext* lpCntxtPriv,UINT byInput,
										 CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	SPELLINFO* pSpInfo=lpCntxtPriv->spellData+lpCntxtPriv->byCaret;
	WCHAR *pComp=lpCntxtPriv->szWord+lpCntxtPriv->byCaret;
	BYTE byCaret=lpCntxtPriv->byCaret;
	if(byInput>='a' && byInput<='z')
	{
		bCompChar=TRUE;
		if(pSpInfo->bySpellLen==0)
		{
			if(byInput=='u' || byInput=='v')
			{
				if(!IsTempSpell() && lpCntxtPriv->bySyllables==1)
				{//切换到用户自定义模式
					ClearContext(CPC_ALL);
					lpCntxtPriv->inState=INST_USERDEF;
					InputOpen();
					InputUpdate();
					InputStart();
				}
				bCompChar=FALSE;
			}else if(byInput=='i')
			{
				if(!IsTempSpell() && lpCntxtPriv->bySyllables==1)
				{//切换到笔画输入状态
					ClearContext(CPC_ALL);
					lpCntxtPriv->inState=INST_LINEIME;
					InputOpen();
					InputUpdate();
					InputStart();
				}
				bCompChar=FALSE;
			}else
			{
				if(g_SettingsG->bShowOpTip && !IsTempSpell())
				{
					lpCntxtPriv->bShowTip=TRUE;
					lpCntxtPriv->iTip=Tips_Rand(TRUE,lpCntxtPriv->szTip);
				}
			}
		}

		if(!bCompChar)	return lpCntxtPriv->bySyllables==1;
	}else if(byInput=='\'' && pSpInfo->bySpellLen)
	{
		bCompChar=TRUE;
	}

	if(bCompChar)
	{
		PMSGDATA pData;
		if(lpCntxtPriv->sbState==SBST_ASSOCIATE) 
		{
			ClearContext(CPC_ALL);
		}
		if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
		{//开始编码输入,生成开始编码消息以获取光标跟随时输入窗口的坐标
			InputOpen();
			InputStart();
		}
		pSpInfo->szSpell[pSpInfo->bySpellLen++]=byInput;
		if(CIsSvrProxy::GetSvrCore()->ReqSpell2ID(pSpInfo->szSpell,pSpInfo->bySpellLen)==ISACK_SUCCESS)
		{//更新当前正在编辑的音节
			CIsSvrProxy::GetSvrCore()->ReqSpellGetBlur(pSpInfo->szSpell,pSpInfo->bySpellLen);
			pData=CIsSvrProxy::GetSvrCore()->GetAck();
			if(lpCntxtPriv->pbyBlur[byCaret]) free(lpCntxtPriv->pbyBlur[byCaret]);
			lpCntxtPriv->pbyBlur[byCaret]=(LPBYTE)malloc(pData->sSize);
			memcpy(lpCntxtPriv->pbyBlur[byCaret],pData->byData,pData->sSize);
			lpCntxtPriv->bySelect[byCaret]=0;
		}else
		{//插入一个新音节
			pSpInfo->bySpellLen--;
			if(lpCntxtPriv->bySyllables<MAX_SYLLABLES)
			{
				//将当前插入符后的音节后移一个音节,以增加一个音节空间
				SpellBuf_Move(lpCntxtPriv,
					byCaret+1,
					lpCntxtPriv->bySyllables-(byCaret+1),
					1);
				lpCntxtPriv->bySyllables++;
				lpCntxtPriv->byCaret++;
				//清空当前音节数据
				SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->byCaret);
				pSpInfo++;
				pComp++;
				byCaret++;
				//根据当前输入填写当前音节数据
				if(byInput!='\'')
				{
					pSpInfo->szSpell[pSpInfo->bySpellLen++]=byInput;
					if(CIsSvrProxy::GetSvrCore()->ReqSpell2ID(pSpInfo->szSpell,1)==ISACK_SUCCESS)
					{
						pData=CIsSvrProxy::GetSvrCore()->GetAck();
					}else
					{//不是拼音，但可能通过借声母形成拼音
						pData=CIsSvrProxy::GetSvrCore()->GetAck();
						pData->byData[0]=0;
					}
					if(pData->byData[0]==0 && 
						lpCntxtPriv->bySyllables>1 &&
						byCaret==lpCntxtPriv->bySyllables-1 && 
						lpCntxtPriv->spellData[byCaret-1].bySpellLen>1 )
					{//正在编辑最后一个音节，当前输入的不是声母，判断是否需要从前一个音节借一个声母
						SPELLINFO siPrev,siCur;
						siPrev=lpCntxtPriv->spellData[byCaret-1];
						siCur=lpCntxtPriv->spellData[byCaret];
						siCur.szSpell[0]=siPrev.szSpell[siPrev.bySpellLen-1];
						siCur.szSpell[1]=byInput;
						siCur.bySpellLen=2;
						siPrev.bySpellLen--;
						if(CIsSvrProxy::GetSvrCore()->ReqSpell2ID(siPrev.szSpell,siPrev.bySpellLen)==ISACK_SUCCESS &&
							CIsSvrProxy::GetSvrCore()->ReqSpell2ID(siCur.szSpell,siCur.bySpellLen)==ISACK_SUCCESS)
						{//借声母成功,修改前一音节数据
							_ASSERT(lpCntxtPriv->pbyBlur[byCaret-1]);
							free(lpCntxtPriv->pbyBlur[byCaret-1]);	//注意释放原拼音占用的堆内存
							SpellBuf_ClearSyllable(lpCntxtPriv,byCaret-1);
							CIsSvrProxy::GetSvrCore()->ReqSpellGetBlur(siPrev.szSpell,siPrev.bySpellLen);
							pData=CIsSvrProxy::GetSvrCore()->GetAck();
							lpCntxtPriv->pbyBlur[byCaret-1]=(LPBYTE)malloc(pData->sSize);
							memcpy(lpCntxtPriv->pbyBlur[byCaret-1],pData->byData,pData->sSize);
							lpCntxtPriv->spellData[byCaret-1]=siPrev;
							lpCntxtPriv->spellData[byCaret]=siCur;
						}
					}
					if(CIsSvrProxy::GetSvrCore()->ReqSpellGetBlur(pSpInfo->szSpell,pSpInfo->bySpellLen)==ISACK_SUCCESS)
					{
						pData=CIsSvrProxy::GetSvrCore()->GetAck();
						lpCntxtPriv->pbyBlur[byCaret]=(LPBYTE)malloc(pData->sSize);
						memcpy(lpCntxtPriv->pbyBlur[byCaret],
							pData->byData,
							pData->sSize);
					}else
					{//清空临时音节
						pSpInfo->bySpellLen=0;
					}
				}
			}
		}
		bRet=TRUE;
	}else if(byInput==VK_BACK)
	{
		if(pSpInfo->bySpellLen)
		{
			pSpInfo->bySpellLen--;
			free(lpCntxtPriv->pbyBlur[byCaret]);
			lpCntxtPriv->pbyBlur[byCaret]=NULL;
			lpCntxtPriv->bySelect[byCaret]=0;
			memset(lpCntxtPriv->szWord+byCaret,0x20,2);
			if(pSpInfo->bySpellLen)
			{
				PMSGDATA pData;
				CIsSvrProxy::GetSvrCore()->ReqSpellGetBlur(pSpInfo->szSpell,pSpInfo->bySpellLen);
				pData=CIsSvrProxy::GetSvrCore()->GetAck();
				lpCntxtPriv->pbyBlur[byCaret]=(LPBYTE)malloc(pData->sSize);
				memcpy(lpCntxtPriv->pbyBlur[byCaret],
					pData->byData,
					pData->sSize);
			}else if(byCaret==lpCntxtPriv->bySyllables-1 && lpCntxtPriv->bySyllables>1)
			{//删除最后一个音节的唯一字母
				lpCntxtPriv->bySyllables--;
				lpCntxtPriv->byCaret--;
				pSpInfo--;
				byCaret--;
			}
			bRet=TRUE;
		}else if(lpCntxtPriv->bySyllables>1)//至少保证要有一个音节空间
		{//清除当前的空白音节
			SpellBuf_Move(lpCntxtPriv,
				lpCntxtPriv->byCaret+1,
				lpCntxtPriv->bySyllables-(lpCntxtPriv->byCaret+1),
				-1);
			SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->bySyllables-1);
			lpCntxtPriv->bySyllables--;
			if(lpCntxtPriv->byCaret==lpCntxtPriv->bySyllables)
			{
				lpCntxtPriv->byCaret--;
				pSpInfo--;
				byCaret--;
			}
			bRet=TRUE;
		}else if(g_SettingsG->compMode!=IM_SPELL)
		{//temp spell mode will ignore VK_BACK key.
			bRet=TRUE;
		}
		if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
		{
			InputEnd();
			if(!IsTempSpell())
				InputHide(TRUE);
		}
	}else if(byInput==VK_ESCAPE)
	{
		InputEnd();
		InputHide(TRUE);
		ClearContext(CPC_ALL);
		if(IsTempSpell())
		{//restore shape code input mode
			m_ctx.compMode=IM_SHAPECODE;
			StatusbarUpdate();
		}	
		bRet=TRUE;
	}

	if(bRet )
	{
		if(lpCntxtPriv->bySyllables>1) 
			m_ctx.bShowTip=FALSE;
		ClearContext(CPC_CAND);//清除当前重码
		if(pSpInfo->bySpellLen)
		{
			//重新预测本句的候选句
			KeyIn_Spell_Forecast(lpCntxtPriv,0);
			//获取当前插入符位置的重码
			KeyIn_Spell_UpdateCandList(lpCntxtPriv,byCaret);
		}
	}
	return bRet;
}

BOOL CInputState::KeyIn_Spell_GetSpellInput(InputContext * lpCntxtPriv,BYTE bySpellID[MAX_SYLLABLES][2])
{
	BOOL bRet=TRUE;
	BYTE i,j,k,iWord;
	PMSGDATA pData=NULL;

	for(i=0,iWord=0;i<lpCntxtPriv->bySyllables;i++)
	{
		BOOL  bFind=FALSE;
		if(lpCntxtPriv->spellData[i].bySpellLen==0) continue;
		//找到当前用户选择的汉字的拼音，通过多音字与模糊音两者匹配获得
		CIsSvrProxy::GetSvrCore()->ReqQueryWordSpell(lpCntxtPriv->szWord[i]);//获得汉字的所有拼音的ID
		pData=CIsSvrProxy::GetSvrCore()->GetAck();

		for(j=0;j<pData->byData[0];j++)
		{
			short sBlur=0;
			BYTE  *pbyBlur=lpCntxtPriv->pbyBlur[i];
			memcpy(&sBlur,pbyBlur,2);//模糊音个数
			pbyBlur+=4;//模糊音ID,跨过数量及完全匹配索引号
			for(k=0;k<sBlur;k++)
			{
				if(memcmp(pData->byData+1+2*j,pbyBlur+2*k,2)==0)
				{//得到真正的拼音ID
					memcpy(bySpellID[iWord],pbyBlur+2*k,2);
					bFind=TRUE;
				}
			}
			if(bFind) break;
		}
		if(!bFind) bRet=FALSE;
		lpCntxtPriv->szComp[iWord]=lpCntxtPriv->szWord[i];
		iWord++;
	}
	lpCntxtPriv->cComp=iWord;
	return bRet;
}


//拼音状态下将结果输入到编辑器中
BOOL CInputState::KeyIn_Spell_InputText(InputContext* lpCntxtPriv,UINT byInput,
										CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	if(lpCntxtPriv->bySyllables<=1 && lpCntxtPriv->spellData[0].bySpellLen==0) return FALSE;

	if(byInput==VK_SPACE)
	{
		BYTE bySpellID[MAX_SYLLABLES][2];//用户输入的汉字的拼音的真正ID
		SStringW strResult;
		BOOL bGetSpID=KeyIn_Spell_GetSpellInput(lpCntxtPriv,bySpellID);
		if(lpCntxtPriv->cComp)
		{
			strResult = SStringW(lpCntxtPriv->szComp,lpCntxtPriv->cComp);
		}
		ClearContext(CPC_ALL);
		InputResult(strResult,GetKeyinMask(!IsTempSpell(),MKI_RECORD|MKI_TTSINPUT));
		InputEnd();
		InputHide(TRUE);

		if (IsTempSpell())
		{//临时拼音模式自动获得输入拼音的编码
			lpCntxtPriv->bShowTip = TRUE;
			lpCntxtPriv->iTip = -1;
			GetShapeComp(strResult, strResult.GetLength());
			lpCntxtPriv->compMode = IM_SHAPECODE;
			StatusbarUpdate();
		}

		//将用户输入提交给服务器保存
		if(bGetSpID) CIsSvrProxy::GetSvrCore()->ReqSpellMemEx(strResult,strResult.GetLength(),bySpellID);
		bRet=TRUE;
	}else if ( byInput == VK_RETURN && g_SettingsG->compMode == IM_SPELL)
	{//回车输入编码

		SStringW strResult;
		BYTE i;
		for(i=0;i<lpCntxtPriv->bySyllables;i++)
		{
			if(lpCntxtPriv->spellData[i].bySpellLen)
			{
				strResult += SStringW(lpCntxtPriv->spellData[i].szSpell,lpCntxtPriv->spellData[i].bySpellLen);
			}
		}
		//通知应用程序接收数据
		InputResult(strResult,GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT));
		InputEnd();
		InputHide(FALSE);
		ClearContext(CPC_ALL);
		bRet=TRUE;
	}
	return bRet;
}

//通过大写输入来定位音节
BOOL CInputState::KeyIn_Spell_Locate(InputContext *lpCntxtPriv,UINT byInput,
									 CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	if(byInput>='A' && byInput<='Z' && lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen!=0)
	{//大写输入,非笔画输入状态，非空码状态
		BYTE byCaret=lpCntxtPriv->byCaret+1;
		byInput+=0x20;
		while(byCaret<lpCntxtPriv->byCaret+lpCntxtPriv->bySyllables)
		{
			if(lpCntxtPriv->spellData[byCaret%lpCntxtPriv->bySyllables].szSpell[0]==byInput)
			{
				lpCntxtPriv->byCaret=byCaret%lpCntxtPriv->bySyllables;
				KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
				bRet=TRUE;
				break;
			}
			byCaret++;
		}
	}
	return bRet;
}

//输入符号
BOOL CInputState::KeyIn_Spell_Symbol(InputContext* lpCntxtPriv,UINT byInput,
									 CONST BYTE* lpbKeyState)
{
	BOOL bRet=FALSE;
	if(!IsTempSpell() && (lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen!=0 || lpCntxtPriv->bySyllables==1))
	{//输入符号
		BYTE bySpellID[MAX_SYLLABLES][2];
		BOOL bGetSpID=KeyIn_Spell_GetSpellInput(lpCntxtPriv,bySpellID);
		SStringW strResult;
		if(lpCntxtPriv->cComp)
		{
			if(bGetSpID) CIsSvrProxy::GetSvrCore()->ReqSpellMemEx(lpCntxtPriv->szComp,lpCntxtPriv->cComp,bySpellID);
			strResult = SStringW(lpCntxtPriv->szComp,lpCntxtPriv->cComp);
		}
		strResult += Symbol_Convert(&m_ctx,byInput,lpbKeyState);
		InputStart();
		InputResult(strResult,GetKeyinMask(FALSE,MKI_RECORD));
		InputEnd();
		InputHide(FALSE);
		ClearContext(CPC_ALL);
		bRet=TRUE;
	}
	return bRet;
}

BOOL CInputState::KeyIn_Spell_Normal(InputContext * lpCntxtPriv,UINT byInput,
									 CONST BYTE * lpbKeyState)
{
	BOOL bHandle=KeyIn_Spell_SyFix(lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_ChangeComp(lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_InputText(	lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_Locate(lpCntxtPriv,byInput,lpbKeyState);
	if(!bHandle) bHandle=KeyIn_Spell_Symbol(lpCntxtPriv,byInput,lpbKeyState);
	return bHandle;
}


//输入并且联想
BOOL CInputState::KeyIn_InputAndAssociate(InputContext * lpCntxtPriv,LPCWSTR pszInput,short sLen,BYTE byMask)
{
	lpCntxtPriv->sbState=SBST_ASSOCIATE;
	lpCntxtPriv->sCandCount=0;
	CIsSvrProxy::GetSvrCore()->ReqKeyIn(m_pListener->GetHwnd(),pszInput,sLen,byMask);
	return TRUE;
}

//选择一个重码
//BYTE byInput: Virtual Key
BOOL CInputState::KeyIn_All_SelectCand(InputContext * lpCntxtPriv,UINT byInput,char cCompLen,
									   CONST BYTE * lpbKeyState,bool bKeepVisible)
{
	BOOL bRet=FALSE;

	short iCand = -1;
	if(byInput>='0' && byInput<='9')
	{
		iCand = m_pListener->SelectCandidate((byInput-'0'+9)%10);		
	}
	if(iCand != -1)
	{//有效的重码
		BYTE byMask=GetKeyinMask(cCompLen==0,MKI_ALL);
		LPBYTE pCandInfo=lpCntxtPriv->ppbyCandInfo[iCand];//rate + gbk flag+ len + phrase + len + comp
		bRet=TRUE;
		SStringW strResult;
		if(lpCntxtPriv->inState==INST_CODING)
		{//普通编码输入
			lpCntxtPriv->bShowTip=FALSE;
			if(lpCntxtPriv->sbState==SBST_NORMALSTATE)
			{//编码输入状态
				if(lpCntxtPriv->compMode==IM_SPELL)
				{//拼音输入状态
					BYTE iWord=0;
					BYTE byPhraseLen=pCandInfo[2];
					BYTE byCaret=lpCntxtPriv->byCaret;
					if(byCaret==lpCntxtPriv->bySyllables-1)
					{
						if(byPhraseLen>lpCntxtPriv->bySyllables)
							byCaret=0;//不完整联想重码
						else
							byCaret-=byPhraseLen-1;
					}
					wcsncpy(lpCntxtPriv->szWord+byCaret,(WCHAR*)(pCandInfo+3),byPhraseLen);
					for(BYTE i=0;i<byPhraseLen;i++)
					{
						lpCntxtPriv->bySelect[i+byCaret]=byPhraseLen;
					}

					if(byPhraseLen>lpCntxtPriv->bySyllables)
					{//不完整输入长词重码
						SStringW strResult((WCHAR*)(pCandInfo+3),pCandInfo[2]);
						BOOL isTempSpell = IsTempSpell();

						ClearContext(CPC_ALL);
						InputResult(strResult,GetKeyinMask(!isTempSpell,MKI_ALL));
						InputEnd();

						if(isTempSpell) 
						{//临时拼音模式获得输入字的编码
							lpCntxtPriv->bShowTip=TRUE;
							lpCntxtPriv->iTip = -1;
							GetShapeComp(strResult,strResult.GetLength());
						}
						if(isTempSpell)
						{//退出临时拼音
							lpCntxtPriv->compMode=IM_SHAPECODE;
							InputUpdate();
						}
					}
					else
					{
						KeyIn_Spell_Forecast(lpCntxtPriv,lpCntxtPriv->byCaret);
						if(byCaret+byPhraseLen<lpCntxtPriv->bySyllables)
						{
							lpCntxtPriv->byCaret+=byPhraseLen;
							KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
							InputUpdate();
						}else
						{
							KeyIn_Spell_InputText(lpCntxtPriv,VK_SPACE,lpbKeyState);
						}
					}
					goto end; //拼音处理比较特殊
				}else
				{//非拼音
					if( (lpbKeyState[VK_CONTROL]&0x80) && (lpbKeyState[VK_SHIFT]&0x80) )
					{//Ctrl+Shift+数字键:在线删词,在形码状态支持
						if(lpCntxtPriv->sCandCount>1)
						{//只在有多个重码的时候才有效
							BYTE* pbyPhrase=pCandInfo+2;//rate+gbk flag.
							BYTE* pbyComp = pbyPhrase+1+pbyPhrase[0]*2;
							char cPhraseLen=pbyPhrase[0];
							char cCompLen=pbyComp[0];
							WCHAR * pPhrase =(WCHAR*)(pbyPhrase+1);
							WCHAR * pComp = (WCHAR*)(pbyComp+1);
							if(cCompLen==0)
								pComp=lpCntxtPriv->szComp,cCompLen=lpCntxtPriv->cComp;
							if(cPhraseLen>1 || !g_SettingsG->bDisableDelWordCand)
							{
								if(CIsSvrProxy::GetSvrCore()->ReqPhraseDel(m_pListener->GetHwnd(),pComp,cCompLen,pPhrase,cPhraseLen)==ISACK_SUCCESS)
								{//删除重码成功,更新显示
									memcpy(lpCntxtPriv->ppbyCandInfo+iCand,lpCntxtPriv->ppbyCandInfo+iCand+1,(lpCntxtPriv->sCandCount-iCand-1)*sizeof(LPBYTE));
									lpCntxtPriv->sCandCount--;
									InputUpdate();
									goto end;
								}else
								{
									CUtils::SoundPlay(_T("error"));
								}
							}else
							{
								CUtils::SoundPlay(_T("error"));
							}
						}
						CUtils::SoundPlay(_T("error"));
						goto end;
					}else if(pCandInfo[0]==RATE_USERCMD)
					{//命令直通车数据
						CUtils::CmdExecute(pCandInfo);
						byMask=0;
					}else
					{//普通的选择重码
						LPBYTE pCand=pCandInfo+2;//rate+gbkFlag
						LPBYTE pComp=pCand+1+pCand[0]*2;
						if(pCandInfo[0]==RATE_USERDEF)
						{//自定义编码
							byMask=0;
						}
						strResult = SStringW((WCHAR*)(pCand+1),pCand[0]);
						if(pCandInfo[0]!=RATE_FORECAST)
						{//不是预测词，词频调整
							SStringW strComp = SStringW((WCHAR*)(pComp+1),pComp[0]);
							if(lpbKeyState[VK_CONTROL] & 0x80)
							{
								CIsSvrProxy::GetSvrCore()->ReqRateAdjust(m_pListener->GetHwnd(),strComp,strComp.GetLength(),strResult,strResult.GetLength(),RAM_FAST);
							}else if(g_SettingsG->byRateAdjust) 
							{
								CIsSvrProxy::GetSvrCore()->ReqRateAdjust(m_pListener->GetHwnd(),strComp,strComp.GetLength(),strResult,strResult.GetLength(),g_SettingsG->byRateAdjust==1?RAM_AUTO:RAM_FAST);
							}
						}else
						{//预测词，自动造词
							CIsSvrProxy::GetSvrCore()->ReqMakePhrase(strResult,strResult.GetLength());
						}
					}
				}
			}else //if(lpCntxtPriv->sbState==SBST_ASSOCIATE || lpCntxtPriv->sbState==SBST_SENTENCE)
			{
				if(g_SettingsG->byAstMode==AST_CAND)
				{//当前是词组联想
					InputStart();
					strResult= SStringW((WCHAR*)(pCandInfo+3)+pCandInfo[0],pCandInfo[2]-pCandInfo[0]);
				}else if(g_SettingsG->byAstMode==AST_ENGLISH)
				{//当前是英文联想
					InputStart();
					strResult =SStringW((WCHAR*)(pCandInfo+1),pCandInfo[0]);
					cCompLen=0;//不进行继续联想
				}
			}			
		}else if(lpCntxtPriv->inState==INST_ENGLISH)
		{//英文单词输入
			strResult = SStringW((WCHAR*)(pCandInfo+1),pCandInfo[0]);
			byMask&=~MKI_ASTENGLISH;
		}else if(lpCntxtPriv->inState==INST_USERDEF)
		{//用户自定义输入
			if(pCandInfo[0]==RATE_USERCMD)
			{//命令直通车数据
				CUtils::CmdExecute(pCandInfo);
				byMask=0;
			}else
			{//一般的自定义
				LPBYTE pCand = pCandInfo + 2;
				LPBYTE pComp= pCand +1+pCand[0]*2;
				if(pComp[0]!=0) 
					strResult = SStringW((WCHAR*)(pComp + 1), pComp[0]);
				else
					strResult = SStringW((WCHAR*)(pCand + 1), pCand[0]);
				byMask=GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT);//不联想
			}
		}else if(lpCntxtPriv->inState==INST_LINEIME)
		{//笔画输入状态
			strResult=SStringW((WCHAR*)(pCandInfo+3),pCandInfo[2]);
		}
		ClearContext(CPC_ALL);
		InputResult(strResult,byMask);
		InputEnd();

		InputUpdate();
		if (!bKeepVisible)
		{
			InputHide(byMask &(MKI_ASTENGLISH | MKI_ASTCAND | MKI_ASTSENT | MKI_PHRASEREMIND) || lpCntxtPriv->bShowTip);
		}
	}
end:
	return bRet;
}


//重码翻页处理
//BYTE byInput:Vitual Key
BOOL CInputState::KeyIn_All_TurnCandPage(InputContext * lpCntxtPriv,UINT byInput,
										 CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	if(lpCntxtPriv->sCandCount )
	{
		//联想状态及单词输入状态只能使用上下箭头翻页,以避免与符号输入冲突
		if(byInput==VK_DOWN || byInput ==VK_PRIOR 
			|| (!(lpbKeyState[VK_SHIFT]&0x80) && byInput==g_SettingsG->byTurnPageDownVK && lpCntxtPriv->sbState!=SBST_ASSOCIATE && lpCntxtPriv->inState != INST_ENGLISH))
		{
			bRet = m_pListener->GoNextCandidatePage();
		}else if(byInput==VK_UP || byInput == VK_NEXT
			|| (!(lpbKeyState[VK_SHIFT]&0x80) && byInput==g_SettingsG->byTurnPageUpVK&& lpCntxtPriv->sbState!=SBST_ASSOCIATE && lpCntxtPriv->inState != INST_ENGLISH))
		{
			bRet = m_pListener->GoPrevCandidatePage();
		}
	}
	if(bRet){
		if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_ASSOCIATE)
		{//联想状态的翻页,开启延时定时器
			InputHide(TRUE);
		}
		if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->compMode==IM_SPELL)
		{//拼音状态自动更新当前字符
			short iCand = m_pListener->SelectCandidate(0);
			if(iCand!=-1)
			{//将翻页结果更新到预测，等同于手动选字
				LPBYTE pCand=lpCntxtPriv->ppbyCandInfo[iCand];
				BYTE byPhraseLen = pCand[2];
				BYTE byCaret= lpCntxtPriv->byCaret;
				wcsncpy(lpCntxtPriv->szWord+byCaret,(WCHAR*)(pCand+3),byPhraseLen);
				for(BYTE i=0;i<byPhraseLen;i++)
				{
					lpCntxtPriv->bySelect[i+byCaret]=byPhraseLen;
				}
				KeyIn_Spell_Forecast(lpCntxtPriv,lpCntxtPriv->byCaret);
			}
		}
	}

	return bRet;
}

BOOL CInputState::IsTempSpell() const
{
	if(m_ctx.compMode != IM_SPELL) return FALSE;
	if(g_SettingsG->compMode == IM_SPELL) return FALSE;
	return TRUE;
}


BOOL CInputState::KeyIn_Code_Normal(InputContext * lpCntxtPriv,UINT byInput,
									CONST BYTE * lpbKeyState)
{
	BOOL bRet=KeyIn_Code_ChangeComp(lpCntxtPriv,byInput,lpbKeyState);
	if(!bRet) bRet=KeyIn_Code_Symbol(lpCntxtPriv,byInput,lpbKeyState);
	return bRet;
}

//普通编码状态下编码改变处理
BOOL CInputState::KeyIn_Code_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
										CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	BOOL bWild = byInput==CDataCenter::getSingletonPtr()->GetData().m_compInfo.cWild;
	if(bWild)
	{
		bCompChar=TRUE;
	}else
	{
		bCompChar = CDataCenter::getSingletonPtr()->GetData().m_compInfo.IsCompChar(byInput);
	}
	if(bCompChar)
	{
		if(lpCntxtPriv->sbState==SBST_ASSOCIATE) 
		{
			ClearContext(CPC_ALL);
		}

		if(KeyIn_Code_IsMaxCode2(lpCntxtPriv)
			&& !KeyIn_Code_IsValidComp(lpCntxtPriv,byInput)
			&& g_SettingsG->bAutoInput)
		{
			if(lpCntxtPriv->sCandCount)
			{
				//防止符号输入时出现错误:标点不能做首编码,退出当前过程,进入标点顶字上屏过程
				if((byInput<'a' || byInput>'z') && !CIsSvrProxy::GetSvrCore()->GetCompHead()->bSymbolFirst)
					return FALSE;
				BYTE *pByCand = lpCntxtPriv->ppbyCandInfo[0];
				if(pByCand[0]!=RATE_FORECAST && (pByCand[0]!=RATE_GBK || g_SettingsG->nGbkMode!=CSettingsGlobal::GBK_SHOW_MANUAL))
				{
					KeyIn_All_SelectCand(lpCntxtPriv,'1',1,lpbKeyState,true);
				}
			}
			lpCntxtPriv->cComp=0;
		}

		if(lpCntxtPriv->cComp==0)
		{
			if(byInput<'a' || byInput>'z')
			{//标点：要么不是自定义模式快捷键，或者不支持快捷自定义模式切换
				if(!CIsSvrProxy::GetSvrCore()->GetCompHead()->bSymbolFirst)
					return FALSE;//符号顶字上屏
			}
			if(bWild && g_SettingsG->bDisableFirstWild)//禁止首码万能键
				return FALSE;
			if(g_SettingsG->bShowOpTip)
			{//有编码后面显示操作提示
				lpCntxtPriv->bShowTip=TRUE;
				lpCntxtPriv->iTip = Tips_Rand(FALSE,lpCntxtPriv->szTip);
			}
			//开始编码输入,生成开始编码消息以获取光标跟随时输入窗口的坐标
			InputStart();
			InputOpen();
		}
		if(lpCntxtPriv->cComp<MAX_COMP)
		{
			lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput;
			bRet=TRUE;
		}
	}else if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->cComp>0) lpCntxtPriv->cComp--;
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE)
	{
		lpCntxtPriv->cComp=0;
		lpCntxtPriv->sbState=SBST_NORMALSTATE;
		lpCntxtPriv->bShowTip=FALSE;
		bRet=TRUE;
	}else if(byInput==VK_RETURN)
	{
		if(g_SettingsG->bEnterClear)
		{//清空编码
			lpCntxtPriv->cComp=0;
			lpCntxtPriv->sbState=SBST_NORMALSTATE;
			bRet=TRUE;
		}else if(lpCntxtPriv->cComp>0)
		{//
			SStringW strResult(lpCntxtPriv->szComp,lpCntxtPriv->cComp);
			ClearContext(CPC_ALL);
			InputResult(strResult,GetKeyinMask(FALSE,MKI_ALL));
			InputEnd();
			InputHide(TRUE);
			bRet = TRUE;
		}
	}else if(lpCntxtPriv->cComp < MAX_COMP)
	{
		lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput;
		if((!g_SettingsG->bAutoInput && byInput>='a' && byInput<='z')) 
			bRet=TRUE;
	}

	if(bRet)
	{
		LPBYTE lpbyCand=NULL;
		BYTE byMask=MQC_NORMAL|g_SettingsG->byForecast;
		if(g_SettingsG->bAutoMatch) byMask|=MQC_MATCH;
		if(g_SettingsG->bBlendUD) byMask|=MQC_USERDEF;
		if(g_SettingsG->bBlendSpell) byMask|=MQC_SPCAND;
		if(g_SettingsG->bAutoPrompt) byMask|=MQC_AUTOPROMPT;
		if(g_SettingsG->bOnlySimpleCode) byMask|=MQC_ONLYSC;

		ClearContext(CPC_CAND);

		if(lpCntxtPriv->cComp==0)
		{
			InputEnd();
			InputHide(TRUE);
		}else
		{
			lpCntxtPriv->sbState=SBST_NORMALSTATE;	//退出联想状态
			if(CIsSvrProxy::GetSvrCore()->ReqQueryCand(m_pListener->GetHwnd(),lpCntxtPriv->szComp,lpCntxtPriv->cComp,byMask)==ISACK_SUCCESS)
			{
				PMSGDATA pMsgData=CIsSvrProxy::GetSvrCore()->GetAck();
				LPBYTE pbyData,pCandData;
				short i,sCount,sSingleWords=0;
				memcpy(m_pbyMsgBuf,pMsgData->byData,pMsgData->sSize);
				lpCntxtPriv->byCandType = m_pbyMsgBuf[0];
				pbyData=m_pbyMsgBuf+1;
				memcpy(&sCount,pbyData,2);
				pbyData+=2;
				lpCntxtPriv->ppbyCandInfo=(LPBYTE *)malloc(sizeof(LPBYTE)*sCount);
				lpCntxtPriv->sCandCount=0;
				pCandData=pbyData;
				//先找出单字数量
				for(i=0;i<sCount;i++)
				{
					//format: rate+bGbk+candLen+cand+compLen+comp
					if(pCandData[2]==1) sSingleWords++;
					pCandData+=pCandData[2]*2+3;	//偏移词组信息
					pCandData+=pCandData[0]*2+1;	//偏移编码信息					
				}
				pCandData=pbyData;
				for(i=0;i<sCount;i++)
				{
					if (pCandData[1]!=0)
					{
						if (!g_SettingsUI->bFilterGbk && (g_SettingsG->nGbkMode !=  CSettingsGlobal::GBK_HIDE ||  sSingleWords<2))
						{//GBK显示或者不是GBK重码
							lpCntxtPriv->ppbyCandInfo[lpCntxtPriv->sCandCount++] = pCandData;
						}
					}
					else
					{
						lpCntxtPriv->ppbyCandInfo[lpCntxtPriv->sCandCount++] = pCandData;
					}
					pCandData+=pCandData[2]*2+3;	//偏移词组信息
					pCandData+=pCandData[0]*2+1;	//偏移编码信息
				}
			}
			InputUpdate();
		}
		if(lpCntxtPriv->sCandCount)
		{
			lpbyCand=lpCntxtPriv->ppbyCandInfo[0];
		}
		if((lpCntxtPriv->byCandType&MCR_AUTOSELECT ||(KeyIn_Code_IsMaxCode(lpCntxtPriv) && g_SettingsG->bAutoInput)))
		{
			BYTE *pByCand = lpCntxtPriv->ppbyCandInfo[0];
			if(lpCntxtPriv->sCandCount==1 && pByCand[0]!=RATE_FORECAST && (pByCand[0]!=RATE_GBK || g_SettingsG->nGbkMode!=CSettingsGlobal::GBK_SHOW_MANUAL))
			{
				KeyIn_All_SelectCand(lpCntxtPriv,'1',0,lpbKeyState);
			}else
			{
				if(lpCntxtPriv->sCandCount>1)
					CUtils::SoundPlay(_T("ChongMa"));
				else if(lpCntxtPriv->sCandCount==0)
					CUtils::SoundPlay(_T("KongMa"));
				else
					CUtils::SoundPlay(_T("LianXiang"));
			}
		}
	}
	return bRet;
}

BOOL CInputState::KeyIn_Code_Symbol(InputContext * lpCntxtPriv,UINT byInput,
									CONST BYTE * lpbKeyState)
{
	SStringW strResult;
	if(lpCntxtPriv->sCandCount && 
		lpCntxtPriv->inState==INST_CODING && 
		lpCntxtPriv->sbState==SBST_NORMALSTATE)
	{
		short iCand = m_pListener->SelectCandidate(0);
		if(iCand!=-1)
		{
			LPBYTE pbyCand=lpCntxtPriv->ppbyCandInfo[iCand];
			strResult = SStringW((WCHAR*)(pbyCand+3),pbyCand[2]);
		}
	}

	strResult += Symbol_Convert(&m_ctx,byInput,lpbKeyState);

	ClearContext(CPC_ALL);
	InputStart();
	InputResult(strResult,GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT));
	InputEnd();
	InputHide(FALSE);
	return TRUE;
}

//联想状态下的输入处理,进行状态改变处理
BOOL CInputState::KeyIn_All_Associate(InputContext * lpCntxtPriv,UINT byInput,
									  CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	SASSERT(lpCntxtPriv->sbState==SBST_ASSOCIATE);

	if((lpCntxtPriv->compMode==IM_SHAPECODE && byInput==g_SettingsG->bySentMode) ||(lpCntxtPriv->compMode==IM_SPELL && byInput==';'))
	{
		if(lpCntxtPriv->sSentLen)
		{//切换到语句输入状态
			lpCntxtPriv->sbState=SBST_SENTENCE;
			ClearContext(CPC_CAND);
			lpCntxtPriv->sSentCaret=0;
			InputStart();
			InputOpen();
			if(g_SettingsG->bShowOpTip)
			{
				m_ctx.bShowTip=TRUE;
				m_ctx.iTip = -1;
				_tcscpy(m_ctx.szTip,_T("逗号逐字选择,空格上屏;句号全部上屏"));
			}

			bRet=TRUE;
		}
	}

	if(!bRet)
	{
		if(lpCntxtPriv->compMode==IM_SPELL)
			bRet=KeyIn_Spell_Normal(lpCntxtPriv,byInput,lpbKeyState);
		else
			bRet=KeyIn_Code_Normal(lpCntxtPriv,byInput,lpbKeyState);
	}
	return bRet;
}

//语句输入状态下的输入处理
BOOL CInputState::KeyIn_All_Sentence(InputContext * lpCntxtPriv,UINT byInput,
									 CONST BYTE * lpbKeyState)
{
	_ASSERT(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_SENTENCE);
	if(byInput==',')
	{
		if(lpCntxtPriv->sSentCaret<lpCntxtPriv->sSentLen)
			lpCntxtPriv->sSentCaret++;
	}else if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->sSentCaret>0)
			lpCntxtPriv->sSentCaret--;
	}else if((lpCntxtPriv->compMode==IM_SHAPECODE && byInput==g_SettingsG->bySentMode) ||(lpCntxtPriv->compMode==IM_SPELL && byInput==';'))
	{
		if(lpCntxtPriv->sSentCaret==0) return FALSE;//返回FALSE,以便进入符号输入过程
	}else if(byInput=='.')
	{
		lpCntxtPriv->sSentCaret=lpCntxtPriv->sSentLen;
		KeyIn_Sent_Input(lpCntxtPriv);
	}else if(byInput==VK_SPACE)
	{
		if(lpCntxtPriv->sSentCaret==0) lpCntxtPriv->sSentCaret=lpCntxtPriv->sSentLen;
		KeyIn_Sent_Input(lpCntxtPriv);
	}else if(byInput==VK_ESCAPE)
	{
		ClearContext(CPC_ALL);
		InputEnd();
		InputHide(FALSE);
	}

	if(byInput>='0' && byInput<='9')
		return FALSE;//交给后续的重码选择接口继续执行
	return TRUE;
}

void  CInputState::KeyIn_Sent_Input(InputContext* lpCntxtPriv)
{
	if(lpCntxtPriv->sSentLen && lpCntxtPriv->sSentCaret)
	{
		BYTE byMask=0;
		if(g_SettingsUI->bRecord)
			byMask|=MKI_RECORD;
		if(g_SettingsUI->bSound)
			byMask|=MKI_TTSINPUT;

		SStringW strResult(lpCntxtPriv->szSentText,lpCntxtPriv->sSentCaret);

		ClearContext(CPC_ALL);
		InputStart();
		InputResult(strResult,byMask);
		InputEnd();
		InputHide(FALSE);
	}
}


//英文单词状态的输入处理
BOOL CInputState::KeyIn_Code_English(InputContext * lpCntxtPriv,UINT byInput,
									 CONST BYTE* lpbKeyState)
{
	SASSERT(lpCntxtPriv->inState==INST_ENGLISH);
	if(byInput==VK_RETURN)
	{//输入当前英文
		SStringW strResult = SStringW(lpCntxtPriv->szComp, lpCntxtPriv->cComp);
		InputResult(strResult,GetKeyinMask(FALSE,MKI_TTSINPUT));
		InputEnd();
		InputHide(FALSE);
		ClearContext(CPC_ALL);
	}else if(byInput==VK_ESCAPE)
	{//清除输入
		InputEnd();
		InputHide(TRUE);
		ClearContext(CPC_ALL);
	}else
	{
		if(byInput==VK_BACK)
		{
			if(lpCntxtPriv->cComp) 
			{
				lpCntxtPriv->cComp--;
				if(lpCntxtPriv->cComp==0)
				{
					InputEnd();
					InputHide(FALSE);
					lpCntxtPriv->inState=INST_CODING;
				}
			}
		}else
		{
			lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput;
		}
		ClearContext(CPC_CAND);
		if(lpCntxtPriv->cComp)
		{//编码已经修改
			if(CIsSvrProxy::GetSvrCore()->ReqQueryCandEn(lpCntxtPriv->szComp,lpCntxtPriv->cComp)==ISACK_SUCCESS)
			{
				PMSGDATA pData=CIsSvrProxy::GetSvrCore()->GetAck();
				LPBYTE  pBuf=m_pbyMsgBuf;
				BYTE i,byCount=0;
				memcpy(m_pbyMsgBuf,pData->byData,pData->sSize);
				lpCntxtPriv->pbyEnSpell=pBuf;
				pBuf+=1+pBuf[0]*2;
				lpCntxtPriv->pbyEnPhontic=pBuf;
				pBuf+=1+pBuf[0]*2;
				byCount=*pBuf++;
				lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(byCount*sizeof(LPBYTE));
				for(i=0;i<byCount;i++)
				{
					lpCntxtPriv->ppbyCandInfo[i]=pBuf;
					pBuf+=1+pBuf[0]*2;
				}
				lpCntxtPriv->sCandCount=byCount;
			}else
			{
				lpCntxtPriv->pbyEnSpell=NULL;
				lpCntxtPriv->pbyEnPhontic=NULL;
				lpCntxtPriv->pbyEnAstPhrase=NULL;
			}
		}
	}
	return TRUE;
}

//数字输入
BOOL CInputState::KeyIn_Digital_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
										   CONST BYTE* lpbKeyState)
{
	BOOL bRet=FALSE;
	if((byInput>='0' && byInput<='9') || ((byInput=='.' ||byInput==',') && g_SettingsG->bAutoDot) )
	{
		SStringW strResult((WCHAR)byInput);
		InputStart();
		InputResult(strResult,GetKeyinMask(FALSE,MKI_TTSINPUT));
		InputEnd();
		bRet=TRUE;
	}else
	{
		lpCntxtPriv->inState=INST_CODING;
		if(lpCntxtPriv->compMode==IM_SPELL)
			bRet=KeyIn_Spell_Normal(lpCntxtPriv,byInput,lpbKeyState);
		else
			bRet=KeyIn_Code_Normal(lpCntxtPriv,byInput,lpbKeyState);
	}
	return bRet;
}

//用户自定义输入
BOOL CInputState::KeyIn_UserDef_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
										   CONST BYTE* lpbKeyState)
{
	if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->cComp>0)	lpCntxtPriv->cComp--;
	}else if(byInput==VK_ESCAPE)
	{
		ClearContext(CPC_ALL);
		InputEnd();
		InputHide(FALSE);
	}else if(byInput==VK_RETURN)
	{
		if(lpCntxtPriv->cComp)
		{//输入编码
			//切换回正常状态
			SStringW strResult(lpCntxtPriv->szComp,lpCntxtPriv->cComp);
			InputResult(strResult,GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT));
			InputEnd();
			InputHide(FALSE);
			ClearContext(CPC_ALL);
			return TRUE;
		}
	}else
	{
		lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput;
		lpCntxtPriv->szComp[lpCntxtPriv->cComp]=0;
	}
	ClearContext(CPC_CAND);
	if(lpCntxtPriv->cComp && ISACK_SUCCESS==CIsSvrProxy::GetSvrCore()->ReqQueryCandUD(lpCntxtPriv->szComp,lpCntxtPriv->cComp))
	{//获取自定义词组,支持多个重码
		PMSGDATA pMsgData=CIsSvrProxy::GetSvrCore()->GetAck();
		LPBYTE pbyData;
		short i;
		memcpy(m_pbyMsgBuf,pMsgData->byData,pMsgData->sSize);
		pbyData=m_pbyMsgBuf;
		//save auto complete composition string
		lpCntxtPriv->cCompACLen = pbyData[0];
		wcsncpy(lpCntxtPriv->szCompAutoComplete, (WCHAR*)(pbyData + 1), pbyData[0]);
		pbyData+=pbyData[0]*2+1;
		memcpy(&lpCntxtPriv->sCandCount,pbyData,2);
		pbyData+=2;
		lpCntxtPriv->ppbyCandInfo=(LPBYTE *)malloc(sizeof(LPBYTE)*lpCntxtPriv->sCandCount);
		for(i=0;i<lpCntxtPriv->sCandCount;i++)
		{
			lpCntxtPriv->ppbyCandInfo[i]=pbyData;
			pbyData+=pbyData[2]*2+3;//重码
			pbyData+=pbyData[0]*2+1;//编码
		}
	}
	InputUpdate();
	return TRUE;
}

//笔画输入状态
BOOL CInputState::KeyIn_Line_ChangeComp(InputContext * lpCntxtPriv,UINT byInput,
										CONST BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	if(byInput==VK_BACK)
	{//回退
		if(lpCntxtPriv->cComp>0)
		{
			lpCntxtPriv->cComp--;
			bCompChar=TRUE;
		}
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE||byInput==VK_RETURN)
	{//退出状态
		ClearContext(CPC_ALL);
		InputUpdate();
		bRet=TRUE;
	}else if(byInput>=VK_NUMPAD1 && byInput<=VK_NUMPAD6)
	{//编辑
		lpCntxtPriv->szComp[lpCntxtPriv->cComp++]=byInput-VK_NUMPAD1+'1';
		lpCntxtPriv->szComp[lpCntxtPriv->cComp]=0;
		bCompChar=TRUE;
		bRet=TRUE;
	}
	if(bCompChar)
	{
		ClearContext(CPC_CAND);
		if(lpCntxtPriv->cComp && CIsSvrProxy::GetSvrCore()->ReqQueryCandLine(lpCntxtPriv->szComp,lpCntxtPriv->cComp)==ISACK_SUCCESS)
		{//查询重码
			PMSGDATA pMsgData=CIsSvrProxy::GetSvrCore()->GetAck();
			short i,sCount=0;
			LPBYTE pBuf=m_pbyMsgBuf;
			memcpy(m_pbyMsgBuf,pMsgData->byData,pMsgData->sSize);
			memcpy(&sCount,pBuf,2);
			pBuf+=2;
			lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(sCount*sizeof(LPBYTE));
			for(i=0;i<sCount;i++)
			{
				lpCntxtPriv->ppbyCandInfo[i]=pBuf;
				pBuf+=pBuf[2]+3;
			}
			lpCntxtPriv->sCandCount=sCount;
		}
	}
	return bRet;
}


void CInputState::TurnToTempSpell()
{
	if(m_ctx.compMode==IM_SPELL)
	{//拼音输入状态
		if(IsTempSpell())
		{//退出临时拼音状态
			m_ctx.compMode = IM_SHAPECODE;
			InputHide(FALSE);
			InputEnd();
			StatusbarUpdate();
			ClearContext(CPC_ALL);
		}
	}else if(g_SettingsG->compMode==IM_SHAPECODE && m_ctx.inState==INST_CODING)
	{//五笔输入状态，进入临时拼音状态
		ClearContext(CPC_ALL);
		m_ctx.compMode=IM_SPELL;
		m_ctx.bShowTip=TRUE;
		m_ctx.iTip = -1;
		_tcscpy(m_ctx.szTip,_T("临时拼音:上屏后自动提示编码"));
		InputOpen();
		InputUpdate();
		StatusbarUpdate();
		SLOG_INFO("");
		if (!m_pListener->IsCompositing())
		{//query cursor position
			InputStart();
			InputEnd();
		}
	} 
}

BOOL CInputState::TestKeyDown(UINT uKey,LPARAM lKeyData,const BYTE * lpbKeyState)
{
	BOOL bRet=FALSE;
	BOOL bKeyDown = !(lKeyData & 0x80000000);
	if (!bKeyDown && (uKey != VK_SHIFT && uKey !=VK_CONTROL))
		return FALSE;
	if(uKey==VK_SPACE 
		&& !g_SettingsG->bFullSpace
		&& m_ctx.inState==INST_CODING 
		&& m_ctx.bySyllables==0 && m_ctx.cComp==0
		&& m_ctx.sbState!=SBST_SENTENCE && m_ctx.sCandCount==0)
	{
		return FALSE;
	}
	BOOL bOpen = m_pListener->IsInputEnable();
	if (!bOpen)
	{
		if (uKey == VK_SHIFT)
		{//handle shift while input is disabled.
			if (bKeyDown)
			{
				m_bPressShift = TRUE;
				m_bPressOther = FALSE;
			}
			if (!bKeyDown && m_bPressShift)
			{
				m_bPressShift = FALSE;
				BYTE byKey = (BYTE)(lKeyData >> 16);
				if (!m_bPressOther && !m_bPressCtrl && g_SettingsG->bySwitchKey == byKey)
				{//激活输入
					m_pListener->EnableInput(TRUE);
					if (KeyIn_IsCoding(&m_ctx))
					{
						InputOpen();
					}
					return TRUE;
				}
			}
			return FALSE;
		}
		else
		{
			m_bPressOther = TRUE;
			return FALSE;
		}
	}

	//检查快捷键
	int iHotKey = TestHotKey(uKey, lpbKeyState);
	if (iHotKey != -1)
	{
		BOOL bRet = TRUE;
		if ( m_pListener)
		{
			switch (iHotKey)
			{
			case HKI_LoadDebugSkin:
				{
					SStringT debugSkin=g_SettingsG->strDebugSkinPath;
					m_pListener->OnCommand(CMD_CHANGESKIN, (LPARAM)&debugSkin);
				}
				break;
			case HKI_UnloadDebugSkin:
				{
					SStringT curSkin=g_SettingsG->strSkin;
					m_pListener->OnCommand(CMD_CHANGESKIN, (LPARAM)&curSkin);
				}
				break;
			case HKI_CharMode:
				m_pListener->OnCommand(CMD_HOTKEY_CHARMODE, 0);
				break;
			case HKI_MakePhrase:
				m_pListener->OnCommand(CMD_HOTKEY_MAKEPHRASE, 0);
				break;
			case HKI_EnSwitch:
				m_pListener->OnCommand(CMD_HOTKEY_ENGLISHMODE, 0);
				break;
			case HKI_Mode:
				m_pListener->OnCommand(CMD_HOTKEY_INPUTMODE, 0);
				break;
			case HKI_ShowRoot:
				m_pListener->OnCommand(CMD_HOTKEY_KEYMAP, 0);
				break;
			case HKI_HideStatus:
				m_pListener->OnCommand(CMD_HOTKEY_HIDESTATUSBAR, 0);
				break;
			case HKI_Query:
				m_pListener->OnCommand(CMD_HOTKEY_QUERYINFO, 0);
				break;
			case HKI_FilterGbk:
				m_pListener->OnCommand(CMD_HOTKEY_FILTERGBK, 0);
				break;
			case HKI_TTS:
				m_pListener->OnCommand(CMD_HOTKEY_TTS, 0);
				break;
			case HKI_Record:
				m_pListener->OnCommand(CMD_HOTKEY_RECORD, 0);
				break;
			case HKI_TempSpell:
				TurnToTempSpell();
				break;
			case HKI_Repeat:
				bRet = KeyIn_Test_RepeatInput(&m_ctx,lpbKeyState);
				break;
			}
		}
		m_bPressOther = TRUE;
		return bRet;
	}

	if(uKey==VK_CAPITAL)
	{
		if(lpbKeyState[VK_CAPITAL]&0x01)
		{
			ClearContext(CPC_ALL);
			InputEnd();
			InputHide(FALSE);
		}
		return FALSE;
	}else if(uKey==VK_SHIFT)
	{
		if(m_bPressCtrl)
		{
			m_bPressOther=TRUE;
			return FALSE;
		}
		if(!bKeyDown)//弹起SHIFT键
		{
			if(!m_bPressOther && m_bPressShift)//右SHIFT键按下后没有按下其它键，表示使用快捷关闭功能
			{
				BYTE byKey=(BYTE)(lKeyData>>16);
				m_bPressShift=FALSE; 
				if(!(lpbKeyState[VK_SPACE]&0x80) &&
					g_SettingsG->bySwitchKey==byKey)
				{ //check the scan code
					//关闭输入，如果当前有输入内容,则将当前的输入内容输出到编辑器中
					if (m_ctx.cComp != 0)
					{
						SStringW result(m_ctx.szComp, m_ctx.cComp);
						InputResult(result, GetKeyinMask(FALSE,MKI_TTSINPUT));
						ClearContext(CPC_ALL);
					}
					InputEnd();
					m_pListener->EnableInput(FALSE);
				}
			}else//还原状态
			{
				m_bPressShift=FALSE;
				m_bPressOther=FALSE;
			}	
		}else
		{//按下SHIFT键，初始化状态
			m_bPressShift=TRUE;
			m_bPressOther=FALSE;
		}
	}else if(uKey==VK_CONTROL)
	{
		if(!m_pListener->GetOpenStatus())
			return FALSE;
		if(m_bPressShift)
		{
			m_bPressOther=TRUE;
			return FALSE;
		}
		if(!bKeyDown)//弹起Ctrl键
		{
			BYTE byKey=(BYTE)(lKeyData>>24);
			if(!m_bPressOther && m_bPressCtrl)//Ctrl键按下后没有按下其它键，表示使用快捷关闭功能
			{
				if(g_SettingsG->byTempSpellKey==byKey)
				{//临时拼音
					TurnToTempSpell();
				}else if(!g_SettingsG->bDisableFnKey && ((g_SettingsG->byTempSpellKey==0 && byKey==0xC1)||g_SettingsG->byTempSpellKey!=0))
				{//功能键, 0xC1=右Ctrl
					if(!KeyIn_IsCoding(&m_ctx)) 
					{
						if(m_ctx.inState==INST_CODING && m_ctx.sbState==SBST_ASSOCIATE
							&& ((m_ctx.compMode==IM_SHAPECODE &&  m_ctx.sSentLen) || (m_ctx.compMode==IM_SPELL && m_ctx.bySyllables==1 && m_ctx.spellData[0].bySpellLen==0) )
							)
						{//进入语句联想状态
							m_ctx.sbState=SBST_SENTENCE;
							ClearContext(CPC_CAND);//clear candidate list
							m_ctx.sSentCaret=0;
							InputStart();
							InputOpen();
							if(g_SettingsG->bShowOpTip)
							{
								m_ctx.bShowTip=TRUE;
								m_ctx.iTip = -1;
								_tcscpy(m_ctx.szTip,_T("语句输入模式"));
							}

							bRet=TRUE;
						}else
						{//进入用户自定义输入状态
							ClearContext(CPC_ALL);
							m_ctx.inState=INST_USERDEF;
							InputStart();
							bRet=TRUE;
						}
					}else
					{
						if(m_ctx.inState==INST_CODING && m_ctx.sbState==SBST_SENTENCE
							&& ((m_ctx.compMode==IM_SHAPECODE &&  m_ctx.sSentLen) || (m_ctx.compMode==IM_SPELL && m_ctx.bySyllables==1 && m_ctx.spellData[0].bySpellLen==0) )
							)
						{//语句联想状态
							m_ctx.sbState=SBST_ASSOCIATE;
							InputEnd();
							bRet=TRUE;
						}else if(m_ctx.inState==INST_USERDEF)
						{//状态还原
							m_ctx.inState=INST_CODING;
							InputEnd();
							bRet=TRUE;
						}
					}
				}
				m_bPressCtrl=FALSE; 
			}else//还原状态
			{
				m_bPressCtrl=FALSE;
				m_bPressOther=FALSE;
			}		
		}else
		{//按下Ctrl键，初始化状态
			m_bPressCtrl=TRUE;
			m_bPressOther=FALSE;
		}
	}else if(!(lpbKeyState[VK_CAPITAL]&0x01)) 
	{
		m_bPressOther=TRUE;
		if(m_pListener->GetOpenStatus())
		{
			if(lpbKeyState[VK_CONTROL]&0x80 && lpbKeyState[VK_SHIFT]&0x80)
			{//Ctrl + Shift
				bRet=(uKey>='0' && uKey<='9');
				return bRet;
			}else if(!(lpbKeyState[VK_CONTROL]&0x80 || lpbKeyState[VK_MENU]&0x80))
			{
				BOOL bCoding=KeyIn_IsCoding(&m_ctx);
				if(uKey>='A' && uKey<='Z')
				{//字母输入
					if(m_ctx.compMode==IM_SPELL)
					{
						bRet=TRUE;
					}else 
					{
						char cKey=uKey+0x20;	//将VK转换成字符
						bRet = CDataCenter::getSingleton().GetData().m_compInfo.IsCompChar(cKey);
					}
				}else if(uKey>=VK_NUMPAD0 && uKey<=VK_NUMPAD9)
				{//小键盘数字输入
					bRet=KeyIn_IsNumCode(&m_ctx) || m_ctx.sCandCount;
				}else if(uKey>=VK_MULTIPLY && uKey<=VK_DIVIDE)
				{//小键盘公式输入
					bRet=KeyIn_IsNumCode(&m_ctx);
				}else
				{
					bRet=KCompKey[uKey];
					if(!bRet && m_ctx.sCandCount)
					{//检查是否是用户定义的各种重码相关快捷键
						bRet= (uKey==g_SettingsG->by2CandVK 
							|| uKey==g_SettingsG->by3CandVK 
							|| uKey==g_SettingsG->byTurnPageDownVK 
							|| uKey==g_SettingsG->byTurnPageUpVK
							);
					}
					if(!bRet && m_ctx.bySyCaret!=0xFF)
						bRet= uKey==VK_DELETE;
					//!编码相关的切换键必须是可打印字符,因此不需要判断
				}
				if(!bCoding) 
				{//不是编码输入过程
					if(uKey==VK_BACK || uKey==VK_RETURN|| uKey==VK_ESCAPE 
						|| uKey==VK_LEFT || uKey==VK_RIGHT 
						|| uKey==VK_UP || uKey==VK_DOWN //重码翻页键，不需要翻页的时候
						|| uKey==VK_HOME || uKey==VK_END )//编辑器移动光标，结束当前句
					{
						//编辑器移动光标
						bRet=FALSE;
						ClearContext(CPC_ALL);
						InputHide(FALSE);
						if(uKey==VK_BACK) 
							CIsSvrProxy::GetSvrCore()->ReqKeyIn(m_pListener->GetHwnd(),L"\b",1,0);
						else
							CIsSvrProxy::GetSvrCore()->ReqKeyIn(m_pListener->GetHwnd(),L".",1,g_SettingsUI->bRecord?MKI_RECORD:0);
					}
				}
				if(!bRet && !bCoding)
					InputHide(FALSE);
			}
		}
	}

	return bRet;
}

void CInputState::OnImeSelect(BOOL bSelect)
{
	SLOG_INFO("fOpen:" << bSelect);
	m_fOpen = bSelect;
}


BOOL CInputState::OnSvrNotify(UINT wp, PMSGDATA pMsg)
{
	SLOG_INFO("code="<<wp<<",m_fOpen:"<<m_fOpen);
	if(wp == NT_KEYIN)
	{//输入时返回的联想数据
		if(m_fOpen)
		{
			InputContext * ctx = &m_ctx;
			if(INST_CODING== ctx->inState && SBST_ASSOCIATE==ctx->sbState)
			{//保证当前状态是等待联想数据状态
				ClearContext(CPC_CAND);
				if(pMsg->sSize)
				{
					LPBYTE pbyData=pMsg->byData;
					if(pbyData[0]==MKI_ASTCAND)
					{//诩组联想
						short iCand=0,sCount=0;
						pbyData++;
						memcpy(&sCount,pbyData,2);
						pbyData+=2;
						ctx->ppbyCandInfo=(LPBYTE*)malloc(sCount*sizeof(LPBYTE));
						for(iCand=0;iCand<sCount;iCand++)
						{//枚举所有联想词组：词头长度(1BYTE)+rate+length+wsWord[length]
							ctx->ppbyCandInfo[iCand]=pbyData;
							pbyData+=pbyData[2]*2+3;
						}
						ctx->sCandCount=sCount;
						SLOG_INFO("词组联想:"<<ctx->sCandCount);
					}
					if(pbyData-pMsg->byData<pMsg->sSize && pbyData[0]==MKI_ASTENGLISH)
					{//英文联想
						BYTE i;
						pbyData+=2;//MKI_ASTENGLISH + match length.
						ctx->pbyEnAstPhrase=pbyData;
						BYTE cLen = pbyData[0];
						pbyData+=1+cLen*2;
						ctx->sCandCount=*pbyData++;
						ctx->ppbyCandInfo=(LPBYTE*)malloc(ctx->sCandCount*sizeof(LPBYTE));
						for(i=0;i<ctx->sCandCount;i++)
						{
							ctx->ppbyCandInfo[i]=pbyData;
							pbyData+=pbyData[0]*2+1;
							pbyData+=pbyData[0]*2+1;						
						}
						SLOG_INFO("英文联想:"<<ctx->sCandCount);
					}
					if(pbyData-pMsg->byData<pMsg->sSize && pbyData[0]==MKI_PHRASEREMIND)
					{//已有词组提示
						ctx->bShowTip=TRUE;
						ctx->iTip = -1;
						_tcscpy(ctx->szTip,_T("系统词组:"));
						wcsncpy(ctx->szTip+5,(WCHAR*)(pbyData+2),pbyData[1]);
						ctx->szTip[5+pbyData[1]]=0;
						pbyData+=2+pbyData[1];
						SLOG_INFO("已有词组提示:"<<ctx->szTip);
					}
					if(pbyData-pMsg->byData<pMsg->sSize && pbyData[0]==MKI_ASTSENT)
					{//句子联想
						short sLen=0,iWord=0,sOffset=0;
						pbyData++;
						memcpy(&sLen,pbyData,2);
						SASSERT(sLen<=MAX_SENTLEN);
						pbyData+=2;
						wcsncpy(ctx->szSentText,(WCHAR*)pbyData,sLen);
						ctx->sSentCaret=0;
						ctx->sSentLen=sLen;
						SLOG_INFO("句子联想:"<<SStringW(ctx->szSentText,sLen));
					}
				}
				if(ctx->bShowTip || ctx->sCandCount || ctx->sSentLen)
				{//有联想词组或有联想句子
					SLOG_INFO("Update Input Window");
					if (ctx->sCandCount == 0 && g_SettingsG->bShowOpTip && !ctx->bShowTip)
					{//没有候选时,在侯选位置显示操作提示
						ctx->bShowTip=TRUE;
						if(ctx->sSentLen )
							wcscpy(ctx->szTip,L"分号进入语句输入"),ctx->iTip = -1;
						else
							ctx->iTip = Tips_Rand(ctx->compMode == IM_SPELL, ctx->szTip);
					}
					InputUpdate();
				}else
				{//关闭窗口
					SLOG_INFO("Close Input Window");
					InputHide();
				}

			}else
			{
				SLOGFMTI("IME state is not waitting for notify,inState=%d,sbState=%d",ctx->inState,ctx->sbState);
			}
		}else
		{
			SLOG_INFO("Close Input Window");
			InputHide();
		}
	}
	return FALSE;
}
