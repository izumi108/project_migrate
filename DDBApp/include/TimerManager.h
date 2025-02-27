#ifndef TimerManager_h__
#define TimerManager_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	TimerManager.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:27:37
// 说明		:	定时器管理器
/////////////////////////////////////////////////////////////*/
#pragma once

class BaseTimer;

// 定时器管理
class SINGLETON_API TimerManager
{
    friend class Singleton;
public:

    void AddTimer(BaseTimer* pTimer);
    void DelTimer(BaseTimer* pTimer);

private:
    TimerManager();    // private，只能在Singleton中创建
    ~TimerManager();

    void CreateTimer();

    HANDLE _hTimerQueue;    // timer队列
};
#endif // TimerManager_h__
