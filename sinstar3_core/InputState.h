#pragma once

#include "inputContext.h"

class CInputState
{
public:
	CInputState(void);
	~CInputState(void);

	BOOL HandleKeyDown(UINT vKey,UINT uScanCode);
	const InputContext * GetInputContext() const {return &m_ctx;}
private:
	InputContext m_ctx;
};
