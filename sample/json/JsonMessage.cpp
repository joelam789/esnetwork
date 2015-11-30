#include "JsonMessage.h"

using namespace esnlib;

JsonMessage::JsonMessage(int msgtype)
{
	m_jsonobj = NULL;
	m_jsonstr = "";
	m_type = msgtype;
}

JsonMessage::JsonMessage(int msgtype, const json::Object * obj)
{
	m_jsonobj = (json::Object *) obj;
	m_jsonstr = "";
	m_type = msgtype;
}

JsonMessage::JsonMessage(int msgtype, const std::string& str)
{
	m_jsonobj = NULL;
	m_jsonstr = str;
	m_type = msgtype;
}

JsonMessage::JsonMessage(int msgtype, const json::Object * obj, const std::string& str)
{
	m_jsonobj = (json::Object *) obj;
	m_jsonstr = str;
	m_type = msgtype;
}

json::Object * JsonMessage::GetJsonObj()
{
	return m_jsonobj;
}

void JsonMessage::SetJsonObj(json::Object * obj)
{
	m_jsonobj = obj;
}

const char * JsonMessage::GetJsonStr() const
{
	return m_jsonstr.c_str();
}

const std::string& JsonMessage::GetJsonStr()
{
	return m_jsonstr;
}

void JsonMessage::SetJsonStr(const std::string& str)
{
	m_jsonstr = str;
}

int JsonMessage::GetMessageType()
{
	return m_type;
}

int JsonMessage::GetMessageTypeFromBuffer(IoBuffer* data)
{
	data->SetReadPos(JsonMessage::SIGN_SIZE);
    return data->GetInt();
}

int JsonMessage::GetJsonStrLenFromBuffer(IoBuffer* data)
{
	data->SetReadPos(JsonMessage::LEN_POS);
    return data->GetInt();
}

void JsonMessage::Send(Session * session, JsonMessage * jsonmsg)
{
	if(session && jsonmsg) session->Write((void*)jsonmsg);
}

void JsonMessage::SendObject(Session * session, int msgtype, const json::Object & jsonobj)
{
	if(!session) return;
	JsonMessage msg(msgtype, &jsonobj);
	session->Write((void*)((&msg)));
}
