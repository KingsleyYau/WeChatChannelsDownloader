/**
  * Author	: Samson
  * Date	: 2016-01-26
  * Description	: for check momory leak
  * File	: CheckMemoryLeak.h
  */

#pragma once

#ifdef _CHECK_MEMORY_LEAK


void * operator new(size_t size);
void * operator new(size_t size, const char *file, int line);
#ifndef _WIN32
void * operator new[](size_t size, const char *file, int line);
#endif

void operator delete(void * p);
void operator delete[](void * p);
void operator delete(void * p, const char *file, int line);
void operator delete[](void * p, const char *file, int line);

#define new new(__FILE__, __LINE__)
//#define delete delete(__FILE__, __LINE__)

#endif

void OutputMemoryLeakInfo(const char* dir = NULL);
