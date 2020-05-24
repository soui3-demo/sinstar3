#include "stdafx.h"
#include "Encoder.h"
#include <string>
#include <assert.h>

using namespace std;
string Encoder::UrlEncode(const string &str)
{
    string strResult;
    size_t nLength = str.length();
    unsigned char* pBytes = (unsigned char*)str.c_str();
    char szAlnum[2];
    char szOther[4];
    for (size_t i = 0; i < nLength; i++)
    {
        if (isalnum((BYTE)str[i]))
        {
            sprintf_s(szAlnum, sizeof(szAlnum), "%c", str[i]);
            strResult.append(szAlnum);
        }
        else if (isspace((BYTE)str[i]))
        {
            strResult.append("+");
        }
        else
        {
            sprintf_s(szOther, sizeof(szOther), "%%%X%X", pBytes[i] >> 4, pBytes[i] % 16);
            strResult.append(szOther);
        }
    }
    return strResult;
}

string Encoder::UrlDecode(const string &str)
{
    string strResult;
    char szTemp[2];
    size_t i = 0;
    size_t nLength = str.length();
    while (i < nLength)
    {
        if (str[i] == '%')
        {
            szTemp[0] = str[i + 1];
            szTemp[1] = str[i + 2];
            strResult += StrToBin(szTemp);
            i = i + 3;
        }
        else if (str[i] == '+')
        {
            strResult += ' ';
            i++;
        }
        else
        {
            strResult += str[i];
            i++;
        }
    }
    return strResult;
}

string Encoder::UTF8UrlEncode(const string &str)
{
    return UrlEncode(AnsiStringToUTF8String(str));
}

string Encoder::UTF8UrlDecode(const string &str)
{
    return UTF8StringToAnsiString(UrlDecode(str));
}

string Encoder::UTF8StringToAnsiString(const string &strUtf8)
{
    string strResult;
    int nUTF8StringLength = strUtf8.length();
    int nResultLength = nUTF8StringLength + (nUTF8StringLength >> 2) + 2;
    strResult.resize(nResultLength);
    int i = 0;
    int j = 0;
    char szBuffer[4] = { 0 };
    WCHAR cchWideChar;
    while (i < nUTF8StringLength)
    {
        if (strUtf8[i] >= 0)
        {
            strResult[j++] = strUtf8[i++];
        }
        else
        {
            UTF8CharToUnicodeChar(&cchWideChar, &strUtf8[i]);
            UnicodeToAnsi(szBuffer, 2, &cchWideChar, 1);

            strResult[j] = szBuffer[0];
            strResult[j + 1] = szBuffer[1];
            strResult[j + 2] = szBuffer[2];
            i += 3;
            j += 2;
        }
    }
    return strResult;
}

string Encoder::AnsiStringToUTF8String(const string& strAnsi)
{
    string strResult;
    int nAnsiStringLength = strAnsi.length();
    char szBuffer[4] = { 0 };
    strResult.clear();
    int i = 0;
    char szAscii[2] = { 0 };
    WCHAR cchWideChar;
    while (i < nAnsiStringLength)
    {
        if (strAnsi[i] >= 0)
        {
            szAscii[0] = (strAnsi[i++]);
            strResult.append(szAscii);
        }
        else
        {
            AnsiToUnicode(&cchWideChar, 1, &strAnsi[i], 2);
            UnicodeCharToUTF8Char(szBuffer, &cchWideChar);
            strResult.append(szBuffer);
            i += 2;
        }
    }
    return strResult;
}

void Encoder::AnsiToUnicode(WCHAR* pUnicodeBuffer, int nUnicodeBufferSize, const char *pAnsiBuffer, int nAnsiBufferSize)
{
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pAnsiBuffer, nAnsiBufferSize, pUnicodeBuffer, nUnicodeBufferSize);
}

void Encoder::UnicodeToAnsi(char* pAnsiBuffer, int nAnsiBufferSize, WCHAR* pUnicodeBuffer, int nUnicodeBufferSize)
{
    WideCharToMultiByte(CP_ACP, NULL, pUnicodeBuffer, nUnicodeBufferSize, pAnsiBuffer, nAnsiBufferSize, NULL, NULL);
}

void Encoder::UTF8CharToUnicodeChar(WCHAR* pUnicodeBuffer, const char *pUTF8Buffer)
{
    char* pChar = (char *)pUnicodeBuffer;
    pChar[1] = ((pUTF8Buffer[0] & 0x0F) << 4) + ((pUTF8Buffer[1] >> 2) & 0x0F);
    pChar[0] = ((pUTF8Buffer[1] & 0x03) << 6) + (pUTF8Buffer[2] & 0x3F);
}

void Encoder::UnicodeCharToUTF8Char(char* pUTF8Buffer, const WCHAR* pUnicodeBuffer)
{
    const char* pChar = (const char *)pUnicodeBuffer;
    pUTF8Buffer[0] = (0xE0 | ((pChar[1] & 0xF0) >> 4));
    pUTF8Buffer[1] = (0x80 | ((pChar[1] & 0x0F) << 2)) + ((pChar[0] & 0xC0) >> 6);
    pUTF8Buffer[2] = (0x80 | (pChar[0] & 0x3F));
}

char Encoder::CharToInt(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return (char)(ch - '0');
    }
    if (ch >= 'a' && ch <= 'f')
    {
        return (char)(ch - 'a' + 10);
    }
    if (ch >= 'A' && ch <= 'F')
    {
        return (char)(ch - 'A' + 10);
    }
    return -1;
}

char Encoder::StrToBin(char *pString)
{
    char szBuffer[2];
    char ch;
    szBuffer[0] = CharToInt(pString[0]); //make the B to 11 -- 00001011 
    szBuffer[1] = CharToInt(pString[1]); //make the 0 to 0 -- 00000000 
    ch = (szBuffer[0] << 4) | szBuffer[1]; //to change the BO to 10110000 
    return ch;
}
