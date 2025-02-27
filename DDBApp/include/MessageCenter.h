#ifndef MessageCenter_h__
#define MessageCenter_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	MessageCenter.h
// ������	: 	daviyang35@QQ.com
// ����ʱ��	:	2014-07-05 12:13:06
// ˵��		:	��Ϣ���ģ�ע�ᣬ������Ϣ֪ͨ
// ʹ�÷���	��

��Ϣ�����ʽԼ����
	1.ǰ׺�����±�
	2.��׺��Msg
	3.ֻ��������ȷ����д�ؼ��ʿ�ʹ����д���������ʹ������Ӣ�ĵ�����
	4.Ӣ�ĵ���������ĸ��д
����:
	1. RequestAccountInfoMsg
	2. NotifyBSSInfoUpdateMsg

ǰ׺��
			ǰ׺			��Ӧ�̳л���
	������ Request			BaseHandler
	������ Response			BaseHandler
	��������Ϊһ����Գ��֣�����첽���������ص�

	֪ͨ�� Notify			BaseHandler
	������ Service			BaseGetData

˵����
	����һ���ڹ����߳��������ʹ�ã�UI�߳��������ע���ʱ����
	ԭ����ֻ�����µķ�����Ϣ����UI�߳�ֱ�ӵ���
	1.����ʱ���Ԥ��
	2.���ʱ������1.5��
	3.�����ڲ���������Ϣϵͳ����������

1.������Ϣ�ṹ��
struct InitMsg
	: public BaseMessage<InitMsg>
{
	// InitMsg ��Ϣ������Ա
	CDXString version;
	......	// N��Ĳ���
};

2.ע��ص��ӿ�
class CPane_wycs
	: public BaseHandler<NotifyInitMsg>		// ��Ӧ����ģ���Լ������
	, public BaseGetData<RequestTestMsg>	// ��Ӧ����ģ���Լ������
{
	virtual bool Handle(const NotifyInitMsg* pMsg);
	virtual bool Handle(RequestTestMsg* pMsg);
	virtual bool Available()
	{   // ����ӿ��ر�ע�⣬VAX����Ĭ�ϲ���ѡ
		return true;
	}
}

Remark:
	BaseHandler�����¼�֪ͨ������,��ӦResponse��Notifyǰ׺��Ϣ
	BaseGetData����������������Handle�и�ֵ�����÷����߽���ý������ӦRequestǰ׺��Ϣ
	BaseHandler&BaseGetData�ڹ���/�����������Զ�ע��/ж����Ϣ�󶨣�ʹ���߲�����ע�������

3.����֪ͨ��Ϣ
	NotifyInitMsg msg;	// ������Ϣ����
	msg.version = _T("2014070501");
	msg.Send();		// �ڵ�ǰ�߳�ͬ�������ص�����Ϣ�����д�����
	msg.SendToUI();	// ��  UI�߳�ͬ�������ص�����Ϣ�����д�����
	msg.PostToUI();	// ��  UI�߳��첽�ص�����Ϣ�����д�����

Remark��
	SendToUI() & PostToUI()�������ݵ���new����ʵ�����ɽ����߸����ͷ�

4.����������Ϣ
	RequestTestMsg msg;	// ������Ϣ����
	msg.version = "Param" ;	// ������������ǰ��д����
	msg.SendAndGetReturn();	// �ڵ�ǰ�߳�ͬ�������ص����лص�����
	TRACE(msg.version);		// ʹ�÷��ؽ��

5.UI���д������
	LRESULT CMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	// ����UI���������´���
	else if ( uMsg == WM_EVENT_MESSAGE)
	{
		IMessage* pMessage = reinterpret_cast<IMessage*>(lParam);
		Singleton::GetMessageCenter()->Send(pMessage);
		delete pMessage;		// ��Ϣ�����߸����ͷ��ڴ�
		return 0;
	}
	// �������
	bHandled = FALSE;
	if( bHandled ) return lRes; 
	return CDDBFrameWnd::HandleMessage(uMsg, wParam, lParam);
}
Remark��
	��Ϣ���մ�ͳһ�ͷ��ڴ���¼���Ӧ����������ҵ���߼���Ӧ����ֹ�ڴ�й©
	��Ϣ������Ϣ��Ҫ�ǽ�����߳����⣬�ûص�������UI�߳���
	���ڽ��յ���10000�ŹܼҶ��ԣ�ʹ���߽����ע�Ƿ��ڹ������߳��ϲ���
	������ÿ�����������UI���д������
/////////////////////////////////////////////////////////////*/
#pragma once
#include "BaseCSLock.h"
#include "Thread/ThreadUtils.h"

#include <list>
#include <map>

//////////////////////////////////////////////////////////////////////////
// ���Ա��뿪��
#define RWLOCK
//#define CSLOCK
//////////////////////////////////////////////////////////////////////////

#pragma warning (disable:4251)

class IMessage; 
class IHandler;

// ��Ϣ����
class SINGLETON_API MessageCenter
#ifdef CSLOCK
	: private BaseCSLock
#endif
{
    friend class Singleton;
public:
    // ��ָ����Ϣ����ӽ�����
    void Attach(IHandler* pHandler);

    // ��ָ����Ϣ���Ƴ�������
    void Detach(IHandler* pHandler);

    // �㲥��Ϣ��ͬ���������Ƿ��н�����
    bool Send(const IMessage* pMsg);

    // ��¼��Ϣ
    inline void RecordMessage( const IMessage* pMsg ) const;
    inline void RecordMessage( const IMessage* pMsg, bool begin ) const;
    inline void RecordHandle( IHandler* p, const IMessage* pMsg ) const;

    // ͨ��Windows��Ϣ���ƹ㲥��Ϣ���첽��
    void SendToUI(HWND hWnd, const IMessage* pMsg);

    // ͨ����Ϣ��ȡ���ݣ�ֱ������ϢIMessage�ڷ��ؽ��
    void GetData(IMessage* pMsg);

    // ���ԣ��Ƿ��¼��Ϣ
    bool GetRecordMessage() const { return _recordMessage; }
    void SetRecordMessage(bool val) { _recordMessage = val; }

    // ��������Ϣ�Ľ�����
    void GroupMsgHandler(IMessage* pMsg, IHandler**& _receiverArray, unsigned int& _arrayLength );

    // ����Ϣ��ʼ��ͬ����
    void Begin(const IMessage* pMsg);

    // ����Ϣ������ͬ����
    void End(const IMessage* pMsg);

private:
    MessageCenter();
    virtual ~MessageCenter();

private:
    typedef std::list<IHandler*>     HANDLER_LIST;    // ����Handler���б�
    std::map<std::string, HANDLER_LIST*>  _handlers;       // ������Ϣ������
    bool _recordMessage;    // �Ƿ��¼��Ϣ���ܿ��أ�ÿ����Ϣ���Լ����أ�

#ifdef RWLOCK
	DDBUtils::CRWLock m_rwLock;					// ��д��
#elif defined CSLOCK
	CRITICAL_SECTION            _msgLock;        // _msgReceivers����
#endif
};
#endif // MessageCenter_h__
