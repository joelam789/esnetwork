#ifndef _ESN_STRING_REPLY_PROCESS_H_
#define _ESN_STRING_REPLY_PROCESS_H_

#include "JsonClientMessageProcess.h"

namespace esnlib
{

class StringReplyProcess : public JsonClientMessageProcess
{
public:

	static const int MSG_TYPE = 2;

	static const int ACT_UPPER = 1;
	static const int ACT_LOWER = 2;

	virtual void Process(SessionPtr session, ClientBusiness* biz, json::Object& obj);

protected:
private:
};


}

#endif // _ESN_STRING_REPLY_PROCESS_H_
