#ifndef BaseCSLock_h__
#define BaseCSLock_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	BaseCSLock.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:29:12
// 说明			:	锁基类：需要一个锁的类，直接用private继承，在方法中使用AutoLock
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Singleton_Export.h"
#define AutoLock CSLock lockCS(this->GetLock());

class SINGLETON_API BaseCSLock
{
public:
    BaseCSLock() 
    {
        InitializeCriticalSection(&_lock);
    }

    virtual ~BaseCSLock() 
    {
        DeleteCriticalSection(&_lock);
    }

    LPCRITICAL_SECTION GetLock() { return &_lock; }

private:
    CRITICAL_SECTION	_lock;
};

//#define CSLOCK CSLock lockCS(&_csLock);
//#define SetCSLock(e) CSLock lockCS(&e);

/************************************************************************/
/* 锁基类：需要一个锁的类，直接继承即可
/* 注意：凡是需要加锁的对象，锁处理应封装在类中
/************************************************************************/
class SINGLETON_API CSLock
{
public:
	CSLock(LPCRITICAL_SECTION pLock) 
		: _pLock(pLock) 
	{
		EnterCriticalSection(pLock);
	}
	virtual ~CSLock() 
	{
		LeaveCriticalSection(_pLock);
	}

private:
	LPCRITICAL_SECTION	_pLock;
};
#endif // BaseCSLock_h__