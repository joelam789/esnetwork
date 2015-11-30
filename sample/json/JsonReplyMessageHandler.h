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
    , m_clients(NULL)
    , m_svrip("")
    , m_svrport(0)
    , m_readcount(0)
    { }

    JsonReplyMessageHandler(WorkManagerPtr manager,
                            std::vector<ClientPtr> * clients,
                            const std::string& svrip,
                            int svrport)
    :JsonMessageHandler(manager)
    , m_clients(clients)
    , m_svrip(svrip)
    , m_svrport(svrport)
    , m_readcount(0)
    { }

    virtual void OnConnect(SessionPtr session);
    virtual int OnRead(SessionPtr session, IoBufferPtr data);
    virtual int OnWrite(SessionPtr session, IoBufferPtr data);

    virtual bool IsOrderlyMessage(IoBuffer* data);

    void AutoConnectServer();
    void ConnectAll();
    void DisconnectAll();
    int GetConnectedClientCount();
    int GetTotalErrorCount();
    int GetTotalPassCount();
    int GetTotalReadCount();

protected:

    virtual BusinessPtr CreateBusinessObj();

private:

    boost::mutex m_cnnmutex;
    boost::mutex m_readmutex;

    std::vector<ClientPtr> * m_clients;
    std::string m_svrip;
    int m_svrport;

    int m_readcount;

};

}

#endif // _ESN_JSONREPLYMESSAGEHANDLER_H_
