#pragma once
#include <wtypes.h>
#include <string>

class Encoder
{
public:

    static std::string UrlEncode(const std::string &str);
    static std::string UrlDecode(const std::string &str);

    static std::string UTF8UrlEncode(const std::string &str);
    static std::string UTF8UrlDecode(const std::string &str);


private:

    static std::string UTF8StringToAnsiString(const std::string &strUtf8);
    static std::string AnsiStringToUTF8String(const std::string& strAnsi);

    static void AnsiToUnicode(WCHAR* pUnicodeBuffer, int nUnicodeBufferSize, const char *pAnsiBuffer, int nAnsiBufferSize);
    static void UnicodeToAnsi(char* pAnsiBuffer, int nAnsiBufferSize, WCHAR* pUnicodeBuffer, int nUnicodeBufferSize);

    static void UTF8CharToUnicodeChar(WCHAR* pUnicodeBuffer, const char *pUTF8Buffer);
    static void UnicodeCharToUTF8Char(char* pUTF8Buffer, const WCHAR* pUnicodeBuffer);

    static char CharToInt(char ch);
    static char StrToBin(char *pString);

};
