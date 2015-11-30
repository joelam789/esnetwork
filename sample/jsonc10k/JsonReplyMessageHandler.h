#ifndef _ESN_JSONREPLYMESSAGEHANDLER_H_
#define _ESN_JSONREPLYMESSAGEHANDLER_H_

#include <vector>
#include <boost/thread/mutex.hpp>

#include "Client.h"

#include "JsonMessageHandler.h"

namespace esnlib
{

class JsonReplyMessageHandler : public JsonMessageHandler
{
public:

    explicit JsonReplyMessageHandler(WorkManagerPtr manager)
    :JsonMessageHandler(manager)
    , m_svrip("")
    , m_svrport(0)
    , m_readcount(0)
    { }

    JsonReplyMessageHandler(WorkManagerPtr manager,
                            const std::string& svrip,
                            int svrport)
    :JsonMessageHandler(manager)
    , m_svrip(svrip)
    , m_svrport(svrport)
    , m_readcount(0)
    { }

    virtual void OnConnect(SessionPtr session);
    virtual int OnRead(SessionPtr session, IoBufferPtr data);
    virtual int OnWrite(SessionPtr session, IoBufferPtr data);

    virtual bool IsOrderlyMessage(IoBuffer* data);

    void SetServerIp(const std::string& svrip);
    std::vector<ClientPtr>* GetClients();

    bool AutoConnectServer();
    void ConnectAll();
    void DisconnectAll();
    int GetConnectedClientCount();
    int GetTotalErrorCount();
    int GetTotalPassCount();
    int GetTotalReadCount();

    void GetNumbers(int& cnnCount, int& errCount, int& passCount, int& readCount);

protected:

    virtual BusinessPtr CreateBusinessObj();

    void AutoSendMessage();

private:

    boost::mutex m_cnnmutex;
    boost::mutex m_readmutex;

    std::vector<ClientPtr> m_clients;
    std::string m_svrip;
    int m_svrport;

    int m_readcount;

};

}

#endif // _ESN_JSONREPLYMESSAGEHANDLER_H_
