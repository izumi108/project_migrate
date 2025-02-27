#include "stdafx.h"
#include "Singleton.h"

#include "ThreadManager.h"
#include "TimerManager.h"
#include "EventManager.h"
#include "ModuleManager.h"
#include "Message.h"
#include "MessagePump.h"

Singleton*              Singleton::s_pInstance              = NULL;
HINSTANCE               Singleton::s_hInstance              = NULL;
ThreadManager*      Singleton::s_pThreadManager     = NULL;
TimerManager*       Singleton::s_pTimerManager      = NULL;
EventManager*       Singleton::s_pEventManager      = NULL;
ModuleManager*  Singleton::s_pModuleManager = NULL;
MessageCenter*      Singleton::s_pMessageCenter     = NULL;
MessagePump*        Singleton::s_pMessagePump           = NULL;

Singleton::Singleton()
{
}

void Singleton::Init(HINSTANCE hInstance)
{
    s_hInstance = hInstance;
    ILog4zManager::getInstance()->start();
    IEXIT_HANDLE;
    GetMessagePump();
    GetMessageCenter();
    GetTimerManager();
    GetThreadManager();
    GetEventManager();
    GetModuleManager();
    CUIHungCheckThread::GetInstance();
}

void Singleton::Destory()
{
    s_hInstance = NULL;
    CUIHungCheckThread::ReleaseInstance();
    SAFE_DELETE(s_pEventManager);
    SAFE_DELETE(s_pMessageCenter);
    SAFE_DELETE(s_pTimerManager);
    //SAFE_DELETE(s_pThreadManager  );
    SAFE_DELETE(s_pModuleManager);
    SAFE_DELETE(s_pInstance);
    ILog4zManager::freeInstance();
}

ThreadManager* Singleton::GetThreadManager()
{
    if(! s_pThreadManager)
    {
        s_pThreadManager = new ThreadManager();
    }

    return s_pThreadManager;
}

TimerManager* Singleton::GetTimerManager()
{
    if(! s_pTimerManager)
    {
        s_pTimerManager = new TimerManager();
    }

    return s_pTimerManager;
}

EventManager* Singleton::GetEventManager()
{
    if(! s_pEventManager)
    {
        s_pEventManager = new EventManager();
    }

    return s_pEventManager;
}

ModuleManager* Singleton::GetModuleManager()
{
    if(! s_pModuleManager)
    {
        s_pModuleManager = new ModuleManager();
    }

    return s_pModuleManager;
}

MessageCenter* Singleton::GetMessageCenter()
{
    if(! s_pMessageCenter)
    {
        s_pMessageCenter = new MessageCenter();
    }

    return s_pMessageCenter;
}

ILog4zManager* Singleton::GetLogManager()
{
    return ILog4zManager::getInstance();
}

MessagePump* Singleton::GetMessagePump()
{
    if(!s_pMessagePump)
    {
        s_pMessagePump = new MessagePump;
        s_pMessagePump->SetHinstance(s_hInstance);
        HWND hWnd = s_pMessagePump->Create(HWND_MESSAGE, _T("PumpWindow"), WS_POPUPWINDOW , 0);

        if(hWnd == NULL)
        {
            delete s_pMessagePump;
            s_pMessagePump = NULL;
            return s_pMessagePump;
        }
    }

    return s_pMessagePump;
}

Singleton::~Singleton()
{

}

HWND Singleton::GetMessageWnd()
{
    MessagePump* pMessagePump = GetMessagePump();

    if(pMessagePump)
    {
        return s_pMessagePump->GetHWND();
    }

    return NULL;
}

