#include <cstdio>

#include "JsonMessage.h"
#include "JsonMessageHandler.h"

using namespace esnlib;

JsonMessageHandler::JsonMessageHandler(WorkManagerPtr manager)
: MessageHandler(manager)
{
    //ctor
}

int JsonMessageHandler::GetIntKey(IoBuffer* data)
{
    return JsonMessage::GetMessageTypeFromBuffer(data);
}


BusinessPtr JsonMessageHandler::CreateBusinessObj()
{
    BusinessPtr newobj(new Business());
    return newobj;
}

BusinessPtr JsonMessageHandler::GetBusinessObj(SessionPtr session)
{
    BusinessPtr biz;

    if(session->HasData())
    {
        CommonDataPtr sessiondata = session->GetData();
        biz = sessiondata->GetObj(Business::BASE_CODE);
        if(!biz)
        {
            biz = CreateBusinessObj();
            sessiondata->PutObj(Business::BASE_CODE, biz);
        }
    }

    return biz;
}
