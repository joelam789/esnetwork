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

#include "Client.h"

using namespace esnlib;

Client::Client()
{
    //ctor
}

Client::~Client()
{
    //dtor
}


class ClientImpl : public Client
{
public:
    ClientImpl();
    ClientImpl(int readBufferSize, int writeBufferSize);
    virtual ~ClientImpl();

    virtual bool Connect(const std::string& svraddr, int svrport);
    virtual void Disconnect();

    virtual bool Connected();
    virtual bool IsConnecting();

    virtual void Write(IoBufferPtr data);
    virtual void Write(void* data);

    virtual void SetConnectTimeOut(int seconds);

    virtual void SetIdleTime(int idletype, int idletime);

    virtual void SetIoHandler(IoHandlerPtr handler);
    virtual void SetIoFilter(IoFilterPtr filter);

    virtual void SetIoBufferManager(IoBufferManagerPtr manager);
    virtual void SetIoServiceManager(IoServiceManagerPtr manager);

    virtual IoBufferPtr GetFreeBuffer(int bufsize);

    virtual int GetId();
    virtual void SetId(int id);

    virtual bool HasGlobalData();
    virtual CommonDataPtr GetGlobalData();

    virtual std::string GetDefaultLocalIp();

    virtual std::string GetLocalIp();
    virtual int GetLocalPort();

    virtual std::string GetRemoteIp();
    virtual int GetRemotePort();

    virtual SessionPtr GetCurrentSession();

    virtual bool EnableSsl(bool needverify = true, const std::string& verifyfile = std::string(),
                           const std::string& certfile = std::string(), const std::string& keyfile = std::string());

    virtual bool HasSsl();

protected:

    void OnConnectTimeOut(SessionPtr session);
    void OnConnect(SessionPtr session, const boost::system::error_code& error);
    //void OnResolve(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator itr);

    bool ConnectWithIpAddr(const std::string& svraddr, int svrport);
    bool ConnectWithDomainName(const std::string& svraddr, int svrport);

private:

    boost::asio::io_service * m_service;

    #ifdef ESN_WITH_SSL
    boost::asio::ssl::context * m_context;
    boost::asio::io_service::strand * m_strand;
    #endif

    boost::thread * m_thread;

    //boost::asio::ip::tcp::resolver::query * m_query;
    //boost::asio::ip::tcp::resolver * m_resolver;

    std::list<IoServicePtr> m_svclist;

    SessionManager m_climgr;

    IoBufferManagerPtr m_bufmgr;

    IoServiceManagerPtr m_svcmgr;

    IoHandlerPtr m_handler;

    IoFilterPtr m_filter;

    int m_id;
    bool m_connecting;

    int m_currentport;
    std::string m_currentaddr;
    std::string m_currentip;

    int m_readbufsize;
    int m_writebufsize;

    boost::asio::deadline_timer * m_cnntimer;
    int m_cnntimeout;

    bool m_gotssl;

};


ClientImpl::ClientImpl()
{
    m_service = NULL;

    m_thread = NULL;

    //m_query = NULL;
    //m_resolver = NULL;

    m_id = 0;
    m_connecting = false;

    m_cnntimer = NULL;
    m_cnntimeout = 0;

    m_currentport = 0;
    m_currentaddr = "";
    m_currentip = "";

    m_gotssl = false;
    #ifdef ESN_WITH_SSL
    m_context = NULL;
    m_strand = NULL;
    #endif

    m_readbufsize = ESN_DEFAULT_SOCK_BUFSIZE;
    m_writebufsize = ESN_DEFAULT_SOCK_BUFSIZE;

}

ClientImpl::ClientImpl(int readBufferSize, int writeBufferSize)
{
    m_service = NULL;

    m_thread = NULL;

    //m_query = NULL;
    //m_resolver = NULL;

    m_id = 0;
    m_connecting = false;

    m_cnntimer = NULL;
    m_cnntimeout = 0;

    m_currentport = 0;
    m_currentaddr = "";
    m_currentip = "";

    m_gotssl = false;
    #ifdef ESN_WITH_SSL
    m_context = NULL;
    m_strand = NULL;
    #endif

    m_readbufsize = readBufferSize;
    if(m_readbufsize <= 0) m_readbufsize = ESN_DEFAULT_SOCK_BUFSIZE;

    m_writebufsize = writeBufferSize;
    if(m_writebufsize <= 0) m_writebufsize = ESN_DEFAULT_SOCK_BUFSIZE;

}

ClientImpl::~ClientImpl()
{
    Disconnect();

    #ifdef ESN_WITH_SSL
    if(m_context)
    {
        delete m_context;
        m_context = NULL;
    }
    #endif
}

int ClientImpl::GetId()
{
    return m_id;
}
void ClientImpl::SetId(int id)
{
    m_id = id;
}

bool ClientImpl::IsConnecting()
{
    return m_connecting;
}

void ClientImpl::SetIoHandler(IoHandlerPtr handler)
{
    m_handler = handler;
}

void ClientImpl::SetIoFilter(IoFilterPtr filter)
{
    m_filter = filter;
}

void ClientImpl::SetIoBufferManager(IoBufferManagerPtr manager)
{
    m_bufmgr = manager;
}

void ClientImpl::SetIoServiceManager(IoServiceManagerPtr manager)
{
    m_svcmgr = manager;
}

IoBufferPtr ClientImpl::GetFreeBuffer(int bufsize)
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

std::string ClientImpl::GetDefaultLocalIp()
{
    //std::string defaultIP = "127.0.0.1";
    std::string prefix = "127.0.";
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
           if(addr.is_v4() && ipstr.find(prefix) != 0)
           {
               result = ipstr;
               break;
           }
           endpointitr++;
        }
    }

    return result;
}

std::string ClientImpl::GetLocalIp()
{
    SessionPtr session = GetCurrentSession();
    if(session)
    {
        return session->GetLocalIp();
    }
    return GetDefaultLocalIp();
}
int ClientImpl::GetLocalPort()
{
    SessionPtr session = GetCurrentSession();
    if(session)
    {
        return session->GetLocalPort();
    }
    return 0;
}

std::string ClientImpl::GetRemoteIp()
{
    return m_currentip;
}
int ClientImpl::GetRemotePort()
{
    return m_currentport;
}

SessionPtr ClientImpl::GetCurrentSession()
{
    return m_climgr.GetFirstSession();
}

bool ClientImpl::ConnectWithIpAddr(const std::string& svraddr, int svrport)
{
    boost::system::error_code ec;
    boost::asio::ip::address ipaddr = boost::asio::ip::address::from_string(svraddr, ec);

    if(ec)
    {
        //std::cout << "Fail to parse the IP address " << svraddr << " : " << ec.message() << std::endl;
        //std::cout << "The address string \"" << svraddr << "\" is not a valid IP string." << std::endl;
        return false;
    }

    Disconnect();

    m_currentaddr = svraddr;
    m_currentport = svrport;

    m_currentip = ipaddr.to_string();

    m_service = NULL;
    m_svclist.clear();

    IoServicePtr svc;
    boost::asio::io_service* iosvc = NULL;

    if(!m_svcmgr)
    {
        m_service = new boost::asio::io_service();

        #ifdef ESN_WITH_SSL
        m_strand = new boost::asio::io_service::strand(*m_service);
        #endif

        iosvc = m_service;
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
            m_service = new boost::asio::io_service();

            #ifdef ESN_WITH_SSL
            m_strand = new boost::asio::io_service::strand(*m_service);
            #endif

            iosvc = m_service;
        }
    }

    if(iosvc == NULL) return false;

    boost::asio::ip::tcp::endpoint endpoint(ipaddr, svrport);

    ClientSessionPtr session;

    #ifdef ESN_WITH_SSL
    if(m_gotssl && m_context != NULL)
    {
        boost::asio::io_service::strand * keeper = NULL;
        if(m_service != NULL && m_service == iosvc) keeper = m_strand;
        else if(svc) keeper = (boost::asio::io_service::strand*)(svc->GetOrderSustainer());
        ClientSessionPtr newSession(new ClientSession(*iosvc, *m_context, keeper, m_climgr, m_readbufsize, m_writebufsize, false));
        session = newSession;
    }
    else
    {
        ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, false));
        session = newSession;
    }
    #else
    if(!m_gotssl)
    {
        ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, false));
        session = newSession;
    }
    #endif

    session->SetIoHandler(m_handler);
    session->SetIoFilter(m_filter);
    session->SetBufferManager(m_bufmgr);

    m_connecting = true;

    session->GetSocket().async_connect(endpoint,
                                          boost::bind(&ClientImpl::OnConnect, this, session,
                                          boost::asio::placeholders::error));
    if(m_cnntimeout > 0)
    {
        m_cnntimer = new boost::asio::deadline_timer(*iosvc);
        m_cnntimer->expires_from_now(boost::posix_time::seconds(m_cnntimeout));
        m_cnntimer->async_wait(boost::bind(&ClientImpl::OnConnectTimeOut, this, session));
    }

    if(m_service != NULL && m_service == iosvc)
    {
        m_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, m_service));
    }
    else if(m_svclist.size() > 0)
    {
        //m_svclist.front()->Start();
    }

    //std::cout << "\nClient(" << m_id << ") is connecting to " << svraddr << ":" << svrport << " ... " << std::endl;
    std::stringstream ss;
    ss << "Client(" << m_id << ") is connecting to " << svraddr << ":" << svrport << " ... ";
    LogManager::Info(ss.str());

    return true;
}
bool ClientImpl::ConnectWithDomainName(const std::string& svraddr, int svrport)
{
    Disconnect();

    m_service = NULL;
    m_svclist.clear();

    char portchars[20];
    sprintf(portchars, "%d", svrport);

    std::string portstr = portchars;

    boost::system::error_code ec;

    //m_resolver = new boost::asio::ip::tcp::resolver(*m_service);
    //m_query = new boost::asio::ip::tcp::resolver::query(svraddr, portstr);
    //m_resolver.async_resolve(query, OnResolve);

    boost::asio::io_service ioservice;
    boost::asio::ip::tcp::resolver resolver(ioservice);
    boost::asio::ip::tcp::resolver::query query(svraddr, portstr);
    boost::asio::ip::tcp::resolver::iterator endpointitr = resolver.resolve(query, ec);

    if(ec)
    {
        if(m_service)
        {
            m_service->stop();
        }

        #ifdef ESN_WITH_SSL
        if(m_strand)
        {
            delete m_strand;
            m_strand = NULL;
        }
        #endif

        if(m_service)
        {
            delete m_service;
            m_service = NULL;
        }

        //std::cout << "Fail to resolve the remote address " << svraddr << " : " << ec.message() << std::endl;
        LogManager::Warning("Failed to resolve the remote address " + svraddr + " : " + ec.message());

        return false;
    }
    else
    {
        m_currentaddr = svraddr;
        m_currentport = svrport;

        m_currentip = (*endpointitr).endpoint().address().to_string();
    }

    IoServicePtr svc;
    boost::asio::io_service* iosvc = NULL;

    if(!m_svcmgr)
    {
        m_service = new boost::asio::io_service();

        #ifdef ESN_WITH_SSL
        m_strand = new boost::asio::io_service::strand(*m_service);
        #endif

        iosvc = m_service;
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
            m_service = new boost::asio::io_service();

            #ifdef ESN_WITH_SSL
            m_strand = new boost::asio::io_service::strand(*m_service);
            #endif

            iosvc = m_service;
        }
    }

    if(iosvc == NULL) return false;

    ClientSessionPtr session;

    #ifdef ESN_WITH_SSL
    if(m_gotssl && m_context != NULL)
    {
        boost::asio::io_service::strand * keeper = NULL;
        if(m_service != NULL && m_service == iosvc) keeper = m_strand;
        else if(svc) keeper = (boost::asio::io_service::strand*)(svc->GetOrderSustainer());
        ClientSessionPtr newSession(new ClientSession(*iosvc, *m_context, keeper, m_climgr, m_readbufsize, m_writebufsize, false));
        session = newSession;
    }
    else
    {
        ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, false));
        session = newSession;
    }
    #else
    if(!m_gotssl)
    {
        ClientSessionPtr newSession(new ClientSession(*iosvc, m_climgr, m_readbufsize, m_writebufsize, false));
        session = newSession;
    }
    #endif

    session->SetIoHandler(m_handler);
    session->SetIoFilter(m_filter);
    session->SetBufferManager(m_bufmgr);

    m_connecting = true;

    boost::asio::async_connect(session->GetSocket(), endpointitr,
        boost::bind(&ClientImpl::OnConnect, this, session,
          boost::asio::placeholders::error));

    if(m_cnntimeout > 0)
    {
        m_cnntimer = new boost::asio::deadline_timer(*iosvc);
        m_cnntimer->expires_from_now(boost::posix_time::seconds(m_cnntimeout));
        m_cnntimer->async_wait(boost::bind(&ClientImpl::OnConnectTimeOut, this, session));
    }

    if(m_service != NULL && m_service == iosvc)
    {
        m_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, m_service));
    }
    else if(m_svclist.size() > 0)
    {
        //m_svclist.front()->Start();
    }

    //std::cout << "\nClient(" << m_id << ") is connecting to " << svraddr << ":" << portstr << " ... " << std::endl;
    std::stringstream ss;
    ss << "Client(" << m_id << ") is connecting to " << svraddr << ":" << portstr << " ... ";
    LogManager::Info(ss.str());

    return true;
}

bool ClientImpl::Connect(const std::string& svraddr, int svrport)
{
    if(svraddr.length() <= 0 || svrport <= 0) return false;

    if(m_climgr.GetSessionCount() > 0 &&
       m_currentaddr == svraddr && m_currentport == svrport) return true;

    bool result = ConnectWithIpAddr(svraddr, svrport);
    if(!result) result = ConnectWithDomainName(svraddr, svrport);

    return result;
}
void ClientImpl::Disconnect()
{
    if(m_service)
    {
        m_service->stop();
    }

    m_climgr.Clear();

    if(m_thread)
    {
        m_thread->join();
        delete m_thread;
        m_thread = NULL;
    }

    if(m_cnntimer)
    {
        delete m_cnntimer;
        m_cnntimer = NULL;
    }

    #ifdef ESN_WITH_SSL
    if(m_strand)
    {
        delete m_strand;
        m_strand = NULL;
    }
    #endif

    if(m_service)
    {
        delete m_service;
        m_service = NULL;
    }

    m_connecting = false;

    m_currentport = 0;
    m_currentaddr = "";
    m_currentip = "";

    //std::cout << "\nStopped All\n " << std::endl;
}

bool ClientImpl::Connected()
{
    return m_climgr.GetSessionCount() > 0;
}

void ClientImpl::Write(IoBufferPtr data)
{
    m_climgr.Broadcast(data);
}

void ClientImpl::Write(void* data)
{
    m_climgr.Broadcast(data);
}

void ClientImpl::SetConnectTimeOut(int seconds)
{
    m_cnntimeout = seconds;
}

void ClientImpl::SetIdleTime(int idletype, int idletime)
{
    m_climgr.SetIdleTime(idletype, idletime);
}

bool ClientImpl::HasGlobalData()
{
    return m_climgr.HasData();
}
CommonDataPtr ClientImpl::GetGlobalData()
{
    return m_climgr.GetData();
}

bool ClientImpl::HasSsl()
{
    #ifdef ESN_WITH_SSL
    return m_gotssl && m_context != NULL;
    #else
    return m_gotssl;
    #endif
}

bool ClientImpl::EnableSsl(bool needverify, const std::string& verifyfile, const std::string& certfile, const std::string& keyfile)
{
    if(Connected() || IsConnecting()) return m_gotssl;

    #ifdef ESN_WITH_SSL

    if(m_context != NULL)
    {
        delete m_context;
        m_context = NULL;
    }

    m_gotssl = false;

    m_context = new boost::asio::ssl::context(boost::asio::ssl::context::sslv23);

    boost::system::error_code ec;

    m_context->set_options(boost::asio::ssl::context::default_workarounds, ec);
    if(ec)
    {
        LogManager::Warning("Failed to set options of SSL context, error: " + ec.message());
        return m_gotssl;
    }

    if(needverify) m_context->set_verify_mode(boost::asio::ssl::verify_peer);
    else m_context->set_verify_mode(boost::asio::ssl::verify_none);

    if(needverify && verifyfile.length()>0)
    {
        m_context->load_verify_file(verifyfile, ec);
        if(ec)
        {
            LogManager::Warning("Failed to load verify file of SSL context, error: " + ec.message());
            return m_gotssl;
        }
    }

    if(certfile.length() > 0)
    {
        m_context->use_certificate_chain_file(certfile, ec);
        if(ec)
        {
            LogManager::Warning("Failed to set certificate file of SSL context, error: " + ec.message());
            return m_gotssl;
        }
    }

    if(keyfile.length() > 0)
    {
        m_context->use_private_key_file(keyfile, boost::asio::ssl::context::pem, ec);
        if(ec)
        {
            LogManager::Warning("Failed to set private key file of SSL context, error: " + ec.message());
            return m_gotssl;
        }
    }

    m_gotssl = true;

    #endif

    return m_gotssl;
}

void ClientImpl::OnConnectTimeOut(SessionPtr session)
{
    if(m_cnntimer)
    {
        if(m_handler)
        {
            std::string errMsg = "Connect timeout.";
            try { m_handler->OnError(session, 0, 0, errMsg); }
            catch(...) { LogManager::Warning("Exception found in OnError() event."); }
        }
        if(m_service) m_service->stop();
        session->Close(); // force to close the session ...
    }

    m_connecting = false;
}

void ClientImpl::OnConnect(SessionPtr session, const boost::system::error_code& error)
{
    boost::asio::io_service* iosvc = NULL;
    if(m_svclist.size() > 0)
    {
        IoServicePtr svc = m_svclist.front();
        if(svc) iosvc = (boost::asio::io_service*)(svc->GetRunner());
    }
    if(iosvc == NULL) iosvc = m_service;

    if (!error && iosvc)
    {
        if(m_cnntimer)
        {
            boost::system::error_code ec;
            m_cnntimer->cancel(ec);
            if(ec) LogManager::Warning("Cancel connect timeout timer error: " + ec.message());
            delete m_cnntimer;
            m_cnntimer = NULL;
        }
        session->Open();
    }
    else if(error)
    {
        if(m_handler)
        {
            try { m_handler->OnError(session, 0, error.value(), error.message()); }
            catch(...) { LogManager::Warning("Exception found in OnError() event."); }
        }

        //if(session) session->Close(true);
    }

    m_connecting = false;
}

esnlib::ClientPtr esnlib::CreateClient(int readBufferSize, int writeBufferSize)
{
    esnlib::ClientPtr cli(new ClientImpl(readBufferSize, writeBufferSize));
    return cli;
}

