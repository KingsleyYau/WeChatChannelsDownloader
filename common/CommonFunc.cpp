/*
 * CommonFunc.cpp
 *
 *  Created on: 2015-3-12
 *      Author: Samson.Fan
 * Description: 存放公共函数
 */

#include "CommonFunc.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>

#ifndef _WIN32
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#else
#include <windows.h>
#endif

// 判断文件是否存在
bool IsFileExist(const string& path)
{
	bool result = false;

	struct stat st = {0};
	if (0 == stat(path.c_str(), &st))
	{
		// 不是目录
#ifndef _WIN32
		if (!S_ISDIR(st.st_mode)) 
#else
		if ((st.st_mode & _S_IFDIR) == 0) 
#endif
		{
			result = true;
		}
	}
	return result;
}

// 判断目录是否存在
bool IsDirExist(const string& path)
{
	bool result = false;
	string dirPath = path;
	if (!dirPath.empty() 
		&& (dirPath.at(dirPath.length()-1) == '\\'
			|| dirPath.at(dirPath.length()-1) == '/')) 
	{
		dirPath.erase(dirPath.length()-1);
	}

	struct stat st = {0};
	if (0 == stat(dirPath.c_str(), &st))
	{
		// 是目录
#ifndef _WIN32
		if (S_ISDIR(st.st_mode)) 
#else
		if ((st.st_mode & _S_IFDIR) > 0) 
#endif
		{
			result = true;
		}
	}
	return result;
}

// 新建目录（可多级）
bool MakeDir(const string& path)
{
	bool result = true;

	size_t begin = 0;
	bool isFirstDir = true;
	while (result)
	{
		// 查找目录分隔符
		size_t pos = path.find('/', begin);
		if (string::npos == pos) {
			pos = path.find('\\', begin);
		}

		// 第一级目录不处理
		if (isFirstDir) {
			isFirstDir = false;
		}
		else {
			// 若目录不存在则创建
			string parentDir = path.substr(0, pos);
			if (!IsDirExist(parentDir)) {
				// 设备可读写权限（没有执行权限）
#ifndef _WIN32
//#ifndef IOS
//				result = (0 == mkdir(parentDir.c_str(), 0777));
//#else
                int mod = S_IRWXU | S_IRWXG | S_IRWXO;
                result = (0 == mkdir(parentDir.c_str(), mod));
//#endif
#else
				result = (TRUE == CreateDirectory(parentDir.c_str(), NULL));
#endif
			}
		}

		if (string::npos != pos && path.length()-1 != pos) {
			// 偏移位置
			begin = pos + 1;
		}
		else {
			// 完成遍历
			break;
		}
	}

	return result;
}

// 删除目录（包括目录中所有文件及文件夹）
bool RemoveDir(const string& path)
{
	bool result = false;
	if (IsDirExist(path))
	{
		CleanDir(path);
#ifndef _WIN32
		result = (0 == rmdir(path.c_str()));
#else
		result = (TRUE == RemoveDirectory(path.c_str()));
#endif
	}

	return result;
}

// 删除文件
bool RemoveFile(const string& path)
{
	bool result = true;
	if (IsFileExist(path))
	{
		result = (0 == remove(path.c_str()));
	}
	return result;
}

// 修改文件名
bool RenameFile(const string& srcPath, const string& desPath)
{
	bool result = false;
	if (IsFileExist(srcPath))
	{
		result = (0 == rename(srcPath.c_str(), desPath.c_str()));
	}
	return result;
}

// 复制文件
bool CopyFile(const string& srcPath, const string& desPath)
{
	bool result = false;
    
    if (IsFileExist(srcPath))
	{
        if (srcPath == desPath) {
            result = true;
        }
        else if (!desPath.empty()
                 && desPath.at(desPath.length()-1) != '/'
                 && desPath.at(desPath.length()-1) != '\\')
        {
            // 删除目标文件
            RemoveFile(desPath);

#ifdef _WIN32
            std::locale loc1 = std::locale::global(std::locale(""));
#endif

            ifstream srcStream(srcPath.c_str(), ios::in | ios::binary);
            if (!srcStream.bad())
            {
                ofstream dstStream(desPath.c_str(), ios::out | ios::binary);
                if (!dstStream.bad())
                {
                    dstStream << srcStream.rdbuf();
                    dstStream.close();
                }
                srcStream.close();
            }
#ifdef _WIN32
            std::locale::global(std::locale(loc1));
#endif

            // 判断目标文件是否存在
            result = IsFileExist(desPath);
        }
	}
	return result;
}

// 清空目录（删除目录里所有文件及文件夹）
bool CleanDir(const string& path)
{
	bool result = false;
	if (IsDirExist(path))
	{
#ifndef _WIN32
		struct dirent *dp = NULL;
		DIR *dfd = NULL;

		// 打开目录
		dfd = opendir(path.c_str());
		if (NULL != dfd)
		{
			result = true;
			while ((dp = readdir(dfd)) != NULL)
			{
				// 过滤 . 和 ..
				if (0 == strcmp(dp->d_name, ".")
					|| 0 == strcmp(dp->d_name, ".."))
				{
					continue;
				}

				string subPath = path + '/' + dp->d_name;
				if (IsFileExist(subPath)) {
					// 删除文件
					RemoveFile(subPath);
				}
				else if (IsDirExist(subPath)) {
					// 删除目录
					RemoveDir(subPath);
				}
			}
			closedir(dfd);
		}
#else
		// 修正目录路径
		string dirPath = path;
		if (!dirPath.empty() 
			&& dirPath.at(dirPath.length()-1) != '\\'
			&& dirPath.at(dirPath.length()-1) != '/')
		{
			dirPath += '\\';
		}

		// 遍历目录文件
		WIN32_FIND_DATA ffd; 
		string strFind = dirPath + "*.*";
		HANDLE hFind = FindFirstFile(strFind.c_str(), &ffd);
		if (INVALID_HANDLE_VALUE != hFind)
		{
			while (true)
			{
				if (0 != strcmp(ffd.cFileName, ".")
					&& 0 != strcmp(ffd.cFileName, ".."))
				{
					string subPath = path + '/' + ffd.cFileName;
					if (IsFileExist(subPath)) {
						// 删除文件
						RemoveFile(subPath);
					}
					else if (IsDirExist(subPath)) {
						// 删除目录
						RemoveDir(subPath);
					}
				}

				if(!FindNextFile(hFind, &ffd)) {
					break;
				}
			}

			// 关闭句柄
			FindClose(hFind);
		}
#endif
	}
	return result;
}

// 初始化random
bool InitRandom()
{
	srand( (unsigned)time( NULL ) );
	return true;
}

// 获取random数
int GetRandomValue()
{
	return rand();
}

#ifndef _WIN32
// 获取当前时间（Unix Timestamp）
long GetCurrentTime()
{
	return time(NULL);
}
#endif

