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
    return false; // no orderly msg ...
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
    return MessageHandler::OnWrite(session, data);
}

void JsonReplyMessageHandler::OnConnect(SessionPtr session)
{
    MessageHandler::OnConnect(session);

    AutoConnectServer();

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

    //if(AutoConnectServer())
    //{
    //    AutoSendMessage();
    //}

}

BusinessPtr JsonReplyMessageHandler::CreateBusinessObj()
{
    BusinessPtr newobj(new ClientBusiness());
    return newobj;
}

void JsonReplyMessageHandler::SetServerIp(const std::string& svrip)
{
    m_svrip = svrip;
}
std::vector<ClientPtr>* JsonReplyMessageHandler::GetClients()
{
    return &m_clients;
}

bool JsonReplyMessageHandler::AutoConnectServer()
{
    if(m_clients.size() <= 0 || m_svrip.length() <= 0 || m_svrport == 0) return false;

    boost::unique_lock<boost::mutex> lock(m_cnnmutex);
    int clicount = m_clients.size();
    int cnncount = 0;
    int newcount = 0;
    for(int i=0; i<clicount; i++)
    {
        ClientPtr client = m_clients.at(i);
        bool connected = client->Connected();
        if (!connected && !client->IsConnecting())
        {
            client->Connect(m_svrip, m_svrport);
            newcount++;
            if(newcount >= 50)
            {
                boost::this_thread::sleep(boost::posix_time::milliseconds(50));
                break;
            }
        }
        else if(connected)
        {
            cnncount++;
        }
    }

    return cnncount >= clicount;
}

void JsonReplyMessageHandler::AutoSendMessage()
{
    if(m_clients.size() <= 0 || m_svrip.length() <= 0 || m_svrport == 0) return;

    boost::unique_lock<boost::mutex> lock(m_cnnmutex);
    int clicount = m_clients.size();
    for(int i=0; i<clicount; i++)
    {
        ClientPtr client = m_clients.at(i);
        SessionPtr session = client->GetCurrentSession();
        if(session)
        {
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

            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }
    }
}

void JsonReplyMessageHandler::DisconnectAll()
{
    if(m_clients.size() <= 0) return;

    int clicount = m_clients.size();
    for(int i=0; i<clicount; i++)
    {
        ClientPtr client = m_clients.at(i);
        if(client->Connected()) client->Disconnect();
    }
}

void JsonReplyMessageHandler::ConnectAll()
{
    if(m_clients.size() <= 0 || m_svrip.length() <= 0 || m_svrport == 0) return;

    int clicount = m_clients.size();
    for(int i=0; i<clicount; i++)
    {
        ClientPtr client = m_clients.at(i);
        if(!client->Connected())
        {
            std::cout << i+1 << " ";
            client->Connect(m_svrip, m_svrport);
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }
    }
}

int JsonReplyMessageHandler::GetConnectedClientCount()
{
    int total = 0;

    if(m_clients.size() > 0)
    {
        int clicount = m_clients.size();
        for(int i=0; i<clicount; i++)
        {
            ClientPtr client = m_clients.at(i);
            if(client->Connected()) total++;
        }
    }

    return total;
}

int JsonReplyMessageHandler::GetTotalErrorCount()
{
    int total = 0;

    if(m_clients.size() > 0)
    {
        int clicount = m_clients.size();
        for(int i=0; i<clicount; i++)
        {
            ClientPtr client = m_clients.at(i);
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

    if(m_clients.size() > 0)
    {
        int clicount = m_clients.size();
        for(int i=0; i<clicount; i++)
        {
            ClientPtr client = m_clients.at(i);
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

void JsonReplyMessageHandler::GetNumbers(int& cnnCount, int& errCount, int& passCount, int& readCount)
{
    int totalError = 0;
    int totalPass = 0;

    int totalCnn = 0;
    int totalRead = m_readcount;

    if(m_clients.size() > 0)
    {
        int clicount = m_clients.size();
        totalCnn = clicount;
        for(int i=0; i<clicount; i++)
        {
            ClientPtr client = m_clients.at(i);
            SessionPtr session = client->GetCurrentSession();

            ClientBusiness* clibiz = JsonClientMessageProcess::GetClientBusinessFromSession(session);
            if(clibiz)
            {
                totalError += clibiz->GetErrorCount();
                totalPass += clibiz->GetPassCount();
            }
        }
    }

    cnnCount = totalCnn;
    errCount = totalError;
    passCount = totalPass;
    readCount = totalRead;
}
