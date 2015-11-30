#include "DisplayWork.h"

#include "IoBuffer.h"
#include "StringAction.h"

#include <cstdio>

#include <boost/thread/mutex.hpp>

using namespace esnlib;

static boost::mutex g_printmutex;

DisplayWork::DisplayWork()
{
    //ctor
}

DisplayWork::~DisplayWork()
{
    //dtor
}

int DisplayWork::Handle(IoBufferPtr data)
{
    IoBuffer* workdata = data.get();

    SessionPtr session = workdata->session();
    if(!session) return 0;

    int headersize = StringAction::MSG_HEADER_SIZE;

    std::string str(workdata->data() + headersize, workdata->size() - headersize);

    int len = str.length();

    if(len > 0)
    {
        boost::unique_lock<boost::mutex> lock(g_printmutex);
        printf("Result: %s \n", str.c_str());
    }

    return len;
}
