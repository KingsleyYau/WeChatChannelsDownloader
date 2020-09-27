
#ifndef __FILECTRL_H__
#define __FILECTRL_H__

#include <pthread.h>
#include <stdio.h>

typedef unsigned long   LOG_LEVEL;

#define LOG_OFF         (LOG_LEVEL)0
#define LOG_ALERT     	(LOG_LEVEL)1
#define LOG_ERR    		(LOG_LEVEL)2
#define LOG_WARNING     (LOG_LEVEL)3
#define LOG_NOTICE     	(LOG_LEVEL)4
#define LOG_INFO        (LOG_LEVEL)5
#define LOG_DEBUG       (LOG_LEVEL)6

#ifndef MAX_PATH
    #define MAX_PATH        256
#endif
#ifndef BUFFER_SIZE_1K
    #define BUFFER_SIZE_1K  1024
#endif
#ifndef BUFFER_SIZE_2K
    #define BUFFER_SIZE_2K  2048
#endif
#ifndef BUFFER_SIZE_5K
    #define BUFFER_SIZE_5K  5120
#endif

/*
  create a memory block, flush memory into file by application or memories were filled [ usLen == 0 then flush into file immediately]
  make sure the memory block is larger than 1 record
 */

class CFileCtrl
{
public:
    CFileCtrl();
    virtual ~CFileCtrl();

public:
    int Initialize(
    		const char *szlogPath,
			const char* szLogFileName,
			unsigned long alFileLen = 30,
			unsigned int iMemSize = 0,
			int bSingle = 0
    		);
    FILE* OpenLogFile();
    FILE* FileOpen(const char *szFileName, int aiMod);
    int LogMsg(const char* pszFormat, int aiLen, const char* pszHead, bool unSafe = false);
    int ReadMsg(char *szFormat, int aiLen);
    bool SetFileSeek(unsigned long nOffset);
    int printLog(const char *szfmt, ...);
    void Mem2File();
	
    bool IsEnd();

    int CloseFile();
	
    long GetFileLen() {return m_nFileLen;};
    long GetCurLen() {return m_ncurRead;};

    void SetFlushBuffer(unsigned int iLen);
    void FlushMem2File();

protected:
    FILE* CreateLog();
    int Mkdirs(const char *path, mode_t mode);

private:
    pthread_mutex_t m_hMutex;
    FILE* m_pLogFile;
    unsigned long m_dwMaxFileLength;    //Max file length
    char m_szLogFileName[MAX_PATH];     //File name
    char m_szLogPath[MAX_PATH];         //Path name
    long m_nFileLen;                    //file size
    long m_ncurRead;                    //current position
    int m_bSingle;                      //need for mutithread
    char* m_pBuffer;                    //buffer
    long m_nBufferUse;                  //buffer in use

    unsigned m_iMemSize;
    pthread_mutex_t m_hMutexBuffer;

};
#endif
