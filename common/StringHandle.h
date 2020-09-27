/*
 * File         : StringHandle.h
 * Date         : 2012-07-02
 * Author       : Kingsley Yau
 * Copyright    : City Hotspot Co., Ltd.
 * Description  : DrPalm StringHandle include
 */

#ifndef _INC_STRINGHANDLE_
#define _INC_STRINGHANDLE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <list>
#include <vector>

#include "Arithmetic.h"
using namespace std;
class StringHandle {
public:
	/**
	 * 寻找分隔符的第一个字符串
	 * @param	src 原始字符串
	 * @param	sep 分隔符
	 * @param	index 开始寻找位置
	 * @param	nextIndex 下次开始位置( 没有找到则返回 string::npos )
	 * @return	返回第一个分隔的字符串, 没有找到则返回index开始到结尾的字符串
	 */
	static string findFirstString(const string& src, const string& sep, string::size_type index, string::size_type& nextIndex) {
		string ret = "";

		string::size_type pos = string::npos;
		nextIndex = string::npos;

		if( index < src.length() ) {
			pos = src.find(sep, index);
			if( pos != string::npos ) {
				ret = src.substr(index, pos - index);
				nextIndex = pos + sep.length();
			} else {
				ret = src.substr(index, src.length() - index);
			}
		}

		return ret;
	}

	static string replace(const string& str, const string& src, const string& dest) {
	    string ret;

	    string::size_type pos_begin = 0;
	    string::size_type pos = str.find(src);
	    while( pos != string::npos ) {
	        ret.append(str.data() + pos_begin, pos - pos_begin);
	        ret += dest;
	        pos_begin = pos + src.length();
	        pos = str.find(src, pos_begin);
	    }
	    if( pos_begin < str.length() ) {
	        ret.append(str.begin() + pos_begin, str.end());
	    }
	    return ret;
	}

	static list<string> split(string str, string pattern) {
	    string::size_type pos;
	    list<string> result;
	    str += pattern;
	    string::size_type size = str.size();

	    for(string::size_type i = 0; i < size; i++) {
	        pos = str.find(pattern, i);
	        if( pos < size ) {
	            string s = str.substr(i, pos - i);
	            result.push_back(s);
	            i = pos + pattern.size() - 1;
	        }
	    }
	    return result;
	}

	static vector<string> splitWithVector(string str, string pattern) {
	    string::size_type pos;
	    vector<string> result;
	    str += pattern;
	    string::size_type size = str.size();

	    for(string::size_type i = 0; i < size; i++) {
	        pos = str.find(pattern, i);
	        if( pos < size ) {
	            string s = str.substr(i, pos - i);
	            result.push_back(s);
	            i = pos + pattern.size() - 1;
	        }
	    }
	    return result;
	}

	static string trim(const string &str) {
		string s = str;
	    if (s.empty()) {
	        return s;
	    }

	    s.erase(0, s.find_first_not_of(" "));
	    s.erase(s.find_last_not_of(" ") + 1);

	    return s;
	}

	static inline char* strIstr(const char *haystack, const char *needle) {
	    if (!*needle) {
	    	return (char*)haystack;
	    }
	    for (; *haystack; ++haystack) {
	    	if (toupper(*haystack) == toupper(*needle)) {
	    		const char *h, *n;
	    		for (h = haystack, n = needle; *h && *n; ++h, ++n) {
	    			if (toupper(*h) != toupper(*n)) {
	    				break;
	    			}
	    		}
	    		if (!*n) {
	    			return (char*)haystack;
	    		}
	    	}
	    }
	    return 0;
	}
	static inline string findStringBetween(char* pData, char* pBegin, char* pEnd, char* pTmpBuffer, int iTmpLen) {
		string strRet = "";
		char *pC_Begin = NULL, *pC_End = NULL, *pRep = NULL;
		int iLen = 256;

		if (pTmpBuffer && iTmpLen > 0) {
			pRep = pTmpBuffer;
			iLen = iTmpLen;
		} else {

			pRep = new char[256];

		}
		bzero(pRep, iLen);

		if ((pC_Begin = strIstr(pData, pBegin)) > 0) {

			if ((pC_End = strIstr(pC_Begin, pEnd)) > 0) {

				memcpy(pRep, pC_Begin, pC_End - pC_Begin);
				strRet = pRep;
			}

		}

		if (!pTmpBuffer || iTmpLen <= 0) {
			delete pRep;
		}

		return strRet;
	}
};

#endif


