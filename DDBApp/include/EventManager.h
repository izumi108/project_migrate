#ifndef EventManager_h__
#define EventManager_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	EventManager.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:31:05
// 说明		:	退出事件管理器
/////////////////////////////////////////////////////////////*/
#pragma once

#include "BaseThread.h"
#include "Singleton.h"

// 系统退出事件
class SINGLETON_API EventManager 
{
    friend class Singleton;
public:

    void ExitSystem();          // 系统退出通知，MainDlg::OnClose调用

    HANDLE SystemExitEvent();       // 系统退出的Event，调用者负责CloseHandle
    const bool* SystemExitFlag();   // 系统退出标志变量

private:
    EventManager();     // private，只能在Singleton中创建
    ~EventManager();

    HANDLE DupHandle(HANDLE h); // 复制句柄

    HANDLE  _systemExitEvent;   // 系统退出事件
    bool    _systemExit;        // 系统退出标志：TRUE表示退出
                                // 附注：X86硬件保证了读写操作原子性

};
#endif // EventManager_h__