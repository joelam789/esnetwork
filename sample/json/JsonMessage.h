#ifndef _ESN_JSONMESSAGE_H_
#define _ESN_JSONMESSAGE_H_

#include "json.h"
#include "IoBuffer.h"

namespace esnlib
{

class JsonMessage
{
public:

    static const int HEADER_SIGN = 0;
	static const int HEADER_SIZE = 16;

	static const int SIGN_SIZE = 4;
	static const int TYPE_SIZE = 4;
	static const int FLAG_SIZE = 4;
	static const int LEN_SIZE = 4;
	static const int LEN_POS = 12;

	static const int DEFAULT_FLAG = 3;

	explicit JsonMessage(int msgtype);
	JsonMessage(int msgtype, const json::Object * obj);
	JsonMessage(int msgtype, const std::string& str);
	JsonMessage(int msgtype, const json::Object * obj, const std::string& str);

	json::Object * GetJsonObj();
	void SetJsonObj(json::Object * obj);

	const char * GetJsonStr() const;
	const std::string& GetJsonStr();
	void SetJsonStr(const std::string& str);

	int GetMessageType();

	static int GetMessageTypeFromBuffer(IoBuffer* data);
	static int GetJsonStrLenFromBuffer(IoBuffer* data);

	static void Send(Session * session, JsonMessage * jsonmsg);
	static void SendObject(Session * session, int msgtype, const json::Object & jsonobj);

protected:
private:

	json::Object * m_jsonobj;
	std::string m_jsonstr;

	int m_type;

};

}

#endif // _ESN_JSONMESSAGE_H_
