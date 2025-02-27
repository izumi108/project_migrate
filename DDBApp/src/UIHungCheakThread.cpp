

#include "stdafx.h"
#include "UIHungCheakThread.h"
//获取系统开机时间
#include <Mmsystem.h>   
#pragma comment(lib, "Winmm.lib")  

#ifdef DLOGPRINT
#ifndef COM_CODE_STATIC
#define COM_CODE_STATIC
#endif
#include "self_log/self_log.h"
#define XLOG SelfLogInfo
#define XLOGERROR SelfLogError
#else
#define XLOG(_FORMAT,...)
#define DLOGERROR XLOG
#endif

using namespace std;


CUIHungCheckThread *CUIHungCheckThread::_stthread = NULL;

CUIHungCheckThread::CUIHungCheckThread()
{
    _startfun = NULL;
    _endfun = NULL;

    _hUser32 = NULL;
    _IsHungAppWindow = NULL;
    _IsHungThread = NULL;
    _bnt = true;
    m_dwDelay = 300;
}
CUIHungCheckThread::~CUIHungCheckThread()
{
    if (_hUser32)
    {
        //CloseHandle(_hUser32);
    }
}
CUIHungCheckThread *CUIHungCheckThread::GetInstance()
{
    if (NULL == _stthread)
    {
        _stthread = new CUIHungCheckThread;
        //_stthread->SetAutoDelete(true);
        //_stthread->Run();
    }

    return _stthread;
}
void CUIHungCheckThread::ReleaseInstance()
{
    if (_stthread)
    {
        // 要求线程退出
		_stthread->Exit();
		if ( _stthread->WaitTerminate(3000) )
		{
			// 等待了3s还未退出，Terminate
			_stthread->Terminate();
		}

        delete _stthread;
        _stthread = NULL;
    }
}
bool CUIHungCheckThread::Available()
{
    return true;
}
bool CUIHungCheckThread::Add(UIHungInfo &info)
{
    AutoLock;
    _infos.push_back(info);
    return true;
}
bool CUIHungCheckThread::Del(UIHungInfo &info)
{
    AutoLock;

    for (vector<UIHungInfo>::iterator itor = _infos.begin();
         itor != _infos.end(); ++itor)
    {
        if (info._h == itor->_h)
        {
            _infos.erase(itor);
            return true;
        }
    }

    return false;
}
void CUIHungCheckThread::SetOnUIHungFun(OnUIHungStartFun fun1, OnUIHungEndFun fun2)
{
    _startfun = fun1;
    _endfun = fun2;
}
BOOL CUIHungCheckThread::IsHung(HWND &h)
{
    //return ::IsHungAppWindow(h);

    if (_bnt)
    {
        if (_IsHungAppWindow)
        {
            return _IsHungAppWindow(h);
        }
    }
    else
    {
        if (_IsHungThread)
        {
            return _IsHungThread(GetWindowThreadProcessId(h, NULL));
        }
    }

    return FALSE;
}
unsigned long CUIHungCheckThread::Main()
{
    XLOG("begin ui hung check thread...\r\n");
    CHECK_EXIT;

    _hUser32 = GetModuleHandleA("user32");

    if (!_hUser32)
    {
        XLOG("GetModuleHandle(user32) Fail.\r\n");
        return 0xdead;
    }

    CHECK_EXIT;

    _IsHungAppWindow = (PROCISHUNGAPPWINDOW)GetProcAddress( _hUser32, "IsHungAppWindow");
    _IsHungThread = (PROCISHUNGTHREAD) GetProcAddress( _hUser32, "IsHungThread" );

    if (!_IsHungAppWindow && !_IsHungThread)
    {
        XLOG("!_IsHungAppWindow && !_IsHungThread.\r\n");
        return 0xdead;
    }

    CHECK_EXIT;

    OSVERSIONINFO osver;
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    if (!GetVersionEx(&osver))
    {
        XLOG("GetVersionEx Fail.\r\n");
        return 0xdead;
    }

    if (osver.dwPlatformId & VER_PLATFORM_WIN32_NT)
    {
        XLOG("Windows Ver[NT].\r\n");
        _bnt = true;
    }
    else
    {
        XLOG("Windows Ver[Other(%d)].\r\n", osver.dwPlatformId);
        _bnt = false;
    }
    do 
    {   
        DWORD startMSCount;//从开机到现在的毫秒数
        startMSCount = timeGetTime();
        DWORD dwRuntime = startMSCount / 1000;
        if (dwRuntime < m_dwDelay)
        {

            //Sleep((m_dwDelay - dwRuntime) * 1000);
            continue;
        }
        else
        {
            break;
        }
    } while (!WaitExit(1000));

    while (!WaitExit(1000))
    {
        //XLOG("ui hung check loop...\r\n");
        {
            AutoLock;

            for (size_t i = 0; i < _infos.size(); ++i)
            {
                _infos[i].Check(*this);
            }
        }
    }

    return 0xdead;
}
void CUIHungCheckThread::OnUIHungStart(UIHungInfo &info)
{
    struct tm ttm;
    localtime_s(&ttm, &(info._startTmSends));
    char pbuf[256] = {0};
    strftime(pbuf, sizeof(pbuf) - 1, "%Y-%m-%d %H:%M:%S", &ttm);

    XLOG("UI[%s] Hung Start Time[%s].\r\n", info.Show().c_str(), pbuf);

    if (_startfun)
    {
        _startfun(info);
    }
}
void CUIHungCheckThread::OnUIHungEnd(UIHungInfo &info)
{
    XLOG("UI[%s] Hung End Continue_Time[%.4f]ms.\r\n", info.Show().c_str(), (info._endTm - info._startTm));

    if (_endfun)
    {
        _endfun(info);
    }
}

void CUIHungCheckThread::SetDelay( DWORD dwDelay /*= 300*/ )
{
    m_dwDelay = dwDelay;
}

UIHungInfo::UIHungInfo()
{
    _h = NULL;
    _bhung = false;
    _startTm = 0.0;
    _endTm = 0.0;
    _startTmSends = 0;
}
void UIHungInfo::Check(CUIHungCheckThread &uihungthread)
{
    double runtm = clock();

    if (uihungthread.IsHung(_h))
    {
        if (_bhung) // hung ing~
        {

        }
        else
        {
            // first hung
            _startTm = runtm;
            _endTm = 0.0;
            _startTmSends = time(0);
            uihungthread.OnUIHungStart(*this);
        }

        _bhung = true;
    }
    else
    {
        if (_bhung)
        {
            // hung stop ,report
            _endTm = runtm;
            uihungthread.OnUIHungEnd(*this);
        }
        else
        {
            // no hung happen~
        }

        _bhung = false;
        _startTm = 0.0;
        _startTmSends = 0;
        _endTm = 0.0;
    }
}
string &UIHungInfo::Show()
{
    if (_name.empty())
    {
        char buf[64] = {0};
        sprintf_s(buf, sizeof(buf) - 1, "%d", _h);
        _name.assign(buf);
    }

    return _name;
}
