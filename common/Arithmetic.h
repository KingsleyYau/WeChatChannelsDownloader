/*
 * File         : Arithmetic.hpp
 * Date         : 2007-07-12
 * Author       : Keqin Su
 * Copyright    : City Hotspot Co., Ltd.
 * Description  : include file for Arithmetic.h 
 */

#ifndef _INC_ARITHMETIC
#define _INC_ARITHMETIC

#include <stdio.h>
#include <stdlib.h>

#include <string>
using namespace std;

class Arithmetic
{
public:
    int TeaEncode(char* p_in, int i_in_len, char* p_key, char* p_out);
    int TeaDecode(char* p_in, int i_in_len, char* p_key, char* p_out);

    int Base64Encode(const char* data, int length, char** code);
    string Base64Encode(const char* data, int length);
    int Base64Decode(const char* data, int length, char* code);

    static int AsciiToHex(const char* data, int i_in_len, char* code);
    static int HexToAscii(const char* data, int i_in_len, char* code);

    int encode_url(const char* data, int length, char *code);
    int decode_url(const char* data, int length, char *code);

    int encode_urlspecialchar(const char* data, int length, char* code);
    int decode_urlspecialchar(const char* data, int length, char* code);

    unsigned long MakeCRC32(char* data, int i_in_len);
    
    bool String2Mac(char* pstr, char* Mac);
    bool Mac2String(char* pstr, char* Mac);
    
    static string AesEncrypt(string initKey, string src);
    static string AesDecrypt(string initKey, string src);

    size_t ChangeCharset(char* outbuf, size_t outbytes, const char* inbuf, const char* fromcode, const char* tocode, bool conv_begin = true);

    string AsciiToHexWithSep(const char* data, int i_in_len, string sep = ":");

protected:
    void encipher(void* aData, const void* aKey);
    void decipher(void* aData, const void* aKey);

protected:
    static char hex[16];
    static int encode[64];
    static char rstr[128];
    static unsigned long crc_32_tab[256];
};

#endif
