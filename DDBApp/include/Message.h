#ifndef Message_h__
#define Message_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	Message.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:12:45
// 说明		:	消息基类、消息处理基类
/////////////////////////////////////////////////////////////*/
/************************************************************************
消息定义约定前缀：
请求用 Request	对应BaseGetData处理模版
返回用 Response	一般存在1到N个数据
通知用 Notify	常规事件通知，参数0到N个
请求的消息放在接收者定义里
通知的消息放在发送者定义里
************************************************************************/
#pragma once
#include <windows.h>

#include "Singleton.h"
#include "MessageCenter.h"

//////////////////////////////////////////////////////////////////////////
// 发给主线程的消息：所有继承BaseMessage的消息都用这个类型
#ifdef _DEBUG
// Debug版使用字符串，便于消息跟踪
static const TCHAR* EventMessage				= _T("WM_EVENT_MESSAGE");
static const TCHAR* EventGroupBegin			= _T("WM_EVENT_GROUP_BEGIN");
static const TCHAR* EventGroupEnd				= _T("WM_EVENT_GROUP_END");
static const TCHAR* EventTimerTick				= _T("WM_EVENT_TIME_TICK");
#else
// Release外发版本使用GUID，增加消息跟踪难度
static const TCHAR* EventMessage				= _T("{8286D104-BA6C-447D-B71C-6EDD85F01B87}");
static const TCHAR* EventGroupBegin			= _T("{244518B1-B115-4FF2-A8A9-2A2C78704F65}");
static const TCHAR* EventGroupEnd				= _T("{71492166-A434-4E52-B9EF-849745B4CD42}");
static const TCHAR* EventTimerTick				= _T("{DDEAD2A8-91BD-479E-852B-84EC5B5EF7D1}");
#endif

static DWORD WM_EVENT_MESSAGE			= ::RegisterWindowMessage(EventMessage);			// 用户自定义消息
static DWORD WM_EVENT_GROUP_BEGIN	= ::RegisterWindowMessage(EventGroupBegin);		// 用户自定义消息：组消息开始
static DWORD WM_EVENT_GROUP_END		= ::RegisterWindowMessage(EventGroupEnd);			// 用户自定义消息：组消息结束
static DWORD WM_EVENT_TIMER_TICK			= ::RegisterWindowMessage(EventTimerTick);			// Timer消息

/************************************************************************/
/* 抽象接口：是否可用
/************************************************************************/
class SINGLETON_API IAvailable
{
public:
    IAvailable() { _available = false; }
    // 返回是否可用
    // 纯虚改为非纯虚，因为有概率引发pure function call error
    virtual bool Available() { return false; }; 
protected:
    bool _available;                            // 是否可用
};

/************************************************************************
 IHandler : 消息处理抽象类
    若子类需要初始化后才能接收消息，用 bool Available() 控制是否可用
    若无需初始化，子类实现Available()直接返回true即可
************************************************************************/
class SINGLETON_API IHandler : virtual public IAvailable
{
public:
    virtual std::string GetType() = 0;
};

/************************************************************************/
/* 消息抽象类
/************************************************************************/
class SINGLETON_API IMessage
{
public:
    IMessage() { _bRecordMessage = false; };
    IMessage(const IMessage& rhs) { _bRecordMessage = rhs._bRecordMessage; };
    IMessage& operator = (const IMessage& rhs) { if ( this == &rhs ) return *this; _bRecordMessage = rhs._bRecordMessage; return *this; }

    virtual ~IMessage() {}
    // Return bool 是否继续传递
    virtual bool SendTo(IHandler* pHandler) const = 0;
    virtual void PostToUI(HWND hWnd) const = 0;
    // Return bool 是否继续传递
    virtual bool SendAndGetReturn(IHandler* pHandler) = 0;
    virtual void Begin(IHandler* pHandler) const = 0;
    virtual void End(IHandler* pHandler) const = 0;

    // 消息Log开关
    bool RecordMessage() const { return _bRecordMessage; }
    void RecordMessage(bool val) { _bRecordMessage = val; }

    // 用于Log输出
    virtual std::wstring ToString() const { return L""; }

private:
    bool _bRecordMessage;    // 是否打开本消息Log
};

/************************************************************************/
/* 消息基类（模板参数：子类）
/************************************************************************/
template<class M>
class BaseMessage : public IMessage
{
    friend class MessageCenter;
public:
	BaseMessage()
	{
#ifdef _DEBUG 
		RecordMessage(true);
#endif
	}
    virtual ~BaseMessage() {}

    // 快捷的调用方式：同步
    virtual bool Send() const
    {
        return Singleton::GetMessageCenter()->Send(this);
    }

    // 快捷的调用方式：异步
    virtual void PostToUI() const 
    {
        PostToUI(Singleton::GetMessageWnd());        
    }

    // 快捷的调用方式：同步
    virtual void SendToUI() const 
    {
        SendToUI(Singleton::GetMessageWnd());        
    }

    // 快捷的调用方式：同步并等待返回值
    //virtual void SendAndGetReturn()
    //{
    //    Singleton::GetMessageCenter()->GetData(this);
    //}

private:
    virtual bool SendTo(IHandler* pHandler) const
    {
        BaseHandler<M>* p = dynamic_cast<BaseHandler<M>*>(pHandler);
        assert ( p );
        if ( ! p ) return false;

        Singleton::GetMessageCenter()->RecordHandle(p, this);
        return p->Handle((const M*) this);
    }


    virtual void PostToUI(HWND hWnd) const 
    {
        LPARAM p = (LPARAM)new M(*((M*)this));
        assert ( p );
        if ( !p ) return;

        ((M*)p)->RecordMessage(this->RecordMessage());
        PostMessage(hWnd, WM_EVENT_MESSAGE, NULL, p);
    }

    virtual void SendToUI(HWND hWnd) const 
    {
        LPARAM p = (LPARAM)new M(*((M*)this));
        assert ( p );
        if ( !p ) return;
        ((M*)p)->RecordMessage(this->RecordMessage());
        SendMessage(hWnd, WM_EVENT_MESSAGE, NULL, p);
    }

    // 发送消息
    virtual bool SendAndGetReturn(IHandler* pHandler)
    {
        BaseGetData<M>* p = dynamic_cast<BaseGetData<M>*>(pHandler);
        assert ( p );
        if ( ! p ) return false;

        Singleton::GetMessageCenter()->RecordHandle(p, this);
        return p->Handle((M*) this);
    }

    // 组消息的开始通知
    virtual void Begin(IHandler* pHandler) const
    {
        BaseHandler<M>* p = dynamic_cast<BaseHandler<M>*>(pHandler);
        assert ( p );
        if ( ! p ) return;
        return p->Begin((const M*)this);
    }

    // 组消息的结束通知
    virtual void End(IHandler* pHandler) const
    {
        BaseHandler<M>* p = dynamic_cast<BaseHandler<M>*>(pHandler);
        assert ( p );
        if ( ! p ) return;
        return p->End((const M*)this);
    }

public:
    int nRequestID;    //通用请求编号
    int nUserData;     //通用用户数据
    int nParam;        //通用int参数
    std::wstring strParam;  //通用wsring参数
};

/************************************************************************/
/* 消息处理基类（模板参数：要处理的消息）
/************************************************************************/
template<class M>
class BaseHandler : public IHandler
{
public:
    BaseHandler() { Singleton::GetMessageCenter()->Attach(this); }
    virtual ~BaseHandler() { Singleton::GetMessageCenter()->Detach(this); }
    virtual std::string GetType() { return typeid(M).raw_name(); }

    // 子类需要实现：消息的处理方法
    virtual bool Handle(const M* pMsg) = 0;

    // 子类按需实现：组消息的起始通知
    virtual void Begin(const M* pMsg) {}

    // 子类按需实现：组消息的结束通知
    virtual void End(const M* pMsg) {}
};

/************************************************************************/
/* 即时返回结果的消息处理基类（模板参数：要处理的消息）
/************************************************************************/
template<class M>
class BaseGetData : public BaseHandler<M>
{
public:
    BaseGetData() : BaseHandler<M>() {}

    // 需要实现非 const 参数的方法
    virtual bool Handle(M* pMsg) = 0;

private:
    // 不实现const参数的方法
    virtual bool Handle(const M* pMsg) 
    { 
        assert(false); 
        return true; 
    }
};
#endif // Message_h__
