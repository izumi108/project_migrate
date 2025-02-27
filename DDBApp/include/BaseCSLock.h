#ifndef BaseCSLock_h__
#define BaseCSLock_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	BaseCSLock.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2014-07-05 12:29:12
// ˵��			:	�����ࣺ��Ҫһ�������ֱ࣬����private�̳У��ڷ�����ʹ��AutoLock
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
/* �����ࣺ��Ҫһ�������ֱ࣬�Ӽ̳м���
/* ע�⣺������Ҫ�����Ķ���������Ӧ��װ������
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