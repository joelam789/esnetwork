#ifndef _ESN_STRING_REQ_PROCESS_H_
#define _ESN_STRING_REQ_PROCESS_H_

#include "JsonServerMessageProcess.h"

namespace esnlib
{

class StringRequestProcess : public JsonServerMessageProcess
{
public:

	static const int MSG_TYPE = 1;

	static const int ACT_UPPER = 1;
	static const int ACT_LOWER = 2;

	virtual void Process(SessionPtr session, ServerBusiness* biz, json::Object& msg);

protected:
private:
};


}

#endif // _ESN_STRING_REQ_PROCESS_H_
