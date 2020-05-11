#include "StdAfx.h"
#include "TxtReader.h"
#include <fstream>

CTxtReader::CTxtReader(WCHAR cRemarkFlag):m_remarkFlag(cRemarkFlag),m_length(0)
{
}

CTxtReader::~CTxtReader(void)
{
}

BOOL CTxtReader::Open(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f)
		return FALSE;
	BYTE BOM[3]={0};
	fread(BOM,1,3,f);
	fseek(f,0,SEEK_END);
	long len = ftell(f);

	if(BOM[0]==0xff && BOM[1]==0xfe)
	{//utf16
		fseek(f,2,SEEK_SET);
		len -=2;
		len /= sizeof(WCHAR);
		WCHAR *pBuf = new WCHAR[len+1];
		fread(pBuf,sizeof(WCHAR),len,f);
		pBuf[len]=0;
		m_data<<pBuf;
		delete []pBuf;
		m_length = len;
	}else if(BOM[0]==0xef && BOM[1]==0xbb && BOM[2]==0xbf )
	{//utf8
		fseek(f,3,SEEK_SET);
		len -=3;
		char *pSour=new char[len];
		fread(pSour,1,len,f);
		int wlen = MultiByteToWideChar(CP_UTF8,0,pSour,len,NULL,0);

		WCHAR *pBuf = new WCHAR[wlen+1];
		MultiByteToWideChar(CP_UTF8,0,pSour,len,pBuf,wlen);
		pBuf[wlen]=0;
		m_data<<pBuf;
		delete []pBuf;
		delete []pSour;
		m_length = wlen;
	}else
	{//gb2312
		fseek(f,0,SEEK_SET);
		char *pSour=new char[len];
		fread(pSour,1,len,f);
		int wlen = MultiByteToWideChar(CP_ACP,0,pSour,len,NULL,0);

		WCHAR *pBuf = new WCHAR[wlen+1];
		MultiByteToWideChar(CP_ACP,0,pSour,len,pBuf,wlen);
		pBuf[wlen]=0;
		m_data<<pBuf;
		delete []pBuf;
		delete []pSour;
		m_length = wlen;
	}
	fclose(f);
	return TRUE;
}

void CTxtReader::Close()
{
	m_data.clear();
	m_length = 0;
}

BOOL CTxtReader::getline(WCHAR *pBuf,int nLineMax)
{
	m_data.getline(pBuf,nLineMax);
	BOOL bRet = !(m_data.rdstate()&m_data.failbit);
	if(bRet)
	{
		int nReaded = m_data.gcount();
		if(nReaded>2 && pBuf[nReaded-2]==0x0d)
			pBuf[nReaded-2]=0;
	}
	if(bRet && m_remarkFlag!=0 && pBuf[0]==m_remarkFlag)
		return getline(pBuf,nLineMax);
	else
		return bRet;
}

long CTxtReader::getLength() const
{
	return m_length;
}

long CTxtReader::getReadPos() const
{
	return m_data.tellg();
}

BOOL CTxtReader::Save(LPCTSTR pszFileName)
{
	if(m_length == 0)
		return FALSE;
	std::fstream f(pszFileName,std::ios::out|std::ios::binary);
	if(!f.good())
		return FALSE;
	f<<"\xff\xfe";
	f.write((const char*)m_data.str().c_str(),m_data.str().size()*2);
	return TRUE;
}

