/*
 * CommonFunc.h
 *
 *  Created on: 2015-3-12
 *      Author: Samson.Fan
 * Description: 存放公共函数
 */

#ifndef COMMONFUNCDEFINE_H_
#define COMMONFUNCDEFINE_H_

#include <string>
using namespace std;

#ifndef _WIN32
// 获取数组元素个数
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif

// 判断文件是否存在
bool IsFileExist(const string& path);
// 判断目录是否存在
bool IsDirExist(const string& path);
// 新建目录
bool MakeDir(const string& path);
// 删除目录（包括目录中所有文件及文件夹）
bool RemoveDir(const string& path);
// 删除文件
bool RemoveFile(const string& path);
// 修改文件名
bool RenameFile(const string& srcPath, const string& desPath);
// 复制文件
bool CopyFile(const string& srcPath, const string& desPath);
// 清空目录（删除目录里所有文件及文件夹）
bool CleanDir(const string& path);

// 初始化random
bool InitRandom();
// 获取random数
int GetRandomValue();

#ifdef WIN32
	// include 头文件
	#include <windows.h>
	#include <stdio.h>
	#include <time.h>
	
	// define
	#define snprintf sprintf_s
	#define usleep(x) Sleep((x/1000))

	// function
	inline int gettimeofday(struct timeval *tp, void *tzp)
	{
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;

		GetLocalTime(&wtm);
		tm.tm_year = wtm.wYear - 1900;
		tm.tm_mon = wtm.wMonth - 1;
		tm.tm_mday = wtm.wDay;
		tm.tm_hour = wtm.wHour;
		tm.tm_min = wtm.wMinute;
		tm.tm_sec  = wtm.wSecond;
		tm. tm_isdst = -1;
		clock = mktime(&tm);
		tp->tv_sec = clock;
		tp->tv_usec = wtm.wMilliseconds * 1000;

		return (0);
	}
#else
	// include 头文件
	#include <stdio.h>
	#include <sys/time.h>
	#include <unistd.h>

	// define
	#define Sleep(ms)  usleep(ms * 1000)

#endif

// 获取当前时间（Unix Timestamp ms）
inline long long getCurrentTime()
{
	long long result = 0;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	result =  (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return result;
}

inline long long DiffTime(long long start, long long end)
{
    return end - start;
//	return (end > start ? end - start : (unsigned long)-1 - end + start);
}

#endif /* COMMONFUNCDEFINE_H_ */
