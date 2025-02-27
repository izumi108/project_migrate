#ifndef MessageCenter_h__
#define MessageCenter_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	MessageCenter.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-07-05 12:13:06
// 说明		:	消息中心：注册，发送消息通知
// 使用方法	：

消息定义格式约定：
	1.前缀：见下表
	2.后缀：Msg
	3.只有意义明确的缩写关键词可使用缩写，其他情况使用完整英文单词组
	4.英文单词组首字母大写
例子:
	1. RequestAccountInfoMsg
	2. NotifyBSSInfoUpdateMsg

前缀：
			前缀			响应继承基类
	请求用 Request			BaseHandler
	返回用 Response			BaseHandler
	以上两个为一组配对出现，完成异步请求与结果回调

	通知用 Notify			BaseHandler
	服务用 Service			BaseGetData

说明：
	服务一般在工作线程请求服务使用，UI线程请求必须注意耗时问题
	原则上只有以下的服务消息可在UI线程直接调用
	1.返回时间可预测
	2.最长耗时不超过1.5秒
	3.服务内部不依赖消息系统的其他服务

1.定义消息结构体
struct InitMsg
	: public BaseMessage<InitMsg>
{
	// InitMsg 消息参数成员
	CDXString version;
	......	// N多的参数
};

2.注册回调接口
class CPane_wycs
	: public BaseHandler<NotifyInitMsg>		// 响应基类模版见约定部分
	, public BaseGetData<RequestTestMsg>	// 响应基类模版见约定部分
{
	virtual bool Handle(const NotifyInitMsg* pMsg);
	virtual bool Handle(RequestTestMsg* pMsg);
	virtual bool Available()
	{   // 这个接口特别注意，VAX助手默认不勾选
		return true;
	}
}

Remark:
	BaseHandler用于事件通知处理器,对应Response和Notify前缀消息
	BaseGetData用于请求处理器，在Handle中赋值，调用发起者将获得结果，对应Request前缀消息
	BaseHandler&BaseGetData在构造/析构函数中自动注册/卸载消息绑定，使用者不关心注册等问题

3.发送通知消息
	NotifyInitMsg msg;	// 发送消息类型
	msg.version = _T("2014070501");
	msg.Send();		// 在当前线程同步阻塞回调该消息的所有处理函数
	msg.SendToUI();	// 在  UI线程同步阻塞回调该消息的所有处理函数
	msg.PostToUI();	// 在  UI线程异步回调该消息的所有处理函数

Remark：
	SendToUI() & PostToUI()方法传递的是new的新实例，由接收者负责释放

4.发送请求消息
	RequestTestMsg msg;	// 发送消息类型
	msg.version = "Param" ;	// 发送请求命令前填写参数
	msg.SendAndGetReturn();	// 在当前线程同步阻塞回调所有回调函数
	TRACE(msg.version);		// 使用返回结果

5.UI队列处理代码
	LRESULT CMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	// 其他UI库仅添加如下代码
	else if ( uMsg == WM_EVENT_MESSAGE)
	{
		IMessage* pMessage = reinterpret_cast<IMessage*>(lParam);
		Singleton::GetMessageCenter()->Send(pMessage);
		delete pMessage;		// 消息接收者负责释放内存
		return 0;
	}
	// 这里结束
	bHandled = FALSE;
	if( bHandled ) return lRes; 
	return CDDBFrameWnd::HandleMessage(uMsg, wParam, lParam);
}
Remark：
	消息接收处统一释放内存后，事件响应函数仅处理业务逻辑响应，防止内存泄漏
	消息队列消息主要是解决跨线程问题，让回调运行在UI线程上
	对于江苏电信10000号管家而言，使用者仅需关注是否在工作者线程上操作
	不用在每个窗口中添加UI队列处理代码
/////////////////////////////////////////////////////////////*/
#pragma once
#include "BaseCSLock.h"
#include "Thread/ThreadUtils.h"

#include <list>
#include <map>

//////////////////////////////////////////////////////////////////////////
// 调试编译开关
#define RWLOCK
//#define CSLOCK
//////////////////////////////////////////////////////////////////////////

#pragma warning (disable:4251)

class IMessage; 
class IHandler;

// 消息中心
class SINGLETON_API MessageCenter
#ifdef CSLOCK
	: private BaseCSLock
#endif
{
    friend class Singleton;
public:
    // 对指定消息，添加接收者
    void Attach(IHandler* pHandler);

    // 对指定消息，移除接收者
    void Detach(IHandler* pHandler);

    // 广播消息（同步）返回是否有接收者
    bool Send(const IMessage* pMsg);

    // 记录消息
    inline void RecordMessage( const IMessage* pMsg ) const;
    inline void RecordMessage( const IMessage* pMsg, bool begin ) const;
    inline void RecordHandle( IHandler* p, const IMessage* pMsg ) const;

    // 通过Windows消息机制广播消息（异步）
    void SendToUI(HWND hWnd, const IMessage* pMsg);

    // 通过消息获取数据，直接在消息IMessage内返回结果
    void GetData(IMessage* pMsg);

    // 属性：是否记录消息
    bool GetRecordMessage() const { return _recordMessage; }
    void SetRecordMessage(bool val) { _recordMessage = val; }

    // 设置组消息的接收者
    void GroupMsgHandler(IMessage* pMsg, IHandler**& _receiverArray, unsigned int& _arrayLength );

    // 组消息开始（同步）
    void Begin(const IMessage* pMsg);

    // 组消息结束（同步）
    void End(const IMessage* pMsg);

private:
    MessageCenter();
    virtual ~MessageCenter();

private:
    typedef std::list<IHandler*>     HANDLER_LIST;    // 所有Handler的列表
    std::map<std::string, HANDLER_LIST*>  _handlers;       // 所有消息接收者
    bool _recordMessage;    // 是否记录消息（总开关，每个消息有自己开关）

#ifdef RWLOCK
	DDBUtils::CRWLock m_rwLock;					// 读写锁
#elif defined CSLOCK
	CRITICAL_SECTION            _msgLock;        // _msgReceivers的锁
#endif
};
#endif // MessageCenter_h__
