#ifndef MessageManager_h__
#define MessageManager_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	MessageManager.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2015-03-20 14:27:27
// ˵��			:	��Ϣ����ת���¼���PumpWindow
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
