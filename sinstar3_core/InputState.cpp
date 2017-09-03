#include "StdAfx.h"
#include "InputState.h"

CInputState::CInputState(void)
{
	m_ctx.inState = INST_CODING;
	m_ctx.sbState = ::SBST_NORMAL;
	m_ctx.compMode  = g_SettingsG.compMode;
}

CInputState::~CInputState(void)
{
}

BOOL CInputState::HandleKeyDown(UINT vKey,UINT uScanCode)
{
	if(isprint(vKey))
	{
		vKey = tolower(vKey);
	}
	if(m_ctx.inState == INST_CODING)
	{
		if(isprint(vKey))
		{
			if(m_ctx.strInput.GetLength() == 0 && vKey == CDataCenter::GetAutoLockerInstance()->GetData().m_compInfo.cWild)
			{
				m_ctx.inState = INST_USERDEF;
			}else
			{
				m_ctx.strInput.Append((WCHAR)vKey);
			}
		}else if(vKey == VK_BACK)
		{
			m_ctx.strInput = m_ctx.strInput.Left(m_ctx.strInput.GetLength()-1);
		}else if(vKey == VK_ESCAPE)
		{
			m_ctx.strInput.Empty();
		}
	}else if(m_ctx.inState == INST_USERDEF)
	{
		if(isprint(vKey))
		{
			m_ctx.strInput.Append((WCHAR)vKey);
		}else if(vKey == VK_BACK)
		{
			m_ctx.strInput = m_ctx.strInput.Left(m_ctx.strInput.GetLength()-1);
		}else if(vKey == VK_ESCAPE)
		{
			m_ctx.strInput.Empty();
			m_ctx.inState = INST_CODING;
		}
	}

	return TRUE;
}
