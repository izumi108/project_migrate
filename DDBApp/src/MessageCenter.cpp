#include "stdafx.h"
#include "MessageCenter.h"
#include "Message.h"

MessageCenter::MessageCenter()
{
    _recordMessage = false;
}

MessageCenter::~MessageCenter()
{
#ifdef RWLOCK
	DDBUtils::ShareWriteAutoLock locker(&m_rwLock);
#elif defined CSLOCK
	AutoLock;
#endif
    for(std::map<std::string, HANDLER_LIST*>::iterator it = _handlers.begin(); 
        it != _handlers.end(); ++it)
        SAFE_DELETE(it->second);
}

void MessageCenter::Attach(IHandler* pHandler)
{
    RETURN_IF( ! pHandler );

    pHandler->Available();

    std::string t = pHandler->GetType();
#ifdef RWLOCK
	DDBUtils::ShareWriteAutoLock locker(&m_rwLock);
#elif defined CSLOCK
    AutoLock;
#endif
    HANDLER_LIST* pList = _handlers[t];

    if (pList)
    {
        for(HANDLER_LIST::iterator it = pList->begin(); it != pList->end(); ++it)
        {
            RETURN_IF( *it == pHandler); // 避免重复添加
        }
        pList->push_back(pHandler);
    }
    else
    {
        pList = new HANDLER_LIST;
        pList->push_back(pHandler);
        _handlers[t] = pList;
    }
}

void MessageCenter::Detach(IHandler* pHandler)
{
    RETURN_IF( ! pHandler );

    std::string t = pHandler->GetType();
#ifdef RWLOCK
	DDBUtils::ShareWriteAutoLock locker(&m_rwLock);
#elif defined CSLOCK
    AutoLock;
#endif
    HANDLER_LIST* pList = _handlers[t];

    if ( pList )
    {
        for(HANDLER_LIST::iterator it = pList->begin(); it != pList->end(); ++it)
        {
            if( *it == pHandler)
            {
                *it = NULL;     // 暂不删除：SendMsg()中会调用到 DetachReceiver 导致SendMsg中崩溃
                break;
            }
        }
    }
}

// 发送消息给所有接收者，处理完成后才返回
bool MessageCenter::Send(const IMessage* pMsg)
{
    RETURN_FALSE_IF( ! pMsg );

    std::string t = typeid(*pMsg).raw_name();
#ifdef RWLOCK
	DDBUtils::ShareReadAutoLock locker(&m_rwLock);
#elif defined CSLOCK
    AutoLock;
#endif
    HANDLER_LIST* pList = _handlers[t];
    if ( pList )
    {
        RecordMessage(pMsg);
        for(HANDLER_LIST::iterator it = pList->begin(); it != pList->end(); ++it )
        {
            IHandler* pHandler = *it;
            if(pHandler && pHandler->Available() )
            {
                if (!pMsg->SendTo(pHandler))
                    break;
            }
            else if ( pHandler && ! pHandler->Available())
            {
				LOGFMTD("Handle unavailable【%s:0x%08x】%s",
                typeid(*pMsg).name(),
				pMsg,
				pMsg->ToString().c_str());
            }
        }
        return pList->size() > 0;
    }
    return false;
}

// 发送windows消息，消息放在 lparam，消息接收者需负责释放
void MessageCenter::SendToUI(HWND hWnd, const IMessage* pMsg)
{
    RETURN_IF( ! pMsg );
    pMsg->PostToUI(hWnd);
}


void MessageCenter::GetData( IMessage* pMsg )
{
    RETURN_IF( ! pMsg );

    std::string t = typeid(*pMsg).raw_name();
#ifdef RWLOCK
	DDBUtils::ShareReadAutoLock locker(&m_rwLock);
#elif defined CSLOCK
	AutoLock;
#endif
    HANDLER_LIST* pList = _handlers[t];
    if ( pList )
    {
        RecordMessage(pMsg);
        for(HANDLER_LIST::iterator it = pList->begin(); it != pList->end(); ++it )
        {
            IHandler* pHandler = *it;
            if(pHandler && pHandler->Available())
            {
                if (!pMsg->SendAndGetReturn(pHandler))
                    break;
            }
            else if ( pHandler && ! pHandler->Available())
            {
				LOGFMTD("Handle unavailable【%s:0x%08x】%s",
                typeid(*pMsg).name(),
				pMsg,
				StringApi::UnicodeToMbcs(pMsg->ToString().c_str()));
            }
        }
    }
}

void MessageCenter::GroupMsgHandler(IMessage* pMsg,
                                    IHandler**& pArray,
                                    unsigned int& _arrayLength)
{
    string t = typeid(*pMsg).raw_name();
#ifdef RWLOCK
	DDBUtils::ShareReadAutoLock locker(&m_rwLock);
#elif defined CSLOCK
    AutoLock;
#endif
    HANDLER_LIST* pList = _handlers[t];
    if ( pList && pList->size() )
    {
        pArray = new IHandler*[pList->size()];
        int index = 0;
        for(HANDLER_LIST::iterator it = pList->begin(); it != pList->end();
            ++it, ++index )
        {
            IHandler* pHandler = *it;
            if ( pHandler && pHandler->Available() )
            {
                pArray[index] = pHandler;
            }
            else if ( pHandler && ! pHandler->Available())
            {
                LOGFMTD("Handle unavailable【%s:0x%08x】%s",
                    typeid(*pMsg).name(),
					pMsg, 
					StringApi::UnicodeToMbcs(pMsg->ToString().c_str()));
            }
        }
        _arrayLength = pList->size();
    }
}

void MessageCenter::Begin( const IMessage* pMsg )
{
    RETURN_IF( ! pMsg );

    string t = typeid(*pMsg).raw_name();
#ifdef RWLOCK
	DDBUtils::ShareReadAutoLock locker(&m_rwLock);
#elif defined CSLOCK
    AutoLock;
#endif
    HANDLER_LIST* pList = _handlers[t];
    if ( pList )
    {
        RecordMessage(pMsg, true);
        for(HANDLER_LIST::iterator it = pList->begin(); it != pList->end(); ++it )
        {
            IHandler* pHandler = *it;
            if(pHandler && pHandler->Available())
				pMsg->Begin(pHandler);
        }
    }
}

void MessageCenter::End( const IMessage* pMsg )
{
    RETURN_IF( ! pMsg );

    string t = typeid(*pMsg).raw_name();
#ifdef RWLOCK
	DDBUtils::ShareReadAutoLock locker(&m_rwLock);
#elif defined CSLOCK
    AutoLock;
#endif
    HANDLER_LIST* pList = _handlers[t];
    if ( pList )
    {
        RecordMessage(pMsg, false);
        for(HANDLER_LIST::iterator it = pList->begin(); it != pList->end(); ++it )
        {
            IHandler* pHandler = *it;
            if(pHandler && pHandler->Available()) 
            {
                pMsg->End(pHandler);
            }
            else if ( pHandler && ! pHandler->Available())
            {
                LOGFMTD("Handle unavailable【%s:0x%08x】%s",
                    typeid(*pMsg).name(),
					pMsg,
					StringApi::UnicodeToMbcs(pMsg->ToString().c_str()));
            }
        }
    }
}

void MessageCenter::RecordMessage( const IMessage* pMsg ) const
{
    if ( _recordMessage && pMsg->RecordMessage() )
    {
        LOGFMTD("【%s:0x%08x】%s",
			typeid(*pMsg).name(),
			pMsg,
			StringApi::UnicodeToMbcs(pMsg->ToString().c_str()));
    }
}

void MessageCenter::RecordMessage( const IMessage* pMsg, bool begin ) const
{
    if ( _recordMessage && pMsg->RecordMessage() )
	{
		LOGFMTD("【%s:0x%08x】%s",
			typeid(*pMsg).name(),
			pMsg,
			begin? L"begin":L"end");
    }
}

void MessageCenter::RecordHandle(IHandler* p, const IMessage* pMsg ) const
{
    if ( _recordMessage && pMsg->RecordMessage() )
    {
        LOGFMTD("【%s:0x%08x】【%s:0x%08x】%s",
			typeid(*pMsg).name(),
			pMsg,
			typeid(*p).name(),
			p,
			StringApi::UnicodeToMbcs(pMsg->ToString().c_str()));
    }
}
