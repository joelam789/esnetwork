#ifndef _ESN_JSON_CLI_MSG_PROCESS_H_
#define _ESN_JSON_CLI_MSG_PROCESS_H_

#include "JsonMessageProcess.h"
#include "ClientBusiness.h"

namespace esnlib
{

class JsonClientMessageProcess : public JsonMessageProcess
{
public:

    static ClientBusiness* GetClientBusinessFromSession(SessionPtr session);

	virtual void Process(SessionPtr session, BusinessPtr biz, json::Object& msg);
	virtual void Process(SessionPtr session, ClientBusiness* biz, json::Object& msg);

protected:

private:
};

}

#endif // _ESN_JSON_CLI_MSG_PROCESS_H_
