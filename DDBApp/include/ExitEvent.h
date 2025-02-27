#ifndef ExitEvent_h__
#define ExitEvent_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ExitEvent.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:30:18
// 说明		:	退出事件通知接口管理器
/////////////////////////////////////////////////////////////*/
#pragma once
#include "IExit.h"
#include "Singleton_Export.h"
#include <vector>

#define IEXIT_HANDLE ExitEvent::GetInstance()

class SINGLETON_API ExitEvent
{
private:
	static ExitEvent* _pInstance;   // 单例

public:
	// 取得单例对象
	static ExitEvent* GetInstance();
	// 释放单力对象
	static void Release();

	//添加event
	void AddExitEvent(IExit * iexit);

	//释放event
	void DeleteExitEvent(IExit * iexit);
	
	bool RunExit();

public:
	ExitEvent();
	~ExitEvent();

private:

#pragma warning(disable:4251)
	std::vector<IExit*> m_CallExit;
#pragma warning(default:4251)

};


#endif // ExitEvent_h__
