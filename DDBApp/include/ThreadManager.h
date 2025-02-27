#ifndef ThreadManager_h__
#define ThreadManager_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	ThreadManager.h
// ������	: 	daviyang35@QQ.com
// ����ʱ��	:	2014-07-05 12:33:18
// ˵��		:	�̹߳�����
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Singleton_Export.h"
#include "BaseThread.h"
#include "BaseCSLock.h"

#pragma warning (disable:4251)

// �̹߳����࣬����
// ���̰߳�ȫ��true
class SINGLETON_API ThreadManager : public BaseCSLock
{
    friend class Singleton;
public:

    // �����߳�����
    void SetThreadID(int nID);

    // ��ȡ�߳�����
    int GetThreadID() const;

    // �����߳�
    void InsertThread(BaseThread* pThread, LPCTSTR lpszID);

    // ȡ�߳�
    BaseThread* GetThread(LPCTSTR lpszID);
    BaseThread* GetThread(int nGUID);

    // �Ƴ��߳�
    void RemoveThread(BaseThread* pThread);

    // �Ƴ��߳�
    void RemoveThread(LPCTSTR lpszName);

    // ��ȡ�ڲ�ID
    int GetInternalID();

    // �˳��߳�
    void ExitThread(LPCTSTR lpszName);
    void ExitThread(int nGUID);

    // �˳����в����豸����ΪdeviceID���߳�
    void ExitThreads(LPCTSTR lpszDeviceID);

	bool IsExistThread(CDXString id);

	bool WaitAllThread(DWORD dwTimedOut=3000);

private:
    ThreadManager();    // private��ֻ����Singleton�д���
    ~ThreadManager();

    // key: �߳�ID
    // value���߳���ָ��
    typedef std::map<CDXString, BaseThread*> THREAD_MAP;

    THREAD_MAP m_threadMap;		// �߳��б�
    int m_nThreadID;			// �߳�����
    int m_nGUIDIndex;			// ȫ������
	int m_nThreadCount;			// ��ֳ�����
};
#endif // ThreadManager_h__