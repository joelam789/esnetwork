#ifndef _ESN_JSON_SVR_MSG_PROCESS_H_
#define _ESN_JSON_SVR_MSG_PROCESS_H_

#include "JsonMessageProcess.h"
#include "ServerBusiness.h"

namespace esnlib
{

class JsonServerMessageProcess : public JsonMessageProcess
{
public:

    static ServerBusiness* GetServerBusinessFromSession(SessionPtr session);

	virtual void Process(SessionPtr session, BusinessPtr biz, json::Object& msg);
	virtual void Process(SessionPtr session, ServerBusiness* biz, json::Object& msg);

protected:

private:
};

}

#endif // _ESN_JSON_SVR_MSG_PROCESS_H_
