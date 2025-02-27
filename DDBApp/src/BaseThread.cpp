#include "stdafx.h"
#include <excpt.h>
#include "BaseThread.h"
#include "ThreadManager.h"
#include "EventManager.h"


CDXStringW MbcsToUnicode(LPCSTR lpcszString)
{
    int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
    UINT nLength = strlen(lpcszString);
    int nChar = ::MultiByteToWideChar(codepage, 0, lpcszString, nLength, NULL, 0);
    wchar_t *pUnicode = new wchar_t[nChar + 1];
    ZeroMemory(pUnicode, (nChar + 1) * sizeof(wchar_t));
    ::MultiByteToWideChar(codepage, 0, lpcszString, nLength, (LPWSTR)pUnicode, nChar);
    CDXStringW wString = (wchar_t *)pUnicode;
    delete [] pUnicode;
    return wString;
}

CDXStringA UnicodeToMbcs(LPCWSTR lpwcszWString)
{
    UINT nLength = wcslen(lpwcszWString);
    int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
    int nChar = ::WideCharToMultiByte(codepage, 0, lpwcszWString, nLength, NULL, 0, NULL, NULL);
    char *pElementText = new char[nChar + 1];
    ZeroMemory(pElementText, (nChar + 1)* sizeof(char));
    ::WideCharToMultiByte(codepage, 0, lpwcszWString, nLength, pElementText, nChar, NULL, NULL);
    CDXStringA strReturn(pElementText);
    delete [] pElementText;
    return strReturn;
}

CDXString format(LPCTSTR lpszFormat, ...)
{
    CDXString sFormat = lpszFormat;
    // Do ordinary printf replacements
    // NOTE: Documented max-length of wvsprintf() is 1024
    TCHAR szBuffer[1025] = { 0 };
    va_list argList;
    va_start(argList, lpszFormat);
    int iRet = ::wvsprintf(szBuffer, sFormat.c_str(), argList);
    va_end(argList);
    szBuffer[iRet] = _T('\0');
    return szBuffer;
}

const DWORD kVCThreadNameException = 0x406D1388;

typedef struct tagTHREADNAME_INFO {
	DWORD dwType;  // Must be 0x1000.
	LPCSTR szName;  // Pointer to name (in user addr space).
	DWORD dwThreadID;  // Thread ID (-1=caller thread).
	DWORD dwFlags;  // Reserved for future use, must be zero.
} THREADNAME_INFO;

// This function has try handling, so it is separated out of its caller.
void SetThreadDebugName(DWORD thread_id, const char* name)
{
	//只在调试附加的时候生效 
	if (!::IsDebuggerPresent()) 
		return;
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name;
	info.dwThreadID = thread_id;
	info.dwFlags = 0;

	// 设置线程名最终实现是基于异常的，等待调试器捕获响应
	// 使用try防止没有调试器捕获到异常导致程序崩溃
	// 上面的IsDebuggerPresent是一个比较好的防御
	__try
	{
		RaiseException(kVCThreadNameException, 0, sizeof(info)/sizeof(DWORD), reinterpret_cast<DWORD_PTR*>(&info));
	}
	__except(EXCEPTION_CONTINUE_EXECUTION) 
	{
	}
}

BaseThread::BaseThread(const CDXString id)
    : m_bExit(false)
    , _autoDelete(false)
    , m_nGroup(0)
    , m_nTag(0)
	, m_nThreadID(0)
    , m_hThread(NULL)
{
    // 复制系统退出的Event
    m_hSystemExitEvent = Singleton::GetEventManager()->SystemExitEvent();
    m_pSystemExit = Singleton::GetEventManager()->SystemExitFlag();
    // 分配一个内部跟踪ID
    m_nInternalID = Singleton::GetThreadManager()->GetInternalID();
    m_hExitEvent = CreateEvent(NULL, true, false, NULL);
}

BaseThread::~BaseThread()
{
    if(m_hExitEvent)
    {
        ::CloseHandle(m_hExitEvent);
        m_hExitEvent = NULL;
    }

    if(m_hSystemExitEvent)
    {
        ::CloseHandle(m_hSystemExitEvent);
        m_hSystemExitEvent = NULL;
    }

    if(m_hThread)
    {
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

UINT WINAPI BaseThread::ThreadMain(LPVOID lpParam)
{
    RETURN_NULL_IF(! lpParam);
    ULONG dwExitCode = 0;
    BaseThread *pThread = (BaseThread *) lpParam;
    // 这里可以通过C++ RTTI获取派生类的名字，内部跟踪ID拼接字符串：'派生类名_InternalID'
    CDXStringA strThreadName;

    if(pThread->m_ThreadName.empty())
    {
		const LPCSTR pszRawName = typeid(*pThread).raw_name();
        pThread->m_strThreadID = format(_T("%s_%d"), MbcsToUnicode(pszRawName).c_str(), pThread->m_nInternalID);
        strThreadName = UnicodeToMbcs(pThread->m_strThreadID.c_str());
#ifdef _DEBUG
		// 非调试模式直接禁用掉
		SetThreadDebugName(pThread->m_nThreadID,strThreadName.c_str());
#endif
    }

	LOGFMTD("Thread Begin:%s", strThreadName.c_str());

	if(! pThread->m_strThreadID.empty())
	{
		Singleton::GetThreadManager()->InsertThread(pThread, pThread->m_strThreadID.c_str());
	}

	dwExitCode = pThread->Main();

	if(! pThread->m_strThreadID.empty())
	{
		Singleton::GetThreadManager()->RemoveThread(pThread);
	}

	LOGFMTD("Thread end:%s", strThreadName.c_str());

	if(pThread->_autoDelete)
	{
		delete pThread;
		pThread = NULL;
	}

    return dwExitCode;
}

bool BaseThread::Run()
{
	m_hThread=(HANDLE)_beginthreadex(NULL,0,ThreadMain,LPVOID(this),0,&m_nThreadID);
    //m_hThread = ::CreateThread(
    //                NULL,           // default security attributes
    //                0,              // use default stack size
    //                ThreadMain,     // thread function
    //                LPVOID(this),   // argument to thread function
    //                0,              // use default creation flags
    //                &m_nThreadID);   // returns the thread identifier
    //LOG->Write(Log::LOG_DEBUG, L"Start thead : %d", _hThread);

    return m_hThread != NULL;
}

void BaseThread::Exit()
{
    m_bExit = true;
    SetEvent(m_hExitEvent);
}

bool BaseThread::IsRunning()
{
	if ( m_hThread == NULL)
	{
		return false;
	}

	return ::WaitForSingleObject(m_hThread,0) == WAIT_TIMEOUT;
}

bool BaseThread::WaitExit(DWORD dwMilliseconds)
{
    HANDLE events[2];
    events[0] = m_hSystemExitEvent;
    events[1] = m_hExitEvent;
    return ::WaitForMultipleObjects(2, events, false, dwMilliseconds) != WAIT_TIMEOUT;
}

bool BaseThread::WaitTerminate(DWORD dwMilliseconds/*=INFINITE*/)
{
    return ::WaitForSingleObject(m_hThread, dwMilliseconds) != WAIT_TIMEOUT;
}

bool BaseThread::Terminate(DWORD dwExitCode /*= 0xdeaddead*/)
{
    return ::TerminateThread(m_hThread, dwExitCode) == TRUE;
}

bool BaseThread::CheckExit()
{
    return (m_bExit || *m_pSystemExit);
}

void BaseThread::SetAutoDelete(bool value)
{
    _autoDelete = value;
}
