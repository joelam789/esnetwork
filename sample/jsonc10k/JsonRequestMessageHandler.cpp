#include "ServerBusiness.h"
#include "JsonRequestMessageHandler.h"

using namespace esnlib;

bool JsonRequestMessageHandler::IsOrderlyMessage(IoBuffer* data)
{
    return false; // no orderly msg ...
}

void JsonRequestMessageHandler::OnConnect(SessionPtr session)
{
    MessageHandler::OnConnect(session);
    if(session) session->SetOrderlyHandling(1, false); // no orderly msg ...
}

BusinessPtr JsonRequestMessageHandler::CreateBusinessObj()
{
    BusinessPtr newobj(new ServerBusiness());
    return newobj;
}

