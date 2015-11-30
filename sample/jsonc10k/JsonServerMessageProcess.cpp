#include "JsonRequestMessageHandler.h"
#include "JsonServerMessageProcess.h"

using namespace esnlib;

ServerBusiness* JsonServerMessageProcess::GetServerBusinessFromSession(SessionPtr session)
{
    ServerBusiness* svrbiz = NULL;
    if(session)
    {
        IoHandlerPtr handler = session->GetIoHandler();
        if(handler)
        {
            JsonRequestMessageHandler* jsonhandler = (JsonRequestMessageHandler*)handler.get();
            BusinessPtr biz = jsonhandler->GetBusinessObj(session);
            if(biz) svrbiz = (ServerBusiness*)biz.get();
        }
    }

    return svrbiz;
}

void JsonServerMessageProcess::Process(SessionPtr session, BusinessPtr biz, json::Object& msg)
{
	if(biz)
	{
		ServerBusiness* svrbiz = (ServerBusiness*) biz.get();
		Process(session, svrbiz, msg);
	}
}

void JsonServerMessageProcess::Process(SessionPtr session, ServerBusiness* biz, json::Object& msg)
{
	return;
}
