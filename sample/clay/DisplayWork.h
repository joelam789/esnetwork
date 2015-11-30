#ifndef _ESN_DISPLAYWORK_H_
#define _ESN_DISPLAYWORK_H_

#include "IoWork.h"

namespace esnlib
{

class DisplayWork : public IoWork
{
public:
    DisplayWork();
    virtual ~DisplayWork();

    virtual int Handle(IoBufferPtr data);

protected:
private:
};

}

#endif // _ESN_DISPLAYWORK_H_
