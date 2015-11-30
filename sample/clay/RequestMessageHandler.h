#ifndef _ESN_REQUESTMESSAGEHANDLER_H_
#define _ESN_REQUESTMESSAGEHANDLER_H_

#include "MessageHandler.h"

namespace esnlib
{

class RequestMessageHandler : public MessageHandler
{
public:

    explicit RequestMessageHandler(WorkManagerPtr manager);
    RequestMessageHandler(WorkManagerPtr manager, WorkPtr work);

    virtual int GetIntKey(IoBuffer* data);

    virtual int GetCode(IoBuffer* data);

    virtual int GetFlag(IoBuffer* data);

protected:
private:
};

}

#endif // _ESN_REQUESTMESSAGEHANDLER_H_
