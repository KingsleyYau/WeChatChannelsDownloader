/*
 * author: Samson.Fan
 *   date: 2015-03-30
 *   file: IAutoLock.h
 *   desc: 跨平台锁
 */

#pragma once

// 普通锁接口类
class IAutoLock
{
public:
	static IAutoLock* CreateAutoLock();
	static void ReleaseAutoLock(IAutoLock* lock);
public:
	IAutoLock() {}
	virtual ~IAutoLock() {}
public:
	virtual bool Init() = 0;
	virtual bool TryLock() = 0;
	virtual bool Lock() = 0;
	virtual bool Unlock() = 0;
};

// 读写锁接口类
//class IAutoRWLock
//{
//public:
//	static IAutoRWLock* CreateAutoRWLock();
//	static void ReleaseAutoRWLock(IAutoRWLock* lock);
//public:
//	IAutoRWLock() {}
//	virtual ~IAutoRWLock() {}
//public:
//	virtual bool Init() = 0;
//	virtual void WriteLock() = 0;
//	virtual void ReadLock() = 0;
//	virtual void Unlock() = 0;
//};
