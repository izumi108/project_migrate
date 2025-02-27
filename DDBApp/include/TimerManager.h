#ifndef TimerManager_h__
#define TimerManager_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	TimerManager.h
// ������	: 	daviyang35@QQ.com
// ����ʱ��	:	2014-07-05 12:27:37
// ˵��		:	��ʱ��������
/////////////////////////////////////////////////////////////*/
#pragma once

class BaseTimer;

// ��ʱ������
class SINGLETON_API TimerManager
{
    friend class Singleton;
public:

    void AddTimer(BaseTimer* pTimer);
    void DelTimer(BaseTimer* pTimer);

private:
    TimerManager();    // private��ֻ����Singleton�д���
    ~TimerManager();

    void CreateTimer();

    HANDLE _hTimerQueue;    // timer����
};
#endif // TimerManager_h__
