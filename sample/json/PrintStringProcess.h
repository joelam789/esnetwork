#ifndef _ESN_PRINT_STRING_PROCESS_H_
#define _ESN_PRINT_STRING_PROCESS_H_

#include "IoWork.h"

namespace esnlib
{

class PrintStringProcess : public IoWork
{
public:

    static const int MSG_TYPE = 10;

    virtual int Handle(IoBufferPtr data);

protected:
private:
};

}

#endif // _ESN_PRINT_STRING_PROCESS_H_
