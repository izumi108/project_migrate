
/*
    UIHungCheakThread

    UI��������߳�

    lilin
    2015/03/06

    record
    time        author      info
    2015/03/06  lilin       creat
    use:
    CUIHungCheckThread* stthread = CUIHungCheckThread::GetInstance();   // �����߳�
    if (stthread)
    {
        UIHungInfo info;
        info._h = hWnd; // ���������ھ��
        info._name.assign("RouterMgrUI"); // ����һ������
        stthread->Add(info); // ��ӵ�����б� ������Ӷ��
        stthread->SetOnUIHungFun(UIHungStart,UIHungEnd); // ���ü����Ӧ����  ��Ҫ���������ϱ���UI��ס�������� �벻Ҫ���������ܾõĺ���!
        stthread->SetDelay(300);    //���ÿ�����ú�ʼ��أ���λ�룬Ĭ��300�루5���ӣ�
        stthread->Run();            //��ʼ���
    }
    // UI��Ϣ������
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
    double _endTm;      // ����ʱ��
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
    DWORD m_dwDelay;    //������ú�ſ�ʼ����
public:
    static CUIHungCheckThread *GetInstance();
    static void ReleaseInstance();

    bool Available();

    bool Add(UIHungInfo &info);
    bool Del(UIHungInfo &info);
    void SetDelay(DWORD dwDelay = 300); //��λ��
    BOOL IsHung(HWND &h);

    void SetOnUIHungFun(OnUIHungStartFun fun1, OnUIHungEndFun fun2);

    virtual void OnUIHungStart(UIHungInfo &info);
    virtual void OnUIHungEnd(UIHungInfo &info);

public:
    unsigned long Main();
};



