#include "stdafx.h"
#include "EventManager.h"

EventManager::EventManager()
{
    _systemExitEvent = CreateEvent(NULL, true, false, NULL);
    _systemExit = false;
}

EventManager::~EventManager()
{
    if ( _systemExitEvent )
        CloseHandle(_systemExitEvent);
}

HANDLE EventManager::SystemExitEvent()
{
    HANDLE returnEvent = NULL;

    if ( ! DuplicateHandle(GetCurrentProcess(), 
        _systemExitEvent, 
        GetCurrentProcess(),
        &returnEvent, 
        0,
        FALSE,
        DUPLICATE_SAME_ACCESS))
    {
        //LOG->Write(Log::LOG_ALERT, L"EventManager::DupHandle() fail: %d", GetLastError());
    }

    return returnEvent;
}

void EventManager::ExitSystem()
{
    _systemExit = true;
    SetEvent(_systemExitEvent);
}

const bool* EventManager::SystemExitFlag()
{
    return &_systemExit;
}
