
#define CHAT_SERVER
//#define CHAT_CLIENT

#include <iostream>
#include <boost/thread.hpp>

#ifdef CHAT_SERVER
#include "Server.h"
#elif defined(CHAT_CLIENT)
#include "Client.h"
#endif

#include "StringCodec.h"
#include "ChatHandler.h"


using namespace std;
using namespace esnlib;


#ifdef CHAT_SERVER

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

#elif defined(CHAT_CLIENT)

void PrintHelp()
{
    std::cout << std::endl;

    std::cout << "[Commands of the Client] " << std::endl;
    std::cout << ":quit              ---    Quit" << std::endl;
    std::cout << ":help              ---    Print this text" << std::endl;
    std::cout << ":login username    ---    Login with the username (should always use first)" << std::endl;
    std::cout << "Any other string will be sent to server as chat words (only after login)." << std::endl;

    std::cout << std::endl;

}

void HandleInput(ClientPtr client)
{
    char line[256];
    std::string cmd = "";
    std::string msg = "";

    bool cnnok = false;

    while (true)
    {
        std::cout << std::endl;
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

        std::vector<std::string> results;
        std::string str = line;

        ChatHandler::SplitStr(str, " ", results);

        if(results.size() <= 0) continue;

        cmd = results[0];
        cmd = ChatHandler::TrimStr(cmd);

        msg = "";
        if(results.size() > 1)
        {
            msg = results[1];
            msg = ChatHandler::TrimStr(msg);
        }

        if(cmd == ":login" && msg.length() > 0)
        {
            std::string outmsg = "LOGIN " + msg;
            client->Write((void*)outmsg.c_str());
            continue;
        }

        if(cmd == ":quit")
        {
            std::string outmsg = "QUIT";
            client->Write((void*)outmsg.c_str());
            client->Disconnect();
            break;
        }

        if(cmd == ":help")
        {
            PrintHelp();
            continue;
        }

        if(cmd.length() > 0 && cmd[0] != ':')
        {
            std::string outmsg = "BROADCAST " + str;
            client->Write((void*)outmsg.c_str());
        }

    }
}

#endif


int main()
{
    // print help
    PrintHelp();

    // Create the buffer pool with minimum 128 buffers, and every buffer will have 8192 bytes
    IoBufferManagerPtr bufmgr = CreateIoBufferManager(128, 8192);

    // Create IO Handler
    IoHandlerPtr handler(new ChatHandler());

    // Create IO Filter to extract the chat message (normal string which ends with a specific char)
    IoFilterPtr filter(new StringCodec());


#ifdef CHAT_SERVER

    // Create the server
    ServerPtr server = CreateServer();

    // Set IO Filter
    server->SetIoFilter(filter);

    // Set IO Handler
    server->SetIoHandler(handler);

    // Set memory pool
    server->SetIoBufferManager(bufmgr);

    // Start to listen
    bool svrok = server->Start(9099);

    if (svrok)
    {
        std::cout << "Server Address: " << server->GetDefaultLocalIp() << ":" << server->GetCurrentPort() << std::endl;

        boost::thread input(HandleInput, server);
        input.join();
    }

    std::cout << std::endl << "--- END OF SERVER ---" << std::endl;


#elif defined(CHAT_CLIENT)

    // Create the client
    ClientPtr client = CreateClient();

    // Set IO Filter
    client->SetIoFilter(filter);

    // Set IO Handler
    client->SetIoHandler(handler);

    // Set memory pool
    client->SetIoBufferManager(bufmgr);

    // Start to connect server
    bool cliok = client->Connect("localhost", 9099);

    if (cliok)
    {
        //boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

        size_t waitcounter = 1;
        while(client->IsConnecting() && waitcounter < 50)
        {
            waitcounter++;
            boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }

        std::cout << "Client Address: " << client->GetLocalIp() << ":" << client->GetLocalPort() << std::endl;

        boost::thread input(HandleInput, client);
        input.join();
    }

    std::cout << std::endl << "--- END OF CLIENT ---" << std::endl;



#endif


    return 0;

}
