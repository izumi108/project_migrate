
#include "stdafx.h"
#include "ExitEvent.h"

ExitEvent* ExitEvent::_pInstance = NULL;
ExitEvent* ExitEvent::GetInstance()
{
	if (_pInstance)
		return _pInstance;
	_pInstance = new ExitEvent();
	return _pInstance;
}

void ExitEvent::Release()
{
	if (_pInstance)
		delete _pInstance;
	_pInstance = NULL;
}

ExitEvent::ExitEvent()
{

}

ExitEvent::~ExitEvent()
{

}

void ExitEvent::AddExitEvent( IExit * iexit )
{
	// 注册退出事件到队尾
	m_CallExit.push_back(iexit);
}

bool ExitEvent::RunExit()
{
	// 栈辗转开解
	// 从队尾开始执行，确保最后注册事件最先执行
	std::vector<IExit*>::reverse_iterator iter =m_CallExit.rbegin();
	std::vector<IExit*>::reverse_iterator end = m_CallExit.rend();
	while (iter != end)
	{
		if (!(*iter)->OnExit())
			return false;
		++iter;
	}
	//for (size_t i=(m_CallExit.size()-1);i<=0;--i)
	//{
	//	if (!m_CallExit.at(i)->OnExit())
	//		return false;
	//}
	return true;
}

void ExitEvent::DeleteExitEvent( IExit * iexit )
{
	for(vector<IExit*>::iterator it = m_CallExit.begin(); it != m_CallExit.end(); it++)
	{
		IExit *dev = *it;
		if ( dev == iexit )
		{
			m_CallExit.erase(it);
			break;
		}
	}
}
