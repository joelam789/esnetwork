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

#ifndef _ESN_SESSION_H_
#define _ESN_SESSION_H_

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "CommonData.h"

namespace esnlib
{

class Buffer;
class IoBuffer;
class IoFilter;
class IoHandler;

/// Session class (interface)
class Session
{

public:

    Session();
    virtual ~Session();

    /// Open the session
    virtual void Open() = 0;

    /// Close the session
    ///
    /// @param rightnow Whether close the session immediately
    virtual void Close(bool rightnow = true) = 0;

    /// Check whether the valid connection of the session is created
    ///
    /// @return Return true if the connection is created
    virtual bool Connected() = 0;

    /// Write data
    ///
    /// @param data The data (a buffer pointer)
    virtual void Write(boost::shared_ptr<IoBuffer> data) = 0;

    /// Write buffer
    ///
    /// @param buf The buffer (a char pointer)
    /// @param bufsize The size of the buffer
    virtual void Write(const char* buf, int bufsize) = 0;

    /// Write data
    ///
    /// @param data The data (a common pointer)
    virtual void Write(void* data) = 0;

    /// Write data
    ///
    /// @param data The data (a common pointer)
    virtual void WriteData(void* data) = 0;

    /// Write buffer
    ///
    /// @param data The pointer of the buffer
    virtual void WriteBuffer(boost::shared_ptr<IoBuffer> data) = 0;

    /// Broadcast data
    ///
    /// @param data The data (a buffer pointer)
    virtual void Broadcast(boost::shared_ptr<IoBuffer> data) = 0;

    /// Broadcast data
    ///
    /// @param data The data (a common pointer)
    virtual void Broadcast(void* data) = 0;

    /// Get total number of all sessions
    ///
    /// @return Total number of all sessions
    virtual int GetSessionCount() = 0;

    /// Get all sessions
    ///
    /// @param sessions The vector used to save all the sessions
    /// @return Total number of all sessions
    virtual int GetSessions(std::vector< boost::shared_ptr<Session> > & sessions) = 0;

    /// Get a session by its ID
    ///
    /// @param id The session ID
    /// @return The pointer of the session
    virtual boost::shared_ptr<Session> GetSessionById(int id) = 0;

    /// Check whether the session is idle (with specific idle type and idle time)
    ///
    /// @param idletype The idle type: 1 for read idle, 2 for write idle, 0 for both
    /// @param idletime The idle time (in second)
    /// @return Return true if the session is idle
    virtual bool TestIdle(int idletype, int idletime) = 0;

    /// Get the session's ID
    ///
    /// @return The session ID
    virtual int GetId() = 0;

    /// Set the session's ID
    ///
    /// @param id The session ID
    virtual void SetId(int id) = 0;

    /// Check whether the session will process messages sequentially
    ///
    /// @param optype The operation type: 1 for read, 2 for write
    /// @return Whether the session will process messages sequentially
    virtual bool GetOrderlyHandling(int optype) = 0;

    /// Set whether the session will process messages sequentially
    ///
    /// @param optype The operation type: 1 for read, 2 for write, 0 for both
    /// @param value Whether the session will process messages sequentially
    virtual void SetOrderlyHandling(int optype, bool value) = 0;

    /// Get max message queue size
    ///
    /// @param optype The operation type: 1 for read, 2 for write
    /// @return The max message queue size
    virtual int GetMaxMessageQueueSize(int optype) = 0;

    /// Set max message queue size
    ///
    /// @param optype The operation type: 1 for read, 2 for write, 0 for both
    /// @param value The max message queue size
    virtual void SetMaxMessageQueueSize(int optype, int value) = 0;

    /// Get the session's state
    ///
    /// @return The session state
    virtual int GetState() = 0;

    /// Get the IO filter
    ///
    /// @return The pointer of the IO filter
    virtual boost::shared_ptr<IoFilter> GetIoFilter() = 0;

    /// Get the IO handler
    ///
    /// @return The pointer of the IO handler
    virtual boost::shared_ptr<IoHandler> GetIoHandler() = 0;

    /// Get the read cache
    ///
    /// @return The pointer of the read cache (IO buffer)
    virtual boost::shared_ptr<IoBuffer> GetReadCache() = 0;

    /// Get a free buffer
    ///
    /// @param bufsize The size of the free buffer
    /// @return The pointer of the free buffer (IO buffer)
    virtual boost::shared_ptr<IoBuffer> GetFreeBuffer(int bufsize) = 0;

    /// Take back the buffer (back to the memory pool)
    ///
    /// @param buf The pointer of the buffer (IO buffer)
    virtual void TakeBack(boost::shared_ptr<IoBuffer> buf) = 0;

    /// Take back the buffer (back to the memory pool)
    ///
    /// @param buf The pointer of the buffer
    virtual void TakeBack(boost::shared_ptr<Buffer> buf) = 0;

    /// Encode buffer
    ///
    /// @param data The buffer (a common pointer)
    /// @return The pointer of the encoded buffer (IO buffer)
    virtual boost::shared_ptr<IoBuffer> EncodeBuffer(void* data) = 0;

    /// Decode buffer
    ///
    /// @param data The pointer of the encoded buffer (IO buffer)
    /// @return The original buffer (a common pointer)
    virtual void* DecodeBuffer(boost::shared_ptr<IoBuffer> data) = 0;

    /// Check whether the session has SSL(Secure Sockets Layer)
    ///
    /// @return Return true if the session has SSL
    virtual bool HasSsl() = 0;

    /// Check whether the session has local data
    ///
    /// @return Return true if the session has local data
    virtual bool HasData() = 0;

    /// Get the local data of the session
    ///
    /// @return The pointer of the local data
    virtual CommonDataPtr GetData() = 0;

    /// Check whether the session has global data attached
    ///
    /// @return Return true if the session has global data
    virtual bool HasGlobalData() = 0;

    /// Get the global data of the session
    ///
    /// @return The pointer of the global data
    virtual CommonDataPtr GetGlobalData() = 0;

    /// Get the IP of the remote side
    ///
    /// @return The IP of the remote side
    virtual std::string GetRemoteIp() = 0;

    /// Get the port of the remote side
    ///
    /// @return The port of the remote side
    virtual int GetRemotePort() = 0;

    /// Get the local IP of the session
    ///
    /// @return The local IP of the session
    virtual std::string GetLocalIp() = 0;

    /// Get the port of the local side
    ///
    /// @return The port of the local side
    virtual int GetLocalPort() = 0;

    /// Process incoming data
    ///
    /// @param asynccall It is an async call or not
    virtual void ProcessIncomingData(bool asynccall) = 0;

    /// Process outgoing data
    ///
    /// @param asynccall It is an async call or not
    virtual void ProcessOutgoingData(bool asynccall) = 0;

protected:

private:

};

typedef boost::shared_ptr<Session> SessionPtr;
typedef boost::weak_ptr<Session> SessionRef;

}


#endif // _ESN_SESSION_H_

