/*
 * DrLog.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef _INC_DRLOG_
#define _INC_DRLOG_

#include <string>
using namespace std;

#include "LogManager.h"

#define FileLog(fileNamePre, format, ...) LogManager::GetLogManager()->Log(__FILE__, __LINE__, LOG_DEBUG, format, ## __VA_ARGS__)

// add by samson，把定义放到头文件，给外部知道
#define MAX_LOG_BUFFER 200 * 1024

#endif
