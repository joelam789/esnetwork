#include "RequestMessageHandler.h"

using namespace esnlib;

RequestMessageHandler::RequestMessageHandler(WorkManagerPtr manager)
: MessageHandler(manager)
{
    //ctor
}
RequestMessageHandler::RequestMessageHandler(WorkManagerPtr manager, WorkPtr work)
: MessageHandler(manager, work)
{
    //ctor
}

int RequestMessageHandler::GetIntKey(IoBuffer* data)
{
    data->SetReadPos(4);
    return data->GetShort();
}

int RequestMessageHandler::GetCode(IoBuffer* data)
{
    data->SetReadPos(4);
    return data->GetShort();
}

int RequestMessageHandler::GetFlag(IoBuffer* data)
{
    data->SetReadPos(6);
    return data->GetShort();
}
