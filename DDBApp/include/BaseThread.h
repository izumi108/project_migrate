#ifndef BaseThread_h__
#define BaseThread_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	BaseThread.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:28:38
// 说明		:	线程基类，通过ThraedManager可以根据ID获得指针
Remark		:

实现一个工作线程
	class XXXThread
		: public BaseThread
	{
	public:
		XXXThread()
			: BaseThread(_T(""))	// 基类初始化，可传递一个线程名
			, ...			// 成员初始化
		{
			// 设置为自动删除对象，运行后不关心资源释放
			// 否则需要处理线程对象释放
			SetAutoDelete(true);
		}

		 virtual unsigned long Main()
		{
			// 线程逻辑代码
			...
			CHECK_EXIT;		// 在适合的地方放置该宏做退出检测，建议为不可分割的原子功能段后，发送消息通知前
			...				// 又做了一些操作
			CHECK_EXIT;

			while( !WaitExit(1000))	// 1秒检测一次系统是否关闭，没有则执行业务逻辑代码
			{
				// 该方式适合定时循环执行的线程
				// 注意：WaitExit前面的 ！
				// 此循环只有在两种情况下才会打断
				// 1. 系统退出				Singleton::GetEventManager()->ExitSystem();
				// 2. 实例Exit方法被调用	pThread->Exit();
				// 这里面一般不用再写 CHECK_EXIT; 检测宏


				...			// 循环执行的代码段
			}

			return 0xdead;
		}

	private:
		... // private

		// 警告：
		// 线程内不允许持有界面和控件指针
	};

调用一个线程：
	XXXThread* pThread = new XXXThread;
	...	//二段构造代码
	pThread->Run();	// 运行线程

	如果	SetAutoDelete(true); 则pThread实例自动删除
	否则 创建者需要管理实例生命周期，不然会发生内存泄漏

	// 命名线程对象，可如下操作
	// 查询名称为：ThreadName 的线程是否存在
	Singleton::GetThreadManager()->IsExistThread(_T("ThreadName"));
/////////////////////////////////////////////////////////////*/
#pragma once
#pragma warning (disable:4251)
#include "Singleton_Export.h"

#define CHECK_EXIT  if (CheckExit()) return 0;
#define WAIT_EXIT(t)   if (WaitExit(t)) return 0;

class SINGLETON_API BaseThread
{
    friend class ThreadManager;
public:

    BaseThread(const CDXString id=_T("")); // id：线程id，不重复的非空字符串
    virtual ~BaseThread();

    // 设定是否在线程结束时删除本对象
    void SetAutoDelete(bool value);

    // 启动线程，返回成功或失败
    bool Run();

    // 停止线程
    void Exit();

	bool IsRunning();

	HANDLE GetThreadHandle() { return m_hThread; };
    int GetTag() { return m_nTag; };
protected:
	int				m_nInternalID;	// 内部分配线程数字ID
    int             m_nGroup;       // 线程分组
    int             m_nTag;			// 线程标识
	UINT			m_nThreadID;	// 活动线程ID
    HANDLE          m_hThread;      // 线程句柄
    CDXString       m_ThreadName;	// 用户设置线程名
	CDXString       m_deviceID;     // 对应的设备id,为空表示无对应设备；设备断开时用来终止对应的线程
	CDXString		m_strThreadID;	// 内部跟踪名
    
    // 下面是退出机制，子类可以使用二个方法，或直接使用变量

    // 检测退出标志
    virtual bool CheckExit();

    // 等待退出内核对象：系统或用户自己调用Exit函数
    // return : bool 是否要退出
    virtual bool WaitExit(DWORD dwMilliseconds=INFINITE);

	// 等待线程句柄：线程是否已退出
	bool WaitTerminate(DWORD dwMilliseconds=INFINITE);

	// 强制结束线程
	bool Terminate(DWORD dwExitCode = 0xdeaddead);

    HANDLE      m_hExitEvent;        // 线程退出事件  
    bool        m_bExit;              // 线程退出标志变量

    HANDLE      m_hSystemExitEvent;  // 系统退出事件  
    volatile const bool* m_pSystemExit;       // 系统退出标志变量

    bool        _autoDelete;        // 线程结束时删除本对象
	
private:

    // 线程执行函数    
    static UINT WINAPI ThreadMain( LPVOID lpParam ) ;

    // 实际线程执行函数，子类实现
    virtual unsigned long Main() = 0;
};
#endif // BaseThread_h__