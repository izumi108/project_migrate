#include "stdafx.h"
#include "BaseTimer.h"
#include "Singleton.h"
#include "TimerManager.h"

BaseTimer::BaseTimer(DWORD waitTime, DWORD period)
{
    _waitTime = waitTime;
    _period = period;
    _hTimer = NULL;
	_IsStop = true;
}

BaseTimer::~BaseTimer(void)
{
    StopTimer();
}

VOID CALLBACK BaseTimer::TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    BaseTimer* pTimer = (BaseTimer*)lpParam;
	if(!pTimer->_IsStop)
    pTimer->OnTimer();

}

void BaseTimer::CreateTimer(HANDLE timerQueue)
{
    if (!CreateTimerQueueTimer( &_hTimer, timerQueue, 
        (WAITORTIMERCALLBACK)TimerRoutine, (PVOID)this, _waitTime, _period,
        WT_EXECUTEONLYONCE))    // 若当前已触发TimerRoutine，不重复触发
    {
		//TRACE(_T("CreateTimerQueueTimer fail"));
    }
}

void BaseTimer::StartTimer()
{
	AutoLock;
	_IsStop=false;
	if(!_hTimer)
	{
		Singleton::GetTimerManager()->AddTimer(this);
	}
}

void BaseTimer::StopTimer()
{
	AutoLock;
	_IsStop=true;
	if(_hTimer)
	{
		Singleton::GetTimerManager()->DelTimer(this);
		_hTimer=NULL;
	}
}


