#include "ChatHandler.h"

#include <cstdio>
#include <vector>

using namespace esnlib;

ChatHandler::ChatHandler()
{
    //ctor
}

ChatHandler::~ChatHandler()
{
    //dtor
}

void ChatHandler::HandleString(SessionPtr session, const std::string& str)
{
    std::vector<std::string> results;

    StringHandler::SplitStr(str, " ", results);

    if(results.size() <= 0) return;

    std::string cmd = results[0];
    cmd = StringHandler::TrimStr(cmd);

    std::string status = "";
    if(results.size() > 1)
    {
        status = results[1];
        status = StringHandler::TrimStr(status);
    }

    if(cmd == "LOGIN")
    {
        std::string outmsg = "";
        std::string allmsg = "";

        if(status == "OK")
        {
            printf("You have joined the chat session.\n");
        }
        else if(status == "ERROR")
        {
            printf("Login failed.\n");
        }
        else if(results.size() > 1)
        {
            std::string user = results[1];
            user = StringHandler::TrimStr(user);

            if(m_users.find(user) != m_users.end())
            {
                outmsg = "LOGIN ERROR user " + user + " already logged in.";
            }
            else
            {
                if(session->HasData())
                {
                    CommonDataPtr sessiondata = session->GetData();
                    sessiondata->PutStr("user", user);
                }

                m_users.insert(user);

                outmsg = "LOGIN OK";
                allmsg = "BROADCAST OK The user " + user + " has joined the chat session.";
            }
        }
        else
        {
            outmsg = "LOGIN ERROR invalid login command.";
        }

        if(outmsg.length() > 0) session->Write((void*)outmsg.c_str());
        if(allmsg.length() > 0) session->Broadcast((void*)allmsg.c_str());

    }
    else if(cmd == "QUIT")
    {
        if(status != "OK")
        {
            std::string user = "";

            if(session->HasData())
            {
                CommonDataPtr sessiondata = session->GetData();
                user = sessiondata->GetStr("user");
            }

            m_users.erase(user);

            std::string outmsg = "";
            std::string allmsg = "";

            outmsg = "QUIT OK";
            allmsg = "BROADCAST OK The user " + user + " is leaving ...";

            if(outmsg.length() > 0) session->Write((void*)outmsg.c_str());
            if(allmsg.length() > 0) session->Broadcast((void*)allmsg.c_str());
        }

        session->Close();
    }
    else if(cmd == "BROADCAST")
    {
        if(status != "OK")
        {
            int msgcount = results.size();
            std::string chatmsg = "";
            for(int i=1; i<msgcount; i++)
            {
                chatmsg = chatmsg + " " + results[i];
            }

            if(chatmsg.length() > 0)
            {
                std::string user = "";

                if(session->HasData())
                {
                    CommonDataPtr sessiondata = session->GetData();
                    user = sessiondata->GetStr("user");
                }

                chatmsg = "BROADCAST OK " + user + ": " + chatmsg;

                session->Broadcast((void*)chatmsg.c_str());

            }

        }
        else
        {
            int msgcount = results.size();
            std::string chatmsg = "";
            for(int i=2; i<msgcount; i++)
            {
                chatmsg = chatmsg + " " + results[i];
            }
            if(chatmsg.length() > 0)
            {
                std::string timestr = GetTimeStr();
                printf("[%s]%s\n", timestr.c_str(), chatmsg.c_str());
            }
        }
    }
    else
    {
        printf("Unknown chat command(%d): %s \n", session->GetId(), cmd.c_str());
    }

}

