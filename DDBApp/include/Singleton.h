#ifndef Singleton_h__
#define Singleton_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	Singleton.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:14:33
// 说明		:	单例中心
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Singleton_Export.h"
#include "ExitEvent.h"
#include "IExit.h"
#include "Message.h"
#include "MessageCenter.h"
#include "MessagePump.h"
#include "ModuleManager.h"
#include "ThreadManager.h"
#include "EventManager.h"
#include "TimerManager.h"
#include "UIHungCheakThread.h"
#include "log4z.h"

// Singleton： 单例类
// 需要单例的类都放在这里，以便控制初始化顺序
// 需要单例的类：须将构造函数私有化，并设置Singleton为friend class
class SINGLETON_API Singleton
{
public:
	static void Init(HINSTANCE hInstance);
    static void Destory();

    static HWND GetMessageWnd();

    static ThreadManager*   GetThreadManager();
    static TimerManager*    GetTimerManager();
    static EventManager*    GetEventManager();
    static ModuleManager*   GetModuleManager();
    static MessageCenter*   GetMessageCenter();
	static ILog4zManager*	GetLogManager();
	static MessagePump*			GetMessagePump();

private:
    Singleton();
    ~Singleton();
    static Singleton*   s_pInstance;
    static HINSTANCE         s_hInstance;

    static ThreadManager*	s_pThreadManager;
    static TimerManager*		s_pTimerManager;
    static EventManager*		s_pEventManager;
    static ModuleManager*	s_pModuleManager;
    static MessageCenter*	s_pMessageCenter;
	static MessagePump*		s_pMessagePump;
};

#endif // Singleton_h__