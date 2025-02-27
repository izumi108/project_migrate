
/*
    UIHungCheakThread

    UI卡死检测线程

    lilin
    2015/03/06

    record
    time        author      info
    2015/03/06  lilin       creat
    use:
    CUIHungCheckThread* stthread = CUIHungCheckThread::GetInstance();   // 开启线程
    if (stthread)
    {
        UIHungInfo info;
        info._h = hWnd; // 设置主窗口句柄
        info._name.assign("RouterMgrUI"); // 设置一个别名
        stthread->Add(info); // 添加到检测列表 可以添加多个
        stthread->SetOnUIHungFun(UIHungStart,UIHungEnd); // 设置检测响应函数  主要用来数据上报和UI卡住后续操作 请不要设置阻塞很久的函数!
        stthread->SetDelay(300);    //设置开机多久后开始监控，单位秒，默认300秒（5分钟）
        stthread->Run();            //开始监控
    }
    // UI消息结束后：
    CUIHungCheckThread::ReleaseInstance();
*/
#pragma once
#include "Singleton_Export.h"
#include <time.h>
#include <vector>

//---------------------------------------
typedef BOOL    (WINAPI *PROCISHUNGAPPWINDOW)(HWND);
typedef BOOL    (WINAPI *PROCISHUNGTHREAD)(DWORD);

// --------------------------------------
class CUIHungCheckThread;
struct SINGLETON_API UIHungInfo
{
    std::string _name;
    HWND _h;

    bool _bhung;
    double _startTm; // ms  clock
    double _endTm;      // 阻塞时间
    time_t _startTmSends;

    UIHungInfo();
    void Check(CUIHungCheckThread &uihungthread);
    std::string &Show();
};
typedef void (*OnUIHungStartFun)(UIHungInfo &info);
typedef void (*OnUIHungEndFun)(UIHungInfo &info);
class SINGLETON_API CUIHungCheckThread
	: public BaseThread
	, public BaseCSLock
{
public:
    CUIHungCheckThread();
    virtual ~CUIHungCheckThread();

private:
    static CUIHungCheckThread *_stthread;

    // ui handle
    std::vector<UIHungInfo> _infos;
    OnUIHungStartFun _startfun;
    OnUIHungEndFun _endfun;

    HMODULE _hUser32;
    PROCISHUNGAPPWINDOW    _IsHungAppWindow;
    PROCISHUNGTHREAD       _IsHungThread;
    bool _bnt;
    DWORD m_dwDelay;    //开机多久后才开始监视
public:
    static CUIHungCheckThread *GetInstance();
    static void ReleaseInstance();

    bool Available();

    bool Add(UIHungInfo &info);
    bool Del(UIHungInfo &info);
    void SetDelay(DWORD dwDelay = 300); //单位秒
    BOOL IsHung(HWND &h);

    void SetOnUIHungFun(OnUIHungStartFun fun1, OnUIHungEndFun fun2);

    virtual void OnUIHungStart(UIHungInfo &info);
    virtual void OnUIHungEnd(UIHungInfo &info);

public:
    unsigned long Main();
};



