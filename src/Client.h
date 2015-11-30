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

#ifndef _ESN_CLIENT_H_
#define _ESN_CLIENT_H_

#include "IoFilter.h"
#include "IoHandler.h"
#include "IoBufferManager.h"
#include "IoServiceManager.h"

namespace esnlib
{

/// Client class (interface)
class Client
{
public:
    Client();
    virtual ~Client();

    /// Connect the server
    ///
    /// @param svraddr The IP of the server
    /// @param svrport The port of the server
    /// @return Return true if connect successfully
    virtual bool Connect(const std::string& svraddr, int svrport) = 0;

    /// Disconnect the server
    virtual void Disconnect() = 0;

    /// Check whether the client has connected to the server
    ///
    /// @return Return true if the client has connected to the server
    virtual bool Connected() = 0;

    /// Check whether the client is connecting to the server
    ///
    /// @return Return true if the client is connecting to the server
    virtual bool IsConnecting() = 0;

    /// Write data to server
    ///
    /// @param data The data (a buffer pointer)
    virtual void Write(IoBufferPtr data) = 0;

    /// Write data to server
    ///
    /// @param data The data (a common pointer)
    virtual void Write(void* data) = 0;

    /// Set connect timeout value
    ///
    /// @param seconds The value of timeout time (in second)
    virtual void SetConnectTimeOut(int seconds) = 0;

    /// Set idle time
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

    /// Get the local default IP of the client
    ///
    /// @return The default IP of the client
    virtual std::string GetDefaultLocalIp() = 0;

    /// Get the local IP of the client
    ///
    /// @return The IP of the client
    virtual std::string GetLocalIp() = 0;

    /// Get the local port
    ///
    /// @return The port of the client
    virtual int GetLocalPort() = 0;

    /// Get the remote server's IP
    ///
    /// @return The IP of the server
    virtual std::string GetRemoteIp() = 0;

    /// Get the remote server's port
    ///
    /// @return The port of the server
    virtual int GetRemotePort() = 0;

    /// Get current session
    ///
    /// @return The pointer of current session
    virtual SessionPtr GetCurrentSession() = 0;

    /// Get the client's ID
    ///
    /// @return The client ID
    virtual int GetId() = 0;

    /// Set the client's ID
    ///
    /// @param id The client ID
    virtual void SetId(int id) = 0;

    /// Check whether the client has global data attached
    ///
    /// @return Return true if the client has global data
    virtual bool HasGlobalData() = 0;

    /// Get the global data of the client
    ///
    /// @return The pointer of the global data
    virtual CommonDataPtr GetGlobalData() = 0;

    /// Try to enable SSL(Secure Sockets Layer)
    ///
    /// @param needverify Whether need to verity the certificate
    /// @param verifyfile The verify file
    /// @param certfile The Cert. file
    /// @param keyfile The private key file
    /// @return Return true if set up SSL successfully
    virtual bool EnableSsl(bool needverify = true, const std::string& verifyfile = std::string(),
                           const std::string& certfile = std::string(), const std::string& keyfile = std::string()) = 0;

    /// Check whether the client has SSL(Secure Sockets Layer)
    ///
    /// @return Return true if the client has SSL
    virtual bool HasSsl() = 0;


protected:
private:
};

typedef boost::shared_ptr<Client> ClientPtr;

/** \addtogroup Client
 *  @{
 */

/// Create a client with specific read/write buffer size
///
/// @param readBufferSize The size of the read buffer
/// @param writeBufferSize The size of the write buffer
/// @return The pointer of the client
ClientPtr CreateClient(int readBufferSize = 8192, int writeBufferSize = 8192);

/** @} */

}

#endif // _ESN_CLIENT_H_

