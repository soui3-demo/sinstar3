#include "StdAfx.h"
#include "InputState.h"

CInputState::CInputState(void):m_pListener(NULL),m_bCoding(FALSE)
{
	memset(&m_ctx,0,sizeof(InputContext));
	ClearContext(CPC_ALL);
	m_ctx.compMode  = g_SettingsG.compMode;
	m_ctx.bShowTip = FALSE;
	m_pbyMsgBuf=(LPBYTE)malloc(MAX_BUF_ACK);
}

CInputState::~CInputState(void)
{
	free(m_pbyMsgBuf);
}


void CInputState::ClearContext(UINT dwMask)
{
	if(dwMask&CPC_COMP)
	{
		m_ctx.szInput[0]=0;
		m_ctx.cInput = 0;
	}
	if(dwMask&CPC_ENGLISH)
	{
		m_ctx.pbyEnSpell=NULL;
		m_ctx.pbyEnPhontic=NULL;
	}
	if(dwMask&CPC_STATE)
	{
		m_ctx.inState=INST_CODING;
		m_ctx.sbState=::SBST_NORMAL;
		m_ctx.bWebMode=FALSE;
	}
	if(dwMask&CPC_CAND)
	{
		if(m_ctx.ppbyCandInfo)
		{
			free(m_ctx.ppbyCandInfo);
			m_ctx.ppbyCandInfo=NULL;
		}
		m_ctx.sCandCount=0;
		m_ctx.pbyEnAstPhrase=NULL;
	}
	if(dwMask&CPC_SENTENCE)
	{
		m_ctx.sSentCaret=m_ctx.sSentLen=0;
		m_ctx.sSentWords=0;
		memset(m_ctx.pbySentWord,0,sizeof(LPBYTE)*MAX_SENTLEN);
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
		m_ctx.bPYBiHua=FALSE;
	}
}


void CInputState::OnInputStart()
{
	if(m_bCoding) return;
	m_bCoding = TRUE;
	SASSERT(m_pListener);
	m_pListener->OnInputStart();
}

void CInputState::OnInputEnd(const SStringT &strResult)
{
	SASSERT(m_pListener);
	m_pListener->OnInputEnd(strResult);
	ClearContext(CPC_ALL);
	m_bCoding = FALSE;
}

BOOL CInputState::HandleKeyDown(UINT vKey,UINT uScanCode)
{
	if(isprint(vKey))
	{
		vKey = tolower(vKey);
	}

	if(UpdateInputMode(vKey,uScanCode))
		return TRUE;

	if(HandleShapeCodeKeyDown(vKey,uScanCode))
		return TRUE;

	return FALSE;
}

BOOL CInputState::UpdateInputMode(UINT vKey,UINT uScanCode)
{
	if(m_ctx.inState == INST_CODING 
		&& m_ctx.cInput == 0 
		&& vKey == CDataCenter::GetAutoLockerInstance()->GetData().m_compInfo.cWild)
	{
		m_ctx.inState = INST_USERDEF;
		OnInputStart();
		return TRUE;
	}
	if(m_ctx.inState == INST_USERDEF
		&& vKey == VK_ESCAPE)
	{
		ClearContext(CPC_ALL);
		m_ctx.inState = INST_CODING;
		OnInputEnd(SStringT());
		return TRUE;
	}
	if(m_ctx.compMode == IM_SHAPECODE
		&& vKey == VK_CONTROL
		&& m_ctx.cInput == 0)
	{
		m_ctx.compMode = IM_SPELL;
		return TRUE;
	}
	if(m_ctx.compMode == IM_SPELL
		&& g_SettingsG.compMode == IM_SHAPECODE
		&& vKey == VK_ESCAPE)
	{
		ClearContext(CPC_SPELL|CPC_COMP|CPC_CAND);
		m_ctx.compMode = IM_SHAPECODE;
		return TRUE;
	}
	if(g_SettingsG.compMode == IM_SPELL && !m_bCoding)
	{
		if(vKey == 'u' || vKey == 'v')
		{
			m_ctx.inState = INST_USERDEF;
			OnInputStart();
			return TRUE;
		}
		if(vKey == 'i')
		{
			m_ctx.inState = INST_LINEIME;
			m_pListener->OnInputStart();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CInputState::HandleShapeCodeKeyDown(UINT vKey,UINT uScanCode)
{

	if(isprint(vKey))
	{
		if(!CDataCenter::GetAutoLockerInstance()->GetData().m_compInfo.IsCompChar(vKey))
		{
			return FALSE;
		}
		OnInputStart();
		m_ctx.szInput[m_ctx.cInput++]=((TCHAR)vKey);
	}else if(vKey == VK_BACK)
	{
		m_ctx.cInput -- ;
	}else if(vKey == VK_ESCAPE)
	{
		m_ctx.cInput = 0;
		OnInputEnd(_T(""));
		return TRUE;
	}

	BYTE byMask=MQC_NORMAL|g_SettingsG.byForecast;
	if(g_SettingsG.bAutoMatch) byMask|=MQC_MATCH;
	if(g_SettingsG.bBlendUD) byMask|=MQC_USERDEF;
	if(g_SettingsG.bBlendSpWord) byMask|=MQC_SPCAND;
	if(g_SettingsG.bAutoPrompt) byMask|=MQC_AUTOPROMPT;
	if(g_SettingsG.bOnlySimpleCode) byMask|=MQC_ONLYSC;

	SStringA strCompA = S_CT2A(SStringT(m_ctx.szInput,m_ctx.cInput));
	if(ISACK_SUCCESS == ISComm_QueryCand(strCompA,strCompA.GetLength(),byMask,m_pListener->GetHwnd()))
	{
		PMSGDATA pMsgData=ISComm_GetData();
		LPBYTE pbyData,pCandData;
		short i,sCount,sSingleWords=0;
		memcpy(m_pbyMsgBuf,pMsgData->byData,pMsgData->sSize);
		pbyData=m_pbyMsgBuf+1;
		memcpy(&sCount,pbyData,2);
		pbyData+=2;
		m_ctx.ppbyCandInfo=(LPBYTE *)malloc(sizeof(LPBYTE)*sCount);
		m_ctx.sCandCount=0;
		pCandData=pbyData;
		//先找出单字数量
		for(i=0;i<sCount;i++)
		{
			if(pCandData[1]==2) sSingleWords++;
			pCandData+=pCandData[1]+2;	//偏移词组信息
			pCandData+=pCandData[0]+1;	//偏移编码信息					
		}
		pCandData=pbyData;
		for(i=0;i<sCount;i++)
		{
			if(g_SettingsG.nGbkMode!=0 || pCandData[0]!=RATE_GBK || sSingleWords<2)
			{//GBK显示或者不是GBK重码
				m_ctx.ppbyCandInfo[m_ctx.sCandCount++]=pCandData;
			}
			pCandData+=pCandData[1]+2;	//偏移词组信息
			pCandData+=pCandData[0]+1;	//偏移编码信息
		}
	}
	return TRUE;
}



