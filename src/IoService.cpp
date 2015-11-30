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

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

#include "IoService.h"

using namespace esnlib;

class IoServiceImpl : public IoService
{
public:

	explicit IoServiceImpl(int threadStackSize);
	virtual ~IoServiceImpl();

    virtual void * GetRunner();
    virtual void * GetOrderSustainer();

	virtual void Start();

	virtual void Stop();

protected:

	void Abort();

private:

	boost::asio::io_service m_svc;
	boost::asio::io_service::strand m_strand;
	boost::asio::io_service::work m_work;

    boost::thread * m_thread;

	int m_threadstacksize;

};


IoServiceImpl::IoServiceImpl(int threadStackSize)
: m_svc()
, m_work(m_svc)
, m_strand(m_svc)
, m_thread(NULL)
{
	m_threadstacksize = threadStackSize;
	if(m_threadstacksize < 0) m_threadstacksize = 0;
}

IoServiceImpl::~IoServiceImpl()
{
	Abort();
}

void * IoServiceImpl::GetRunner()
{
	return (void*) &m_svc;
}

void * IoServiceImpl::GetOrderSustainer()
{
	return (void*) &m_strand;
}

void IoServiceImpl::Start()
{
	if(!m_thread)
	{
		if(m_threadstacksize > 0)
		{
			boost::thread::attributes attrs;
			attrs.set_stack_size(m_threadstacksize);
			m_thread = new boost::thread(attrs, boost::bind(&boost::asio::io_service::run, &m_svc));
		}
		else
		{
			m_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, &m_svc));
		}
	}
}

void IoServiceImpl::Stop()
{
    if(m_thread)
    {
        m_svc.reset();
		m_svc.stop();
        m_thread->join();
        delete m_thread;
        m_thread = NULL;
    }
}

void IoServiceImpl::Abort()
{
    if(m_thread)
    {
        m_svc.reset();
		m_svc.stop();
        if(m_thread) m_thread->interrupt();
        delete m_thread;
        m_thread = NULL;
    }
}

esnlib::IoServicePtr esnlib::CreateIoService(int threadStackSize)
{
    esnlib::IoServicePtr iosvc(new IoServiceImpl(threadStackSize));
    return iosvc;
}
