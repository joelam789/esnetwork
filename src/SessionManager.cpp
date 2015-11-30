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

#include <iostream>
#include <boost/bind.hpp>

#include "SharedData.h"
#include "SessionManager.h"

using namespace esnlib;

SessionManager::SessionManager()
{
    //ctor
    m_sessions.clear();
    m_sessionidx = 0;

    m_idletype = 0;
    m_idletime = 0;

    m_idlethread = NULL;

    m_idlesvc = NULL;

    m_idlework = NULL;

    m_idletimer = NULL;

}

SessionManager::~SessionManager()
{
    //dtor

    if(m_idlesvc)
    {
        if(m_idlework || m_idletimer) m_idlesvc->reset();
        m_idlesvc->stop();
    }

    if(m_idlethread)
    {
        m_idlethread->join();
        delete m_idlethread;
        m_idlethread = NULL;
    }

    if(m_idlework)
    {
        delete m_idlework;
        m_idlework = NULL;
    }

    if(m_idletimer)
    {
        delete m_idletimer;
        m_idletimer = NULL;
    }

    if(m_idlesvc)
    {
        delete m_idlesvc;
        m_idlesvc = NULL;
    }

    Clear();
    m_sessionidx = 0;

    //std::cout << "\nReleased SessionManager\n " << std::endl;
}

void SessionManager::SetIdleTime(int idletype, int idletime)
{
    if(m_idletime > 0)
    {
        if(m_idlesvc)
        {
            if(m_idlework || m_idletimer) m_idlesvc->reset();
            m_idlesvc->stop();
        }

        if(m_idlethread)
        {
            m_idlethread->join();
            delete m_idlethread;
            m_idlethread = NULL;
        }

        if(m_idlework)
        {
            delete m_idlework;
            m_idlework = NULL;
        }

        if(m_idletimer)
        {
            delete m_idletimer;
            m_idletimer = NULL;
        }

        if(m_idlesvc)
        {
            delete m_idlesvc;
            m_idlesvc = NULL;
        }
    }

    if(idletime > 0)
    {
        m_idletype = idletype;
        m_idletime = idletime;

        m_idlesvc = new boost::asio::io_service();
        m_idlework = new boost::asio::io_service::work(*m_idlesvc);

        m_idletimer = new boost::asio::deadline_timer(*m_idlesvc);

        m_idletimer->expires_from_now(boost::posix_time::seconds(m_idletime));
        m_idletimer->async_wait(boost::bind(&SessionManager::OnIdleSessions, this,
                                            boost::asio::placeholders::error));

        m_idlethread = new boost::thread(boost::bind(&boost::asio::io_service::run, m_idlesvc));
    }
}

void SessionManager::OnIdleSessions(const boost::system::error_code &error)
{
    if(error)
    {
        std::cout << "Error - OnIdleSessions: " << error.message() << std::endl;
        return;
    }

    if(m_idletime > 0)
    {
        // check idle ...
        std::vector<SessionPtr> sessions;
        int total = GetSessions(sessions);
        if(total > 0)
        {
            std::for_each(sessions.begin(), sessions.end(),
                boost::bind(&Session::TestIdle, _1, m_idletype, m_idletime));
        }

        // next round ...
        if(m_idletimer)
        {
            m_idletimer->expires_from_now(boost::posix_time::seconds(m_idletime));
            m_idletimer->async_wait(boost::bind(&SessionManager::OnIdleSessions, this,
                                                boost::asio::placeholders::error));
        }

    }

}

void SessionManager::Clear()
{
    boost::unique_lock<boost::mutex> lockall(m_sessionmutex);
    m_sessions.clear();
}

void SessionManager::AddSession(SessionPtr session)
{
    boost::unique_lock<boost::mutex> lock(m_sessionmutex);
    m_sessionidx++;
    session->SetId(m_sessionidx);
    m_sessions.insert(std::map<int, SessionPtr>::value_type(m_sessionidx, session));
}

void SessionManager::RemoveSession(SessionPtr session)
{
    boost::unique_lock<boost::mutex> lock(m_sessionmutex);
    m_sessions.erase(session->GetId());
}

int SessionManager::GetSessionCount()
{
    return m_sessions.size();
}

int SessionManager::GetSessions(std::vector<SessionPtr>& sessions)
{
    int total = 0;
    if(m_sessions.size() > 0)
    {
        boost::unique_lock<boost::mutex> lock(m_sessionmutex);
        std::map<int,SessionPtr>::iterator itr;
        for(itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
        {
            SessionPtr session = itr->second;
            sessions.push_back(session);
            total++;
        }
    }
    return total;
}

SessionPtr SessionManager::GetSessionById(int id)
{
    SessionPtr session;
    if(m_sessions.size() > 0)
    {
        boost::unique_lock<boost::mutex> lock(m_sessionmutex);
        std::map<int,SessionPtr>::iterator itr = m_sessions.find(id);
        if(itr != m_sessions.end()) session = itr->second;
    }
    return session;
}

SessionPtr SessionManager::GetFirstSession()
{
    SessionPtr session;
    if(m_sessions.size() > 0)
    {
        boost::unique_lock<boost::mutex> lock(m_sessionmutex);
        std::map<int,SessionPtr>::iterator itr = m_sessions.begin();
        if(itr != m_sessions.end()) session = itr->second;
    }
    return session;
}

void SessionManager::Broadcast(IoBufferPtr data)
{
    boost::unique_lock<boost::mutex> lock(m_sessionmutex);
    std::for_each(m_sessions.begin(), m_sessions.end(),
        boost::bind(&Session::WriteBuffer,
                    boost::bind(&std::map<int,SessionPtr>::value_type::second, _1),
                    data));
}

void SessionManager::Broadcast(void* data)
{
    boost::unique_lock<boost::mutex> lock(m_sessionmutex);
    std::for_each(m_sessions.begin(), m_sessions.end(),
        boost::bind(&Session::WriteData,
                    boost::bind(&std::map<int,SessionPtr>::value_type::second, _1),
                    data));
}

bool SessionManager::HasData()
{
    return true;
}

CommonDataPtr SessionManager::GetData()
{
    CommonDataPtr data(new SharedData(m_datamutex,
                                      m_dataintintmap, m_datastrintmap,
                                      m_dataintstrmap, m_datastrstrmap,
                                      m_dataintbufmap, m_datastrbufmap,
                                      m_dataintobjmap, m_datastrobjmap));
    return data;
}

