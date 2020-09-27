/*
 * File         : TimeProc.hpp
 * Date         : 2012-10-19
 * Author       : FGX
 * Copyright    : City Hotspot Co., Ltd.
 * Description  :
 */

#ifndef __TIMEPROC_DEF_H_
#define __TIMEPROC_DEF_H_

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define DAY_USECONDS	86400000
#define GetTickCountDifferences(start, end)    ((start) <= (end) ? (end) - (start) : DAY_USECONDS - (start) + (end))
inline unsigned int GetTickCount()
{
    struct timeval tv;
    if(gettimeofday(&tv, NULL) != 0) {
        return 0;
    }

    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

inline void GetLocalTimeString(char* szTime, int size, time_t tTime)
{
    struct tm *tmLocalTime = localtime(&tTime);
    snprintf(szTime, size, "%d-%d-%d %d:%d:%d"
        , 1900 + tmLocalTime->tm_year
        , tmLocalTime->tm_mon+1
        , tmLocalTime->tm_mday
        , tmLocalTime->tm_hour
        , tmLocalTime->tm_min
        , tmLocalTime->tm_sec);
}

#endif
