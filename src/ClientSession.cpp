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

#include <sstream>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/thread/locks.hpp>

#include "LogManager.h"

#include "SharedData.h"

#include "ClientSession.h"

using namespace esnlib;

void ClientSession::Init()
{
    m_id = 0;

    m_state = 0;

    m_closing = false;

    m_orderlyhandlingread = false;
    m_orderlyhandlingwrite = false;

    m_maxreadqueuesize = 1024;
    m_maxwritequeuesize = 0;

    m_remoteip = "";
    m_remoteport = 0;

    m_localip = "";
    m_localport = 0;

    m_asyncreadevents = 0;
    m_asyncwriteevents = 0;

    //std::cout << "\nInitialized ClientSession: " << (int)this << std::endl;
}

ClientSession::ClientSession(boost::asio::io_service& service, SessionManager& mgr, int readbufsize, int writebufsize, bool svrside)
: m_mgr(mgr)
{
    Init();

    m_gotssl = false;
    m_svrside = svrside;

    m_socket = new boost::asio::ip::tcp::socket(service);

    #ifdef ESN_WITH_SSL
    m_sslsocket = NULL;
    m_strand = NULL;
    #endif

    m_readbufsize = readbufsize;
    if(m_readbufsize <= 0) m_readbufsize = ESN_DEFAULT_SOCK_BUFSIZE;

    m_writebufsize = writebufsize;
    if(m_writebufsize <= 0) m_writebufsize = ESN_DEFAULT_SOCK_BUFSIZE;

}
#ifdef ESN_WITH_SSL
ClientSession::ClientSession(boost::asio::io_service& service,
                             boost::asio::ssl::context& context,
                             boost::asio::io_service::strand* strand, // boost::asio::ssl::stream need it for concurrent read/write actions...
                             SessionManager& mgr, int readbufsize, int writebufsize, bool svrside)
: m_mgr(mgr)
{
    Init();

    m_gotssl = true;
    m_svrside = svrside;

    m_socket = NULL;

    m_sslsocket = new boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(service, context);
    m_strand = strand;

    m_readbufsize = readbufsize;
    if(m_readbufsize <= 0) m_readbufsize = ESN_DEFAULT_SOCK_BUFSIZE;

    m_writebufsize = writebufsize;
    if(m_writebufsize <= 0) m_writebufsize = ESN_DEFAULT_SOCK_BUFSIZE;

}
#endif
ClientSession::~ClientSession()
{
    if(m_state > 0 && GetSocket().is_open())
    {
        boost::unique_lock<boost::mutex> lock(m_closingmutex);

        #ifdef ESN_WITH_SSL
        if(m_gotssl)
        {
            m_strand->post(boost::bind(&ClientSession::InternalShutdown, shared_from_this()));
        }
        else
        {
            InternalShutdown();
        }
        #else
        InternalShutdown();
        #endif

    }

    m_state = -1;

    m_asyncreadevents = 0;
    m_asyncwriteevents = 0;

    if(m_readbuffer && m_readbuffer->recyclable())
    {
        TakeBack(m_readbuffer);
    }

    if(m_readcache && m_readcache->recyclable())
    {
        TakeBack(m_readcache);
    }

    if(m_roundcache && m_roundcache->recyclable())
    {
        TakeBack(m_roundcache);
    }

    std::map<int, IoBufferPtr>::iterator itr = m_dataintbufmap.begin();
    while(itr != m_dataintbufmap.end())
    {
        IoBufferPtr buf = itr->second;
        if(buf && buf->recyclable()) TakeBack(buf);
        itr++;
    }

    std::map<std::string, IoBufferPtr>::iterator its = m_datastrbufmap.begin();
    while(its != m_datastrbufmap.end())
    {
        IoBufferPtr buf = its->second;
        if(buf && buf->recyclable()) TakeBack(buf);
        its++;
    }

    if(m_socket)
    {
        delete m_socket;
        m_socket = NULL;
    }

    #ifdef ESN_WITH_SSL
    if(m_sslsocket)
    {
        delete m_sslsocket;
        m_sslsocket = NULL;
    }
    #endif

    //std::cout << "\nReleased ClientSession: " << (int)this << std::endl;
}

boost::asio::ip::tcp::socket& ClientSession::GetSocket()
{
    #ifdef ESN_WITH_SSL
    if(m_gotssl) return (boost::asio::ip::tcp::socket&) m_sslsocket->lowest_layer();
    else return (*m_socket);
    #else
    return (*m_socket);
    #endif // ESN_WITH_SSL
}

void ClientSession::AfterConnect()
{
    //LogManager::Warning( "AfterConnect()..." );

    try { if(m_handler) m_handler->OnConnect(shared_from_this()); }
    catch(...) { LogManager::Warning("Exception found in OnConnect() event."); }

    if(m_state > 0 && !m_closing)
    {
        m_lastreadtime = boost::posix_time::second_clock::local_time();
        m_lastwritetime = boost::posix_time::second_clock::local_time();

        if(!m_readbuffer)
        {
            m_readbuffer = GetFreeBuffer(m_readbufsize);
        }

        if(!m_readcache)
        {
            m_readcache = GetFreeBuffer(m_readbufsize);
            if(m_readcache) m_readcache->size(0);
        }

        if(!m_roundcache)
        {
            m_roundcache = GetFreeBuffer(m_readbufsize);
            if(m_roundcache) m_roundcache->size(0);
        }

        boost::asio::socket_base::receive_buffer_size option1(m_readbufsize);
        boost::system::error_code ec1;
        GetSocket().set_option(option1, ec1);
        //if(ec1) std::cout << "Error when set receive_buffer_size(" << m_readbufsize << "): " << ec1.message() << std::endl;
        if(ec1)
        {
            std::stringstream ss;
            ss << "Error when set receive_buffer_size(" << m_readbufsize << "): " << ec1.message();
            LogManager::Warning(ss.str());
        }

        boost::asio::socket_base::send_buffer_size option2(m_writebufsize);
        boost::system::error_code ec2;
        GetSocket().set_option(option2, ec2);
        //if(ec2) std::cout << "Error when set send_buffer_size(" << m_writebufsize << "): " << ec2.message() << std::endl;
        if(ec2)
        {
            std::stringstream ss;
            ss << "Error when set send_buffer_size(" << m_writebufsize << "): " << ec2.message();
            LogManager::Warning(ss.str());
        }

        //boost::system::error_code ec3;
        //GetSocket().set_option(boost::asio::ip::tcp::no_delay(true), ec3);
        //if(ec3) std::cout << "Error when set no_delay(true): " << ec3.message() << std::endl;

        if(m_readbuffer && m_readcache && m_roundcache)
        {
            #ifdef ESN_WITH_SSL
            if(m_gotssl) m_strand->post(boost::bind(&ClientSession::Read, shared_from_this()));
            else Read();
            #else
            Read();
            #endif
        }
        else LogManager::Warning("Failed to init cache for reading incoming data.");
    }
}

void ClientSession::Open()
{
    if(m_state > 0) return;

    if(m_remoteip.length() <= 0)
    {
        if(GetSocket().is_open())
        {
            m_remoteip = GetSocket().remote_endpoint().address().to_string();
        }
    }
    if(m_remoteport == 0)
    {
        if(GetSocket().is_open())
        {
            m_remoteport = GetSocket().remote_endpoint().port();
        }
    }

    if(m_localip.length() <= 0)
    {
        if(GetSocket().is_open())
        {
            m_localip = GetSocket().local_endpoint().address().to_string();
        }
    }
    if(m_localport == 0)
    {
        if(GetSocket().is_open())
        {
            m_localport = GetSocket().local_endpoint().port();
        }
    }

    m_mgr.AddSession(shared_from_this());

    m_state = 1;

    m_asyncreadevents = 0;
    m_asyncwriteevents = 0;

    #ifdef ESN_WITH_SSL
    if(m_gotssl) m_strand->post(boost::bind(&ClientSession::SslHandshake, shared_from_this()));
    else AfterConnect();
    #else
    AfterConnect();
    #endif
}

void ClientSession::InternalShutdown()
{
    boost::system::error_code ec;

    #ifdef ESN_WITH_SSL
    if(m_gotssl)
    {
        m_sslsocket->shutdown(ec);
        if(ec) LogManager::Warning("Error found when shutdown socket: " + ec.message());
        GetSocket().close(ec);
        if(ec) LogManager::Warning("Error found when close socket: " + ec.message());
    }
    else
    {
        m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if(ec) LogManager::Warning("Error found when shutdown socket: " + ec.message());
        m_socket->close(ec);
        if(ec) LogManager::Warning("Error found when close socket: " + ec.message());
    }
    #else
    m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if(ec) LogManager::Warning("Error found when shutdown socket: " + ec.message());
    m_socket->close(ec);
    if(ec) LogManager::Warning("Error found when close socket: " + ec.message());
    #endif

}

void ClientSession::Close(bool rightnow)
{
    if(!rightnow)
    {
        boost::unique_lock<boost::mutex> lockw(m_writemutex);
        boost::unique_lock<boost::mutex> lockr(m_readmutex);

        if (m_writelist.empty() && m_readlist.empty()) rightnow = true;
        else m_closing = true;
    }

    if(rightnow)
    {
        m_closing = true;

        if(m_state > 0 && GetSocket().is_open())
        {
            boost::unique_lock<boost::mutex> lock(m_closingmutex);

            #ifdef ESN_WITH_SSL
            if(m_gotssl)
            {
                m_strand->post(boost::bind(&ClientSession::InternalShutdown, shared_from_this()));
            }
            else
            {
                InternalShutdown();
            }
            #else
            InternalShutdown();
            #endif

            try { if(m_handler) m_handler->OnDisconnect(shared_from_this()); }
            catch(...) { LogManager::Warning("Exception found in OnDisconnect() event."); }
        }

        m_state = -1;

        m_asyncreadevents = 0;
        m_asyncwriteevents = 0;

        m_remoteip = "";
        m_remoteport = 0;

        m_localip = "";
        m_localport = 0;

        m_mgr.RemoveSession(shared_from_this());
    }

}

bool ClientSession::Connected()
{
    return m_state > 0 && !m_closing && GetSocket().is_open();
}

int ClientSession::GetState()
{
    return m_state;
}

int ClientSession::GetId()
{
    return m_id;
}
void ClientSession::SetId(int id)
{
    m_id = id;
}

bool ClientSession::GetOrderlyHandling(int optype)
{
    if(optype == 1) return m_orderlyhandlingread;
    else if(optype == 2) return m_orderlyhandlingwrite;

    return false;
}

void ClientSession::SetOrderlyHandling(int optype, bool value)
{
    if(optype == 0)
    {
        m_orderlyhandlingread = value;
        m_orderlyhandlingwrite = value;
    }
    else if(optype == 1) m_orderlyhandlingread = value;
    else if(optype == 2) m_orderlyhandlingwrite = value;
}

int ClientSession::GetMaxMessageQueueSize(int optype)
{
    if(optype == 1) return m_maxreadqueuesize;
    else if(optype == 2) return m_maxwritequeuesize;

    return 0;
}

void ClientSession::SetMaxMessageQueueSize(int optype, int value)
{
    if(optype == 0)
    {
        m_maxreadqueuesize = value;
        m_maxwritequeuesize = value;
    }
    else if(optype == 1) m_maxreadqueuesize = value;
    else if(optype == 2) m_maxwritequeuesize = value;
}

boost::shared_ptr<IoFilter> ClientSession::GetIoFilter()
{
    return m_filter;
}

boost::shared_ptr<IoHandler> ClientSession::GetIoHandler()
{
    return m_handler;
}

boost::shared_ptr<IoBuffer> ClientSession::GetReadCache()
{
    return m_readcache;
}

boost::shared_ptr<IoBuffer> ClientSession::GetFreeBuffer(int bufsize)
{
    IoBufferPtr task;
    if(m_bufmgr)
    {
        IoBufferPtr newtask = m_bufmgr->GetFreeBuffer(bufsize);
        if(newtask && newtask->size() >= bufsize)
        {
            newtask->session(shared_from_this());
            task = newtask;
        }
    }
    else
    {
        IoBufferPtr newtask(new IoBuffer(bufsize));
        if(newtask && newtask->size() >= bufsize)
        {
            newtask->session(shared_from_this());
            task = newtask;
        }
    }
    return task;
}

void ClientSession::TakeBack(boost::shared_ptr<IoBuffer> buf)
{
    if(m_bufmgr && buf && buf->recyclable())
    {
        m_bufmgr->TakeBack(buf);
    }
}

void ClientSession::TakeBack(boost::shared_ptr<Buffer> buf)
{
    if(m_bufmgr && buf && buf->recyclable()) m_bufmgr->TakeBack(buf);
}

boost::shared_ptr<IoBuffer> ClientSession::EncodeBuffer(void* data)
{
    IoBufferPtr buf;
    if(m_filter) buf = m_filter->Encode(shared_from_this(), data);
    return buf;
}
void* ClientSession::DecodeBuffer(boost::shared_ptr<IoBuffer> data)
{
    void* result = NULL;
    if(m_filter) result = m_filter->Decode(shared_from_this(), data);
    return result;
}

bool ClientSession::HasSsl()
{
    return m_gotssl;
}

bool ClientSession::HasData()
{
    return true;
}

CommonDataPtr ClientSession::GetData()
{
    CommonDataPtr data(new SharedData(m_datamutex,
                                      m_dataintintmap, m_datastrintmap,
                                      m_dataintstrmap, m_datastrstrmap,
                                      m_dataintbufmap, m_datastrbufmap,
                                      m_dataintobjmap, m_datastrobjmap));
    return data;
}

bool ClientSession::HasGlobalData()
{
    return m_mgr.HasData();
}
CommonDataPtr ClientSession::GetGlobalData()
{
    return m_mgr.GetData();
}

std::string ClientSession::GetRemoteIp()
{
    if(m_remoteip.length() <= 0)
    {
        if(GetSocket().is_open())
        {
            m_remoteip = GetSocket().remote_endpoint().address().to_string();
        }
    }
    return m_remoteip;
}
int ClientSession::GetRemotePort()
{
    if(m_remoteport == 0)
    {
        if(GetSocket().is_open())
        {
            m_remoteport = GetSocket().remote_endpoint().port();
        }
    }
    return m_remoteport;
}

std::string ClientSession::GetLocalIp()
{
    if(m_localip.length() <= 0)
    {
        if(GetSocket().is_open())
        {
            m_localip = GetSocket().local_endpoint().address().to_string();
        }
    }
    return m_localip;
}
int ClientSession::GetLocalPort()
{
    if(m_localport == 0)
    {
        if(GetSocket().is_open())
        {
            m_localport = GetSocket().local_endpoint().port();
        }
    }
    return m_localport;
}

void ClientSession::SetIoFilter(IoFilterPtr filter)
{
    if(!m_filter) m_filter = filter;
}

void ClientSession::SetIoHandler(IoHandlerPtr handler)
{
    if(!m_handler) m_handler = handler;
}

void ClientSession::SetBufferManager(IoBufferManagerPtr manager)
{
    if(!m_bufmgr) m_bufmgr = manager;
}

bool ClientSession::TestIdle(int idletype, int idletime)
{
    if(m_state <= 0 || m_closing) return false;
    if(idletype < 0 || idletype > 2) return false;

    boost::posix_time::ptime current = boost::posix_time::second_clock::local_time();

    boost::posix_time::time_duration readidle = current - m_lastreadtime;
    boost::posix_time::time_duration writeidle = current - m_lastwritetime;

    bool isidle = false;

    if(idletype == 0)
    {
        if(readidle.total_seconds() > idletime && writeidle.total_seconds() > idletime)
            isidle = true;

    }
    else if(idletype == 1)
    {
        if(readidle.total_seconds() > idletime) isidle = true;
    }
    else if(idletype == 2)
    {
        if(writeidle.total_seconds() > idletime) isidle = true;
    }

    if(isidle)
    {
        if(m_handler)
        {
            try { m_handler->OnIdle(shared_from_this(), idletype); }
            catch(...) { LogManager::Warning( "Exception found in OnIdle() event." ); }
        }
        return true;
    }

    return false;
}

void ClientSession::InternalWrite()
{
    IoBufferPtr task = m_writelist.front();
    if(task)
    {
        #ifdef ESN_WITH_SSL
        if(m_gotssl)
        {
            boost::asio::async_write(*m_sslsocket,
            boost::asio::buffer(task->data(), task->size()),
            m_strand->wrap(
            boost::bind(&ClientSession::OnWrite, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred)));
        }
        else
        {
            boost::asio::async_write(*m_socket,
            boost::asio::buffer(task->data(), task->size()),
            boost::bind(&ClientSession::OnWrite, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
        }
        #else
        boost::asio::async_write(*m_socket,
            boost::asio::buffer(task->data(), task->size()),
            boost::bind(&ClientSession::OnWrite, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
        #endif
    }
}

void ClientSession::InternalProcessOutgoing(bool& isnothing)
{
    boost::unique_lock<boost::mutex> listlock(m_writemutex);

    // before send out a new message (via async_write()), we must remove an old message first (if there are some old messages).
    // and these two actions (removing old and sending new) must be executed in a same thread (with lock(m_writemutex)).
    if (!m_writelist.empty()) m_writelist.pop_front();

    if (m_writelist.empty()) // if no messages waiting for sending
    {
        if(m_closing) // need to check unhandled messages only when closing
        {
            boost::unique_lock<boost::mutex> lock(m_readmutex);
            if (m_readlist.empty()) isnothing = true;
        }
    }
    else
    {
        // continue to send out new messages (if there are some new messages).
        #ifdef ESN_WITH_SSL
        if(m_gotssl)
        {
            m_strand->post(boost::bind(&ClientSession::InternalWrite, shared_from_this()));
        }
        else
        {
            InternalWrite();
        }
        #else
        InternalWrite();
        #endif
    }
}
void ClientSession::ProcessOutgoingData(bool asynccall)
{
    bool isnothing = false;

    if(m_state > 0)
    {
        if(m_orderlyhandlingwrite)
        {
            boost::unique_lock<boost::mutex> processlock(m_processwritemutex);
            if(asynccall) m_asyncwriteevents--;
            if(m_asyncwriteevents > 0) return;

            InternalProcessOutgoing(isnothing);
        }
        else
        {
            InternalProcessOutgoing(isnothing);
        }
    }

    if(isnothing && m_closing)
    {
        Close(true);
    }

}

void ClientSession::Write(boost::shared_ptr<IoBuffer> data)
{
    if(m_state <= 0 || m_closing) return;

    if(!data) return;

    int queuesize = 0;
    if (m_maxwritequeuesize > 0)
    {
        boost::unique_lock<boost::mutex> lock(m_writemutex);
        queuesize = m_writelist.size();
    }
    if (m_maxwritequeuesize > 0 && queuesize >= m_maxwritequeuesize)
    {
        if(m_handler)
        {
            try { m_handler->OnError(shared_from_this(), 2, 0, "Outgoing message queue is full"); }
            catch(...) { LogManager::Warning( "Exception found in OnError() event." ); }
        }
        return;
    }

    boost::unique_lock<boost::mutex> lock(m_writemutex);

    bool writing = !m_writelist.empty();

    IoBufferPtr newtask = data;

    newtask->session(shared_from_this());

    m_writelist.push_back(newtask);

    //printf("Want to write: %d(%d) \n", newtask->size(), data.size());

    if (!writing)
    {
        #ifdef ESN_WITH_SSL
        if(m_gotssl)
        {
            m_strand->post(boost::bind(&ClientSession::InternalWrite, shared_from_this()));
        }
        else
        {
            InternalWrite();
        }
        #else
        InternalWrite();
        #endif

    }
}

void ClientSession::WriteBuffer(boost::shared_ptr<IoBuffer> data)
{
    Write(data);
}

void ClientSession::Write(const char* buf, int bufsize)
{
    if(m_state <= 0 || m_closing) return;

    if(buf == NULL || bufsize <= 0) return;

    int queuesize = 0;
    if (m_maxwritequeuesize > 0)
    {
        boost::unique_lock<boost::mutex> lock(m_writemutex);
        queuesize = m_writelist.size();
    }
    if (m_maxwritequeuesize > 0 && queuesize >= m_maxwritequeuesize)
    {
        if(m_handler)
        {
            try { m_handler->OnError(shared_from_this(), 2, 0, "Outgoing message queue is full"); }
            catch(...) { LogManager::Warning("Exception found in OnError() event."); }
        }
        return;
    }

    boost::unique_lock<boost::mutex> lock(m_writemutex);

    bool writing = !m_writelist.empty();

    int datacount = bufsize / ESN_MAX_IOBUF_SIZE;
    int leftsize = bufsize % ESN_MAX_IOBUF_SIZE;

    char* inputbuf = (char*)buf;

    for(int i = 1; i <= datacount; i++)
    {
        IoBufferPtr newtask;
        if(m_bufmgr)
        {
            newtask = m_bufmgr->GetFreeBuffer(ESN_MAX_IOBUF_SIZE);
            newtask->session(shared_from_this());
            newtask->SetWritePos(0);
            newtask->PutBuf(inputbuf, ESN_MAX_IOBUF_SIZE);
            newtask->SetWritePos(0);
        }
        else
        {
            IoBufferPtr newone(new IoBuffer(inputbuf, ESN_MAX_IOBUF_SIZE));
            newone->session(shared_from_this());
            newtask = newone;
        }

        m_writelist.push_back(newtask);
        inputbuf += ESN_MAX_IOBUF_SIZE;
    }

    if(leftsize > 0)
    {
        IoBufferPtr newtask;
        if(m_bufmgr)
        {
            newtask = m_bufmgr->GetFreeBuffer(leftsize);
            newtask->session(shared_from_this());
            newtask->SetWritePos(0);
            newtask->PutBuf(inputbuf, leftsize);
            newtask->SetWritePos(0);
        }
        else
        {
            IoBufferPtr newone(new IoBuffer(inputbuf, leftsize));
            newone->session(shared_from_this());
            newtask = newone;
        }

        m_writelist.push_back(newtask);
    }

    if (!writing)
    {
        #ifdef ESN_WITH_SSL
        if(m_gotssl)
        {
            m_strand->post(boost::bind(&ClientSession::InternalWrite, shared_from_this()));
        }
        else
        {
            InternalWrite();
        }
        #else
        InternalWrite();
        #endif
    }

}

void ClientSession::Write(void* data)
{
    if(m_filter)
    {
        IoBufferPtr newtask = m_filter->Encode(shared_from_this(), data);
        if(newtask) Write(newtask);
    }
}

void ClientSession::WriteData(void* data)
{
    Write(data);
}

void ClientSession::Broadcast(boost::shared_ptr<IoBuffer> data)
{
    m_mgr.Broadcast(data);
}

void ClientSession::Broadcast(void* data)
{
    m_mgr.Broadcast(data);
}

int ClientSession::GetSessionCount()
{
    return m_mgr.GetSessionCount();
}

int ClientSession::GetSessions(std::vector< boost::shared_ptr<Session> > & sessions)
{
    return m_mgr.GetSessions(sessions);
}

boost::shared_ptr<Session> ClientSession::GetSessionById(int id)
{
    return m_mgr.GetSessionById(id);
}

void ClientSession::SslHandshake()
{
    if(m_state <= 0) return;

    //LogManager::Warning( "SslHandshake()..." );

    #ifdef ESN_WITH_SSL
    if(m_svrside)
    {
        m_sslsocket->async_handshake(boost::asio::ssl::stream_base::server,
        m_strand->wrap(
        boost::bind(&ClientSession::OnSslHandshake, shared_from_this(),
          boost::asio::placeholders::error)));
    }
    else
    {
        m_sslsocket->async_handshake(boost::asio::ssl::stream_base::client,
          m_strand->wrap(
          boost::bind(&ClientSession::OnSslHandshake, shared_from_this(),
            boost::asio::placeholders::error)));
    }
    #endif
}

void ClientSession::OnSslHandshake(const boost::system::error_code& error)
{
    if (!error)
    {
        AfterConnect();
    }
    else
    {
        if(m_handler)
        {
            try { m_handler->OnError(shared_from_this(), 1, error.value(), error.message()); }
            catch(...) { LogManager::Warning( "Exception found in OnError() event." ); }
        }

        Close(false);
    }
}

void ClientSession::Read()
{
    if(m_state <= 0) return;
    //if(m_closing) return;

    if (m_readbuffer)
    {
        m_readbuffer->size(m_readbufsize);
        m_readbuffer->SetReadPos(0);
        m_readbuffer->SetWritePos(0);

        if(m_readbuffer->data() && m_readbuffer->size() > 0)
        {
            #ifdef ESN_WITH_SSL
            if(m_gotssl)
            {
                m_sslsocket->async_read_some(boost::asio::buffer(m_readbuffer->data(), m_readbuffer->size()),
                  m_strand->wrap(
                  boost::bind(&ClientSession::OnRead, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)));
            }
            else
            {
                m_socket->async_read_some(boost::asio::buffer(m_readbuffer->data(), m_readbuffer->size()),
                  boost::bind(&ClientSession::OnRead, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            }
            #else
            m_socket->async_read_some(boost::asio::buffer(m_readbuffer->data(), m_readbuffer->size()),
                  boost::bind(&ClientSession::OnRead, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            #endif

        }

    }

}

void ClientSession::InternalProcessIncoming(bool& isnothing)
{
    bool emptyreadlist = false;

    while(!emptyreadlist)
    {
        IoBufferPtr taskdata;

        if(!emptyreadlist)
        {
            if(m_orderlyhandlingread)
            {
                boost::unique_lock<boost::mutex> listlock(m_readmutex);

                if (m_readlist.empty())
                {
                    emptyreadlist = true;
                    if(m_closing) // need to check unhandled messages only when closing
                    {
                        boost::unique_lock<boost::mutex> lock(m_writemutex);
                        if (m_writelist.empty()) isnothing = true;
                    }
                    break;
                }
                else
                {
                    taskdata = m_readlist.front();
                    m_readlist.pop_front();
                    emptyreadlist = m_readlist.empty();
                }
            }
            else
            {
                if (m_readlist.empty())
                {
                    emptyreadlist = true;
                    if(m_closing) // need to check unhandled messages only when closing
                    {
                        boost::unique_lock<boost::mutex> lock(m_writemutex);
                        if (m_writelist.empty()) isnothing = true;
                    }
                    break;
                }
                else
                {
                    taskdata = m_readlist.front();
                    m_readlist.pop_front();
                    emptyreadlist = m_readlist.empty();
                }
            }

        }

        if(taskdata)
        {
            int processflags = 0;
            try { processflags = m_handler->OnRead(taskdata->session(), taskdata); }
            catch(...) { LogManager::Warning( "Exception found in OnRead() event." ); }

            if((processflags & 1) == 0) // if it is in-sync process
            {
                if(taskdata && taskdata->recyclable()) TakeBack(taskdata);

                // continue to process the rest
                continue;
            }
            else // if it is async process
            {
                if((processflags & 2) != 0) // if it supports concurrency
                {
                    // continue to process the rest
                    continue;
                }
                else
                {
                    //boost::unique_lock<boost::mutex> processlock(m_processreadmutex);
                    if(m_orderlyhandlingread) m_asyncreadevents++;
                }
            }
            break; // wait for next calling
        }
    }
}

void ClientSession::ProcessIncomingData(bool asynccall)
{
    bool isnothing = false;

    if(m_state > 0)
    {
        if(m_orderlyhandlingread)
        {
            boost::unique_lock<boost::mutex> processlock(m_processreadmutex);
            if(asynccall) m_asyncreadevents--;
            if(m_asyncreadevents > 0) return;

            InternalProcessIncoming(isnothing);
        }
        else
        {
            InternalProcessIncoming(isnothing);
        }

    }

    if(isnothing && m_closing)
    {
        Close(true);
    }
}

void ClientSession::OnRead(const boost::system::error_code& error, size_t bytes_transferred)
{
    //std::cout << "\nOnRead " << std::endl;

    if (!error)
    {
        m_lastreadtime = boost::posix_time::second_clock::local_time();

        if (bytes_transferred == 0 || m_closing) // if no data got or closing, end this round
        {
            #ifdef ESN_WITH_SSL
            if(m_gotssl) m_strand->post(boost::bind(&ClientSession::Read, shared_from_this()));
            else Read();
            #else
            Read();
            #endif
            return;
        }

        int datasize = bytes_transferred;

        m_readbuffer->size(datasize);
        m_readbuffer->SetReadPos(0);
        m_readbuffer->SetWritePos(0);

        if(m_readbuffer->size() < datasize)
        {
            // if there is something wrong, end this round and stop reading more data
            return;
        }

        std::vector<IoBufferPtr> readylist;

        if(m_filter)
        {
            int oldsize = m_roundcache->size();
            if(oldsize > 0)
            {
                int newsize = m_roundcache->size(oldsize + m_readbuffer->size());
                if(newsize < oldsize + m_readbuffer->size()) return; // there is something wrong about the memory
                m_roundcache->SetWritePos(oldsize);
                m_roundcache->PutBuf(m_readbuffer->data(), m_readbuffer->size());
            }
            else
            {
                int newsize = m_roundcache->size(m_readbuffer->size());
                if(newsize < m_readbuffer->size()) return; // there is something wrong about the memory
                m_roundcache->SetWritePos(0);
                m_roundcache->PutBuf(m_readbuffer->data(), m_readbuffer->size());
            }

            m_roundcache->SetReadPos(0);
            m_roundcache->SetWritePos(0);

            bool reset = m_filter->Extract(shared_from_this(), m_roundcache, readylist);

            if(reset || m_roundcache->size() <= m_roundcache->GetReadPos())
            {
                m_roundcache->size(0);
                m_roundcache->SetReadPos(0);
                m_roundcache->SetWritePos(0);
            }
            else
            {
                int leftsize = m_roundcache->size() - m_roundcache->GetReadPos();
                memcpy(m_roundcache->data(), m_roundcache->data() + m_roundcache->GetReadPos(), leftsize);
                m_roundcache->size(leftsize);
                m_roundcache->SetReadPos(0);
                m_roundcache->SetWritePos(0);
            }
        }
        else
        {
            IoBufferPtr rawtask = m_readbuffer;
            readylist.push_back(rawtask);
        }

        bool fullqueue = false;
        int total = readylist.size();
        if(total > 0)
        {
            SessionPtr current = shared_from_this();

            if(m_orderlyhandlingread)
            {
                boost::unique_lock<boost::mutex> lock(m_readmutex);

                int queuesize = m_readlist.size();

                for(int i=0; i<total; i++)
                {
                    if(m_maxreadqueuesize > 0)
                    {
                        if(queuesize >= m_maxreadqueuesize)
                        {
                            fullqueue = true;
                            break;
                        }
                    }

                    IoBufferPtr taskdata = readylist[i];
                    taskdata->SetReadPos(0);
                    taskdata->SetWritePos(0);
                    taskdata->type(1); // "1" stands for incoming data
                    taskdata->session(current);

                    m_readlist.push_back(taskdata);

                    queuesize++;
                }
            }
            else
            {
                int queuesize = m_readlist.size();

                for(int i=0; i<total; i++)
                {
                    if(m_maxreadqueuesize > 0)
                    {
                        if(queuesize >= m_maxreadqueuesize)
                        {
                            fullqueue = true;
                            break;
                        }
                    }

                    IoBufferPtr taskdata = readylist[i];
                    taskdata->SetReadPos(0);
                    taskdata->SetWritePos(0);
                    taskdata->type(1); // "1" stands for incoming data
                    taskdata->session(current);

                    m_readlist.push_back(taskdata);

                    queuesize++;
                }
            }

        }

        if(fullqueue)
        {
            if(m_handler)
            {
                try { m_handler->OnError(shared_from_this(), 1, 0, "Incoming message queue is full"); }
                catch(...) { LogManager::Warning( "Exception found in OnError() event." ); }
            }
        }

        if(total > 0) ProcessIncomingData(false); // process in an orderly way by default

        #ifdef ESN_WITH_SSL
        if(m_gotssl) m_strand->post(boost::bind(&ClientSession::Read, shared_from_this()));
        else Read();
        #else
        Read();
        #endif
    }
    else
    {
        //std::cout << "Receive error: " << error.message() << std::endl;

        if(m_handler)
        {
            try { m_handler->OnError(shared_from_this(), 1, error.value(), error.message()); }
            catch(...) { LogManager::Warning( "Exception found in OnError() event." ); }
        }

        #ifdef ESN_WITH_SSL
        if(m_gotssl) m_strand->post(boost::bind(&ClientSession::Read, shared_from_this()));
        else Read();
        #else
        Read();
        #endif
    }
}
void ClientSession::OnWrite(const boost::system::error_code& error, size_t bytes_transferred)
{
    //std::cout << "\nOnWrite " << std::endl;

    if (!error)
    {
        m_lastwritetime = boost::posix_time::second_clock::local_time();

        int processflags = 0;

        IoBufferPtr taskdata;
        if(!taskdata)
        {
            boost::unique_lock<boost::mutex> lock(m_writemutex);
            taskdata = m_writelist.front();
        }

        if(taskdata)
        {
            if(m_handler)
            {
                SessionPtr current = shared_from_this();
                taskdata->size(bytes_transferred);
                taskdata->SetReadPos(0);
                taskdata->SetWritePos(0);
                taskdata->type(2); // "2" stands for outgoing data
                taskdata->session(current);
                try { processflags = m_handler->OnWrite(current, taskdata); }
                catch(...) { LogManager::Warning( "Exception found in OnWrite() event." ); }
            }
        }

        if((processflags & 1) == 0) // if it is in-sync process
        {
            if(taskdata && taskdata->recyclable()) TakeBack(taskdata);

            // continue to process the rest
            ProcessOutgoingData(false);
        }
        else // if it is async process
        {
            if((processflags & 2) != 0) // if it supports concurrency
            {
                // continue to process the rest
                ProcessOutgoingData(false);
            }
            else
            {
                if(m_orderlyhandlingwrite)
                {
                    boost::unique_lock<boost::mutex> processlock(m_processwritemutex);
                    m_asyncwriteevents++;
                }
            }
        }
    }
    else
    {
        if(m_handler)
        {
            try { m_handler->OnError(shared_from_this(), 2, error.value(), error.message()); }
            catch(...) { LogManager::Warning( "Exception found in OnError() event." ); }
        }

        // continue to process the rest (including the failed one)
        ProcessOutgoingData(false);
    }
}




