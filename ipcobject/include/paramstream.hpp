#pragma once
#include "ShareMemBuffer.h"

class CParamStream
{
public:
	CParamStream(CShareMemBuffer *pBuf, bool bOutStream) :m_pBuffer(pBuf)
	{
		m_pBuffer->StartRead();
		if (bOutStream) m_pBuffer->StartWrite();
	}

	CShareMemBuffer * GetBuffer() {
		return m_pBuffer;
	}

	template<typename T>
	CParamStream & operator<<(const T & data)
	{
		m_pBuffer->Write((const BYTE*)&data, sizeof(data));
		return *this;
	}


	template<typename T>
	CParamStream & operator >> (T &data)
	{
		m_pBuffer->Read((BYTE*)&data, sizeof(data));
		return *this;
	}
protected:
	CShareMemBuffer * m_pBuffer;
};
