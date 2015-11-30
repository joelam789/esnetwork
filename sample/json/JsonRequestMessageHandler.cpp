#include "ServerBusiness.h"
#include "JsonRequestMessageHandler.h"

using namespace esnlib;

bool JsonRequestMessageHandler::IsOrderlyMessage(IoBuffer* data)
{
    if(data->type() == 1) return true; // need orderly incoming msg ...
    return MessageHandler::IsOrderlyMessage(data);
}

void JsonRequestMessageHandler::OnConnect(SessionPtr session)
{
    MessageHandler::OnConnect(session);
    session->SetOrderlyHandling(1, true); // orderly incoming msg ...
}

int JsonRequestMessageHandler::OnRead(SessionPtr session, IoBufferPtr data)
{
    int flags = MessageHandler::OnRead(session, data);
    //std::cout << "type: "  << data->type() << " , flags: " << flags << " | ";
    return flags;
}

BusinessPtr JsonRequestMessageHandler::CreateBusinessObj()
{
    BusinessPtr newobj(new ServerBusiness());
    return newobj;
}
