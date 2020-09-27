/*
 * File         : ConfFile.cpp
 * Date         : 2004-09-22
 * Author       : Keqin Su
 * Description  : Class for Setting File Read/Write
 */

#include "ConfFile.hpp"

ConfFile::ConfFile()
{
    m_strFile = "";
    m_MapFileSet.clear();
    pthread_mutex_init(&m_thMutex, NULL);
}

ConfFile::~ConfFile()
{
    pthread_mutex_destroy(&m_thMutex);
}

bool ConfFile::InitConfFile(const string& strConfFile, const string& strCipher)
{
    m_strFile = strConfFile;
    m_strCipher = strCipher;
    return true;
}

bool ConfFile::InitConfFile(const char* pcConfFile, const char* pcCipher)
{
    string strConfFile = pcConfFile;
    string strCipher = pcCipher;
    return InitConfFile(strConfFile, strCipher);
}

bool ConfFile::LoadConfFile()
{
    FILE* fd;
    FILE* fdTmp;
    char cLine[BUFFER_SIZE_4K];
    memset(cLine, 0, BUFFER_SIZE_4K * sizeof(char));

    if (m_strCipher.length() < 1) {
        fd = fopen(m_strFile.c_str(), "rb");
        if (fd == NULL) {
            return false;
        }
    } else {
        struct stat buf;
        int result = stat(m_strFile.c_str(), &buf);
        if (result != 0) {
            return false;
        } else {
            fd = tmpfile();
            if (fd == NULL) {
                return false;
            }
            fdTmp = fopen(m_strFile.c_str(), "rb");
            if (fdTmp == NULL) {
                return false;
            }
            char* pData = new char[buf.st_size];
            char* pDataDes = new char[buf.st_size];
            if (fread(pData, sizeof(char), buf.st_size, fdTmp) == (size_t)buf.st_size) {
                int iRet = m_Arithmetic.HexToAscii(pData, buf.st_size, pDataDes);
                if (iRet) {
                    m_Arithmetic.TeaDecode(pDataDes, iRet, (char*)m_strCipher.c_str(), pData);
                    iRet = fwrite(pData, sizeof(char), iRet, fd);
                    fseek(fd, 0, SEEK_SET);
                }
            }
            delete[] pData;
            delete[] pDataDes;
            fclose(fdTmp);
        }
    }

    string strSpace, strSpaceKey;

    while (fgets(cLine, sizeof(cLine), fd)) {
        if (cLine == NULL) continue;
        if (strlen(cLine) < 1) continue;
        if (cLine[0] == '#') continue;
        if (cLine[0] == '[' && cLine[strlen(cLine) - 2] == ']') {
            cLine[strlen(cLine) - 2] = '\0';
            strSpace = cLine + 1;
            continue;
        }
        char* token = strtok(cLine, "=\n");
        int i = 0;
        string strKey, strValue;

        while (token != NULL) {
            if (i == 0){
                strKey = token;
            }else if (i == 1) {
                strValue = token;
                break;
            }
            token = strtok(NULL, "\n");
            i++;
        }
        strSpaceKey = strSpace + "\t" + strKey;
        m_MapFileSet.insert(ConfMap::value_type(strSpaceKey, strValue));
        memset(cLine, 0, BUFFER_SIZE_4K * sizeof(char));
    }
    fclose(fd);
    return true;
}

string ConfFile::GetPrivate(const string& strSpace, const string& strKey, const string& strDefault)
{
    string strSpaceKey = strSpace + "\t" + strKey;
    string strValue = strDefault;
    ConfMap::iterator pitem = m_MapFileSet.find(strSpaceKey);

    if (pitem != m_MapFileSet.end()) {
        strValue = (*pitem).second;
    }
    return strValue;
}

string ConfFile::GetPrivate(const char* pcSpace, const char* pcKey, const char* pcDefault)
{
    string strSpace = pcSpace;
    string strKey = pcKey;
    string strDefault = pcDefault;
    return GetPrivate(strSpace, strKey, strDefault);
}

bool ConfFile::SetPrivate(const string& strSpace, const string& strKey, const string& strValue)
{
    string strSpaceKey = strSpace + "\t" + strKey;
    ConfMap::iterator pitem = m_MapFileSet.find(strSpaceKey);

    if (pitem != m_MapFileSet.end()) {
        (*pitem).second = strValue;
    }else{
        m_MapFileSet.insert(ConfMap::value_type(strSpaceKey, strValue));
    }
    return true;
}

bool ConfFile::SetPrivate(const char* pcSpace, const char* pcKey, const char* pcValue)
{
    string strSpace = pcSpace;
    string strKey = pcKey;
    string strValue = pcValue;
    return SetPrivate(strSpace, strKey, strValue);
}

bool ConfFile::ErasePrivate(const string& strSpace, const string& strKey)
{
    string strSpaceKey = strSpace + "\t" + strKey;
    return (m_MapFileSet.erase(strSpaceKey) >= 0);
}

bool ConfFile::ErasePrivate(const char* pcSpace, const char* pcKey)
{
    string strSpace = pcSpace;
    string strKey = pcKey;
    return ErasePrivate(strSpace, strKey);
}

bool ConfFile::EraseSpace(const string& strSpace)
{
    if (m_MapFileSet.size() < 1){
        return false;
    }

    string strSpaceEntry = strSpace + "\t";
    ConfMap::iterator info = m_MapFileSet.begin();

    while(info != m_MapFileSet.end()) {
        string strSpaceKey = (*info).first;
        if (strcmp(strSpaceEntry.c_str(), strSpaceKey.c_str()) == 0) {
            m_MapFileSet.erase(info++);
            continue;
        }
        info++;
    }
    return true;
}

bool ConfFile::EraseSpace(const char* pcSpace)
{
    string strSpace = pcSpace;
    return EraseSpace(strSpace);
}

bool ConfFile::EraseAll()
{
    m_MapFileSet.clear();
    return true;
}

bool ConfFile::UpdateConfFile()
{
    if (access(m_strFile.c_str(), 0) == 0) {
        if (unlink(m_strFile.c_str()) == -1) {
            return false;
        }
    }

    FILE* fd;
    FILE* fdTmp = NULL;
//    char lpPathBuffer[BUFFER_SIZE_4K];
    bool bRet = true;

    if (m_strCipher.length() < 1) {
        fd = fopen(m_strFile.c_str(), "w+b");
        if (fd == NULL) {
            return false;
        }
    }else{
        fd = tmpfile();
        if (fd == NULL) {
            return false;
        }
        fdTmp = fopen(m_strFile.c_str(), "w+b");
        if (fdTmp == NULL) {
            return false;
        }
    }

    ConfMap::iterator info = m_MapFileSet.begin();
    string strSpace = "";

    while (info != m_MapFileSet.end()) {
        string strSpaceKey = (*info).first;
        string strValue = (*info).second;
        string strSpaceTmp = strSpaceKey.substr(0, strSpaceKey.find("\t"));
        string strKey = strSpaceKey.substr(strSpaceKey.find("\t") + 1, strSpaceKey.length());
        if (strSpace != strSpaceTmp) {
            strSpace = strSpaceTmp;
            strSpaceTmp = "[" + strSpaceTmp + "]\n";
            if (fputs(strSpaceTmp.c_str(), fd) < 0) {
                bRet = false;
                break;
            }
        }
        strKey = strKey + "=" + strValue + "\n";
        if (fputs(strKey.c_str(), fd) < 0) {
            bRet = false;
            break;
        }
        info++;
    }
    fflush(fd);

    if (m_strCipher.length() > 0) {
        if (fseek(fd, 0, SEEK_END) == 0) { 
            fpos_t ipos;
            if (fgetpos(fd, &ipos) == 0) {
                int isize = ipos.__pos;
                if (fseek(fd, 0, SEEK_SET) == 0) { 
                    char* pData = new char[((isize + 7) * 2)];
                    char* pDataEns = new char[((isize + 7) * 2)];
                    if (fread((char*)pData, sizeof(char), isize, fd) == (size_t)isize) {
                        int iRet;
                        iRet = m_Arithmetic.TeaEncode((char*)pData, isize, (char*)m_strCipher.c_str(), (char*)pDataEns);
                        if (iRet) {
                            iRet = m_Arithmetic.AsciiToHex((char*)pDataEns, iRet, (char*)pData);
                            fwrite((char*)pData, sizeof(char), iRet, fdTmp);
                        }
                    }
                    delete[] pData;
                    delete[] pDataEns;
                }
            }
        }
        fclose(fd);
        fclose(fdTmp);
    }
    return bRet;
}

void ConfFile::Lock()
{
    pthread_mutex_lock(&m_thMutex);
}

void ConfFile::Unlock()
{
    pthread_mutex_unlock(&m_thMutex);
}
