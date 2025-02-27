#ifndef BaseTimer_h__
#define BaseTimer_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	BaseTimer.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:28:16
// 说明		:	定时器基类
Remark		:
	实现一个定时器回调
	class XXXTimer
		: public BaseTimer
	{
	public:
		XXXTimer()
			: BaseTimer(0,1000)
		{
		}

		~XXXTimer()
		{
			// BaseTimer基类调用了StopTimer()函数关闭
		}

		virtual void OnTimer()
		{
			// 该回调发生在非主线程中
			// 由系统定时器队列复用某个工作线程执行
			// 可结合消息中心使用，防止跨线程问题

			...	// 回调代码
		}
	}

	使用方法:
	XXXTimer m_XXXTimer;		// 声明为某个类成员

	m_XXXTimer.StartTimer();	// 合适的地方开始定时器
	m_XXXTimer.StopTimer();		// 不再使用可停止，停止后也可再次打开
/////////////////////////////////////////////////////////////*/
#pragma once

#include "BaseThread.h"
#include "BaseCSLock.h"

// Timer基类
class SINGLETON_API BaseTimer
	: private BaseCSLock
{
    friend class TimerManager;

public:
    // Parameter: waitTime   启动后，等待多少毫秒第一次触发
    // Parameter: period    第一次触发后，每隔多少毫秒触发一次
    //                      若 period = 0 则仅waitTime后触发一次
    BaseTimer(DWORD waitTime, DWORD period);
    ~BaseTimer(void);

    // 传入this对象，子类实现中应dynamic_cast转为自身指针
    // 
    // 注意！！！此函数须快速返回，若有长时间操作请使用线程或发消息
    // 
    virtual void OnTimer() = 0;

    // 创建（自动启动） timer
    void StartTimer();

    // 删除 Timer （等待已触发的TimerRoutine结束后返回）
    // 注意：不可在 OnTimer() 的实现中调用
    void StopTimer();   

private:
    void CreateTimer(HANDLE timerQueue);
	static VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired);
    HANDLE _hTimer;     // timer
    DWORD _waitTime;    // 启动线程后，过多少毫秒第一次触发
    DWORD _period;      // 第一次触发后，每隔多少毫秒触发一次
	bool _IsStop;
};
#endif // BaseTimer_h__
