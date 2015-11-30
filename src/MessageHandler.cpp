/*
-----------------------------------------------------------------------------
This source file is part of "esnetwork" library.
It is licensed under the terms of the BSD license.
For the latest info, see http://esnetwork.sourceforge.net

Copyright (c) 2012-2013 Lin Jia Jun (Joe Lam)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

//#include <cstdio>
#include <sstream>

#include "LogManager.h"

#include "MessageHandler.h"

using namespace esnlib;

MessageHandler::MessageHandler()
{
    //ctor
}

MessageHandler::MessageHandler(WorkManagerPtr manager)
: m_manager(manager)
{
    //ctor
}

MessageHandler::MessageHandler(WorkManagerPtr manager, WorkPtr work)
: m_manager(manager), m_work(work)
{
    //ctor
}

MessageHandler::~MessageHandler()
{
    //dtor
}


WorkManagerPtr MessageHandler::GetWorkManager()
{
    return m_manager;
}

WorkPtr MessageHandler::GetDefaultWork()
{
    return m_work;
}

int MessageHandler::GetCode(IoBuffer* data)
{
    return 0;
}
int MessageHandler::GetFlag(IoBuffer* data)
{
    return 0;
}

int MessageHandler::GetIntKey(IoBuffer* data)
{
    return 0;
}
std::string MessageHandler::GetStrKey(IoBuffer* data)
{
    return "";
}
bool MessageHandler::IsOrderlyMessage(IoBuffer* data)
{
    return false; // assume the message is NOT orderly by default
}
void MessageHandler::HandleMessage(SessionPtr session, IoBufferPtr data)
{
    return;
}

int MessageHandler::DefaultAsyncProcessOnWrite(SessionPtr session, IoBufferPtr data)
{
    int flags = 0; // in sync and no concurrency by default

    if(m_manager)
    {
        if(m_work)
        {
            int code = GetCode(data.get());
            int flag = GetFlag(data.get());

            data->code(code);
            data->flag(flag);

            data->SetReadPos(0);
            data->SetWritePos(0);

            flags = flags | 1; // async process
            if(!IsOrderlyMessage(data.get()) || !session->GetOrderlyHandling(2))
            {
                flags = flags | 2; // disordered messages support concurrency
                data->type(0); // concurrency needs data with no type
            }

            m_manager->AddWorkTask(m_work, data); // async process data in another thread

            return flags;
        }
        else
        {
            WorkPtr work;
            data->SetReadPos(0);
            int key = GetIntKey(data.get());
            if(key != 0) work = m_manager->GetWork(key);
            else
            {
                data->SetReadPos(0);
                std::string strkey = GetStrKey(data.get());
                if(strkey.length() > 0) work = m_manager->GetWork(strkey);
            }

            if(work)
            {
                data->SetReadPos(0);
                data->SetWritePos(0);

                flags = flags | 1; // async process
                if(!IsOrderlyMessage(data.get()) || !session->GetOrderlyHandling(2))
                {
                    flags = flags | 2; // disordered messages support concurrency
                    data->type(0); // concurrency needs data with no type
                }

                m_manager->AddWorkTask(work, data); // async process data in another thread

                return flags;
            }
        }
    }
    else
    {
        HandleMessage(session, data); // process data in current thread (in sync)
    }

    return flags;
}

int MessageHandler::OnRead(SessionPtr session, IoBufferPtr data)
{
    int flags = 0; // in sync and no concurrency by default

    if(m_manager)
    {
        if(m_work)
        {
            int code = GetCode(data.get());
            int flag = GetFlag(data.get());

            data->code(code);
            data->flag(flag);

            data->SetReadPos(0);
            data->SetWritePos(0);

            flags = flags | 1; // async process
            if(!IsOrderlyMessage(data.get()) || !session->GetOrderlyHandling(1))
            {
                flags = flags | 2; // disordered messages support concurrency
                data->type(0); // concurrency needs data with no type
            }

            m_manager->AddWorkTask(m_work, data); // async process data in another thread

            return flags;
        }
        else
        {
            WorkPtr work;
            data->SetReadPos(0);
            int key = GetIntKey(data.get());
            if(key != 0) work = m_manager->GetWork(key);
            else
            {
                data->SetReadPos(0);
                std::string strkey = GetStrKey(data.get());
                if(strkey.length() > 0) work = m_manager->GetWork(strkey);
            }

            if(work)
            {
                data->SetReadPos(0);
                data->SetWritePos(0);

                flags = flags | 1; // async process
                if(!IsOrderlyMessage(data.get()) || !session->GetOrderlyHandling(1))
                {
                    flags = flags | 2; // disordered messages support concurrency
                    data->type(0); // concurrency needs data with no type
                }

                m_manager->AddWorkTask(work, data); // async process data in another thread

                return flags;
            }
        }
    }
    else
    {
        HandleMessage(session, data); // process data in current thread (in sync)
    }

    return flags;

}
int MessageHandler::OnWrite(SessionPtr session, IoBufferPtr data)
{
    //printf("OnWrite: %d \n", data->size());
    return 0; // in sync and no concurrency by default
}

void MessageHandler::OnConnect(SessionPtr session)
{
    //session->SetOrderlyHandling(1, false);
    //session->SetOrderlyHandling(2, false);
    //printf("Network - Connect Event : %d \n", session->GetId());

    std::stringstream ss;
    ss << "Network - Connect Event : " << session->GetId();
    LogManager::Info(ss.str());

}
void MessageHandler::OnDisconnect(SessionPtr session)
{
    //printf("Network - Disconnect Event : %d \n", session->GetId());
    std::stringstream ss;
    ss << "Network - Disconnect Event : " << session->GetId();
    LogManager::Info(ss.str());
}

void MessageHandler::OnIdle(SessionPtr session, int idleType)
{
    std::stringstream ss;
    switch (idleType)
    {
        case 0: ss << "Network - Both Idle (" << session->GetId() << ")"; LogManager::Info(ss.str()); break;
        case 1: ss << "Network - Receiving Idle (" << session->GetId() << ")"; LogManager::Info(ss.str()); break;
        case 2: ss << "Network - Sending Idle (" << session->GetId() << ")"; LogManager::Info(ss.str()); break;
        default: ss << "Network Idle (" << session->GetId() << ")"; LogManager::Info(ss.str()); break;
    }

    //session->Close(); // auto close idle session
}

void MessageHandler::OnError(SessionPtr session, int errorType, int errorCode, const std::string& errorMsg)
{
    std::stringstream ss;
    switch (errorType)
    {
        case -1: ss << "Network - Listening Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
        case  0: ss << "Network - Connection Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
        case  1: ss << "Network - Receiving Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
        case  2: ss << "Network - Sending Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
        default: ss << "Network Error (" << session->GetId() << ") : " << errorMsg; LogManager::Warning(ss.str()); break;
    }

    /*
    if(errorCode == 2        // boost::asio::error::eof
       || errorCode == 995   // boost::asio::error::operation_aborted
       || errorCode == 10009 // boost::asio::error::bad_descriptor
       || errorCode == 10054 // boost::asio::error::connection_reset
    ) session->Close();
    */

    session->Close();
}

