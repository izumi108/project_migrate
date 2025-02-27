#ifndef BaseTimer_h__
#define BaseTimer_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	BaseTimer.h
// ������	: 	daviyang35@QQ.com
// ����ʱ��	:	2014-07-05 12:28:16
// ˵��		:	��ʱ������
Remark		:
	ʵ��һ����ʱ���ص�
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
			// BaseTimer���������StopTimer()�����ر�
		}

		virtual void OnTimer()
		{
			// �ûص������ڷ����߳���
			// ��ϵͳ��ʱ�����и���ĳ�������߳�ִ��
			// �ɽ����Ϣ����ʹ�ã���ֹ���߳�����

			...	// �ص�����
		}
	}

	ʹ�÷���:
	XXXTimer m_XXXTimer;		// ����Ϊĳ�����Ա

	m_XXXTimer.StartTimer();	// ���ʵĵط���ʼ��ʱ��
	m_XXXTimer.StopTimer();		// ����ʹ�ÿ�ֹͣ��ֹͣ��Ҳ���ٴδ�
/////////////////////////////////////////////////////////////*/
#pragma once

#include "BaseThread.h"
#include "BaseCSLock.h"

// Timer����
class SINGLETON_API BaseTimer
	: private BaseCSLock
{
    friend class TimerManager;

public:
    // Parameter: waitTime   �����󣬵ȴ����ٺ����һ�δ���
    // Parameter: period    ��һ�δ�����ÿ�����ٺ��봥��һ��
    //                      �� period = 0 ���waitTime�󴥷�һ��
    BaseTimer(DWORD waitTime, DWORD period);
    ~BaseTimer(void);

    // ����this��������ʵ����Ӧdynamic_castתΪ����ָ��
    // 
    // ע�⣡�����˺�������ٷ��أ����г�ʱ�������ʹ���̻߳���Ϣ
    // 
    virtual void OnTimer() = 0;

    // �������Զ������� timer
    void StartTimer();

    // ɾ�� Timer ���ȴ��Ѵ�����TimerRoutine�����󷵻أ�
    // ע�⣺������ OnTimer() ��ʵ���е���
    void StopTimer();   

private:
    void CreateTimer(HANDLE timerQueue);
	static VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired);
    HANDLE _hTimer;     // timer
    DWORD _waitTime;    // �����̺߳󣬹����ٺ����һ�δ���
    DWORD _period;      // ��һ�δ�����ÿ�����ٺ��봥��һ��
	bool _IsStop;
};
#endif // BaseTimer_h__
