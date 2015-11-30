
#define CLAY_SERVER
//#define CLAY_CLIENT

#include <iostream>
#include <boost/thread.hpp>

#ifdef CLAY_SERVER
#include "Server.h"
#include "ClayWork.h"
#include "RequestMessageHandler.h"
#elif defined(CLAY_CLIENT)
#include "Client.h"
#include "DisplayWork.h"
#include "MessageHandler.h"
#endif

#include "StringAction.h"
#include "MessageCodec.h"


using namespace std;
using namespace esnlib;


#ifdef CLAY_SERVER

void PrintHelp()
{
    std::cout << std::endl;

    std::cout << "[Commands of the Server] " << std::endl;
    std::cout << ":quit    ---    Quit" << std::endl;
    std::cout << ":help    ---    Print this text" << std::endl;

    std::cout << std::endl;

}

void HandleInput(ServerPtr server)
{
    char line[256];
    std::string cmd = "";

    while (true)
    {
        std::cin.getline(line,256);
        cmd = line;

        if(cmd == ":quit")
        {
            server->Stop();
            break;
        }

        if(cmd == ":help")
        {
            PrintHelp();
            continue;
        }
    }
}

#elif defined(CLAY_CLIENT)

void PrintHelp()
{
    std::cout << std::endl;

    std::cout << "[Commands of the Client] " << std::endl;
    std::cout << ":quit    ---    Quit" << std::endl;
    std::cout << ":help    ---    Print this text" << std::endl;
    std::cout << "Any other string will be sent to server." << std::endl;

    std::cout << std::endl;

}

void HandleInput(ClientPtr client)
{
    char line[256];
    std::string cmd = "";

    bool cnnok = false;

    int reqkey = 0;

    while (true)
    {
        std::cin.getline(line,256);
        cmd = line;

        if(!client->Connected())
        {
            if(cmd == ":quit")
            {
                client->Disconnect();
                std::cout << "Quited client" << std::endl;
                break;
            }
            else if(!cnnok)
            {
                std::cout << "Connecting ... please wait for a while" << std::endl;
                continue;
            }
            else
            {
                std::cout << "Lost connection to server ..." << std::endl;
                break;
            }
        }
        else cnnok = true;

        if(cmd == ":quit")
        {
            client->Disconnect();
            std::cout << "Quited" << std::endl;
            break;
        }

        if(cmd == ":help")
        {
            PrintHelp();
            continue;
        }

        if(cmd.length() > 0 && cmd[0] != ':')
        {
            reqkey++;

            int len = cmd.length();
            int mcode = StringAction::MCODE;
            int headersize = StringAction::MSG_HEADER_SIZE;

            IoBufferPtr outbuf = client->GetFreeBuffer(len + headersize);

            // message header (32 bytes)
            outbuf->PutShort(1);     // production
            outbuf->PutShort(1);     // productionVersion
            outbuf->PutShort(mcode); // actionCode
            outbuf->PutShort(1);     // functionCode
            outbuf->PutInt(1);       // packageCount
            outbuf->PutInt(1);       // packageIndex
            outbuf->PutInt(reqkey);  // actionKey
            outbuf->PutInt(0);       // messageFlag
            outbuf->PutInt(len);     // messageTotalLength
            outbuf->PutInt(len);     // messageBodyLength

            // message body
            outbuf->PutStr(cmd);

            client->Write(outbuf);
        }

    }
}

#endif


int main()
{
    // print help
    PrintHelp();

#ifdef CLAY_SERVER

    // Create the work object, which will use clay library in thread's callback function
    WorkPtr claywork(new ClayWork());

    // Create the thread pool with minimum 128 threads
    WorkManagerPtr workmgr = CreateWorkManager(128);

    // Create the buffer pool with minimum 128 buffers, and every buffer will have 8192 bytes
    IoBufferManagerPtr bufmgr = CreateIoBufferManager(128, 8192);

    // Bind buffer pool to thread pool
    workmgr->SetBufferManager(bufmgr);

    // Create IO Handler with thread pool and the work object so that we can use multiple threads to handle requests
    IoHandlerPtr handler(new RequestMessageHandler(workmgr, claywork));

    // Create IO Filter to extract common messages (format: header + body)
    IoFilterPtr filter(new MessageCodec(32, 28, 4, 2 * 1024));

    // Create the server
    ServerPtr server = CreateServer();

    // Set IO Filter
    server->SetIoFilter(filter);

    // Set IO Handler
    server->SetIoHandler(handler);

    // Set memory pool
    server->SetIoBufferManager(bufmgr);

    // check for read idle, limit: 10 seconds
    //server->SetIdleTime(1, 10);

    // Start to listen
    bool svrok = server->Start(9098);

    if (svrok)
    {
        boost::thread input(HandleInput, server);
        input.join();
    }

    std::cout << std::endl << "--- END OF SERVER ---" << std::endl;

#elif defined(CLAY_CLIENT)

    // Create the work object, which will print the result string in thread's callback function
    WorkPtr displaywork(new DisplayWork());

    // Create the thread pool with minimum 32 threads
    WorkManagerPtr workmgr = CreateWorkManager(32);

    // Create the buffer pool with minimum 128 buffers, and every buffer will have 8192 bytes
    IoBufferManagerPtr bufmgr = CreateIoBufferManager(128, 8192);

    // Bind buffer pool to thread pool
    workmgr->SetBufferManager(bufmgr);

    // Create IO Handler with thread pool and the work object so that we can use multiple threads to handle responses
    IoHandlerPtr handler(new MessageHandler(workmgr, displaywork));

    // Create IO Filter to extract common messages (format: header + body)
    IoFilterPtr filter(new MessageCodec(32, 28, 4, 2 * 1024));

    // Create the client
    ClientPtr client = CreateClient();

    // Set IO Filter
    client->SetIoFilter(filter);

    // Set IO Handler
    client->SetIoHandler(handler);

    // Set memory pool
    client->SetIoBufferManager(bufmgr);

    // Other settings
    //client->SetConnectTimeOut(3); // set connect time out time: 3 seconds
    //client->SetIdleTime(2, 5); // check for write idle, limit: 5 seconds

    // Start to connect server
    bool cliok = client->Connect("localhost", 9098);

    if (cliok)
    {
        boost::thread input(HandleInput, client);
        input.join();
    }

    std::cout << std::endl << "--- END OF CLIENT ---" << std::endl;

#endif

    return 0;
}
