#include <string>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

#include "JsonMessage.h"
#include "StringReplyProcess.h"

#include "StringRequestProcess.h"

using namespace esnlib;

void StringRequestProcess::Process(SessionPtr session, ServerBusiness* biz, json::Object& msg)
{
	int action = msg["action"];
	std::string text = msg["text"];

	bool processed = false;

	json::Object reply(msg);

	switch (action)
	{
		case ACT_UPPER:
			boost::to_upper(text);
			reply["text"] = text;
			processed = true;
			break;

		case ACT_LOWER:
			boost::to_lower(text);
			reply["text"] = text;
			processed = true;
			break;

		default:
			break;
	}

	//boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

	if(processed)
	{
		biz->UpdateProcessTimes(1);
		//std::cout << "Session ID: " << session->GetId() << ", Process Times: " << biz->GetProcessTimes() << std::endl;
	}

	//std::cout << text << std::endl;

	JsonMessage::SendObject(session.get(), StringReplyProcess::MSG_TYPE, reply);
}
