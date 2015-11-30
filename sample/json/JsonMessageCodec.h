#ifndef _ESN_JSONMESSAGECODEC_H_
#define _ESN_JSONMESSAGECODEC_H_

#include "MessageCodec.h"

namespace esnlib
{

class JsonMessageCodec : public MessageCodec
{
public:

    JsonMessageCodec();

    virtual IoBufferPtr Encode(SessionPtr session, void* data);
	virtual void* Decode(SessionPtr session, IoBufferPtr data);

protected:
private:
};

typedef boost::shared_ptr<JsonMessageCodec> JsonMessageCodecPtr;

}

#endif // _ESN_JSONMESSAGECODEC_H_
