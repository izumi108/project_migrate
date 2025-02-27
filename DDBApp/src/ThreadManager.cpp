#include "stdafx.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager()
	: m_nGUIDIndex(1)
	, m_nThreadID(0)
	, m_nThreadCount(0)
{   
}

ThreadManager::~ThreadManager()
{
    AutoLock;
    for(THREAD_MAP::iterator it = m_threadMap.begin(); it != m_threadMap.end(); ++it)
    {
		SAFE_DELETE(it->second);
    }
}

void ThreadManager::SetThreadID( int nID )
{
    m_nThreadID = nID;
}

int ThreadManager::GetThreadID() const
{
    return m_nThreadID;
}

void ThreadManager::InsertThread(BaseThread* pThread, LPCTSTR lpszID)
{
    AutoLock;
    m_threadMap[lpszID] = pThread;
	++m_nThreadCount;
	LOGFMTT("活动线程++：%d",m_nThreadCount);
	LOGFMTT("ThreadManager::InsertThread : %s",lpszID);
}

void ThreadManager::RemoveThread( BaseThread* pThread )
{
    AutoLock;
    for(THREAD_MAP::iterator it = m_threadMap.begin(); it != m_threadMap.end(); ++it)
    {
        if (it->second == pThread)
        {
            m_threadMap.erase(it);
			--m_nThreadCount;
			LOGFMTT("活动线程--：%d",m_nThreadCount);
			LOGFMTT("ThreadManager::RemoveThread : %s",pThread->m_strThreadID.c_str());
            return;
        }
    }
}

void ThreadManager::RemoveThread(LPCTSTR lpszID)
{

}

BaseThread* ThreadManager::GetThread( LPCTSTR lpszID )
{
	AutoLock;
    return m_threadMap[lpszID];
}

BaseThread* ThreadManager::GetThread( int nGUID )
{
    AutoLock;
    for(THREAD_MAP::iterator it = m_threadMap.begin(); it != m_threadMap.end(); ++it)
    {
		if ( it->second )
		{
			if (it->second->m_nTag == nGUID)
			{
				return it->second;
			}
		}
    }
    return NULL;
}

int ThreadManager::GetInternalID()
{
    return m_nGUIDIndex ++;
}

void ThreadManager::ExitThread(LPCTSTR lpszName )
{
    AutoLock;
	if (lpszName && m_threadMap.find(lpszName) != m_threadMap.end())
	{
		m_threadMap[lpszName]->Exit();
	}
}

void ThreadManager::ExitThread( int nGUID )
{
    AutoLock;
    for(THREAD_MAP::iterator it = m_threadMap.begin(); it != m_threadMap.end(); ++it)
    {
		if ( it->second )
		{
			if (it->second->m_nTag == nGUID)
			{
				it->second->Exit();
				return;
			}
		}
    }    
}

void ThreadManager::ExitThreads( LPCTSTR lpszDeviceID )
{
    AutoLock;
    for(THREAD_MAP::iterator it = m_threadMap.begin(); it != m_threadMap.end(); ++it)
    {
		if ( it->second )
		{
			if (it->second->m_deviceID == lpszDeviceID)
			{
				it->second->Exit();
			}
		}
    }
}

bool ThreadManager::IsExistThread( CDXString id )
{
	AutoLock;
	if (!id.empty() && m_threadMap.find(id) != m_threadMap.end())
	{
		return true;
	}	
	return false;
}

bool ThreadManager::WaitAllThread(DWORD dwTimedOut/*=3000*/)
{
	DWORD dwWaitCounter = 0;
	while (true)
	{
		::Sleep(10);
		dwWaitCounter+=10;
		if ( m_nThreadCount ==0 )
		{
			return true;
		}
		if ( dwWaitCounter >= dwTimedOut )
		{
			return false;
		}
	}
	return false;
}

