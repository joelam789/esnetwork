#ifndef _ESN_CLAYWORK_H_
#define _ESN_CLAYWORK_H_

#include "IoWork.h"

namespace esnlib
{

class ClayWork : public IoWork
{
public:
    ClayWork();
    virtual ~ClayWork();

    virtual int Handle(IoBufferPtr data);

protected:
private:
};

typedef boost::shared_ptr<ClayWork> ClayWorkPtr;

}

#endif // CLAYWORK_H
