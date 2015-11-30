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

#ifndef _ESN_CLIENTSESSION_H_
#define _ESN_CLIENTSESSION_H_

#include <set>
#include <list>
#include <deque>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

#ifdef ESN_WITH_SSL
#include <boost/asio/ssl.hpp>
#endif

#include "Business.h"

#include "IoBuffer.h"
#include "IoFilter.h"
#include "IoHandler.h"
#include "IoBufferManager.h"

#include "Session.h"
#include "SessionManager.h"

#ifndef ESN_DEFAULT_SOCK_BUFSIZE
#define ESN_DEFAULT_SOCK_BUFSIZE 8192
#endif

namespace esnlib
{

/// Client Session class
class ClientSession
: public Session
, public boost::enable_shared_from_this<ClientSession>
{
public:
    ClientSession(boost::asio::io_service& service, SessionManager& mgr, int readbufsize, int writebufsize, bool svrside = true);
    #ifdef ESN_WITH_SSL
    ClientSession(boost::asio::io_service& service,
                  boost::asio::ssl::context& context,
                  boost::asio::io_service::strand* strand, // boost::asio::ssl::stream need it for concurrent read/write actions...
                  SessionManager& mgr, int readbufsize, int writebufsize, bool svrside = true);
    #endif
    virtual ~ClientSession();

    /// Open the session
    virtual void Open();

    /// Close the session
    ///
    /// @param rightnow Whether close the session immediately
    virtual void Close(bool rightnow = true);

    /// Check whether the client has connected the server
    ///
    /// @return Return true if the client has connected the server
    virtual bool Connected();

    /// Write data to the other side
    ///
    /// @param data The data (a buffer pointer)
    virtual void Write(boost::shared_ptr<IoBuffer> data);

    /// Write buffer to the other side
    ///
    /// @param buf The buffer (a char pointer)
    /// @param bufsize The size of the buffer
    virtual void Write(const char* buf, int bufsize);

    /// Write data to the other side
    ///
    /// @param data The data (a common pointer)
    virtual void Write(void* data);

    /// Write data to the other side
    ///
    /// @param data The data (a common pointer)
    virtual void WriteData(void* data);

    /// Write buffer to the other side
    ///
    /// @param data The pointer of the buffer
    virtual void WriteBuffer(boost::shared_ptr<IoBuffer> data);

    /// Broadcast data
    ///
    /// @param data The data (a buffer pointer)
    virtual void Broadcast(boost::shared_ptr<IoBuffer> data);

    /// Broadcast data
    ///
    /// @param data The data (a common pointer)
    virtual void Broadcast(void* data);

    /// Get total number of all sessions
    ///
    /// @return Total number of all sessions
    virtual int GetSessionCount();

    /// Get all sessions
    ///
    /// @param sessions The vector used to save all the sessions
    /// @return Total number of all sessions
    virtual int GetSessions(std::vector< boost::shared_ptr<Session> > & sessions);

    /// Get a session by its ID
    ///
    /// @param id The session ID
    /// @return The pointer of the session
    virtual boost::shared_ptr<Session> GetSessionById(int id);

    /// Check whether the session is idle (with specific idle type and idle time)
    ///
    /// @param idletype The idle type: 1 for read idle, 2 for write idle, 0 for both
    /// @param idletime The idle time (in second)
    /// @return Return true if the session is idle
    virtual bool TestIdle(int idletype, int idletime);

    /// Get the session's ID
    ///
    /// @return The session ID
    virtual int GetId();

    /// Set the session's ID
    ///
    /// @param id The session ID
    virtual void SetId(int id);

    /// Check whether the session will process messages sequentially
    ///
    /// @param optype The operation type: 1 for read, 2 for write
    /// @return Whether the session will process messages sequentially
    virtual bool GetOrderlyHandling(int optype);

    /// Set whether the session will process messages sequentially
    ///
    /// @param optype The operation type: 1 for read, 2 for write, 0 for both
    /// @param value Whether the session will process messages sequentially
    virtual void SetOrderlyHandling(int optype, bool value);

    /// Get max message queue size
    ///
    /// @param optype The operation type: 1 for read, 2 for write
    /// @return The max message queue size
    virtual int GetMaxMessageQueueSize(int optype);

    /// Set max message queue size
    ///
    /// @param optype The operation type: 1 for read, 2 for write, 0 for both
    /// @param value The max message queue size
    virtual void SetMaxMessageQueueSize(int optype, int value);

    /// Get the session's state
    ///
    /// @return The session state
    virtual int GetState();

    /// Get the IO filter
    ///
    /// @return The pointer of the IO filter
    virtual boost::shared_ptr<IoFilter> GetIoFilter();

    /// Get the IO handler
    ///
    /// @return The pointer of the IO handler
    virtual boost::shared_ptr<IoHandler> GetIoHandler();

    /// Get the read cache
    ///
    /// @return The pointer of the read cache (IO buffer)
    virtual boost::shared_ptr<IoBuffer> GetReadCache();

    /// Get a free buffer
    ///
    /// @param bufsize The size of the free buffer
    /// @return The pointer of the free buffer (IO buffer)
    virtual boost::shared_ptr<IoBuffer> GetFreeBuffer(int bufsize);

    /// Take back the buffer (back to the memory pool)
    ///
    /// @param buf The pointer of the buffer (IO buffer)
    virtual void TakeBack(boost::shared_ptr<IoBuffer> buf);

    /// Take back the buffer (back to the memory pool)
    ///
    /// @param buf The pointer of the buffer
    virtual void TakeBack(boost::shared_ptr<Buffer> buf);

    /// Encode buffer
    ///
    /// @param data The buffer (a common pointer)
    /// @return The pointer of the encoded buffer (IO buffer)
    virtual boost::shared_ptr<IoBuffer> EncodeBuffer(void* data);

    /// Decode buffer
    ///
    /// @param data The pointer of the encoded buffer (IO buffer)
    /// @return The original buffer (a common pointer)
    virtual void* DecodeBuffer(boost::shared_ptr<IoBuffer> data);

    /// Check whether the session has SSL(Secure Sockets Layer)
    ///
    /// @return Return true if the session has SSL
    virtual bool HasSsl();

    /// Check whether the session has local data
    ///
    /// @return Return true if the session has local data
    virtual bool HasData();

    /// Get the local data of the session
    ///
    /// @return The pointer of the local data
    virtual CommonDataPtr GetData();

    /// Check whether the session has global data attached
    ///
    /// @return Return true if the session has global data
    virtual bool HasGlobalData();

    /// Get the global data of the session
    ///
    /// @return The pointer of the global data
    virtual CommonDataPtr GetGlobalData();

    /// Get the IP of the other side
    ///
    /// @return The IP of the other side
    virtual std::string GetRemoteIp();

    /// Get the port of the other side
    ///
    /// @return The port of the other side
    virtual int GetRemotePort();

    /// Get the local IP of the session
    ///
    /// @return The local IP of the session
    virtual std::string GetLocalIp();

    /// Get the port of the local side
    ///
    /// @return The port of the local side
    virtual int GetLocalPort();

    /// Process incoming data
    ///
    /// @param asynccall It is an async call or not
    virtual void ProcessIncomingData(bool asynccall);

    /// Process outgoing data
    ///
    /// @param asynccall It is an async call or not
    virtual void ProcessOutgoingData(bool asynccall);

    /// Set IO Filter
    ///
    /// @param filter The pointer of the filter
    void SetIoFilter(IoFilterPtr filter);

    /// Set IO Handler
    ///
    /// @param handler The pointer of the handler
    void SetIoHandler(IoHandlerPtr handler);

    /// Set Buffer Manager
    ///
    /// @param manager The pointer of the buffer manager
    void SetBufferManager(IoBufferManagerPtr manager);

    /// Process read operation
    void Read();

    /// SSL handshake
    void SslHandshake();

    /// Get the raw socket of the session
    ///
    /// @return The raw socket
    boost::asio::ip::tcp::socket& GetSocket();

protected:

    /// The callback function for the OnRead event of ASIO
    ///
    /// @param error The error code
    /// @param bytes_transferred The total number of the bytes transferred
    void OnRead(const boost::system::error_code& error, size_t bytes_transferred);

    /// The callback function for the OnWrite event of ASIO
    ///
    /// @param error The error code
    /// @param bytes_transferred The total number of the bytes transferred
    void OnWrite(const boost::system::error_code& error, size_t bytes_transferred);

    /// The callback function for the OnHandshake event of ASIO
    ///
    /// @param error The error code
    void OnSslHandshake(const boost::system::error_code& error);

    /// Initialize member variables
    void Init();

    /// The process which will be execute once the connection is established.
    void AfterConnect();

private:

    int m_id;

    int m_state;

    int m_readbufsize;
    int m_writebufsize;

    int m_asyncreadevents;
    int m_asyncwriteevents;

    bool m_closing;

    bool m_gotssl;

    bool m_svrside;

    bool m_orderlyhandlingread;
    bool m_orderlyhandlingwrite;

    int m_maxreadqueuesize;
    int m_maxwritequeuesize;

    int m_remoteport;
    std::string m_remoteip;

    int m_localport;
    std::string m_localip;

    boost::asio::ip::tcp::socket * m_socket;

    #ifdef ESN_WITH_SSL
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> * m_sslsocket;
    boost::asio::io_service::strand * m_strand;
    #endif

    boost::mutex m_readmutex;
    boost::mutex m_writemutex;

    boost::mutex m_processreadmutex;
    boost::mutex m_processwritemutex;

    boost::mutex m_datamutex;

    boost::mutex m_closingmutex;

    boost::posix_time::ptime m_lastreadtime;
    boost::posix_time::ptime m_lastwritetime;

    std::map<int, int> m_dataintintmap;
    std::map<std::string, int> m_datastrintmap;

    std::map<int, std::string> m_dataintstrmap;
    std::map<std::string, std::string> m_datastrstrmap;

    std::map<int, IoBufferPtr> m_dataintbufmap;
    std::map<std::string, IoBufferPtr> m_datastrbufmap;

    std::map<int, BusinessPtr> m_dataintobjmap;
    std::map<std::string, BusinessPtr> m_datastrobjmap;

    SessionManager& m_mgr;

    IoBufferPtr m_readbuffer;

    IoBufferPtr m_readcache;
    IoBufferPtr m_roundcache;

    IoBufferQueue m_readlist;
    IoBufferQueue m_writelist;

    IoFilterPtr m_filter;

    IoHandlerPtr m_handler;

    IoBufferManagerPtr m_bufmgr;

    void InternalProcessIncoming(bool& isnothing);
    void InternalProcessOutgoing(bool& isnothing);

    void InternalWrite();
    void InternalShutdown();
};

typedef boost::shared_ptr<ClientSession> ClientSessionPtr;

}

#endif // _ESN_CLIENTSESSION_H_

