#include <string>
#include <iostream>

#include "IoHandler.h"
#include "IoBuffer.h"

#include "JsonMessageHandler.h"

#include "JsonMessageProcess.h"

using namespace esnlib;

int JsonMessageProcess::Handle(IoBufferPtr data)
{
	if(!data) return 0;

	SessionPtr session = data->session();
    if(!session) return 0;

    BusinessPtr biz;

    std::string jsonstr = "";

	try
	{
        char * str = (char*) session->DecodeBuffer(data);
        if(str) jsonstr = str;

		json::Object msg = json::Deserialize(jsonstr);

		IoHandlerPtr handler = session->GetIoHandler();
		if(handler)
        {
            JsonMessageHandler* jsonhandler = (JsonMessageHandler*)handler.get();
            if(jsonhandler) biz = jsonhandler->GetBusinessObj(session);
        }

        if(biz) Process(session, biz, msg);
	}
	catch(...) { std::cout << "Exception found when handle json string: " << std::endl << jsonstr << std::endl; }

	if(!biz) std::cout << "Fail to find business object! Session: " << session->GetId() << std::endl;

    return 0;
}

void JsonMessageProcess::Process(SessionPtr session, BusinessPtr biz, json::Object& msg)
{
	return;
}
