// SpellEngine.cpp: implementation of the CSpellEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpellEngine.h"
#include "TxtReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int WordPhoneCmp(WORDPHONE *spWord1,WORDPHONE *spWord2,LPARAM lParam)
{
	return (int)spWord1->wWord - (int)spWord2->wWord;
}

int TextCmp(WString *pt1, WString *pt2, LPARAM lParam)
{
	if(pt1->cLen!=pt2->cLen) return pt1->cLen-pt2->cLen;
	return wcsncmp(pt1->szBuf,pt2->szBuf,pt1->cLen);
}

CSpellEngine::CSpellEngine()
:m_arrWord(500),m_arrContext(500)
{

}

CSpellEngine::~CSpellEngine()
{
	for(int i=0;i<m_arrWord.GetSize();i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		for(int j=0;j<pwp->cpp;j++)
		{
			if(pwp->pi[j].pArrCntxt) delete pwp->pi[j].pArrCntxt;
		}
	}
}

#ifdef SPELLENGINE_EDITOR
//导入拼音－＞汉字映射表
//表格式为文本行	如　huang\t黄还...
//可以有注释行 如 #注释
int CSpellEngine::ImportSpell2Word(LPCTSTR pszFileName)
{
	WCHAR szLine[1000];
	CTxtReader reader('#');
	if(!reader.Open(pszFileName)) return 0;
	int nRet=0;
	BYTE byID[2];
	BOOL bRead=reader.getline(szLine,1000);
	while(bRead)
	{
		WCHAR *pszCand=wcsstr(szLine,L"\t");
		ASSERT(pszCand);
		if(g_SpellData.Spell2ID(szLine,pszCand-szLine,byID))
		{
			short sIndex=g_SpellData.GetIDIndex(byID);
			pszCand++;
			m_spellMap.SetSpellCands(sIndex,pszCand,wcslen(pszCand));
			nRet++;
		}
		bRead=reader.getline(szLine,1000);
	}
	reader.Close();
	return nRet;
}


//导入汉字->拼音信息
//格式： 说 shuo #yue;shui;
//#后为多音信息
//在#开头为注释行
int CSpellEngine::ImportWord2Spell(LPCTSTR pszFileName)
{
	WCHAR szLine[100];
	CTxtReader reader('#');
	if(!reader.Open(pszFileName)) return 0;
	int nRet=0;
	WCHAR szDefSpell[7];
	WCHAR szOtherSpell[80];
	WORDPHONE word;
	BOOL bRead=reader.getline(szLine,100);
	while(bRead)
	{
		ASSERT(szLine[1]==0x20);
		memset(&word,0,sizeof(word));
		word.wWord = szLine[0];
		szDefSpell[0]=szOtherSpell[0]=0;
		swscanf(szLine+2,L"%s #%s",szDefSpell,szOtherSpell);
		g_SpellData.Spell2ID(szDefSpell,-1,word.byPhone);
		//将默认读音也计入多音字的一个音
		memcpy(word.pi[0].byPhone,word.byPhone,2);
		word.cpp=1;

		LPWSTR pszBegin=szOtherSpell;
		LPWSTR pszEnd=wcsstr(pszBegin,L";");
		while(pszEnd)
		{
			g_SpellData.Spell2ID(pszBegin,pszEnd-pszBegin,word.pi[word.cpp++].byPhone);
			pszBegin=pszEnd+1;
			pszEnd=wcsstr(pszBegin,L";");
		}

		int nInsert=m_arrWord.SortInsert(word,FALSE,WordPhoneCmp);
		ASSERT(nInsert!=-1);
		nRet++;
		bRead = reader.getline(szLine,100);
	}
	reader.Close();
	return nRet;	
}

//导入字频文件
//格式： 国 225
//字频不超过250
BOOL CSpellEngine::ImportWordRate(LPCTSTR pszFileName)
{
	CSArray<WORDRATE> arrWordRate(500);
	WCHAR szLine[100];
	CTxtReader reader('#');
	if(!reader.Open(pszFileName)) return FALSE;
	WORDRATE wr;
	BOOL bRead=reader.getline(szLine,100);
	while(bRead)
	{
		ASSERT(szLine[1]=0x20);
		wr.wWord = szLine[0];
		wr.byRate=(BYTE)_wtoi(szLine+2);
		if(wr.byRate>MAX_RATE) wr.byRate=MAX_RATE;
		arrWordRate.SortInsert(wr,FALSE,WordRateCmp);
		bRead=reader.getline(szLine,100);
	}
	reader.Close();
	m_spellMap.InitRate(&arrWordRate);
	return TRUE;
}
#endif// SPELLENGINE_EDITOR

//将一个包含多音字拼音的词组分解为可以使用的词组
//如: 音乐yue
void CSpellEngine::MakeSpellPhrase(LPCWSTR pszLine,SPELLPHRASE *pPhrase)
{
	int c=0;
	int i=0;
	char cpps=0;
	while(pszLine[c])
	{
		if(CWordID::IsHanzi(pszLine[c]))
		{//汉字
			pPhrase->szBuf[i++]=pszLine[c++];
		}else
		{//发现一个多音字的读音
			char cBegin=c;
			while(pszLine[c] && CWordID::IsAlphabit(pszLine[c])) c++;
			ASSERT(c-cBegin<7);
			pPhrase->cPPPos[cpps]=i-1;
			CUtils::CopyCompFormWStr2AStr(pPhrase->cPPSp[cpps],pszLine+cBegin,c-cBegin);
			cpps++;
		}
	}
	pPhrase->szBuf[i]=0;
	pPhrase->cLen=i;
	pPhrase->cPPSize=cpps;
}

int CSpellEngine::Context_Export(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("w"));
	if(!f)
	{
		return -1;
	}
	fwrite("\xff\xfe",1,2,f);//write utf16 bom

	int nRet=0;
	int i,j;
	for( i=0;i<m_arrCntxtRemark.GetSize();i++)
	{
		fwrite(L"@",sizeof(WCHAR),1,f);	//注释起始符号
		fwrite(m_arrCntxtRemark[i].pszBuf,sizeof(WCHAR),m_arrCntxtRemark[i].nLen,f);
		fwrite(L"\n",sizeof(WCHAR),1,f);
	}
	char szSpell[100][7];

	for( i=0;i<m_arrContext.GetSize();i++)
	{
		WString *pt=m_arrContext.GetPtAt(i);
		int nWord=MakeSpell(pt->szBuf,pt->cLen,szSpell,100,TRUE);
		if(nWord!=pt->cLen) continue;
		for( j=0;j<pt->cLen;j++)
		{
			WORDPHONE *pwp=GetWordPhone(pt->szBuf[j]);
			fwrite(pt->szBuf+j,sizeof(WCHAR),1,f);		//写入汉字
			if(pwp->cpp!=1) //多音字则写入拼音
			{
				WCHAR wzSpell[7];
				int nLen = strlen(szSpell[j]);
				CUtils::CopyCompFromAStr2WStr(wzSpell,szSpell[j],nLen);
				fwrite(wzSpell,sizeof(WCHAR),nLen,f);
			}
		}
		fwrite(L"\n",sizeof(WCHAR),1,f);
		nRet++;
	}
	fclose(f);
	return nRet;
}

//导入多音字上下文
//文本 如:重chong复
int CSpellEngine::Context_Import(LPCTSTR pszFileName)
{
	WCHAR szLine[1000];
	CTxtReader reader('#');
	if(!reader.Open(pszFileName)) return -1;
	int i,j;
	//清除当前的上下文
	m_arrCntxtRemark.RemoveAll();
	m_arrContext.RemoveAll();
	for( i=0;i<m_arrWord.GetSize();i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		for( j=0;j<pwp->cpp;j++)
		{
			if(pwp->pi[j].pArrCntxt)
			{
				delete pwp->pi[j].pArrCntxt;
				 pwp->pi[j].pArrCntxt=NULL;
			}
		}
	}

	int nRet=0;
	BOOL bRead=reader.getline(szLine,100);
	//载入上下文列表，将一些相互包含的上下文去除
	SPELLPHRASE spPhrase;
	while(bRead)
	{
		if(szLine[0]=='@')
		{//注释行
			m_arrCntxtRemark.AddString(szLine+1);
		}else
		{//词组行
			nRet++;
			memset(&spPhrase,0,sizeof(spPhrase));
			MakeSpellPhrase(szLine,&spPhrase);
			for(char c=0;c<spPhrase.cPPSize;c++)
			{//为上下文中的所有多音字更新上下文列表
				WORDPHONE wp;
				wp.wWord = spPhrase.szBuf[spPhrase.cPPPos[c]];
				int idx=m_arrWord.SortFind(&wp,WordPhoneCmp,0);//在单字表中找到该多音字。
				if(idx!=-1)
				{
					BYTE byPhone[2];
					WORDPHONE *pwp=m_arrWord.GetPtAt(idx);
					if(pwp->cpp>1 && g_SpellData.Spell2ID(spPhrase.cPPSp[c],-1,byPhone))
					{//拼音必须要有效
						for(char j=0;j<pwp->cpp;j++)
						{
							if(memcmp(pwp->pi[j].byPhone,byPhone,2)==0)
							{//找到读音位置,使用当前的上下文来更新该读音的上下文
								PPCONTEXT2 newContext;
								newContext.cLocate=spPhrase.cPPPos[c];
								wcscpy(newContext.szPhrase,spPhrase.szBuf);
								//比较当前上下文与己有上下文的关系
								if(pwp->pi[j].pArrCntxt2==NULL) 
									pwp->pi[j].pArrCntxt2=new CSArray<PPCONTEXT2>;

								CSArray<PPCONTEXT2> *pCntxt=pwp->pi[j].pArrCntxt2;

								BOOL bExist=FALSE;
								for(short k=pCntxt->GetSize()-1;k>=0;k--)
								{
									PPCONTEXT2 *pcntxt=pCntxt->GetPtAt(k);
									WCHAR *pResult=wcsstr(newContext.szPhrase,pcntxt->szPhrase);
									if(pResult && newContext.cLocate-(pResult-newContext.szPhrase) == pcntxt->cLocate )
									{//新的上下文有有效部分已经存在
										bExist=TRUE;
										break;
									}
									pResult=wcsstr(pcntxt->szPhrase,newContext.szPhrase);
									if(pResult && pcntxt->cLocate-(pResult-pcntxt->szPhrase) == newContext.cLocate )
									{//新的上下文包含原上下文的有效部分,删除原上下文
										pCntxt->RemoveAt(k);
									}
								}
								if(!bExist)	pCntxt->Add(newContext);
								break;
							}
						}
					}
				}
			}
		}
		bRead=reader.getline(szLine,100);
	}
	reader.Close();

	//找出所有多音字的上下文并排序加入到m_arrContext中
	WString t;
	for(i=0;i<m_arrWord.GetSize();i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		for(char j=0;j<pwp->cpp;j++)
		{
			if(!pwp->pi[j].pArrCntxt2) continue;
			for(short k=0;k<pwp->pi[j].pArrCntxt2->GetSize();k++)
			{
				PPCONTEXT2 cntxt2=pwp->pi[j].pArrCntxt2->GetAt(k);
				wcscpy(t.szBuf,cntxt2.szPhrase);
				t.cLen=wcslen(t.szBuf);
				m_arrContext.SortInsert(t,FALSE,TextCmp);
			}
		}
	}	
	//将所有多音字的上下文信息用一个索引来引用
	for(i=0;i<m_arrWord.GetSize();i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		for(char j=0;j<pwp->cpp;j++)
		{
			if(!pwp->pi[j].pArrCntxt2) continue;
			CSArray<PPCONTEXT> *pArrCntxt=new CSArray<PPCONTEXT>;
			for(short k=0;k<pwp->pi[j].pArrCntxt2->GetSize();k++)
			{
				PPCONTEXT2 cntxt2=pwp->pi[j].pArrCntxt2->GetAt(k);
				t.cLen=wcslen(cntxt2.szPhrase);
				wcscpy(t.szBuf,cntxt2.szPhrase);
				int nIdx=m_arrContext.SortFind(&t,TextCmp);
				ASSERT(nIdx!=-1);
				PPCONTEXT cntxt;
				cntxt.cLocate=cntxt2.cLocate;
				cntxt.dwIndex=nIdx;
				pArrCntxt->Add(cntxt);
			}
			delete pwp->pi[j].pArrCntxt2;
			pwp->pi[j].pArrCntxt=pArrCntxt;
		}
	}
	return nRet;
}

BOOL CSpellEngine::Save(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("wb"));
	if(!f) return FALSE;
	ISDOC_WriteHeader(f,FLAG_SPELLENGINE,SPVER_MAJON,SPVER_MINOR2,0);
	m_spellMap.WriteData(f);
	//写入多音字信息
	int i,nCount=m_arrWord.GetSize();
	fwrite(&nCount,sizeof(int),1,f);
	for(i=0;i<nCount;i++)
	{
		WORDPHONE *pwp=m_arrWord.GetPtAt(i);
		fwrite(&pwp->wWord,2,1,f);
		fwrite(pwp->byPhone,1,2,f);
		fwrite(&pwp->cpp,1,1,f);
		for(int j=0;j<pwp->cpp;j++)
		{
			fwrite(pwp->pi[j].byPhone,1,2,f);
			short sCount=0;
			if(pwp->pi[j].pArrCntxt) sCount=pwp->pi[j].pArrCntxt->GetSize();
			fwrite(&sCount,sizeof(short),1,f);
			for(short k=0;k<sCount;k++)
			{
				PPCONTEXT *pCntxt=pwp->pi[j].pArrCntxt->GetPtAt(k);
				fwrite(pCntxt,sizeof(PPCONTEXT),1,f);
			}
		}
	}
	//写入上下文
	nCount=m_arrContext.GetSize();
	fwrite(&nCount,sizeof(int),1,f);
	for(i=0;i<nCount;i++)
	{
		WString *pt=m_arrContext.GetPtAt(i);
		fwrite(&pt->cLen,1,1,f);
		fwrite(pt->szBuf,sizeof(WCHAR),pt->cLen,f);
	}
	//Save remark
	nCount=m_arrCntxtRemark.GetSize();
	fwrite(&nCount,sizeof(int),1,f);
	for(i=0;i<nCount;i++)
	{
		BYTE bySize=(BYTE)m_arrCntxtRemark[i].nLen;
		fwrite(&bySize,1,1,f);
		fwrite(m_arrCntxtRemark[i].pszBuf,sizeof(WCHAR),bySize,f);
	}
	fclose(f);
	return TRUE;	
}

BOOL CSpellEngine::Open(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,FLAG_SPELLENGINE,SPVER_MAJON,SPVER_MINOR2))
	{
		fclose(f);
		return FALSE;
	}
	//载入拼音汉字映射表
	m_spellMap.LoadData(f);
	
	//载入多音字信息
	int i,nCount=0;
	fread(&nCount,sizeof(int),1,f);
	m_arrWord.SetSize(nCount);
	WORDPHONE wp;
	for(i=0;i<nCount;i++)
	{
		memset(&wp,0,sizeof(wp));
		fread(&wp.wWord,2,1,f);
		fread(wp.byPhone,1,2,f);
		fread(&wp.cpp,1,1,f);
		for(int j=0;j<wp.cpp;j++)
		{
			fread(wp.pi[j].byPhone,1,2,f);
			short sCount=0;
			fread(&sCount,sizeof(short),1,f);
			if(sCount){
				wp.pi[j].pArrCntxt=new CSArray<PPCONTEXT>(sCount);
				wp.pi[j].pArrCntxt->SetSize(sCount);
			}
			PPCONTEXT cntxt;
			for(short k=0;k<sCount;k++)
			{
				fread(&cntxt,sizeof(cntxt),1,f);
				wp.pi[j].pArrCntxt->SetAt(k,cntxt);
			}
		}
		m_arrWord.SetAt(i,wp);
	}
	//载入上下文
	nCount=0;
	fread(&nCount,sizeof(int),1,f);
	m_arrContext.SetSize(nCount);
	WString t;
	for(i=0;i<nCount;i++)
	{
		memset(&t,0,sizeof(t));
		fread(&t.cLen,1,1,f);
		fread(t.szBuf,sizeof(WCHAR),t.cLen,f);
		m_arrContext.SetAt(i,t);
	}
	//load remark
	nCount=0;
	fread(&nCount,sizeof(int),1,f);
	WCHAR szBuf[256];
	for(i=0;i<nCount;i++)
	{
		BYTE bySize=0;
		fread(&bySize,1,1,f);
		if(fread(szBuf,sizeof(WCHAR),bySize,f)!=bySize) break;
		szBuf[bySize]=0;
		m_arrCntxtRemark.AddString(szBuf);
	}

	fclose(f);
	return TRUE;
}

//计算一句话中一个字的读音，如果是多音字则根据上下文查找最匹配的读音
//LPCSTR pszSent,int nLen,int iWord:输入的句子及目标字所在的位置
//BYTE byPhone[2]:返回拼音ID
//BOOL bPolyPhoneDef:没有找到上下文时是否使用默认读音填充
BOOL CSpellEngine::CalcPolyPhone(LPCWSTR pszSent,int nLen,int iWord,BYTE byPhone[2],BOOL bPolyPhoneDef/*=TRUE*/)
{
	//找到该字据读音信息
	WORDPHONE wp;
	wp.wWord = pszSent[iWord];
	int nIdx=m_arrWord.SortFind(&wp,WordPhoneCmp);
	if(nIdx==-1) return FALSE;
	WORDPHONE *pwp=m_arrWord.GetPtAt(nIdx);
	if(pwp->cpp==1)
	{//不是多音字
		memcpy(byPhone,pwp->byPhone,2);
		return TRUE;
	}
	if(bPolyPhoneDef)
		memcpy(byPhone,pwp->byPhone,2);
	else
		memset(byPhone,0xFF,2);
	
	char cMatchLen=0;	//匹配长度
	for(char i=0;i<pwp->cpp;i++)
	{
		if(!pwp->pi[i].pArrCntxt) continue;	//必须有多音字上下文
		BOOL bFind=FALSE;
		for(int j=0;j<pwp->pi[i].pArrCntxt->GetSize();j++)
		{//搜索上下文
			PPCONTEXT *pCntxt=pwp->pi[i].pArrCntxt->GetPtAt(j);
			WString *pt=m_arrContext.GetPtAt(pCntxt->dwIndex);
			if(pt->cLen<cMatchLen || pCntxt->cLocate>iWord || nLen-(iWord-pCntxt->cLocate)<pt->cLen) continue;
			if(wcsncmp(pszSent+(iWord-pCntxt->cLocate),pt->szBuf,pt->cLen)==0)
			{
				memcpy(byPhone,pwp->pi[i].byPhone,2);
				cMatchLen=pt->cLen;
				break;
			}
		}
	}
	return cMatchLen>0 || bPolyPhoneDef;
}

//---------------------------------------------------------------
//	获取一个句子的拼音
//	LPCSTR pszSent:目标句子
//	BYTE byPhoneIDs[][2]:输出拼音ID
//	int nSize:输出容量
//	BOOL bPolyPhoneDef:没有多音字上下文时是否使用默认拼音代替
//---------------------------------------------------------------
int CSpellEngine::MakeSpell(LPCWSTR pszSent,int nLen, BYTE byPhoneIDs[][2],int nSize,BOOL bPolyPhoneDef/*=TRUE*/)
{
	if(nLen==-1) nLen=wcslen(pszSent);
	int nWords=nLen;
	int nRet=0;
	for(int i=0;i<nWords;i++)
	{
		if(i==nSize)
			break;
		if(!CWordID::IsHanzi(pszSent[i]))//ascii code
			break;
		if(!CalcPolyPhone(pszSent,nLen,i,byPhoneIDs[i],bPolyPhoneDef)) 
			break;
		nRet++;
	}
	return nRet;
}

int CSpellEngine::MakeSpell(LPCWSTR pszSent,int nLen, char szSpells[][7],int nSize,BOOL bPolyPhoneDef/*=TRUE*/)
{
	ASSERT(nSize<200);
	BYTE byIDs[200][2];
	int nRet=MakeSpell(pszSent,nLen,byIDs,nSize,bPolyPhoneDef);
	for(int i=0;i<nRet;i++)
	{
		if(memcmp(byIDs[i],"\0xFF\0xFF",2)==0)
			strcpy(szSpells[i],"none");
		else
			g_SpellData.ID2Spell(byIDs[i],szSpells[i]);
	}
	return nRet;
}

//计算一个词组的拼音,词组中可以包含非中文字符
//LPCSTR pszPhrase,int nLen:词组
//char szSpell[][7]:返回拼音,如果该字是多音字同时缺少匹配的上下文时该拼音为空,如果该字不是中文或者在拼音对照表中没有拼音也为空
//返回获得拼音的单字数
int CSpellEngine::GetPhraseSpell(LPCWSTR pszPhrase,int nLen,char szSpell[][7])
{
	LPCWSTR pSent=pszPhrase;
	int i=0,iWord=0,words=0;
	BYTE byID[2];
	int nRet=0;
	while(i<nLen)
	{
		if(pszPhrase[i]>127)
		{
			if(CalcPolyPhone(pSent,nLen-(pSent-pszPhrase),iWord,byID,FALSE))
			{
				g_SpellData.ID2Spell(byID,szSpell[words]);
				nRet++;
			}else
			{
				szSpell[words][0]=0;
			}
			i+=2;
			iWord++;
		}else
		{
			i++;
			pSent+=iWord*2+1;
			iWord=0;
			szSpell[words][0]=0;
		}
		words++;
	}
	return nRet;
}

//获取一个汉字的全部读音
BOOL CSpellEngine::GetWordSpell(WCHAR wChar, char *pszSpell)
{
	WORDPHONE *pwp=GetWordPhone(wChar);
	if(!pwp) return FALSE;
	char szSpell[7];
	for(char c=0;c<pwp->cpp;c++)
	{
		pszSpell[0]=0x20;
		pszSpell++;
		g_SpellData.ID2Spell(pwp->pi[c].byPhone,szSpell);
		strcpy(pszSpell,szSpell);
		pszSpell+=strlen(szSpell);
	}
	pszSpell[0]=0;
	return TRUE;
}

WORDPHONE * CSpellEngine::GetWordPhone(WCHAR wChar)
{
	WORDPHONE wp;
	wp.wWord = wChar;
	int nIdx=m_arrWord.SortFind(&wp,WordPhoneCmp);
	if(nIdx==-1) return NULL;
	return m_arrWord.GetPtAt(nIdx);
}

//只保存拼音到汉字的信息
BOOL CSpellEngine::SaveSpellMap(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("r+b"));
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,FLAG_SPELLENGINE,SPVER_MAJON,SPVER_MINOR2))
	{
		fclose(f);
		return FALSE;
	}
	//保存拼音汉字映射表
	m_spellMap.WriteData(f);
	fclose(f);
	return TRUE;
}
