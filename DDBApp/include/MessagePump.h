#ifndef MessageManager_h__
#define MessageManager_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	MessageManager.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-03-20 14:27:27
// 说明			:	消息中心转发事件的PumpWindow
/////////////////////////////////////////////////////////////*/
#pragma once
#include "BaseWindow.h"

class SINGLETON_API MessagePump
	: public BaseWindow
{
public:
	MessagePump(void);
	virtual ~MessagePump(void);

	void InitMessageCenter();
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LPCTSTR GetWindowClassName() const;

};

#endif // MessageManager_h__
