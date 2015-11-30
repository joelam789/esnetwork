#include "StringAction.h"

#include <cstdio>

#include <boost/algorithm/string.hpp>

#include "IoBuffer.h"

using namespace esnlib;

static StringAction* g_pStringAction = new StringAction(StringAction::MCODE);

StringAction::StringAction(int code): clay::core::ClayServiceProvider(code)
{
    //ctor
    SetupServices();
    clay::lib::RegisterSp(this);
}

StringAction::~StringAction()
{
    //dtor
}

void StringAction::SetupServices()
{
    RegisterService(1, CLAY_SVC_FUNC(StringAction::UpperCase));
}

int StringAction::UpperCase(void* pParam)
{
    IoBuffer* data = (IoBuffer*)pParam;

    SessionPtr session = data->session();
    if(!session) return 0;

    int datasize = data->size();
    int headersize = StringAction::MSG_HEADER_SIZE;

    // Get output buffer from memory pool
    IoBufferPtr output = session->GetFreeBuffer(datasize);

    // Read body into a string
    std::string str(data->data() + headersize, data->size() - headersize);

    // Uppercase conversion
    boost::to_upper(str);

    // Set header
    output->PutBuf(data->data(), headersize);

    // Set body
    output->PutStr(str);

    // Show how to handle the session's local data
    if(session->HasData())
    {
        CommonDataPtr sessiondata = session->GetData();
        int totalreq = sessiondata->GetInt(0);
        totalreq++;
        sessiondata->PutInt(0, totalreq);

        printf("Handled Request(%d): %d \n", session->GetId(), totalreq);
    }

    datasize = output->size();

    // Write back
    session->Write(output);

    return datasize;
}

