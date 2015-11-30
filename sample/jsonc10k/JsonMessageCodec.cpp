#include <string>
#include <iostream>

#include "json.h"
#include "JsonMessage.h"

#include "JsonMessageCodec.h"

using namespace esnlib;

JsonMessageCodec::JsonMessageCodec()
: MessageCodec(JsonMessage::HEADER_SIZE, JsonMessage::LEN_POS)
{
    //ctor
}

IoBufferPtr JsonMessageCodec::Encode(SessionPtr session, void* data)
{
	IoBufferPtr newtask;
    if(!data) return newtask;

	JsonMessage * msg = (JsonMessage *)data;

    json::Object * obj = msg->GetJsonObj();
	if(!obj) return newtask;

	std::string jsonstr = "";

	try
	{
		jsonstr = json::Serialize(*obj);
	}
	catch(...) { std::cout << "Exception found when encode json object to string. " << std::endl; }

	newtask = session->GetFreeBuffer(jsonstr.length() + JsonMessage::HEADER_SIZE);
    newtask->PutInt(JsonMessage::HEADER_SIGN);
	newtask->PutInt(msg->GetMessageType());
	newtask->PutInt(JsonMessage::DEFAULT_FLAG);
    newtask->PutInt(jsonstr.length());
	newtask->PutStr(jsonstr);
    newtask->SetWritePos(0);
    return newtask;
}

void* JsonMessageCodec::Decode(SessionPtr session, IoBufferPtr data)
{
	if(!data) return NULL;

	int jsonstrlen = JsonMessage::GetJsonStrLenFromBuffer(data.get());
	if(jsonstrlen <= 0) return NULL;

	int len = data->size();
    char lastchar = *(data->data() + (len - 1));
    if(lastchar != 0)
    {
        data->size(len + 1);
        data->SetWritePos(len);
        data->PutByte('\0');
        data->SetWritePos(0);
    }
    return data->data() + JsonMessage::HEADER_SIZE;
}

