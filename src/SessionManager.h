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

#ifndef _ESN_SESSIONMANAGER_H_
#define _ESN_SESSIONMANAGER_H_

#include <map>
#include <vector>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

#include "IoBuffer.h"
#include "Business.h"
#include "Session.h"

namespace esnlib
{

/// Session Manager class
class SessionManager
{
public:

    SessionManager();
    ~SessionManager();

    /// Add a session
    ///
    /// @param session The pointer of the session
    void AddSession(SessionPtr session);

    /// Remove a session
    ///
    /// @param session The pointer of the session
    void RemoveSession(SessionPtr session);

    /// Get total number of all sessions
    ///
    /// @return Total number of all sessions
    int GetSessionCount();

    /// Get all sessions
    ///
    /// @param sessions The vector used to save all the sessions
    /// @return Total number of all sessions
    int GetSessions(std::vector<SessionPtr>& sessions);

    /// Get a session by its ID
    ///
    /// @param id The session ID
    /// @return The pointer of the session
    SessionPtr GetSessionById(int id);

    /// Get the first session (normally it would be called by the client, one client has only one session)
    ///
    /// @return The pointer of the first session (normally it should be the only session of the client)
    SessionPtr GetFirstSession();

    /// Broadcast data
    ///
    /// @param data The data (a buffer pointer)
    void Broadcast(IoBufferPtr data);

    /// Broadcast data
    ///
    /// @param data The data (a common pointer)
    void Broadcast(void* data);

    /// Check whether the manager has data attached (it should be the global data to all sessions)
    ///
    /// @return Return true if the manager has data
    bool HasData();

    /// Get the attached data of the manager
    ///
    /// @return The pointer of the attached data
    CommonDataPtr GetData();

    /// Set idle time for all sessions
    ///
    /// @param idletype The idle type (1 for read idle, 2 for write idle, 0 for both)
    /// @param idletime The idle time (in second)
    void SetIdleTime(int idletype, int idletime);

    /// Remove all sessions
    void Clear();

protected:

    /// The callback function for idle event
    ///
    /// @param error The error code
    void OnIdleSessions(const boost::system::error_code &error);

private:

    int m_sessionidx;

    int m_idletype;
    int m_idletime;

    std::map<int, SessionPtr> m_sessions;
    boost::mutex m_sessionmutex;

    boost::mutex m_datamutex;

    std::map<int, int> m_dataintintmap;
    std::map<std::string, int> m_datastrintmap;

    std::map<int, std::string> m_dataintstrmap;
    std::map<std::string, std::string> m_datastrstrmap;

    std::map<int, IoBufferPtr> m_dataintbufmap;
    std::map<std::string, IoBufferPtr> m_datastrbufmap;

    std::map<int, BusinessPtr> m_dataintobjmap;
    std::map<std::string, BusinessPtr> m_datastrobjmap;

    boost::asio::io_service * m_idlesvc;
    boost::asio::io_service::work * m_idlework;

    boost::thread * m_idlethread;

    boost::asio::deadline_timer * m_idletimer;

};

}

#endif // _ESN_SESSIONMANAGER_H_

