
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "LogFile.hpp"

CFileCtrl::CFileCtrl()
{
    m_dwMaxFileLength = 0;
    m_pLogFile = NULL;
    memset(m_szLogFileName, 0, MAX_PATH * sizeof(char));
    memset(m_szLogPath, 0, MAX_PATH * sizeof(char));
    
	m_bSingle = 0;
	m_pBuffer = NULL;
	m_nBufferUse = 0;

	m_nFileLen = 0;
	m_ncurRead = 0;
	m_iMemSize = 0;

}

CFileCtrl::~CFileCtrl()
{
    pthread_mutex_destroy(&m_hMutex);
    pthread_mutex_destroy(&m_hMutexBuffer);
    
    CloseFile();
    if (m_pBuffer) {
        delete[] m_pBuffer;
    }
}

int CFileCtrl::CloseFile()
{
    if (m_pLogFile){
        fclose(m_pLogFile);
        m_pLogFile = NULL;
    }
    return 0;
}

int CFileCtrl::Initialize(
		const char *szlogPath,
		const char* szFileName,
		unsigned long aiFileLen,
		unsigned int iMemSize,
		int bSingle
		)
{
    m_dwMaxFileLength = aiFileLen;
    m_pLogFile = NULL;
    memset(m_szLogFileName, 0, MAX_PATH * sizeof(char));
    snprintf(m_szLogFileName, MAX_PATH, "%s", szFileName);

    memset(m_szLogPath, 0, MAX_PATH * sizeof(char));
    snprintf(m_szLogPath, MAX_PATH, "%s", szlogPath);

    //printf("%s:%s", m_szLogPath, m_szLogFileName );
	m_bSingle = bSingle;
	if( m_pBuffer ) {
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
	m_nBufferUse = 0;

	m_nFileLen = 0;
	m_ncurRead = 0;

	m_iMemSize = iMemSize;
	if( m_iMemSize > 0 ) {
	    m_pBuffer = new char[m_iMemSize];
	}

    if (m_dwMaxFileLength == 0) {
        m_dwMaxFileLength = 2;
    }

    m_dwMaxFileLength = m_dwMaxFileLength * BUFFER_SIZE_1K * BUFFER_SIZE_1K;

	pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_hMutex, &mattr);
    pthread_mutex_init(&m_hMutexBuffer, NULL);
    pthread_mutexattr_destroy(&mattr);

    mode_t mod = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    Mkdirs(szlogPath, mod);

	return 0;
}

FILE*  CFileCtrl::OpenLogFile()
{
    return CreateLog();
}

FILE* CFileCtrl::CreateLog()
{	
    char szTime[MAX_PATH];
    memset(szTime,0,MAX_PATH);
    
	time_t stm=time(NULL);
	struct tm tTime;
	localtime_r(&stm,&tTime);
	
	DIR *dp=opendir(m_szLogPath);
	if (dp==NULL) {
		mode_t mod = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ;
		int ret = Mkdirs(m_szLogPath, mod);
		if( ret != 0 ) {
			fprintf(stderr, "mkdir \"%s\" fail(%d) \n", m_szLogPath, errno);
		}
	} else {
		closedir(dp);
	}
    snprintf(szTime, MAX_PATH, "%s/%s%d%02d%02d%02d%02d%02d.txt", m_szLogPath, m_szLogFileName, tTime.tm_year+1900, tTime.tm_mon+1, tTime.tm_mday, tTime.tm_hour, tTime.tm_min, tTime.tm_sec);

    if (m_pLogFile) {
        fclose(m_pLogFile);
        m_pLogFile = NULL;
        m_nFileLen = 0;
    }

    m_pLogFile = fopen(szTime, "w+b");
    if ( m_pLogFile ) {
        m_nFileLen = fseek(m_pLogFile, 0L, SEEK_END);
    }
    return m_pLogFile;
}

FILE*  CFileCtrl::FileOpen(const char *szFileName, int aiMod)
{
    if (m_pLogFile) {
        fclose(m_pLogFile);
        m_pLogFile = NULL;
    }

    m_nFileLen = 0;
    m_ncurRead = 0;

    m_pLogFile = fopen(szFileName, "w+b");
    if ( m_pLogFile ) {
        m_nFileLen = fseek(m_pLogFile, 0L, SEEK_END);
        fseek(m_pLogFile, 0 - m_nFileLen, SEEK_CUR);
    }
    return m_pLogFile;
}

bool CFileCtrl::SetFileSeek(unsigned long  nOffset)
{	
    return true;
}

int CFileCtrl::LogMsg(const char* pszFormat, int aiLen, const char* pszHead, bool unSafe)
{
    if (m_pLogFile == NULL || !pszFormat) {
        return -1;
    }

    if (!unSafe && !m_bSingle) {
        pthread_mutex_lock(&m_hMutex); 
    }
    if (!m_pBuffer) {
        int len = (int)fwrite(pszHead, sizeof(char), (unsigned int)strlen(pszHead), m_pLogFile);
        m_nFileLen += len;
        len = (int)fwrite(pszFormat, sizeof(char), (unsigned int)aiLen, m_pLogFile);
    	//file size
        m_nFileLen += len;
        
        fflush(m_pLogFile);
    } else {
        if ((m_nBufferUse + (unsigned int)strlen(pszHead) + (unsigned int)aiLen) >= m_iMemSize) {//memory full
            Mem2File();//change m_nBufferUse to 0
        }
        //add buffer len for those in use
        memcpy(m_pBuffer + m_nBufferUse, pszHead, (unsigned int)strlen(pszHead));
        m_nBufferUse += (unsigned int)strlen(pszHead);
        memcpy(m_pBuffer + m_nBufferUse, pszFormat, (unsigned int)aiLen);
        m_nBufferUse += (unsigned int)aiLen;
        
        m_nFileLen = m_nFileLen +  (unsigned int)strlen(pszHead) + (unsigned int)aiLen;//add file len for create file
    }

	//over file size, create again
    if (m_nFileLen >= (long)m_dwMaxFileLength) {
        Mem2File();
        CreateLog();
    }
    if (!unSafe && !m_bSingle){
        pthread_mutex_unlock(&m_hMutex);
    }
    
    return 0;
}

int CFileCtrl::ReadMsg(char *szFormat, int aiLen)
{	
    if (!szFormat || m_pLogFile == NULL) {
        return -1;
    }
	
    if (!m_bSingle) {
		pthread_mutex_lock(&m_hMutex); 
    }
	
    int len = (int)fread(szFormat, sizeof(char), aiLen, m_pLogFile);

	//current size
    m_ncurRead += len;
	//over file size, create again
    if (m_ncurRead >= m_nFileLen) {
        fclose(m_pLogFile);
        m_pLogFile=NULL;
		
        if (!m_bSingle) {
            pthread_mutex_unlock(&m_hMutex); 
        }
        return len;
    }
    fseek(m_pLogFile, len, SEEK_CUR);

    if (!m_bSingle) {
        pthread_mutex_unlock(&m_hMutex); 
    }
    return len;
}

bool CFileCtrl::IsEnd()
{
    if (m_pLogFile == NULL) {
        return true;
    }

    if (m_ncurRead < m_nFileLen) {
        return false;
    }
    return true;	
}

void CFileCtrl::Mem2File()
{
    if (m_pLogFile && m_pBuffer && (m_nBufferUse > 0)){
        fwrite(m_pBuffer, sizeof(char), m_nBufferUse, m_pLogFile);
        m_nBufferUse = 0;
        fflush(m_pLogFile);
    }
}

void CFileCtrl::SetFlushBuffer(unsigned int iLen)
{
    if (!m_bSingle) {
        pthread_mutex_lock(&m_hMutex);
    }

	Mem2File();

	if( m_pBuffer ) {
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}

    m_iMemSize = iLen;
	if( m_iMemSize > 0 ) {
	    m_pBuffer = new char[m_iMemSize];
	}

    if (!m_bSingle) {
        pthread_mutex_unlock(&m_hMutex);
    }
}

void CFileCtrl::FlushMem2File()
{
    if (!m_bSingle) {
        pthread_mutex_lock(&m_hMutex);
    }

	Mem2File();

    if (!m_bSingle) {
        pthread_mutex_unlock(&m_hMutex);
    }
}

int CFileCtrl::Mkdirs(const char *path, mode_t mode) {
	int ret = 0;
	char curPath[MAX_PATH] = {'\0'};

	int len = strlen(path);
    strncpy(curPath, path, strlen(path));

    for(int i = 0; i < len; i++) {
    	if ( curPath[i] == '/' ) {
    		curPath[i] = '\0';
            if( access(curPath, 0) != 0 ) {
            	ret = mkdir( curPath, mode );
            	if( ret != 0 ) {
            		break;
            	}
            }
            curPath[i]='/';
        }
    }

    if( ret == 0 ) {
		if( len > 0 && access(curPath, 0) != 0 ) {
			ret = mkdir( curPath, mode );
		}
    }

    return ret;
}
