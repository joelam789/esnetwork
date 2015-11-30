#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "JsonMessage.h"
#include "ClientBusiness.h"
#include "JsonClientMessageProcess.h"

#include "StringRequestProcess.h"
#include "StringReplyProcess.h"

using namespace esnlib;

static boost::mutex g_printmutex;

void StringReplyProcess::Process(SessionPtr session, ClientBusiness* biz, json::Object& obj)
{
	std::string actstr = "UNKNOWN";
	int action = obj["action"];
	std::string text = obj["text"];

	json::Object reply(obj);

	switch (action)
	{
		case ACT_UPPER:
			actstr = "UPPER";
			break;

		case ACT_LOWER:
			actstr = "LOWER";
			break;

		default:
			break;
	}

	//biz->CheckLastReply(action, text);

	//std::cout << text << std::endl;

	bool enough = biz->SetLastReply(action, text);

    if(!ClientBusiness::GetSingleMode())
    {
        if(!enough) return;
        text = biz->GetReplayStr();
    }

	bool passed = biz->CheckReply(action, text);

    if(ClientBusiness::GetSingleMode())
    {
        std::string result = "pass";
        if(!passed) result = biz->GetLastRequestStr();
        if(!passed) result = "fail";

        boost::unique_lock<boost::mutex> lock(g_printmutex);

        std::cout << "Action: " << actstr << ", Return: " << text << ", Request: " << biz->GetLastRequestStr()
                  << ", TotalError: " << biz->GetErrorCount() << std::endl;
    }

	if(!passed)
    {
        boost::unique_lock<boost::mutex> lock(g_printmutex);
        std::cout << "Error: " << actstr << ", Return: " << text
            << ", Request: " << biz->GetLastRequestStr() << std::endl;
    }

	if(session && biz && ClientBusiness::GetSingleMode() == false)
    {
        //boost::this_thread::sleep(boost::posix_time::milliseconds(50));

        if(biz)
        {
            int reqact = StringRequestProcess::ACT_UPPER;

            std::string wholestr = "";
            std::vector<std::string> strlist;
            for(int i=0; i<ClientBusiness::GetRequestStringCount(); i++)
            {
                std::string reqstr = ClientBusiness::GetRandomStr() + "_";
                wholestr = wholestr + reqstr;
                strlist.push_back(reqstr);
            }

            biz->SetLastRequest(reqact, strlist.size(), wholestr);

            for(size_t i=0; i<strlist.size(); i++)
            {
                json::Object request;
                request["action"] = reqact;
                request["text"] = strlist[i];

                JsonMessage::SendObject(session.get(), StringRequestProcess::MSG_TYPE, request);
            }
        }
    }

}
