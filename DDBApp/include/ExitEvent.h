#ifndef ExitEvent_h__
#define ExitEvent_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	ExitEvent.h
// ������	: 	daviyang35@QQ.com
// ����ʱ��	:	2014-07-05 12:30:18
// ˵��		:	�˳��¼�֪ͨ�ӿڹ�����
/////////////////////////////////////////////////////////////*/
#pragma once
#include "IExit.h"
#include "Singleton_Export.h"
#include <vector>

#define IEXIT_HANDLE ExitEvent::GetInstance()

class SINGLETON_API ExitEvent
{
private:
	static ExitEvent* _pInstance;   // ����

public:
	// ȡ�õ�������
	static ExitEvent* GetInstance();
	// �ͷŵ�������
	static void Release();

	//���event
	void AddExitEvent(IExit * iexit);

	//�ͷ�event
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
