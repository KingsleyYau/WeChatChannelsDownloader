/*
 * File         : ConfFile.hpp
 * Date         : 2004-09-22
 * Author       : Keqin Su
 * Description  : Class for Setting File Read/Write 
 */

#ifndef _INC_CONFFILE
#define _INC_CONFFILE

#include <map>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Arithmetic.h"

using namespace std;

typedef map< string, string, less<string> > ConfMap;

#ifndef BUFFER_SIZE_4K
    #define BUFFER_SIZE_4K          4096
#endif

/*************************************************
the file format should be like follow description:
[SPACE1]
KEY1=VALUE1
[SPACE2]
KEY2=VALUE2
KEY3=VALUE3
**************************************************/

class ConfFile
{
    public:
        static ConfFile* GlobalConfFile() {
            static ConfFile g_ConfFile;
	        return &g_ConfFile;
        };
        ConfFile();
        ~ConfFile();
        //initialzation the file name that wanted to be use
        bool InitConfFile(const string& strConfFile, const string& strCipher);
        bool InitConfFile(const char* pcConfFile, const char* pcCipher);
        //load config from file
        bool LoadConfFile();
        //get the value by key and space
        string GetPrivate(const string& strSpace, const string& strKey, const string& strDefault);
        string GetPrivate(const char* pcSpace, const char* pcKey, const char* pcDefault);
        //set the value by key and space, it would input a new space if specify space not be found
        bool SetPrivate(const string& strSpace, const string& strKey, const string& strValue);
        bool SetPrivate(const char* pcSpace, const char* pcKey, const char* pcValue);
        //erase the specify entry by key and space
        bool ErasePrivate(const string& strSpace, const string& strKey);
        bool ErasePrivate(const char* pcSpace, const char* pcKey);
        //erase the specify space
        bool EraseSpace(const string& strSpace);
        bool EraseSpace(const char* pcSpace);
        //erase all config
        bool EraseAll();
        //update the config file
        bool UpdateConfFile();
        //lock
        void Lock();
        //unlock
        void Unlock();
		
    protected:
        ConfMap m_MapFileSet;
        string m_strFile, m_strCipher;
        pthread_mutex_t m_thMutex;
        Arithmetic m_Arithmetic;
};
#endif
