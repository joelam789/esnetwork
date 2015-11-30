#ifndef _ESN_JSONMESSAGEHANDLER_H_
#define _ESN_JSONMESSAGEHANDLER_H_

#include "Business.h"
#include "MessageHandler.h"

namespace esnlib
{

class JsonMessageHandler : public MessageHandler
{
public:

    explicit JsonMessageHandler(WorkManagerPtr manager);

    virtual int GetIntKey(IoBuffer* data);

    virtual BusinessPtr GetBusinessObj(SessionPtr session);


protected:

    virtual BusinessPtr CreateBusinessObj();

private:
};

}

#endif // _ESN_REQUESTMESSAGEHANDLER_H_
