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

#include <list>
#include <sstream>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#ifdef ESN_WITH_SSL
#include <boost/asio/ssl.hpp>
#endif

#include "IoFilter.h"
#include "IoHandler.h"
#include "IoBufferManager.h"

#include "ClientSession.h"
#include "SessionManager.h"

#include "LogManager.h"

#include "Server.h"

using namespace esnlib;

Server::Server()
{
    // ...
}

Server::~Server()
{
    // ...
}

class ServerImpl : public Server
{
public:
    ServerImpl();
    ServerImpl(int connectQueueSize, int readBufferSize, int writeBufferSize);
    virtual ~ServerImpl();

    virtual bool Start(const std::string& ipstr, int port);
    virtual bool Start(int port);
    virtual void Stop();

    virtual bool Listening();

    virtual int GetCurrentPort();

    virtual std::string GetCurrentIp();

    virtual std::string GetDefaultLocalIp();

    virtual void Broadcast(IoBufferPtr data);

    virtual int GetSessionCount();
    virtual int GetSessions(std::vector<SessionPtr>& sessions);
    virtual SessionPtr GetSessionById(int id);

    virtual void SetIdleTime(int idletype, int idletime);

    virtual void SetIoHandler(IoHandlerPtr handler);
    virtual void SetIoFilter(IoFilterPtr filter);

    virtual void SetIoBufferManager(IoBufferManagerPtr manager);
    virtual void SetIoServiceManager(IoServiceManagerPtr manager);

    virtual IoBufferPtr GetFreeBuffer(int bufsize);

    virtual bool HasGlobalData();
    virtual CommonDataPtr GetGlobalData();

    virtual bool EnableSsl(const std::string& certfile, const std::string& keyfile, const std::string& password, const std::string& verifyfile = std::string());

    virtual bool HasSsl();

protected:

    bool Listen();

    void OnAccept(SessionPtr session, const boost::system::error_code& error);

    std::string OnGetSslPassword() const;

private:

    boost::asio::ip::tcp::endpoint * m_endpoint;

    boost::asio::ip::tcp::acceptor * m_acceptor;

    #ifdef ESN_WITH_SSL
    boost::asio::ssl::context * m_context;
    boost::asio::io_service::strand * m_strand;
    #endif

    boost::asio::io_service * m_listensvc;

    boost::asio::io_service * m_iosvc;
    boost::asio::io_service::work * m_iowork;

    //boost::thread * m_svcthread;
    //std::vector<boost::thread *> m_svcthreads;

    boost::thread * m_listenthread;
    boost::thread * m_iothread;

    std::list<IoServicePtr> m_svclist;

    SessionManager m_climgr;

    IoBufferManagerPtr m_bufmgr;

    IoServiceManagerPtr m_svcmgr;

    IoHandlerPtr m_handler;

    IoFilterPtr m_filter;

    int m_currentport;
    std::string m_currentip;

    //int m_iothreadcount;

    int m_readbufsize;
    int m_writebufsize;

    int m_cnnqueuesize;

    bool m_gotssl;
    std::string m_sslpwd;

};


ServerImpl::ServerImpl()
{
    m_endpoint = NULL;

    m_acceptor = NULL;

    m_listensvc = NULL;
    m_iosvc = NULL;
    m_iowork = NULL;

    m_listenthread = NULL;
    m_iothread = NULL;

    m_currentport = 0;
    m_currentip = "";

    m_gotssl = false;
    m_sslpwd = "";
    #ifdef ESN_WITH_SSL
    m_context = NULL;
    m_strand = NULL;
    #endif

    //m_iothreadcount = 0;

    m_readbufsize = ESN_DEFAULT_SOCK_BUFSIZE;
    m_writebufsize = ESN_DEFAULT_SOCK_BUFSIZE;

    m_cnnqueuesize = boost::asio::socket_base::max_connections;
}

ServerImpl::ServerImpl(int connectQueueSize, int readBufferSize, int writeBufferSize)
{
    m_endpoint = NULL;

    m_acceptor = NULL;
    //m_svcthread = NULL;

    m_listensvc = NULL;
    m_iosvc = NULL;
    m_iowork = NULL;

    m_listenthread = NULL;
    m_iothread = NULL;

    m_currentport = 0;
    m_currentip = "";

    m_gotssl = false;
    m_sslpwd = "";
    #ifdef ESN_WITH_SSL
    m_context = NULL;
    m_strand = NULL;
    #endif

    //m_iothreadcount = iothreadcount;
    //if(m_iothreadcount < 0) m_iothreadcount = 0;

    m_readbufsize = readBufferSize;
    if(m_readbufsize <= 0) m_readbufsize = ESN_DEFAULT_SOCK_BUFSIZE;

    m_writebufsize = writeBufferSize;
    if(m_writebufsize <= 0) m_writebufsize = ESN_DEFAULT_SOCK_BUFSIZE;

    m_cnnqueuesize = connectQueueSize;
    if(m_cnnqueuesize <= 0) m_cnnqueuesize = boost::asio::socket_base::max_connections;

}

ServerImpl::~ServerImpl()
{
    Stop();

    #ifdef ESN_WITH_SSL
    if(m_context)
    {
        delete m_context;
        m_context = NULL;
    }
    #endif
}

void ServerImpl::SetIoHandler(IoHandlerPtr handler)
{
    m_handler = handler;
}

void ServerImpl::SetIoFilter(IoFilterPtr filter)
{
    m_filter = filter;
}

void ServerImpl::SetIoBufferManager(IoBufferManagerPtr manager)
{
    m_bufmgr = manager;
}

void ServerImpl::SetIoServiceManager(IoServiceManagerPtr manager)
{
    m_svcmgr = manager;
}

IoBufferPtr ServerImpl::GetFreeBuffer(int bufsize)
{
    IoBufferPtr task;
    if(m_bufmgr)
    {
        task = m_bufmgr->GetFreeBuffer(bufsize);
    }
    else
    {
        IoBufferPtr newtask(new IoBuffer(bufsize));
        task = newtask;
    }
    return task;
}

bool ServerImpl::HasGlobalData()
{
    return m_climgr.HasData();
}
CommonDataPtr ServerImpl::GetGlobalData()
{
    return m_climgr.GetData();
}

bool ServerImpl::HasSsl()
{
    #ifdef ESN_WITH_SSL
    return m_gotssl && m_context != NULL;
    #else
    return m_gotssl;
    #endif
}

std::string ServerImpl::OnGetSslPassword() const
{
    return m_sslpwd;
}

bool ServerImpl::EnableSsl(const std::string& certfile, const std::string& keyfile, const std::string& password, const std::string& verifyfile)
{
    if(Listening()) return m_gotssl;

    #ifdef ESN_WITH_SSL

    if(m_context != NULL)
    {
        delete m_context;
        m_context = NULL;
    }

    m_gotssl = false;

    m_context = new boost::asio::ssl::context(boost::asio::ssl::context::sslv23);

    m_sslpwd = password;
    m_context->set_password_callback(boost::bind(&ServerImpl::OnGetSslPassword, this));

    boost::system::error_code ec;

    m_context->set_options(boost::asio::ssl::context::default_workarounds, ec);
    if(ec)
    {
        LogManager::Warning("Failed to set options of SSL context, error: " + ec.message());
        return m_gotssl;
    }

    m_context->use_certificate_chain_file(certfile, ec);
    if(ec)
    {
        LogManager::Warning("Failed to set certificate file of SSL context, error: " + ec.message());
        return m_gotssl;
    }

    m_context->use_private_key_file(keyfile, boost::asio::ssl::context::pem, ec);
    if(ec)
    {
        LogManager::Warning("Failed to set private key file of SSL context, error: " + ec.message());
        return m_gotssl;
    }

    if(verifyfile.length()>0)
    {
        m_context->load_verify_file(verifyfile, ec);
        if(ec)
        {
            LogManager::Warning("Failed to load verify file of SSL context, error: " + ec.message());
            return m_gotssl;
        }
    }

    m_gotssl = true;

    #endif

    return m_gotssl;
}

bool ServerImpl::Listen()
{
    if (!m_endpoint) return false;

    m_listensvc = new boost::asio::io_service();

    m_iosvc = NULL;
    m_iowork = NULL;
    #ifdef ESN_WITH_SSL
    m_strand = NULL;
    #endif
    m_svclist.clear();

    IoServicePtr svc;
    boost::asio::io_service* iosvc = NULL;

    if(!m_svcmgr)
    {
        m_iosvc = new boost::asio::io_service();
        m_iowork = new boost::asio::io_service::work(*m_iosvc);

        #ifdef ESN_WITH_SSL
        m_strand = new boost::asio::io_service::strand(*m_iosvc);
        #endif

        iosvc = m_iosvc;
    }
    else
    {
        if(m_svcmgr)
        {
            svc = m_svcmgr->GetService();
            if(svc)
            {
                m_svclist.push_back(svc);
                iosvc = (boost::asio::io_service*)(svc->GetRunner());
            }
        }

        if(iosvc == NULL)
        {
            m_iosvc = new boost::asio::io_service();
            m_iowork = new boost::asio::io_service::work(*m_iosvc);

            #ifdef ESN_WITH_SSL
            m_strand = new boost::asio::io_service::strand(*m_iosvc);
            #endif

            iosvc = m_iosvc;
        }
    }

    m_acceptor = new boost::asio::ip::tcp::acceptor(*m_listensvc);

    //boost::asio::ip::tcp::acceptor::reuse_address option(true);
    //m_acceptor->set_option(option);

    m_acceptor->open(m_endpoint->protocol());

    boost::system::error_code ec;
    m_acceptor->bind(*m_endpoint, ec);

    if(!ec) m_acceptor->listen(m_cnnqueuesize, ec);

    if(ec)
    {
        if(m_listensvc)
        {
            m_listensvc->stop();
        }

        if(m_iosvc)
        {
            if(m_iowork) m_iosvc->reset();
            m_iosvc->stop();
        }

        if(m_acceptor)
        {
            delete m_acceptor;
            m_acceptor = NULL;
        }

        if(m_listensvc)
        {
            delete m_listensvc;
            m_listensvc = NULL;
        }

        if(m_iowork)
        {
            delete m_iowork;
            m_iowork = NULL;
        }

        #ifdef ESN_WITH_SSL
        if(m_strand)
        {
            delete m_strand;
            m_strand = NULL;
        }
        #endif

        if(m_iosvc)
        {
            delete m_iosvc;
            m_iosvc = NULL;
        }

        m_svclist.clear();

        if(m_endpoint)
        {
            //std::cout << "Fail to listen on "
            //          << m_endpoint->address().to_string() << ":"
            //          << m_endpoint->port() << " - "
            //          << ec.message() << std::endl;

            std::stringstream ss;
            ss << "Failed to listen on "
                  << m_endpoint->address().to_string() << ":"
                  << m_endpoint->port() << " - "
                  << ec.message();
            LogManager::Warning(ss.str());

            delete m_endpoint;
            m_endpoint = NULL;
        }
        else
        {
            //std::cout << "Fail to listen - " << ec.message() << std::endl;
            LogManager::Warning("Failed to listen - " + ec.message());
        }

        return false;
    }
    else
    {
        m_currentport = m_endpoint->port();
        m_currentip = m_endpoint->address().to_string();

        boost::asio::socket_base::receive_buffer_size option1(m_readbufsize);
        boost::system::error_code ec1;
        m_acceptor->set_option(option1, ec1);
        //if(ec1) std::cout << "Error when set acceptor receive_buffer_size(" << m_readbufsize << "): " << ec1.message() << std::endl;
        if(ec1)
        {
            std::stringstream ss;
            ss << "Error when set acceptor receive_buffer_size(" << m_readbufsize << "): " << ec1.message();
            LogManager::Warning(ss.str());
        }

        boost::asio::socket_base::send_buffer_size option2(m_writebufsize);
        boost::system::error_code ec2;
        m_acceptor->set_option(option2, ec2);
        //if(ec2) std::cout << "Error when set acceptor send_buffer_size(" << m_writebufsize << "): " << ec2.message() << std::endl;
        if(ec2)
        {
            std::stringstream ss;
            ss << "Error when set acceptor send_buffer_size(" << m_writebufsize << "): " << ec2.message();
            LogManager::Warning(ss.str());
        }
    }

    ClientSessionPtr session;

    #ifdef ESN_WITH_SSL
    if(m_gotssl && m_context != NULL)
    {
        boost::asio::io_service::strand * keeper = NULL;
        if(m_iosvc != NULL && m_iosvc == iosvc) keeper = m_strand;
        else if(svc) keeper = (boost::asio::io_service::strand*)(svc->GetOrderSustainer());
        ClientSessionPtr newSession(new ClientSession(*iosvc, *m_context, keeper, m_climgr, m_readbufsize, m_writebufsize, true));
        session = newSession;
    }
    else
    {
        ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, true));
        session = newSession;
    }
    #else
    if(!m_gotssl)
    {
        ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, true));
        session = newSession;
    }
    #endif

    session->SetIoHandler(m_handler);
    session->SetIoFilter(m_filter);
    session->SetBufferManager(m_bufmgr);
    m_acceptor->async_accept(session->GetSocket(),
        boost::bind(&ServerImpl::OnAccept, this, session,
          boost::asio::placeholders::error));

    m_listenthread = new boost::thread(boost::bind(&boost::asio::io_service::run, m_listensvc));

    if(m_iosvc != NULL && m_iosvc == iosvc)
    {
        m_iothread = new boost::thread(boost::bind(&boost::asio::io_service::run, m_iosvc));
    }
    else if(m_svclist.size() > 0)
    {
        //m_svclist.front()->Start();
    }

    //int cpucount = boost::thread::hardware_concurrency();

    //std::cout << "\nListening on " << m_endpoint->port() << std::endl;

    std::stringstream ss;
    ss << "Listening on " << m_endpoint->port() ;
    LogManager::Info(ss.str());

    return true;
}
bool ServerImpl::Start(int port)
{
    if(port <= 0) return false;

    if(m_currentport == port) return true;

    Stop();

    m_endpoint = new boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);

    return Listen();
}
bool ServerImpl::Start(const std::string& ipstr, int port)
{
    if(port <= 0) return false;

    if(m_currentport == port && m_currentip == ipstr) return true;

    boost::system::error_code ec;
    boost::asio::ip::address localaddr = boost::asio::ip::address::from_string(ipstr, ec);

    if(ec)
    {
        //std::cout << "Fail to parse IP : " << ipstr << " - " << ec.message() << std::endl;
        std::stringstream ss;
        ss << "Failed to parse IP : " << ipstr << " - " << ec.message();
        LogManager::Warning(ss.str());
        return false;
    }

    Stop();

    m_endpoint = new boost::asio::ip::tcp::endpoint(localaddr, port);

    return Listen();
}

void ServerImpl::Stop()
{
    if(m_acceptor)
    {
        m_acceptor->close();
    }

    if(m_listensvc)
    {
        m_listensvc->stop();
    }

    if(m_iosvc)
    {
        if(m_iowork) m_iosvc->reset();
        m_iosvc->stop();
    }

    m_climgr.Clear();

    if(m_listenthread)
    {
        m_listenthread->join();
        delete m_listenthread;
        m_listenthread = NULL;
    }

    if(m_iothread)
    {
        m_iothread->join();
        delete m_iothread;
        m_iothread = NULL;
    }

    if(m_acceptor)
    {
        delete m_acceptor;
        m_acceptor = NULL;
    }

    if(m_listensvc)
    {
        delete m_listensvc;
        m_listensvc = NULL;
    }

    if(m_iowork)
    {
        delete m_iowork;
        m_iowork = NULL;
    }

    #ifdef ESN_WITH_SSL
    if(m_strand)
    {
        delete m_strand;
        m_strand = NULL;
    }
    #endif

    if(m_iosvc)
    {
        delete m_iosvc;
        m_iosvc = NULL;
    }

    //m_svclist.clear();

    if(m_endpoint)
    {
        delete m_endpoint;
        m_endpoint = NULL;
    }

    m_currentport = 0;
    m_currentip = "";

    //std::cout << "\nStopped All\n " << std::endl;
}

bool ServerImpl::Listening()
{
    return m_acceptor && m_acceptor->is_open();
}

int ServerImpl::GetCurrentPort()
{
    return m_currentport;
}

std::string ServerImpl::GetCurrentIp()
{
    /*
    //std::string defaultIP = "127.0.0.1";
    std::string ignoreHeader = "127.0.";
    std::string result = "127.0.0.1";

    boost::system::error_code ec;
    boost::asio::io_service iosvc;
    boost::asio::ip::tcp::resolver resolver(iosvc);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(),"");
    boost::asio::ip::tcp::resolver::iterator endpointitr = resolver.resolve(query, ec);

    if(ec)
    {
        std::cout << "Fail to resolve the local address : " << ec.message() << std::endl;
    }
    else
    {
        while(endpointitr != boost::asio::ip::tcp::resolver::iterator())
        {
           boost::asio::ip::address addr = endpointitr->endpoint().address();
           std::string ipstr = addr.to_string();
           //std::cout << "Local address : " << ipstr << std::endl;
           if(addr.is_v4() && ipstr.find(ignoreHeader) != 0)
           {
               result = ipstr;
               break;
           }
           endpointitr++;
        }
    }

    return result;
    */

    return m_currentip;
}

std::string ServerImpl::GetDefaultLocalIp()
{
    //std::string defaultIP = "127.0.0.1";
    std::string ignoreHeader = "127.0.";
    std::string result = "127.0.0.1";

    boost::system::error_code ec;
    boost::asio::io_service iosvc;
    boost::asio::ip::tcp::resolver resolver(iosvc);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(),"");
    boost::asio::ip::tcp::resolver::iterator endpointitr = resolver.resolve(query, ec);

    if(ec)
    {
        //std::cout << "Fail to resolve the local address : " << ec.message() << std::endl;
        LogManager::Warning("Failed to resolve the local address : " + ec.message());
    }
    else
    {
        while(endpointitr != boost::asio::ip::tcp::resolver::iterator())
        {
           boost::asio::ip::address addr = endpointitr->endpoint().address();
           std::string ipstr = addr.to_string();
           //std::cout << "Local address : " << ipstr << std::endl;
           if(addr.is_v4() && ipstr.find(ignoreHeader) != 0)
           {
               result = ipstr;
               break;
           }
           endpointitr++;
        }
    }

    return result;

}

int ServerImpl::GetSessionCount()
{
    return m_climgr.GetSessionCount();
}

int ServerImpl::GetSessions(std::vector<SessionPtr>& sessions)
{
    return m_climgr.GetSessions(sessions);
}

SessionPtr ServerImpl::GetSessionById(int id)
{
    return m_climgr.GetSessionById(id);
}

void ServerImpl::Broadcast(IoBufferPtr data)
{
    m_climgr.Broadcast(data);
}

void ServerImpl::SetIdleTime(int idletype, int idletime)
{
    m_climgr.SetIdleTime(idletype, idletime);
}

void ServerImpl::OnAccept(SessionPtr session, const boost::system::error_code& error)
{
    IoServicePtr svc;
    boost::asio::io_service* iosvc = NULL;
    if(m_svclist.size() > 0)
    {
        svc = m_svclist.front();
        if(svc) iosvc = (boost::asio::io_service*)(svc->GetRunner());
    }
    if(iosvc == NULL) iosvc = m_iosvc;

    if (!error && m_acceptor && iosvc)
    {
        session->Open();

        ClientSessionPtr newone;

        #ifdef ESN_WITH_SSL
        if(m_gotssl && m_context != NULL)
        {
            boost::asio::io_service::strand * keeper = NULL;
            if(m_iosvc != NULL && m_iosvc == iosvc) keeper = m_strand;
            else if(svc) keeper = (boost::asio::io_service::strand*)(svc->GetOrderSustainer());
            ClientSessionPtr newSession(new ClientSession(*iosvc, *m_context, keeper, m_climgr, m_readbufsize, m_writebufsize, true));
            newone = newSession;
        }
        else
        {
            ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, true));
            newone = newSession;
        }
        #else
        if(!m_gotssl)
        {
            ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, true));
            newone = newSession;
        }
        #endif

        newone->SetIoHandler(m_handler);
        newone->SetIoFilter(m_filter);
        newone->SetBufferManager(m_bufmgr);
        m_acceptor->async_accept(newone->GetSocket(),
          boost::bind(&ServerImpl::OnAccept, this, newone,
            boost::asio::placeholders::error));

    }
    else if(error)
    {
        int errorType = 0;
        if(!m_acceptor || !iosvc || error == boost::asio::error::operation_aborted) errorType = -1;

        if(m_handler)
        {
            try { m_handler->OnError(session, errorType, error.value(), error.message()); }
            catch(...) { LogManager::Warning( "Exception found in OnError() event." ); }
        }

        //if(session) session->Close(true);

        if(errorType >= 0)
        {
            if(m_acceptor && iosvc)
            {
                ClientSessionPtr newone;

                #ifdef ESN_WITH_SSL
                if(m_gotssl && m_context != NULL)
                {
                    boost::asio::io_service::strand * keeper = NULL;
                    if(m_iosvc != NULL && m_iosvc == iosvc) keeper = m_strand;
                    else if(svc) keeper = (boost::asio::io_service::strand*)(svc->GetOrderSustainer());
                    ClientSessionPtr newSession(new ClientSession(*iosvc, *m_context, keeper, m_climgr, m_readbufsize, m_writebufsize, true));
                    newone = newSession;
                }
                else
                {
                    ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, true));
                    newone = newSession;
                }
                #else
                if(!m_gotssl)
                {
                    ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, true));
                    newone = newSession;
                }
                #endif

                newone->SetIoHandler(m_handler);
                newone->SetIoFilter(m_filter);
                newone->SetBufferManager(m_bufmgr);
                m_acceptor->async_accept(newone->GetSocket(),
                  boost::bind(&ServerImpl::OnAccept, this, newone,
                    boost::asio::placeholders::error));
            }
        }
        else
        {
            //std::cout << "\nStopped listening." << std::endl;
            LogManager::Info("Stopped listening.");
        }
    }
}

esnlib::ServerPtr esnlib::CreateServer(int maxConnectQueueSize, int readBufferSize, int writeBufferSize)
{
    esnlib::ServerPtr svr(new ServerImpl(maxConnectQueueSize, readBufferSize, writeBufferSize));
    return svr;
}

