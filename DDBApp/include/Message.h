#ifndef Message_h__
#define Message_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	Message.h
// ������	: 	daviyang35@QQ.com
// ����ʱ��	:	2014-07-05 12:12:45
// ˵��		:	��Ϣ���ࡢ��Ϣ�������
/////////////////////////////////////////////////////////////*/
/************************************************************************
��Ϣ����Լ��ǰ׺��
������ Request	��ӦBaseGetData����ģ��
������ Response	һ�����1��N������
֪ͨ�� Notify	�����¼�֪ͨ������0��N��
�������Ϣ���ڽ����߶�����
֪ͨ����Ϣ���ڷ����߶�����
************************************************************************/
#pragma once
#include <windows.h>

#include "Singleton.h"
#include "MessageCenter.h"

//////////////////////////////////////////////////////////////////////////
// �������̵߳���Ϣ�����м̳�BaseMessage����Ϣ�����������
#ifdef _DEBUG
// Debug��ʹ���ַ�����������Ϣ����
static const TCHAR* EventMessage				= _T("WM_EVENT_MESSAGE");
static const TCHAR* EventGroupBegin			= _T("WM_EVENT_GROUP_BEGIN");
static const TCHAR* EventGroupEnd				= _T("WM_EVENT_GROUP_END");
static const TCHAR* EventTimerTick				= _T("WM_EVENT_TIME_TICK");
#else
// Release�ⷢ�汾ʹ��GUID��������Ϣ�����Ѷ�
static const TCHAR* EventMessage				= _T("{8286D104-BA6C-447D-B71C-6EDD85F01B87}");
static const TCHAR* EventGroupBegin			= _T("{244518B1-B115-4FF2-A8A9-2A2C78704F65}");
static const TCHAR* EventGroupEnd				= _T("{71492166-A434-4E52-B9EF-849745B4CD42}");
static const TCHAR* EventTimerTick				= _T("{DDEAD2A8-91BD-479E-852B-84EC5B5EF7D1}");
#endif

static DWORD WM_EVENT_MESSAGE			= ::RegisterWindowMessage(EventMessage);			// �û��Զ�����Ϣ
static DWORD WM_EVENT_GROUP_BEGIN	= ::RegisterWindowMessage(EventGroupBegin);		// �û��Զ�����Ϣ������Ϣ��ʼ
static DWORD WM_EVENT_GROUP_END		= ::RegisterWindowMessage(EventGroupEnd);			// �û��Զ�����Ϣ������Ϣ����
static DWORD WM_EVENT_TIMER_TICK			= ::RegisterWindowMessage(EventTimerTick);			// Timer��Ϣ

/************************************************************************/
/* ����ӿڣ��Ƿ����
/************************************************************************/
class SINGLETON_API IAvailable
{
public:
    IAvailable() { _available = false; }
    // �����Ƿ����
    // �����Ϊ�Ǵ��飬��Ϊ�и�������pure function call error
    virtual bool Available() { return false; }; 
protected:
    bool _available;                            // �Ƿ����
};

/************************************************************************
 IHandler : ��Ϣ���������
    ��������Ҫ��ʼ������ܽ�����Ϣ���� bool Available() �����Ƿ����
    �������ʼ��������ʵ��Available()ֱ�ӷ���true����
************************************************************************/
class SINGLETON_API IHandler : virtual public IAvailable
{
public:
    virtual std::string GetType() = 0;
};

/************************************************************************/
/* ��Ϣ������
/************************************************************************/
class SINGLETON_API IMessage
{
public:
    IMessage() { _bRecordMessage = false; };
    IMessage(const IMessage& rhs) { _bRecordMessage = rhs._bRecordMessage; };
    IMessage& operator = (const IMessage& rhs) { if ( this == &rhs ) return *this; _bRecordMessage = rhs._bRecordMessage; return *this; }

    virtual ~IMessage() {}
    // Return bool �Ƿ��������
    virtual bool SendTo(IHandler* pHandler) const = 0;
    virtual void PostToUI(HWND hWnd) const = 0;
    // Return bool �Ƿ��������
    virtual bool SendAndGetReturn(IHandler* pHandler) = 0;
    virtual void Begin(IHandler* pHandler) const = 0;
    virtual void End(IHandler* pHandler) const = 0;

    // ��ϢLog����
    bool RecordMessage() const { return _bRecordMessage; }
    void RecordMessage(bool val) { _bRecordMessage = val; }

    // ����Log���
    virtual std::wstring ToString() const { return L""; }

private:
    bool _bRecordMessage;    // �Ƿ�򿪱���ϢLog
};

/************************************************************************/
/* ��Ϣ���ࣨģ����������ࣩ
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

    // ��ݵĵ��÷�ʽ��ͬ��
    virtual bool Send() const
    {
        return Singleton::GetMessageCenter()->Send(this);
    }

    // ��ݵĵ��÷�ʽ���첽
    virtual void PostToUI() const 
    {
        PostToUI(Singleton::GetMessageWnd());        
    }

    // ��ݵĵ��÷�ʽ��ͬ��
    virtual void SendToUI() const 
    {
        SendToUI(Singleton::GetMessageWnd());        
    }

    // ��ݵĵ��÷�ʽ��ͬ�����ȴ�����ֵ
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

    // ������Ϣ
    virtual bool SendAndGetReturn(IHandler* pHandler)
    {
        BaseGetData<M>* p = dynamic_cast<BaseGetData<M>*>(pHandler);
        assert ( p );
        if ( ! p ) return false;

        Singleton::GetMessageCenter()->RecordHandle(p, this);
        return p->Handle((M*) this);
    }

    // ����Ϣ�Ŀ�ʼ֪ͨ
    virtual void Begin(IHandler* pHandler) const
    {
        BaseHandler<M>* p = dynamic_cast<BaseHandler<M>*>(pHandler);
        assert ( p );
        if ( ! p ) return;
        return p->Begin((const M*)this);
    }

    // ����Ϣ�Ľ���֪ͨ
    virtual void End(IHandler* pHandler) const
    {
        BaseHandler<M>* p = dynamic_cast<BaseHandler<M>*>(pHandler);
        assert ( p );
        if ( ! p ) return;
        return p->End((const M*)this);
    }

public:
    int nRequestID;    //ͨ��������
    int nUserData;     //ͨ���û�����
    int nParam;        //ͨ��int����
    std::wstring strParam;  //ͨ��wsring����
};

/************************************************************************/
/* ��Ϣ������ࣨģ�������Ҫ�������Ϣ��
/************************************************************************/
template<class M>
class BaseHandler : public IHandler
{
public:
    BaseHandler() { Singleton::GetMessageCenter()->Attach(this); }
    virtual ~BaseHandler() { Singleton::GetMessageCenter()->Detach(this); }
    virtual std::string GetType() { return typeid(M).raw_name(); }

    // ������Ҫʵ�֣���Ϣ�Ĵ�����
    virtual bool Handle(const M* pMsg) = 0;

    // ���ఴ��ʵ�֣�����Ϣ����ʼ֪ͨ
    virtual void Begin(const M* pMsg) {}

    // ���ఴ��ʵ�֣�����Ϣ�Ľ���֪ͨ
    virtual void End(const M* pMsg) {}
};

/************************************************************************/
/* ��ʱ���ؽ������Ϣ������ࣨģ�������Ҫ�������Ϣ��
/************************************************************************/
template<class M>
class BaseGetData : public BaseHandler<M>
{
public:
    BaseGetData() : BaseHandler<M>() {}

    // ��Ҫʵ�ַ� const �����ķ���
    virtual bool Handle(M* pMsg) = 0;

private:
    // ��ʵ��const�����ķ���
    virtual bool Handle(const M* pMsg) 
    { 
        assert(false); 
        return true; 
    }
};
#endif // Message_h__
