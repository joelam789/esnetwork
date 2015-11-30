#ifndef _ESN_JSONREQMESSAGEHANDLER_H_
#define _ESN_JSONREQMESSAGEHANDLER_H_

#include "JsonMessageHandler.h"

namespace esnlib
{

class JsonRequestMessageHandler : public JsonMessageHandler
{
public:

    explicit JsonRequestMessageHandler(WorkManagerPtr manager): JsonMessageHandler(manager) { }

    virtual bool IsOrderlyMessage(IoBuffer* data);

    virtual void OnConnect(SessionPtr session);
    virtual int OnRead(SessionPtr session, IoBufferPtr data);

protected:

    virtual BusinessPtr CreateBusinessObj();

private:
};

}

#endif // _ESN_JSONREQMESSAGEHANDLER_H_
