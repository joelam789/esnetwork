#include "JsonReplyMessageHandler.h"
#include "JsonClientMessageProcess.h"

using namespace esnlib;

ClientBusiness* JsonClientMessageProcess::GetClientBusinessFromSession(SessionPtr session)
{
    ClientBusiness* clibiz = NULL;
    if(session)
    {
        IoHandlerPtr handler = session->GetIoHandler();
        if(handler)
        {
            JsonReplyMessageHandler* jsonhandler = (JsonReplyMessageHandler*)handler.get();
            BusinessPtr biz = jsonhandler->GetBusinessObj(session);
            if(biz) clibiz = (ClientBusiness*)biz.get();
        }
    }

    return clibiz;
}

void JsonClientMessageProcess::Process(SessionPtr session, BusinessPtr biz, json::Object& msg)
{
	if(biz)
	{
		ClientBusiness* clibiz = (ClientBusiness*) biz.get();
		Process(session, clibiz, msg);
	}
}

void JsonClientMessageProcess::Process(SessionPtr session, ClientBusiness* biz, json::Object& msg)
{
	return;
}
