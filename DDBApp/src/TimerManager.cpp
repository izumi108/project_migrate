#include "stdafx.h"
#include "TimerManager.h"
#include "BaseTimer.h"

TimerManager::TimerManager()
{
    _hTimerQueue = CreateTimerQueue();
    if (NULL == _hTimerQueue)
    {
        //TRACE(_T("CreateTimerQueue failed"));
    }
}

TimerManager::~TimerManager(void)
{
    if ( _hTimerQueue )
        DeleteTimerQueue(_hTimerQueue);
}

void TimerManager::AddTimer( BaseTimer* pTimer )
{
    pTimer->CreateTimer(_hTimerQueue);
}

void TimerManager::DelTimer( BaseTimer* pTimer )
{
	if (_hTimerQueue && pTimer->_hTimer )
		if (! DeleteTimerQueueTimer(_hTimerQueue, pTimer->_hTimer, INVALID_HANDLE_VALUE))
		{
			//TRACE(_T("DeleteTimerQueueTimer failed"));
		}
}


