#ifndef IExit_h__
#define IExit_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	IExit.h
// ������	: 	daviyang35@QQ.com
// ����ʱ��	:	2014-07-05 12:30:36
// ˵��		:	�˳��¼��ӿ�
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Singleton_Export.h"

class SINGLETON_API IExit
{
public:
	IExit();
	~IExit();
	//ȫ��ģ��ı�����������ʱ���ܺ�ֻ��3��
	//�����н����ģ�鶼����̳д˷��������޲�����ֱ��return true
	virtual bool OnExit();

};


#endif // IExit_h__
