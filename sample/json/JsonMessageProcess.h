#ifndef _ESN_JSONPMSGROCESS_H_
#define _ESN_JSONPMSGROCESS_H_

#include "Business.h"
#include "Session.h"
#include "IoWork.h"
#include "json.h"

namespace esnlib
{

class JsonMessageProcess : public IoWork
{
public:

    virtual int Handle(IoBufferPtr data);
	virtual void Process(SessionPtr session, BusinessPtr biz, json::Object& msg);

protected:

private:
};

//typedef boost::shared_ptr<JsonProcess> JsonProcessPtr;

}

#endif // _ESN_JSONPMSGROCESS_H_
