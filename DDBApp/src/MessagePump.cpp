#include "stdafx.h"
#include "MessagePump.h"


MessagePump::MessagePump(void)
{
}


MessagePump::~MessagePump(void)
{
}

void MessagePump::InitMessageCenter()
{

}

LRESULT MessagePump::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( uMsg == WM_EVENT_MESSAGE )
	{
		IMessage* pMessage = reinterpret_cast<IMessage*>(lParam);
		Singleton::GetMessageCenter()->Send(pMessage);
		delete pMessage;		// 消息接收者负责释放内存
		return 0;
	}
	else if ( uMsg == WM_EVENT_GROUP_BEGIN )
	{
		IMessage* pMessage = reinterpret_cast<IMessage*>(lParam);
		Singleton::GetMessageCenter()->Begin(pMessage);
		delete pMessage;
		return 0;
	}
	else if ( uMsg == WM_EVENT_GROUP_END)
	{
		IMessage* pMessage = reinterpret_cast<IMessage*>(lParam);
		Singleton::GetMessageCenter()->End(pMessage);
		delete pMessage;
		return 0;
	}

	return BaseWindow::WindowProc(uMsg,wParam,lParam);
}

LPCTSTR MessagePump::GetWindowClassName() const
{
	return _T("MessagePump");
}
