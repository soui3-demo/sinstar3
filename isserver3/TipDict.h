#pragma once

typedef
int (*FunTipDict)(LPCWSTR pszKey,int nKeyLen,LPWSTR pszBuf,int nBufLen);

class CTipDict : public SSingleton<CTipDict>
{
public:
	CTipDict(SStringT strPath);
	~CTipDict(void);

	int TipDict(LPCWSTR pszKey,int nKeyLen,LPWSTR pszBuf,int nBufLen);

protected:
	HMODULE		m_hPlugin;
	FunTipDict	m_funTipDict;
};
