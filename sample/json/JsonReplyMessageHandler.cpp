#include <boost/thread.hpp>

#include "JsonMessage.h"
#include "ClientBusiness.h"
#include "JsonClientMessageProcess.h"

#include "StringRequestProcess.h"
#include "PrintStringProcess.h"

#include "JsonReplyMessageHandler.h"

using namespace esnlib;

bool JsonReplyMessageHandler::IsOrderlyMessage(IoBuffer* data)
{
    if(data->type() == 2) return false; // allow outgoing messages to be concurrent
    else if(data->type() == 1) return true; // but need orderly incoming msg ...

    return MessageHandler::IsOrderlyMessage(data);
}

int JsonReplyMessageHandler::OnRead(SessionPtr session, IoBufferPtr data)
{
    if(session)
    {
        boost::unique_lock<boost::mutex> lock(m_readmutex);
        m_readcount++;
    }

    return MessageHandler::OnRead(session, data);
}

int JsonReplyMessageHandler::OnWrite(SessionPtr session, IoBufferPtr data)
{
    // if there are multiple clients, do nothing and return directly
    if(!ClientBusiness::GetSingleMode()) return MessageHandler::OnWrite(session, data);

    ClientBusiness* clibiz = JsonClientMessageProcess::GetClientBusinessFromSession(session);
    if(clibiz)
    {
        std::string jsonstr = "";

        char * str = (char*) session->DecodeBuffer(data);
        if(str) jsonstr = str;

        json::Object msg = json::Deserialize(jsonstr);
        std::string text = msg["text"];

        int reqact = StringRequestProcess::ACT_UPPER;

        clibiz->SetLastRequest(reqact, 1, text);
    }

    return MessageHandler::DefaultAsyncProcessOnWrite(session, data);

}

void JsonReplyMessageHandler::OnConnect(SessionPtr session)
{
    MessageHandler::OnConnect(session);
    session->SetOrderlyHandling(1, true); // orderly incoming msg ...

    if(session)
    {
        //boost::this_thread::sleep(boost::posix_time::milliseconds(50));

        ClientBusiness* clibiz = JsonClientMessageProcess::GetClientBusinessFromSession(session);
        if(clibiz && ClientBusiness::GetSingleMode() == false)
        {
            int reqact = StringRequestProcess::ACT_UPPER;

            std::string wholestr = "";
            std::vector<std::string> strlist;
            for(int i=0; i<ClientBusiness::GetRequestStringCount(); i++)
            {
                std::string reqstr = ClientBusiness::GetRandomStr() + "_";
                wholestr = wholestr + reqstr;
                strlist.push_back(reqstr);
            }

            clibiz->SetLastRequest(reqact, strlist.size(), wholestr);

            for(size_t i=0; i<strlist.size(); i++)
            {
                json::Object request;
                request["action"] = reqact;
                request["text"] = strlist[i];

                JsonMessage::SendObject(session.get(), StringRequestProcess::MSG_TYPE, request);
            }

        }
        else if(ClientBusiness::GetSingleMode() == true)
        {
            std::cout << std::endl;
            std::cout << "Please input a string and then it will be sent to server." << std::endl;
            std::cout << std::endl;
        }
    }

    //AutoConnectServer();

}

BusinessPtr JsonReplyMessageHandler::CreateBusinessObj()
{
    BusinessPtr newobj(new ClientBusiness());
    return newobj;
}

void JsonReplyMessageHandler::AutoConnectServer()
{
    if(!m_clients || m_svrip.length() <= 0 || m_svrport == 0) return;

    boost::unique_lock<boost::mutex> lock(m_cnnmutex);
    int clicount = m_clients->size();
    for(int i=0; i<clicount; i++)
    {
        ClientPtr client = m_clients->at(i);
        if(!client->Connected())
        {
            client->Connect(m_svrip, m_svrport);
            break;
        }
    }
}

void JsonReplyMessageHandler::DisconnectAll()
{
    if(!m_clients) return;

    int clicount = m_clients->size();
    for(int i=0; i<clicount; i++)
    {
        ClientPtr client = m_clients->at(i);
        if(client->Connected()) client->Disconnect();
    }
}

void JsonReplyMessageHandler::ConnectAll()
{
    if(!m_clients || m_svrip.length() <= 0 || m_svrport == 0) return;

    int clicount = m_clients->size();
    for(int i=0; i<clicount; i++)
    {
        ClientPtr client = m_clients->at(i);
        if(!client->Connected())
        {
            std::cout << i+1 << " ";
            client->Connect(m_svrip, m_svrport);
        }
    }
}

int JsonReplyMessageHandler::GetConnectedClientCount()
{
    int total = 0;

    if(m_clients)
    {
        int clicount = m_clients->size();
        for(int i=0; i<clicount; i++)
        {
            ClientPtr client = m_clients->at(i);
            if(client->Connected()) total++;
        }
    }

    return total;
}

int JsonReplyMessageHandler::GetTotalErrorCount()
{
    int total = 0;

    if(m_clients)
    {
        int clicount = m_clients->size();
        for(int i=0; i<clicount; i++)
        {
            ClientPtr client = m_clients->at(i);
            SessionPtr session = client->GetCurrentSession();

            ClientBusiness* clibiz = JsonClientMessageProcess::GetClientBusinessFromSession(session);
            if(clibiz)
            {
                total += clibiz->GetErrorCount();
            }
        }
    }

    return total;
}

int JsonReplyMessageHandler::GetTotalPassCount()
{
    int total = 0;

    if(m_clients)
    {
        int clicount = m_clients->size();
        for(int i=0; i<clicount; i++)
        {
            ClientPtr client = m_clients->at(i);
            SessionPtr session = client->GetCurrentSession();

            ClientBusiness* clibiz = JsonClientMessageProcess::GetClientBusinessFromSession(session);
            if(clibiz)
            {
                total += clibiz->GetPassCount();
            }
        }
    }

    return total;
}

int JsonReplyMessageHandler::GetTotalReadCount()
{
    return m_readcount;
}
