#ifndef EventManager_h__
#define EventManager_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	EventManager.h
// ������	: 	daviyang35@QQ.com
// ����ʱ��	:	2014-07-05 12:31:05
// ˵��		:	�˳��¼�������
/////////////////////////////////////////////////////////////*/
#pragma once

#include "BaseThread.h"
#include "Singleton.h"

// ϵͳ�˳��¼�
class SINGLETON_API EventManager 
{
    friend class Singleton;
public:

    void ExitSystem();          // ϵͳ�˳�֪ͨ��MainDlg::OnClose����

    HANDLE SystemExitEvent();       // ϵͳ�˳���Event�������߸���CloseHandle
    const bool* SystemExitFlag();   // ϵͳ�˳���־����

private:
    EventManager();     // private��ֻ����Singleton�д���
    ~EventManager();

    HANDLE DupHandle(HANDLE h); // ���ƾ��

    HANDLE  _systemExitEvent;   // ϵͳ�˳��¼�
    bool    _systemExit;        // ϵͳ�˳���־��TRUE��ʾ�˳�
                                // ��ע��X86Ӳ����֤�˶�д����ԭ����

};
#endif // EventManager_h__