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

#ifndef _ESN_SERVER_H_INCLUDED_
#define _ESN_SERVER_H_INCLUDED_

#include <vector>

#include "Session.h"
#include "IoFilter.h"
#include "IoHandler.h"
#include "IoBufferManager.h"
#include "IoServiceManager.h"

namespace esnlib
{

/// Server class (interface)
class Server
{
public:

    Server();
    virtual ~Server();

    /// Start to listen
    ///
    /// @param port The listening port
    /// @return Return true if start to listen successfully
    virtual bool Start(int port) = 0;

    /// Start to listen
    ///
    /// @param ipstr Local IP in string format
    /// @param port The listening port
    /// @return Return true if start to listen successfully
    virtual bool Start(const std::string& ipstr, int port) = 0;

    /// Stop listening
    virtual void Stop() = 0;

    /// Check whether the server is listening
    ///
    /// @return Return true if the server is listening
    virtual bool Listening() = 0;

    /// Get current listening port of the server
    ///
    /// @return Current listening port
    virtual int GetCurrentPort() = 0;

    /// Get local IP of the server
    ///
    /// @return The current listening local IP of the server
    virtual std::string GetCurrentIp() = 0;

    /// Get the local default IP of the server
    ///
    /// @return The default IP of the server
    virtual std::string GetDefaultLocalIp() = 0;

    /// Broadcast data
    ///
    /// @param data The data
    virtual void Broadcast(IoBufferPtr data) = 0;

    /// Get total number of all sessions
    ///
    /// @return Total number of all sessions
    virtual int GetSessionCount() = 0;

    /// Get all sessions
    ///
    /// @param sessions The vector used to save all the sessions
    /// @return Total number of all sessions
    virtual int GetSessions(std::vector<SessionPtr>& sessions) = 0;

    /// Get a session by its ID
    ///
    /// @param id The session ID
    /// @return The pointer of the session
    virtual SessionPtr GetSessionById(int id) = 0;

    /// Set idle time for sessions
    ///
    /// @param idletype The idle type (1 for read idle, 2 for write idle, 0 for both)
    /// @param idletime The idle time (in second)
    virtual void SetIdleTime(int idletype, int idletime) = 0;

    /// Set IO Handler
    ///
    /// @param handler The pointer of the handler
    virtual void SetIoHandler(IoHandlerPtr handler) = 0;

    /// Set IO Filter
    ///
    /// @param filter The pointer of the filter
    virtual void SetIoFilter(IoFilterPtr filter) = 0;

    /// Set Buffer Manager (memory pool)
    ///
    /// @param manager The pointer of the buffer manager
    virtual void SetIoBufferManager(IoBufferManagerPtr manager) = 0;

    /// Set IO Thread Manager (IO thread pool)
    ///
    /// @param manager The pointer of the IO service manager
    virtual void SetIoServiceManager(IoServiceManagerPtr manager) = 0;

    /// Get a free buffer from the Buffer Manager
    ///
    /// @param bufsize The size of the free buffer
    /// @return The pointer of the free buffer (IO buffer)
    virtual IoBufferPtr GetFreeBuffer(int bufsize) = 0;

    /// Check whether the server has global data attached
    ///
    /// @return Return true if the server has global data
    virtual bool HasGlobalData() = 0;

    /// Get the global data of the server
    ///
    /// @return The pointer of the global data
    virtual CommonDataPtr GetGlobalData() = 0;

    /// Try to enable SSL(Secure Sockets Layer)
    ///
    /// @param certfile The Cert. file
    /// @param keyfile The private key file
    /// @param password The password for the private key
    /// @param verifyfile The verify file
    /// @return Return true if set up SSL successfully
    virtual bool EnableSsl(const std::string& certfile, const std::string& keyfile, const std::string& password, const std::string& verifyfile = std::string()) = 0;

    /// Check whether the server has SSL(Secure Sockets Layer)
    ///
    /// @return Return true if the server has SSL
    virtual bool HasSsl() = 0;

};

typedef boost::shared_ptr<Server> ServerPtr;

/** \addtogroup Server
 *  @{
 */

/// Create a server with specific read/write buffer size and max connection queue size
///
/// @param maxConnectQueueSize The maximum size of the connection queue
/// @param readBufferSize The size of the read buffer
/// @param writeBufferSize The size of the write buffer
/// @return The pointer of the server
ServerPtr CreateServer(int maxConnectQueueSize = 2048, int readBufferSize = 8192, int writeBufferSize = 8192);

/** @} */

}

#endif // _ESN_SERVER_H_INCLUDED_

