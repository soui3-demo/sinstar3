#pragma once

#include "../iscomm/commfile.h"
#include <string>

template<typename T>
int CF_WriteT(COMFILE &cf, T data)
{
	return CF_Write(&cf, &data, sizeof(T));
}

template<>
int CF_WriteT(COMFILE &cf, char data)
{
	return CF_Write(&cf, &data, sizeof(char));
}

template<>
int CF_WriteT(COMFILE &cf, unsigned char data)
{
	return CF_Write(&cf, &data, sizeof(unsigned char));
}

template<>
int CF_WriteT(COMFILE &cf, short data)
{
	return CF_Write(&cf, &data, sizeof(short));
}

template<>
int CF_WriteT(COMFILE &cf, int data)
{
	return CF_Write(&cf, &data, sizeof(int));
}


template<typename T>
int CF_ReadT(COMFILE &cf,T * pData)
{
	return CF_Read(&cf, pData, sizeof(T));
}

template<>
int CF_ReadT(COMFILE &cf, char * pData)
{
	return CF_Read(&cf, pData, sizeof(pData[0]));
}

template<>
int CF_ReadT(COMFILE &cf, short * pData)
{
	return CF_Read(&cf, pData, sizeof(pData[0]));
}

template<>
int CF_ReadT(COMFILE &cf, int * pData)
{
	return CF_Read(&cf, pData, sizeof(pData[0]));
}

template<>
int CF_ReadT(COMFILE &cf, unsigned char * pData)
{
	return CF_Read(&cf, pData, sizeof(pData[0]));
}

inline int CF_WriteString(COMFILE & cf, const char * pszBuf, int nLen = -1)
{
	if (nLen < 0) nLen = strlen(pszBuf);
	int nRet = CF_WriteT(cf, nLen);
	nRet += CF_Write(&cf, pszBuf, nLen);
	return nRet;
}

inline int CF_ReadString(COMFILE &cf, std::string & str)
{
	int nLen;
	int nRet = CF_ReadT(cf, &nLen);
	char *pBuf = (char*)malloc(nLen + 1);
	nRet += CF_Read(&cf, pBuf, nLen);
	pBuf[nLen] = 0;
	str = pBuf;
	free(pBuf);
	return nRet;
}

