#include "ClayWork.h"
#include "IoBuffer.h"

#include "clay.h"

#include <boost/thread/mutex.hpp>

using namespace esnlib;

// because all clay functions are static, we need to use a static mutex to sync them ...
static boost::mutex g_claymutex;

ClayWork::ClayWork()
{
    //ctor
}

ClayWork::~ClayWork()
{
    //dtor
}
int ClayWork::Handle(IoBufferPtr data)
{
    clay::core::ClayServiceProvider * sp = NULL;
    clay::core::CLAY_SVC_CLASS_FUNC svc = NULL;

    IoBuffer* workdata = data.get();
    int spcode = workdata->code();  // Get the module code
    int svccode = workdata->flag(); // Get the function code
    if(spcode != 0 && svccode != 0)
    {
        // because all clay functions are static, we need to sync them ...
        boost::unique_lock<boost::mutex> lock(g_claymutex);

        // Get Provider by module code
        sp = clay::lib::GetProvider(spcode);

        // Get Service by function code
        if(sp != NULL) svc = sp->GetService(svccode);
    }

    int ret = 0;

    if(sp && svc)
    {
        // Fire the service function
        ret = (sp->*svc)((void*)workdata);
    }

    return ret;
}
