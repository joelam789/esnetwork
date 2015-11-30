#define JSON_SSL

#define JSON_SERVER
//#define JSON_CLIENT
//#define JSON_CLIENTS

#ifdef JSON_CLIENTS
#ifndef JSON_CLIENT
#define JSON_CLIENT
#endif
#endif

#ifdef JSON_CLIENT
#define SERVER_IP_STR "127.0.0.1"
#endif

#ifdef JSON_CLIENTS
#define JSON_CLIENT_COUNT 100
#endif

#include <cstdlib>
#include <ctime>

#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#ifdef JSON_SERVER

#include "Server.h"
#include "JsonRequestMessageHandler.h"

#elif defined(JSON_CLIENT)

#include "Client.h"
#include "JsonReplyMessageHandler.h"

#endif

#include "JsonMessage.h"
#include "JsonMessageCodec.h"

#include "StringRequestProcess.h"
#include "StringReplyProcess.h"

#include "PrintStringProcess.h"


using namespace std;
using namespace esnlib;


#ifdef JSON_SERVER

void PrintHelp()
{
    std::cout << std::endl;

    std::cout << "[Commands of the Server] " << std::endl;
    std::cout << ":count   ---    Show summary" << std::endl;
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
        std::cin.getline(line, 256);
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

        if(cmd == ":count")
        {
            std::vector<SessionPtr> sessions;
            server->GetSessions(sessions);

            int sessionCount = sessions.size();
            int serviceTimes = 0;
            for(int i=0; i<sessionCount; i++)
            {
                SessionPtr session = sessions[i];
                ServerBusiness* svrbiz = JsonServerMessageProcess::GetServerBusinessFromSession(session);
                serviceTimes += svrbiz->GetProcessTimes();
            }

            std::cout << "Session Count: " << sessionCount << ", Process Times: " << serviceTimes << std::endl;

            continue;
        }
    }
}

#elif defined(JSON_CLIENT)

void PrintHelp()
{
    std::cout << std::endl;

#ifdef JSON_CLIENTS
    std::cout << "[Commands (total clients: " << JSON_CLIENT_COUNT << ")] " << std::endl;
    std::cout << ":start   ---    Let clients start to work" << std::endl;
    std::cout << ":count   ---    Show summary" << std::endl;
#else
    std::cout << "[Commands of the Client] " << std::endl;
#endif

    std::cout << ":quit    ---    Quit" << std::endl;
    std::cout << ":help    ---    Print this text" << std::endl;

#ifndef JSON_CLIENTS
    std::cout << "Any other string will be sent to server." << std::endl;
#endif

    std::cout << std::endl;

}

#ifdef JSON_CLIENTS
void HandleInput(IoHandlerPtr handler)
{
    srand(time(0));

    ClientBusiness::SetSingleMode(false);

    JsonReplyMessageHandler* msghandler = (JsonReplyMessageHandler*)handler.get();
    if(!msghandler) return;

    char line[256];
    std::string cmd = "";

    while (true)
    {
        std::cin.getline(line,256);
        cmd = line;

        if(cmd == ":quit")
        {
            msghandler->DisconnectAll();
            std::cout << "Quited" << std::endl;
            break;
        }

        if(cmd == ":help")
        {
            PrintHelp();
            continue;
        }

        if(cmd == ":start")
        {
            //msghandler->AutoConnectServer();
            msghandler->ConnectAll();
            continue;
        }

        if(cmd == ":count")
        {
            int sessionCount = msghandler->GetConnectedClientCount();
            int errorCount = msghandler->GetTotalErrorCount();
            int passCount = msghandler->GetTotalPassCount();
            int readCount = msghandler->GetTotalReadCount();

            std::cout << "Session Count: " << sessionCount
                      << ", Pass: " << passCount << ", Fail: " << errorCount
                      << ", Total Read: " << readCount << std::endl;

            continue;
        }

    }
}

#else

void HandleInput(ClientPtr client)
{
    srand(time(0));

    ClientBusiness::SetSingleMode(true);

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

            int reqact = StringRequestProcess::ACT_UPPER;
            std::string reqstr = cmd;

            SessionPtr session = client->GetCurrentSession();

            ClientBusiness* clibiz = JsonClientMessageProcess::GetClientBusinessFromSession(session);
            if(clibiz)
            {
                clibiz->SetLastRequest(reqact, 1, reqstr);

                json::Object request;
                request["action"] = reqact;
                request["text"] = reqstr;

                JsonMessage::SendObject(session.get(), StringRequestProcess::MSG_TYPE, request);
            }

        }

    }
}


#endif

#endif


int main()
{
    // print help
    PrintHelp();

#ifdef JSON_SERVER

    // Create the thread pool with 128~256 threads
    WorkManagerPtr workmgr = CreateWorkManager(128, 256, 8 * 1024); // on windows, 8K is the minimum thread stack size

    // Create the buffer pool with minimum 128 buffers, and every buffer will have 1024 bytes
    IoBufferManagerPtr bufmgr = CreateIoBufferManager(128, 1024);

    // Bind buffer pool to thread pool
    workmgr->SetBufferManager(bufmgr);

    // Create IO thread pool
    IoServiceManagerPtr svcmgr = CreateIoServiceManager();

    // Create IO Handler with thread pool so that we can use multiple threads to handle requests
    IoHandlerPtr handler(new JsonRequestMessageHandler(workmgr));

    // Create IO Filter to extract common messages (format: header + body)
    IoFilterPtr filter(new JsonMessageCodec());

    // Create the server
    ServerPtr server = CreateServer(1024, 1024, 1024);

    #ifdef JSON_SSL
    server->EnableSsl("server.crt", "server.key", "test");
    #endif

    // Set IO thread pool
    server->SetIoServiceManager(svcmgr);

    // Set IO Filter
    server->SetIoFilter(filter);

    // Set IO Handler
    server->SetIoHandler(handler);

    // Set memory pool
    server->SetIoBufferManager(bufmgr);

	// setup json message processes
    WorkPtr strprocess(new StringRequestProcess());
	workmgr->PutWork(StringRequestProcess::MSG_TYPE, strprocess);

    // Start to listen
    bool svrok = server->Start(9098);

    if (svrok)
    {
        boost::thread input(HandleInput, server);
        input.join();
    }

    std::cout << std::endl << "--- END OF SERVER ---" << std::endl;

#elif defined(JSON_CLIENT)

    // Create the thread pool with minimum 32~64 threads
    WorkManagerPtr workmgr = CreateWorkManager(32, 64, 8 * 1024);

    // Create the buffer pool with minimum 128 buffers, and every buffer will have 1024 bytes
    IoBufferManagerPtr bufmgr = CreateIoBufferManager(128, 1024);

    // Bind buffer pool to thread pool
    workmgr->SetBufferManager(bufmgr);

#ifdef JSON_CLIENTS

    // vector for multiple clients
    std::vector<ClientPtr> clients;

    // Create IO Handler with thread pool so that we can use multiple threads to handle responses
    IoHandlerPtr handler(new JsonReplyMessageHandler(workmgr, &clients, SERVER_IP_STR, 9098));

#else

    // Create IO Handler with thread pool so that we can use multiple threads to handle responses
    IoHandlerPtr handler(new JsonReplyMessageHandler(workmgr));

#endif

    // Create IO Filter to extract common messages (format: header + body)
    IoFilterPtr filter(new JsonMessageCodec());


#ifdef JSON_CLIENTS

    // Create IO thread pool
    IoServiceManagerPtr svcmgr = CreateIoServiceManager();

    for(int i=0; i<JSON_CLIENT_COUNT; i++)
    {
        // Create the client
        ClientPtr client = CreateClient(1024, 1024);

        client->SetId(i+1);

        #ifdef JSON_SSL
        client->EnableSsl(true, "server.crt");
        #endif

        // Set IO Filter
        client->SetIoFilter(filter);

        // Set IO Handler
        client->SetIoHandler(handler);

        // Set memory pool
        client->SetIoBufferManager(bufmgr);

        // Set IO thread pool
        client->SetIoServiceManager(svcmgr);

        // Add it to list
        clients.push_back(client);
    }

#else

    // Create the client
    ClientPtr client = CreateClient();

    // Set IO Filter
    client->SetIoFilter(filter);

    // Set IO Handler
    client->SetIoHandler(handler);

    // Set memory pool
    client->SetIoBufferManager(bufmgr);

#endif


	// setup json message processes
    WorkPtr strprocess(new StringReplyProcess());
	workmgr->PutWork(StringReplyProcess::MSG_TYPE, strprocess);

	WorkPtr printprocess(new PrintStringProcess());
	workmgr->PutWork(PrintStringProcess::MSG_TYPE, printprocess);


#ifdef JSON_CLIENTS

    boost::thread input(HandleInput, handler);
    input.join();

#else

    // Start to connect server
    bool cliok = client->Connect(SERVER_IP_STR, 9098);
    if (cliok)
    {
        boost::thread input(HandleInput, client);
        input.join();
    }

#endif

    std::cout << std::endl << "--- END OF CLIENT ---" << std::endl;

#endif

    return 0;
}
