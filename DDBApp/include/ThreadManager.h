#ifndef ThreadManager_h__
#define ThreadManager_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ThreadManager.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:33:18
// 说明		:	线程管理器
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Singleton_Export.h"
#include "BaseThread.h"
#include "BaseCSLock.h"

#pragma warning (disable:4251)

// 线程管理类，单例
// 多线程安全：true
class SINGLETON_API ThreadManager : public BaseCSLock
{
    friend class Singleton;
public:

    // 设置线程索引
    void SetThreadID(int nID);

    // 获取线程索引
    int GetThreadID() const;

    // 增加线程
    void InsertThread(BaseThread* pThread, LPCTSTR lpszID);

    // 取线程
    BaseThread* GetThread(LPCTSTR lpszID);
    BaseThread* GetThread(int nGUID);

    // 移除线程
    void RemoveThread(BaseThread* pThread);

    // 移除线程
    void RemoveThread(LPCTSTR lpszName);

    // 获取内部ID
    int GetInternalID();

    // 退出线程
    void ExitThread(LPCTSTR lpszName);
    void ExitThread(int nGUID);

    // 退出所有操作设备对象为deviceID的线程
    void ExitThreads(LPCTSTR lpszDeviceID);

	bool IsExistThread(CDXString id);

	bool WaitAllThread(DWORD dwTimedOut=3000);

private:
    ThreadManager();    // private，只能在Singleton中创建
    ~ThreadManager();

    // key: 线程ID
    // value：线程类指针
    typedef std::map<CDXString, BaseThread*> THREAD_MAP;

    THREAD_MAP m_threadMap;		// 线程列表
    int m_nThreadID;			// 线程索引
    int m_nGUIDIndex;			// 全局索引
	int m_nThreadCount;			// 活动现场计数
};
#endif // ThreadManager_h__