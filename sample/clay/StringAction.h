#ifndef _ESN_STRINGACTION_H_
#define _ESN_STRINGACTION_H_

#include "clay.h"

namespace esnlib
{

class StringAction: public clay::core::ClayServiceProvider
{
public:

    static const int MCODE = 1002;
    static const int MSG_HEADER_SIZE = 32;

    explicit StringAction(int code);
    virtual ~StringAction();


    int UpperCase(void* pParam);

protected:

    virtual void SetupServices();

private:

};

}

#endif // STRINGACTION_H
